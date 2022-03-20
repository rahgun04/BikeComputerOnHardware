/*
 * invensensePortAPIs.h
 *
 *  Created on: 7 Nov 2020
 *      Author: Rahul
 */

#ifndef COMPONENTS_EMPL_INVENSENSEPORTAPIS_H_
#define COMPONENTS_EMPL_INVENSENSEPORTAPIS_H_
#include "inv_mpu.h"
#include "mpu6050Main.h"


#endif /* COMPONENTS_EMPL_INVENSENSEPORTAPIS_H_ */


int ported_i2c_write(unsigned char slave_addr,
                     unsigned char reg_addr,
                     unsigned char length,
                     unsigned char const *data);

int ported_i2c_read(unsigned char slave_addr,
                    unsigned char reg_addr,
                    unsigned char length,
                    unsigned char *data);


void ported_delay_ms(unsigned long num_ms);

int ported_get_ms(unsigned long *count);

void reg_int_cb(struct int_param_s * int_param);
