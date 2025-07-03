/*
 * ADIS.h
 *
 *  Created on: Jul 3, 2025
 *      Author: 90553
 */


#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>

void Read_IMU(float *acc_x, float *acc_y, float *acc_z,
              float *gyro_x, float *gyro_y, float *gyro_z);
float Read_Temperature(void);
uint16_t ADIS_ReadReg16(uint16_t reg_addr);

#endif // SENSOR_H





