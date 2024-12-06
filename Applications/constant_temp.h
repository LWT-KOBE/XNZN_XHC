#ifndef __CONSTANT_TEMP_H
#define __CONSTANT_TEMP_H

#include "bsp.h"
#include "pid.h"
#include "processing.h"
#include "util.h"

#define CONSTANT_TEMP_TIM TIM2
#define CONSTANT_TEMP_LENGTH 1000-1
#define CONSTANT_TEMP_PRES 168-1
#define CONSTANT_TEMP_PIN BSP_GPIOB11
#define CONSTANT_TEMP_CCR TIM2->CCR4
#define CONSTANT_TEMP_VAULE	50.0f

typedef struct{
	float tempreature;
	float tempOut;
	lowPassFilterStruct_t lowPassFilterData;
	pidStruct_t *tempPID;
	u32 loops;
}constantTempSturct_t;

void constanTempUpdate(s16 temp);
void constantTempInit(void);

#endif
