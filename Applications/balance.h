#ifndef __BALANCE_H
#define __BALANCE_H

#include "stm32f4xx.h"
#include "FreeRTOS_board.h"
#include "stdbool.h"
#include "bsp.h"
#include "util.h"

#define USE_MAG

#define BALANCE_PRIORITY 9
#define BALANCE_STACK_SIZE 512
#define BALANCE_NORMAL_PERIOD 5



typedef struct {
	TaskHandle_t xHandleTask;
	uint8_t dataInitFlag;
	uint32_t loops;
	int flag;
} balanceStruct_t;

typedef struct{
	uint8_t finish;
	uint8_t change;
	int mileage;
	float speed;
	float Encoder_pr;
	double Current_Mileage;
}Encoding_Wheel;

typedef struct{
	uint8_t finish;
	uint8_t change;
	float target;
}balance_target_t;

typedef struct{
	uint8_t change;
	uint8_t NFC_buf[4];
	uint8_t flag;
	short num;
	short last_num;
	short target_num;
	float ttt;
}balance_NFC_t;

balanceStruct_t* getbalanceData(void);

extern balanceStruct_t balanceData;
extern balance_NFC_t NFC;
extern balance_target_t Angle_Goal;

extern balance_target_t Motor_SpeedA_Goal;
extern balance_target_t Motor_SpeedB_Goal;

extern Encoding_Wheel EW;

void balanceInit(void);
float max_of_four(float a, float b, float c, float d);
#endif
