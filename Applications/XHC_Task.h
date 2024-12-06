#ifndef __XHC_TASK_H
#define __XHC_TASK_H

#include "stm32f4xx.h"
#include "FreeRTOS_board.h"
#include "stdbool.h"
#include "bsp.h"
#include "util.h"

#define USE_MAG

#define XHC_Task_PRIORITY 9
#define XHC_Task_STACK_SIZE 512
#define XHC_Task_NORMAL_PERIOD 50  //30




typedef struct {
	TaskHandle_t xHandleTask;
	uint8_t dataInitFlag;
	uint32_t loops;
} XHC_TaskStruct_t;	

XHC_TaskStruct_t* getXHCData(void);



void XHCDataInit(void);

#endif
