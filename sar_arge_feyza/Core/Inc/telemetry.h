/*
 * telemetry.h
 *
 *  Created on: Jul 3, 2025
 *      Author: 90553
 */

// telemetry.h
#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <stdint.h>   // kesinlikle olmalı

typedef struct
{
    uint8_t header;           // 0xDD
    uint32_t timestamp;       // systick second
    float temp_degC;
    float gyro_x;
    float gyro_y;
    float gyro_z;
    float acc_x;
    float acc_y;
    float acc_z;
    uint16_t error_flags;
    uint8_t port_id;          // 0x01: UART, 0x02: CAN
    uint16_t crc16;           // opsiyonel
} TelemetryPacket;

uint16_t Calculate_CRC(uint8_t *data, uint16_t len);

void SendTelemetryUART(TelemetryPacket *packet);
void SendTelemetryCAN(TelemetryPacket *packet);

#endif // TELEMETRY_H
