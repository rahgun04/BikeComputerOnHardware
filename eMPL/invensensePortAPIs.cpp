/*
 * invensensePortAPIs.c
 *
 *  Created on: 7 Nov 2020
 *      Author: Rahul
 */

#include <wiringPiI2C.h>
#include <wiringPi.h>
#include "mpu6050Main.h"
#include <cstdint>
#include <unistd.h>				//Needed for I2C port
#include <fcntl.h>				//Needed for I2C port
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>		//Needed for I2C port

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <chrono>
#include <iostream>
#include <ctime>

#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>

using namespace std::chrono;

void SelectRegister(uint8_t dev, uint8_t reg){
	/*i2c_cmd_handle_t cmd;
	dev = 0x69;
	cmd = i2c_cmd_link_create();
	esp_error_check(i2c_master_start(cmd));
	esp_error_check(i2c_master_write_byte(cmd, (dev << 1) | i2c_master_write, 1));
	esp_error_check(i2c_master_write_byte(cmd, reg, 1));
	esp_error_check(i2c_master_stop(cmd));
	esp_error_check(i2c_master_cmd_begin(i2c_num, cmd, 1000/porttick_period_ms));
	i2c_cmd_link_delete(cmd);*/
	if (write(fd, &reg, 1) != 1)		//write() returns the number of bytes actually written, if it doesn't match then an error occurred (e.g. no response from the device)
	{
		/* ERROR HANDLING: i2c transaction failed */
		printf("Failed to write address to the i2c bus.\n");
	}

}

int ported_i2c_write(unsigned char slave_addr,
                     unsigned char reg_addr,
                     unsigned char length,
                     unsigned char const *data)
{
	//i2c_cmd_handle_t cmd;
	////printf("%d\n",slave_addr);
	//slave_addr = 0x69;
	//cmd = i2c_cmd_link_create();
	//ESP_ERROR_CHECK(i2c_master_start(cmd));
	//ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (slave_addr << 1) | I2C_MASTER_WRITE, 1));
	//ESP_ERROR_CHECK(i2c_master_write_byte(cmd, reg_addr, 1));
	//ESP_ERROR_CHECK(i2c_master_write(cmd, data, length-1, 0));
	//ESP_ERROR_CHECK(i2c_master_write_byte(cmd, data[length-1], 1));
	//ESP_ERROR_CHECK(i2c_master_stop(cmd));
	//ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM, cmd, 1000/portTICK_PERIOD_MS));
	//i2c_cmd_link_delete(cmd);
	//return 0;
	SelectRegister(slave_addr, reg_addr);
	
	if (write(fd, data, length) != length)		//write() returns the number of bytes actually written, if it doesn't match then an error occurred (e.g. no response from the device)
	{
		/* ERROR HANDLING: i2c transaction failed */
		printf("Failed to write to the i2c bus.\n");
		return -1;
	}
	return 0;
}


int ported_i2c_read(unsigned char slave_addr,
                    unsigned char reg_addr,
                    unsigned char length,
                    unsigned char *data)
{
	/*i2c_cmd_handle_t cmd;
	SelectRegister(slave_addr, reg_addr);
	slave_addr = 0x69;
	cmd = i2c_cmd_link_create();
	ESP_ERROR_CHECK(i2c_master_start(cmd));
	ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (slave_addr << 1) | I2C_MASTER_READ, 1));

	if(length>1)
		ESP_ERROR_CHECK(i2c_master_read(cmd, data, length-1, I2C_MASTER_ACK));

	ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+length-1, I2C_MASTER_NACK));

	ESP_ERROR_CHECK(i2c_master_stop(cmd));
	ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM, cmd, 1000/portTICK_PERIOD_MS));
	i2c_cmd_link_delete(cmd);
	return 0;*/
	SelectRegister(slave_addr, reg_addr);
	
	if (read(fd, data, length) != length)		//read() returns the number of bytes actually read, if it doesn't match then an error occurred (e.g. no response from the device)
	{
		//ERROR HANDLING: i2c transaction failed
		printf("Failed to read from the i2c bus.\n");
		return -1;
	}
	else
	{
		printf("Data read: %s\n", &data);
		return 0;
	}
}



void ported_delay_ms(unsigned long num_ms){
	//vTaskDelay(num_ms/portTICK_PERIOD_MS);
	usleep(1000 * num_ms);

}

int ported_get_ms(unsigned long *count){
	if (!count)
		return 1;
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	duration < double , std::milli > time_span = t2 - t1;
    
	count[0] = (unsigned long) time_span.count();
    return 0;
}





