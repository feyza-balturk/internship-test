/*
 * telemetry.c
 *
 *  Created on: Jul 3, 2025
 *      Author: 90553
 */
#include "telemetry.h"
#include "main.h"              // huart2 ve hcan1 gibi donanım tanımları için

                               // CRC hesaplama fonksiyonu: Modbus CRC16 algoritması kullanılır
uint16_t Calculate_CRC(uint8_t *data, uint16_t len)
{
    uint16_t crc = 0xFFFF;    // CRC başlangıç değeri 0xFFFF
    for (uint16_t i = 0; i < len; i++)
    {
        crc ^= data[i]; // Her bayt ile CRC XOR işlemi
        for (uint8_t j = 0; j < 8; j++)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA001; // Sağ kaydır ve XOR yap
            else
                crc >>= 1;                // Sadece sağ kaydır
        }
    }
    return crc;
}


void SendTelemetryUART(TelemetryPacket *packet)
{
    packet->port_id = 0x01;       // Port ID UART olarak ayarlanır
    packet->crc16 = 0;            // CRC alanı önce sıfırlanır

    uint8_t *ptr = (uint8_t *)packet;                                // Paket pointer'ı byte dizisi olarak alınır
    packet->crc16 = Calculate_CRC(ptr, sizeof(TelemetryPacket) - 2); // CRC hesaplanır ve pakete yazılır (-2 CRC alanı için)

    HAL_UART_Transmit(&huart2, ptr, sizeof(TelemetryPacket), HAL_MAX_DELAY); // UART ile paketi gönder
}


void SendTelemetryCAN(TelemetryPacket *packet)
{
    packet->port_id = 0x02;       // Port ID CAN olarak ayarlanır
    packet->crc16 = 0;            // CRC alanı önce sıfırlanır

    uint8_t *ptr = (uint8_t *)packet;  // Paket pointer'ı byte dizisi olarak alınır
    packet->crc16 = Calculate_CRC(ptr, sizeof(TelemetryPacket) - 2); // CRC hesaplanır ve pakete yazılır (-2 CRC alanı için)

    CAN_TxHeaderTypeDef txHeader;
    uint32_t txMailbox;

    txHeader.StdId = 0x123;       // Standart ID set edilir
    txHeader.RTR = CAN_RTR_DATA;  // Veri frame tipi
    txHeader.IDE = CAN_ID_STD;    // Standart ID kullanılır
    txHeader.DLC = 8;             // Her CAN frame 8 byte veri içerir
    txHeader.TransmitGlobalTime = DISABLE; // Global zaman damgası devre dışı

    // Paket uzunluğuna göre 8 byte'lık parçalar halinde gönderim
    for (uint8_t i = 0; i < sizeof(TelemetryPacket); i += 8)
    {
        HAL_CAN_AddTxMessage(&hcan1, &txHeader, &ptr[i], &txMailbox); // CAN mesajı gönderilir
        HAL_Delay(1);
    }
}
