#pragma once


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


using namespace std::chrono;

extern int fd;
extern high_resolution_clock::time_point t1;