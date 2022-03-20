
#include <wiringSerial.h>
#include <wiringPi.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> 

unsigned char buf[4];
int CS;

int main(void) {
	wiringPiSetupGpio();
	int fd = serialOpen("/dev/ttyS0" , 9600);
	printf("%d\n", fd);

	while (1) {
		
		if (serialDataAvail(fd) > 0) {
			usleep(4000);
			if (serialGetchar(fd) == 0xff) {
				buf[0] = 0xff;
				for (int i = 1; i < 4; i++) {
					buf[i] = serialGetchar(fd);
				}
				CS =  buf[1] + buf[2] - 1;
				unsigned char recCS = serialGetchar(fd);
				if (recCS == (CS & 0xff)) {
					printf("Match Checksum\n");
				}
				else {
					printf("Checksumm Failed: %d ", (CS & 0xff));
					printf(" | %d, %d, %d, %d\n", buf[0], buf[1], buf[2], buf[3]);
				}
				int dist = (buf[1] << 8) + buf[2];
				printf("Distance = %d\n", dist);
			}
		}
	}
	return 0;
}