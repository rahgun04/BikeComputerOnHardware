

#include "mpu6050Main.h"
#include <unistd.h>				//Needed for I2C port
#include <fcntl.h>				//Needed for I2C port
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>		//Needed for I2C port
#include <stdio.h>
#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "math.h"
#include <stdio.h>
#include <string.h>
 
using namespace std::chrono;
int fd;
high_resolution_clock::time_point t1;


unsigned char more;
short gyro[3], accel[3], sensors, gravity[3];
long quat[4];
float ypr[3];
#define M_PI		3.14159265358979323846
unsigned long sensor_timestamp;

/* Starting sampling rate. */
#define DEFAULT_MPU_HZ  (100)

#define FLASH_SIZE      (512)
#define FLASH_MEM_START ((void*)0x1800)

#define divisor 1<<31
/* Data requested by client. */
#define PRINT_ACCEL     (0x01)
#define PRINT_GYRO      (0x02)
#define PRINT_QUAT      (0x04)

#define ACCEL_ON        (0x01)
#define GYRO_ON         (0x02)

#define MOTION          (0)
#define NO_MOTION       (1)


struct rx_s {
    unsigned char header[3];
    unsigned char cmd;
};
struct hal_s {
    unsigned char sensors;
    unsigned char dmp_on;
    unsigned char wait_for_tap;
    volatile unsigned char new_gyro;
    unsigned short report;
    unsigned short dmp_features;
    unsigned char motion_int_mode;
    struct rx_s rx;
};
static struct hal_s hal = { 0 };

/* The sensors can be mounted onto the board in any orientation. The mounting
 * matrix seen below tells the MPL how to rotate the raw data from thei
 * driver(s).
 * TODO: The following matrices refer to the configuration on an internal test
 * board at Invensense. If needed, please modify the matrices to match the
 * chip-to-body matrix for your particular set up.
 */
static signed char gyro_orientation[9] = { 1, 0, 0,
                                           0,1, 0,
                                           0, 0, 1 };

/* These next two functions converts the orientation matrix (see
 * gyro_orientation) to a scalar representation for use by the DMP.
 * NOTE: These functions are borrowed from Invensense's MPL.
 */
static inline unsigned short inv_row_2_scale(const signed char* row)
{
    unsigned short b;

    if (row[0] > 0)
        b = 0;
    else if (row[0] < 0)
        b = 4;
    else if (row[1] > 0)
        b = 1;
    else if (row[1] < 0)
        b = 5;
    else if (row[2] > 0)
        b = 2;
    else if (row[2] < 0)
        b = 6;
    else
        b = 7;      // error
    return b;
}

static inline unsigned short inv_orientation_matrix_to_scalar(
    const signed char* mtx)
{
    unsigned short scalar;

    /*
       XYZ  010_001_000 Identity Matrix
       XZY  001_010_000
       YXZ  010_000_001
       YZX  000_010_001
       ZXY  001_000_010
       ZYX  000_001_010
     */

    scalar = inv_row_2_scale(mtx);
    scalar |= inv_row_2_scale(mtx + 3) << 3;
    scalar |= inv_row_2_scale(mtx + 6) << 6;


    return scalar;
}

uint8_t dmpGetGravity(short* v, long* q) {
    v[0] = 2 * (q[1] * q[3] - q[0] * q[2]);
    v[1] = 2 * (q[0] * q[1] + q[2] * q[3]);
    v[2] = q[0] * q[0] - q[1] * q[1] - q[2] * q[2] + q[3] * q[3];
    return 0;
}
uint8_t dmpGetYawPitchRoll(float* data, long* q, short* gravity) {
    // yaw: (about Z axis)
    //data[0] = atan2(2*q -> x*q -> y - 2*q -> w*q -> z, 2*q -> w*q -> w + 2*q -> x*q -> x - 1);
    //data[0] = atan(gravity -> y / sqrt(gravity -> x*gravity -> x + gravity -> y*gravity -> y));
    data[0] = 0;
    // pitch: (nose up/down, about Y axis)
    data[1] = atan(gravity[0] / sqrt(gravity[1] * gravity[1] + gravity[2] * gravity[2]));
    // roll: (tilt left/right, about X axis)
    data[2] = atan(gravity[1] / sqrt(gravity[0] * gravity[0] + gravity[2] * gravity[2]));
    return 0;
}

void tapfn(unsigned char direction, unsigned char count) {
    printf("dir: %c", direction);
    printf("count: %c", count);
}

struct Quaternion {
    double w, x, y, z;
};

struct EulerAngles {
    double roll, pitch, yaw;
};


EulerAngles ToEulerAngles(Quaternion q) {
    EulerAngles angles;

    // roll (x-axis rotation)
    double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
    double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
    angles.roll = std::atan2(sinr_cosp, cosr_cosp);

    // pitch (y-axis rotation)
    double sinp = 2 * (q.w * q.y - q.z * q.x);
    if (std::abs(sinp) >= 1)
        angles.pitch = std::copysign(M_PI / 2, sinp); // use 90 degrees if out of range
    else
        angles.pitch = std::asin(sinp);

    // yaw (z-axis rotation)
    double siny_cosp = 2 * (q.w * q.z + q.x * q.y);
    double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
    angles.yaw = std::atan2(siny_cosp, cosy_cosp);

    return angles;
}


int main(void) {

    int result;
    unsigned char accel_fsr;
    unsigned short gyro_rate, gyro_fsr;
    unsigned long timestamp;


	t1 = high_resolution_clock::now();
	//----- OPEN THE I2C BUS -----
	char* filename = (char*)"/dev/i2c-1";
	if ((fd = open(filename, O_RDWR)) < 0)
	{
		//ERROR HANDLING: you can check errno to see what went wrong
		printf("Failed to open the i2c bus");
		return -1;
	}
	int addr = 0x68;          //<<<<<The I2C address of the slave
	if (ioctl(fd, I2C_SLAVE, addr) < 0)
	{
		printf("Failed to acquire bus access and/or talk to slave.\n");
		//ERROR HANDLING; you can check errno to see what went wrong
		return -1;
	}

    mpu_init(NULL);
    /* Get/set hardware configuration. Start gyro. */
    /* Wake up all sensors. */
    mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL);
    /* Push both gyro and accel data into the FIFO. */
    mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL);
    mpu_set_sample_rate(DEFAULT_MPU_HZ);
    /* Read back configuration in case it was set improperly. */
    mpu_get_sample_rate(&gyro_rate);
    mpu_get_gyro_fsr(&gyro_fsr);
    mpu_get_accel_fsr(&accel_fsr);

    /* Initialize HAL state variables. */
    memset(&hal, 0, sizeof(hal));
    hal.sensors = ACCEL_ON | GYRO_ON;
    hal.report = PRINT_QUAT;
    dmp_load_motion_driver_firmware();
    dmp_set_orientation(
        inv_orientation_matrix_to_scalar(gyro_orientation));


    /*
     * Known Bug -
     * DMP when enabled will sample sensor data at 200Hz and output to FIFO at the rate
     * specified in the dmp_set_fifo_rate API. The DMP will then sent an interrupt once
     * a sample has been put into the FIFO. Therefore if the dmp_set_fifo_rate is at 25Hz
     * there will be a 25Hz interrupt from the MPU device.
     *
     * There is a known issue in which if you do not enable DMP_FEATURE_TAP
     * then the interrupts will be at 200Hz even if fifo rate
     * is set at a different rate. To avoid this issue include the DMP_FEATURE_TAP
     */
    hal.dmp_features = DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_TAP |
        DMP_FEATURE_ANDROID_ORIENT | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO |
        DMP_FEATURE_GYRO_CAL;
    dmp_enable_feature(hal.dmp_features);
    dmp_set_fifo_rate(DEFAULT_MPU_HZ);
    mpu_set_dmp_state(1);
    hal.dmp_on = 1;

    while (1) {
        dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors,
            &more);
        short raccel = quat[0];
        //printf("quat[0] = %hd\n",raccel);
        //printf("W: %ld\n, ", quat[0]);
        //printf("YAW: %3.1f, ", ypr[0] * 180/M_PI);
        //printf("PITCH: %3.1f, ", ypr[1] * 180/M_PI);
        //printf("ROLL: %3.1f \n", ypr[2] * 180/M_PI);
        //printf("w%ldwa%ldab%ldbc%ldc\n",quat[0], quat[1], quat[2], quat[3]);
        //printf("w%3.1fwa%3.1fab%3.1fbc%3.1fc\n", (float)(quat[0]), (float)quat[1], (float)quat[2], (float)quat[3]);
        Quaternion quaternion;
        float temp = quat[0] / divisor;
        quaternion.w = (double) temp;

        temp = quat[1] / divisor;
        quaternion.x = (double)temp;

        temp = quat[2] / divisor;
        quaternion.y = (double)temp;

        temp = quat[3] / divisor;
        quaternion.z = (double)temp;

        EulerAngles ea =  ToEulerAngles(quaternion);

        printf("%f  %f  %f\n", ea.yaw, ea.pitch, ea.roll);

        //printf("w%3.1fwa%3.1fab%3.1fbc%3.1fc\n",0.0, 0.1, 0.1, 0.1);
        sleep(1);


    }
	
	return 0;
}