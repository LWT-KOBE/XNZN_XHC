#ifndef __DRIVER_BMI088_H
#define __DRIVER_BMI088_H
#include "BSP.h"
#include "std_lib.h"
#include "util.h"
#include "BMI088reg.h"
typedef unsigned char bool_t;


#define BMI088_SPI_MIN_PRESCALER SPI_BaudRatePrescaler_16
#define BMI088_SPI_DATA_SIZE SPI_DataSize_8b
#define	BMI088_ACC PDout(10)
#define	BMI088_GYO PDout(11)

#define BMI088_TEMP_FACTOR 0.125f
#define BMI088_TEMP_OFFSET 23.0f

#define BMI088_WRITE_ACCEL_REG_NUM 6
#define BMI088_WRITE_GYRO_REG_NUM 6

#define BMI088_GYRO_DATA_READY_BIT 0
#define BMI088_ACCEL_DATA_READY_BIT 1
#define BMI088_ACCEL_TEMP_DATA_READY_BIT 2

#define BMI088_LONG_DELAY_TIME 80
#define BMI088_COM_WAIT_SENSOR_TIME 150


#define BMI088_ACCEL_IIC_ADDRESSE (0x18 << 1)
#define BMI088_GYRO_IIC_ADDRESSE (0x68 << 1)

#define BMI088_ACCEL_RANGE_3G
//#define BMI088_ACCEL_RANGE_6G
//#define BMI088_ACCEL_RANGE_12G
//#define BMI088_ACCEL_RANGE_24G

#define BMI088_GYRO_RANGE_2000
//#define BMI088_GYRO_RANGE_1000
//#define BMI088_GYRO_RANGE_500
//#define BMI088_GYRO_RANGE_250
//#define BMI088_GYRO_RANGE_125


#define BMI088_ACCEL_3G_SEN 0.0008974358974f
#define BMI088_ACCEL_6G_SEN 0.00179443359375f
#define BMI088_ACCEL_12G_SEN 0.0035888671875f
#define BMI088_ACCEL_24G_SEN 0.007177734375f


#define BMI088_GYRO_2000_SEN 0.00106526443603169529841533860381f
#define BMI088_GYRO_1000_SEN 0.00053263221801584764920766930190693f
#define BMI088_GYRO_500_SEN 0.00026631610900792382460383465095346f
#define BMI088_GYRO_250_SEN 0.00013315805450396191230191732547673f
#define BMI088_GYRO_125_SEN 0.000066579027251980956150958662738366f



#define BMI088_SPI_DEFAULT \
{ \
	.SPIx = SPI2,\
	.SPI_NSS = BSP_GPIOB12, \
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
} BMI088Struct_t;

enum BMI088_GYRO_FSR {
    BMI088_FSR_2000DPS = 0,
    BMI088_FSR_1000DPS,
    BMI088_FSR_500DPS,
    BMI088_FSR_250DPS,
	BMI088_FSR_125DPS,
	BMI088_FSR_62DPS,
    BMI088_FSR_31DPS,
    BMI088_FSR_15DPS
};

enum BMI088_ACCEL_FSR {
	BMI088_FSR_3G = 0,
    BMI088_FSR_6G,
    BMI088_FSR_12G,
    BMI088_FSR_24G
};

enum
{
    BMI088_NO_ERROR = 0x00,
    BMI088_ACC_PWR_CTRL_ERROR = 0x01,
    BMI088_ACC_PWR_CONF_ERROR = 0x02,
    BMI088_ACC_CONF_ERROR = 0x03,
    BMI088_ACC_SELF_TEST_ERROR = 0x04,
    BMI088_ACC_RANGE_ERROR = 0x05,
    BMI088_INT1_IO_CTRL_ERROR = 0x06,
    BMI088_INT_MAP_DATA_ERROR = 0x07,
    BMI088_GYRO_RANGE_ERROR = 0x08,
    BMI088_GYRO_BANDWIDTH_ERROR = 0x09,
    BMI088_GYRO_LPM1_ERROR = 0x0A,
    BMI088_GYRO_CTRL_ERROR = 0x0B,
    BMI088_GYRO_INT3_INT4_IO_CONF_ERROR = 0x0C,
    BMI088_GYRO_INT3_INT4_IO_MAP_ERROR = 0x0D,

    BMI088_SELF_TEST_ACCEL_ERROR = 0x80,
    BMI088_SELF_TEST_GYRO_ERROR = 0x40,
    BMI088_NO_SENSOR = 0xFF,
};


bool_t bmi088_accel_init(void);
bool_t bmi088_gyro_init(void);
uint16_t BMI088_read(void);
uint8_t BMI088_init(uint8_t *gyroFSR, uint8_t *accelFSR);



extern BMI088Struct_t BMI088Data;
extern BSP_SPI_TypeDef *BMI088_SPI;


float *getBMI088GyroScale(void);
float *getBMI088AccelScale(void);
int16_t getBMI088Gyroscope(uint8_t index);
int16_t getBMI088Acceleration(uint8_t index);
int16_t getBMI088Temperature(void);
uint16_t brustReadBMI088(void);

uint16_t IcmDataCNTR_Check(void);

bool driver_BMI088_Init(uint8_t *gyroFSR, uint8_t *accelFSR);
void imu_data_read(void);
#endif
