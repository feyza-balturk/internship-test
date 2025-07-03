/*
 * ADIS.c
 *
 *  Created on: Jul 3, 2025
 *      Author: 90553
 */
#include "ADIS.h"
#include "main.h"



uint16_t ADIS_ReadReg16(uint16_t reg_addr)
{
	//gönderilecek 2 byte veri icin buffer
    uint8_t tx_buf[2];
    //alinacak 2 byte veri icin buffer
    uint8_t rx_buf[2];
    //sonuc
    uint16_t result;

    //ilk byte okuma yazma bildirimi (0x80 okuma)
    tx_buf[0] = (reg_addr >> 8) | 0x80; // MSB + okuma biti
    //ikinci byte LSB
    tx_buf[1] = reg_addr & 0xFF;

    //TX
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); // CS LOW sensore yazma aktif
    HAL_SPI_Transmit(&hspi1, tx_buf, 2, HAL_MAX_DELAY);   //SPI ile veri gonderme
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);   // CS HIGH yazma islemi tamamlandi

    HAL_Delay(1); // sensör icin gecikme

    //RX
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); // CS LOW
    HAL_SPI_Receive(&hspi1, rx_buf, 2, HAL_MAX_DELAY);    //SPI ile veri alma
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);   // CS HIGH

    // MSB | LSB alınan byte ları birlestiriyoruz
    result = (rx_buf[0] << 8) | rx_buf[1];
    //sonucu donduruyoruz.
    return result;
}

void Read_IMU(float *acc_x, float *acc_y, float *acc_z,
              float *gyro_x, float *gyro_y, float *gyro_z)
{
	//gyro verileri adresleri
    int16_t gx = (int16_t)ADIS_ReadReg16(0x0400);
    int16_t gy = (int16_t)ADIS_ReadReg16(0x0402);
    int16_t gz = (int16_t)ADIS_ReadReg16(0x0404);
    int16_t ax = (int16_t)ADIS_ReadReg16(0x0410);
    int16_t ay = (int16_t)ADIS_ReadReg16(0x0412);
    int16_t az = (int16_t)ADIS_ReadReg16(0x0414);

    //accelerometer verileri ve adresleri ve yapilmasi gereken donusumler
    *gyro_x = gx * 0.05f;     // deg/s
    *gyro_y = gy * 0.05f;
    *gyro_z = gz * 0.05f;
    *acc_x = ax * 0.00025f * 9.81f; // m/s²
    *acc_y = ay * 0.00025f * 9.81f;
    *acc_z = az * 0.00025f * 9.81f;
}

float Read_Temperature(void)
{
    uint32_t adc_value = 0;
    float Vsense = 0.0;        //gerilim
    float temperature = 0.0;  //sicaklik

    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    adc_value = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);

    Vsense = (adc_value * 3.3f) / 4095.0f;              //datasheete gore gerilimden sicaklik degerine donmek icin yapilmasi gereken donusum
    temperature = ((Vsense - 0.76f) / 0.0025f) + 25.0f;

    return temperature;
}



