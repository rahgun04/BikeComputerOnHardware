#include "DistanceSensor.h"
#include <wiringSerial.h>
#include <wiringPi.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> 

int index;
int measurements[2];

bool distRunning;

int distanceItter;

unsigned char buf[4];
int CS;

int distanceLoopCount = 10;
int fd;

int dist;

void distanceInit() {
    //get fd and clear any serial buffer backlog
    fd = serialOpen("/dev/ttyS0", 9600);
    printf("%d\n", fd);
    serialFlush(fd);
    distRunning = true;
}

void distanceExec() {
    
    if (serialDataAvail(fd) > 0) {
	    if (serialGetchar(fd) == 0xff) {
		    buf[0] = 0xff;
		    for (int i = 1; i < 4; i++) {
			    buf[i] = serialGetchar(fd);
		    }
		    CS = buf[1] + buf[2] - 1;
		    unsigned char recCS = serialGetchar(fd);
		    if (recCS == (CS & 0xff)) {
			    printf("Match Checksum\n");
		    }
		    else {
			    printf("Checksumm Failed: %d ", (CS & 0xff));
			    printf(" | %d, %d, %d, %d\n", buf[0], buf[1], buf[2], buf[3]);
		    }
		    dist = (buf[1] << 8) + buf[2];
		    printf("Distance = %d\n", dist);
            serialFlush(fd);
	    }
    }
    /*
    //Simple Moving Average (SMA)
    measurements[index] = dist;
    index++;
    index = index % 2;
    */
    measurements[0] = dist;

}



float average() {
    float ret = 0.0;
    for (int i = 0; i < 2; i++) {
        ret += (float) measurements[i];
    }
    ret = ret / 30.0;
    return ret;
}


float distanceRetreive() {
    //return average();
    return (float)measurements[0];
}

void distanceResFree() {
    //close serial
    distRunning = false;
    serialClose(fd);
}
