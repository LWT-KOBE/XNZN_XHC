#ifndef __DRIVER_ICM42605_H
#define __DRIVER_ICM42605_H
#include "BSP.h"
#include "regmap_icm42605.h"
#include "std_lib.h"
#include "util.h"


#define ICM42605_SPI_MIN_PRESCALER SPI_BaudRatePrescaler_16
#define ICM42605_SPI_DATA_SIZE SPI_DataSize_8b
#define	ICM42605_CS PCout(0)

#define ICM42605_SPI_DEFAULT \
{ \
	.SPIx = SPI2,\
	.SPI_NSS = BSP_GPIOC0, \
	.SPI_SCK = BSP_GPIOB13, \
	.SPI_MISO = BSP_GPIOB14, \
	.SPI_MOSI = BSP_GPIOB15, \
}

typedef struct{
	formatTrans16Struct_t errorFlag;
	formatTrans16Struct_t gyroscope[3];
	formatTrans16Struct_t acceleration[3];
	formatTrans16Struct_t temperature;
	formatTrans16Struct_t dataCNTR;
    
	errorScanStruct_t ImuError;
    
    
    float temperature_C;
    
    float gyroScale;
    float accelScale;
    uint8_t *gyroFSR;
    uint8_t *accelFSR;    
    uint8_t lastGyroFSR;
    uint8_t lastAccelFSR;
    uint8_t whoIAm;
	float 				rawAcc[3];
	float 				rawGyo[3];
    
	volatile float 		acc[3];
	volatile float 		temp;
	volatile float 		gyo[3];
	float time[2];
	float executionTime;
} icm42605Struct_t;

extern icm42605Struct_t icm42605Data;
extern BSP_SPI_TypeDef *ICM42605_SPI;


float *getIcm42605GyroScale(void);
float *getIcm42605AccelScale(void);
int16_t getIcm42605Gyroscope(uint8_t index);
int16_t getIcm42605Acceleration(uint8_t index);
int16_t getIcm42605Temperature(void);
uint16_t brustReadIcm42605(void);

uint16_t IcmDataCNTR_Check(void);

bool driver_Icm42605_Init(uint8_t *gyroFSR, uint8_t *accelFSR);
void imu_data_read(void);
#endif
