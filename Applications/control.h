#ifndef __CONTROL_H
#define __CONTROL_H

#include "stm32f4xx.h"
#include "FreeRTOS_board.h"
#include "stdbool.h"
#include "bsp.h"
#include "util.h"

#define USE_MAG

#define CONTROL_PRIORITY 9
#define CONTROL_STACK_SIZE 512
#define CONTROL_NORMAL_PERIOD 10

// 加速度校准开关
#define IMU_ACC_FLAG  PEin(11)


typedef struct {
	TaskHandle_t xHandleTask;
	uint8_t dataInitFlag;
	uint32_t loops;
} controlStruct_t;	

controlStruct_t* getcontrolData(void);


extern int Menu,Menu1,Menu2;
extern float Set_Cur,Set_Pos,Set_Vel;

void controlInit(void);

#endif
