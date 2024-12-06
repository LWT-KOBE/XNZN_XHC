#ifndef __RUN_H
#define __RUN_H

#include "FreeRTOS_board.h"
#include "stdbool.h"

#define RUN_PRIORITY	    10
#define RUN_STACK_SIZE	  4096

// number of timesteps to average observation sensors' data 平均观测传感器数据的时间步数
#define RUN_SENSOR_HIST		10				
#define ALTITUDE                 (*runData.altPos)
#define VELOCITYD                (*runData.altVel)

typedef struct {
	TaskHandle_t xHandleTask;
	float bestHacc;
	float accMask;
	float accHist[3][RUN_SENSOR_HIST];
	float magHist[3][RUN_SENSOR_HIST];
	float gyoHist[3][RUN_SENSOR_HIST];
	float sumAcc[3];
	float sumMag[3];
	float sumGyo[3];
	int sensorHistIndex;
	float stdAccX, stdAccY, stdAccZ;
	float *altPos;
	float *altVel;
	bool initFlag;
	uint32_t loops;
	double time[2];
	float executionTime;
	formatTrans16Struct_t CNTR;
} runStruct_t;

extern runStruct_t runData;
extern BaseType_t RunEvent;
extern TaskHandle_t xHandleTaskRun;
void runUpdateTask(void *Parameters);
void runInit(void);

#endif



