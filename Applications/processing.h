#ifndef __PROCESSING_H
#define __PROCESSING_H

#include "FreeRTOS_board.h"
#include "imu.h"
#include "parameter.h"
#include "nav_para.h"
 
#define SLAVE_MAG_SCALE 0.3f
#define SLAVE_TEM_SCALE 0.1f

enum{
	ACCEL_X_1000HZ_LOWPASS = 0,
	ACCEL_Y_1000HZ_LOWPASS,
	ACCEL_Z_1000HZ_LOWPASS
};

typedef struct LowPassFilterData{
    float   gx1;
    float   gx2;
    float   gx3;
    float   previousInput;
    float   previousOutput;
} lowPassFilterStruct_t;

typedef struct{
	int16_t accRotate[3];
	int16_t gyoRotate[3];
	int16_t magRotate[3];
	int16_t accOrient[3];
  int16_t gyoOrient[3];
	int16_t magOrient[3];
	int16_t presOrient[1];
	int16_t tempOrient[1];
	lowPassFilterStruct_t lowPassFilterData[6];
} sensorStruct_t;

extern sensorStruct_t sensorRotate;
typedef void SENSORUpdate_t(float *in,float *out,uint8_t t,float translate);
void sensorProcessUpdate(imusensorStruct_t *imuSensor,coordinateFloat_t *accData,coordinateFloat_t *gyoData,coordinateFloat_t *magData,formatTrans16Struct_t *tempreature);
void sensorProcessInit(void);
float meanRecursiveFilter(s16 newVuale,u8 axis);
float LowPass_Filter(float input, struct LowPassFilterData *filterParameters);
void initAccLowPassFilter(void);

#endif 



