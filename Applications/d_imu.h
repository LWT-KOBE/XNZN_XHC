
#ifndef __D_IMU_H
#define __D_IMU_H

#include "stm32f4xx.h"
#include "FreeRTOS_board.h"
#include "bsp.h"
#include "Util.h"
#include "stdbool.h"
#include "driver_spl06.h"

enum{
	 A_X = 0,
	 A_Y,
	 A_Z,
	 G_Y,
	 G_X,
	 G_Z,
	 TEM,
	 ITEMS,
};

enum{
	IMU_CAIL_NO_NEED = 0,
	IMU_CAIL_START ,
	IMU_CAIL_BEING ,
	IMU_CAIL_FINISH ,
};

typedef struct{
  float x;
	float y;
	float z;
} coordinateFloat_t;

typedef struct{
  s16 x;
	s16 y;
	s16 z;
} coordinateInteger_t;

typedef struct{
	formatTrans16Struct_t x;
	formatTrans16Struct_t y;
	formatTrans16Struct_t z;
} coordinateUnion16_t;

typedef struct{
	formatTrans32Struct_t x;
	formatTrans32Struct_t y;
	formatTrans32Struct_t z;	
} coordinateUnion32_t;

/* UNION Data 	   --------------------------------*/
typedef union{
	int16_t value;
	uint8_t bytes[2];
} int16AndUint8_t;

typedef union{
	int32_t value;
	uint8_t bytes[4];
} int32AndUint8_t;

typedef union{
	uint16_t value;
	uint8_t bytes[2];
} uint16AndUint8_t;

typedef struct {
    float temperature;//读取的温度值 单位℃摄氏度
    float presure;//温度补偿后的气压值 单位mpar 毫帕
    s32 baro_height;//解算后的气压高度值，单位mm毫米
}spl06Struct_t;



typedef struct {
	TaskHandle_t xHandleTask;
	int16AndUint8_t originalTemperature;
	coordinateFloat_t rawAcc;
	coordinateFloat_t rawGyo;	
	coordinateFloat_t rawMag;	
	formatTrans32Struct_t normalizedAcc[3];
	formatTrans32Struct_t normalizedGyo[3];
	formatTrans32Struct_t normalizedMag[3];
	formatTrans32Struct_t temp;
	formatTrans32Struct_t yaw, pitch, roll;

	formatTrans16Struct_t MagErrorFlag;
	errorScanStruct_t MagError;
	spl06Struct_t Spl06Data;
	f32_t 				IntervalTime;

		double 	Time[2];
	
	volatile float acc[3];
	volatile float gyo[3];
	volatile float mag[3];
	float accBIAS[3];
	float gyoBIAS[3];
	float magBIAS[3];
	formatTrans16Struct_t gyoUkf[3];
	formatTrans16Struct_t accUkf[3];
	volatile uint8_t accTare;
	uint32_t imuTareLoop;
	uint8_t state;
	double time[2];
	float intervalTime;
	bool initFlag;
	bool deviceInitState;
	u16 dataFault;
	u16 deviceCNTR;
	uint32_t loops;
	u16 MagFlag;
	uint8_t ImuFlag;
	u16 Temp_Control;
	u16 mems_temperature_ok;
	
	
	
}imusensorStruct_t;

#define IMU_STATIC_TIMEOUT	4
#define IMU_STATIC_STD		 0.05f
#define IMU_STATIC_GYO_STD 0.0010f
#define APB1_TIMNER_CLK 84000000
#define APB2_TIMNER_CLK 168000000
#define DIMU_TIM TIM7
#define DIMU_INNER_FRE  500													//500hz
#define DIMU_OUTER_FRE  250													//250hz
#define DIMU_OUTER_DT	    ((float)1.00 / DIMU_OUTER_FRE)			
#define DIMU_INNER_DT	    ((float)1.00 / DIMU_INNER_FRE)			
#define DIMU_PER  (1000-1)
#define DIMU_PRES (84 -1) 
#define DIMU_PreemptionPriority 2
#define DIMU_USART_SubPriority 0
#define DIMU_TARE_LOOP    10000

#define DIMU_PRIORITY	    	8
#define DIMU_STACK_SIZE	  	256
#define DIMU_NORMAL_PERIOD	1
#define DIMU_FAULT_PERIOD		1000
#define IMU_CNTR 	1
#define ERROR_TIME	500

extern imusensorStruct_t imuSensorData;
extern imusensorStruct_t imuSensorOfChassisData;
extern BaseType_t DIMUEvent;
extern TaskHandle_t xHandleTaskDimu;
void dimuInit(void);
void dIMUTare(void); 

#endif




