#ifndef __DRIVER_ADIS16470_H
#define __DRIVER_ADIS16470_H

#include "bsp.h"
#include "d_imu.h"
#include "stdbool.h"

#define	ADIS16470_CS 		PAout(4)
#define BRUSTREAD_CMD		0x6800
#define ADIS16470_SPI_DEFAULT \
{\
	.SPIx = SPI1,\
	.SPI_NSS = BSP_GPIOA4,\
	.SPI_SCK = BSP_GPIOB3,\
	.SPI_MISO = BSP_GPIOB4,\
	.SPI_MOSI = BSP_GPIOB5\
}

/* ¼Ä´æÆ÷±í */
#define xAxisGyroLowWordReg					0x04
#define xAxisGyroHighWordReg				0x06
#define yAxisGyroLowWordReg					0x08
#define yAxisGyroHighWordReg				0x0a
#define zAxisGyroLowWordReg					0x0c
#define zAxisGyroHighWordReg				0x0e
#define xAxisAccLowWordReg					0x10
#define xAxisAccHighWordReg					0x12
#define yAxisAccLowWordReg					0x14
#define yAxisAccHighWordReg					0x16
#define zAxisAccLowWordReg					0x18
#define zAxisAccHighWordReg					0x1a
#define temperatureReg							0x1C
#define data_CNTR										0x22

#define FILT_CTRLReg								0x5c
#define MSC_CTRLReg									0x60
#define DEC_RATEReg									0x64
#define PROD_IDReg									0x72

typedef struct{
	formatTrans16Struct_t	errorFlag;
	coordinateUnion16_t 		gyroscope;
	coordinateUnion16_t 		acceleration;
	coordinateUnion32_t 		gyroscope_h;
	coordinateUnion32_t 		acceleration_h;
	formatTrans16Struct_t temperature;
	formatTrans16Struct_t	dataCNTR;
	u8										checkSum;
	double time[2];
	float executionTime;
}
adis16470Data_t;

extern adis16470Data_t adis16470Data;

void driver_ADIS16470_SPI_Write(u8 regAddr,u16 dat);
u16 driver_ADIS16470_SPI_Read(u8 regAddr);
u16 readADIS16471Data(void);
u16 brustReadADI16470Data(void);
u8 ADIS16470Check(void);
void driver_ADIS16470_Hardware_Init(void);
bool driver_ADIS16470_Init(void);

#endif
