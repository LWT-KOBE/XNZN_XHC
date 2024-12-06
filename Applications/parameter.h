#ifndef __PARAMETER_H
#define __PARAMETER_H

#include "bsp.h"

enum parameterslist{
	CONFIG_VERSION = 0,
/*----------------		非TF卡储存段		----------------*/
	YAW_ANGLE_P,
	YAW_ANGLE_I,
	YAW_ANGLE_D,
	YAW_ANGLE_F,
	YAW_ANGLE_PM,
	YAW_ANGLE_IM,
	YAW_ANGLE_DM,
	YAW_ANGLE_OM,
	YAW_ANGLE_MIX,
	YAW_ANGLE_MIN,    
	PITCH_ANGLE_P,
	PITCH_ANGLE_I,
	PITCH_ANGLE_D,
	PITCH_ANGLE_F,
	PITCH_ANGLE_PM,
	PITCH_ANGLE_IM,
	PITCH_ANGLE_DM,
	PITCH_ANGLE_OM,
	PITCH_ANGLE_MIX,
	PITCH_ANGLE_MIN, 

	PITCH_RATE_P,
	PITCH_RATE_I,
	PITCH_RATE_D,
	PITCH_RATE_F,
	PITCH_RATE_PM,
	PITCH_RATE_IM,
	PITCH_RATE_DM,
	PITCH_RATE_OM,
	YAW_RATE_P,
	YAW_RATE_I,
	YAW_RATE_D,
	YAW_RATE_F,
	YAW_RATE_PM,
	YAW_RATE_IM,
	YAW_RATE_DM,
	YAW_RATE_OM,


	ROLL_ANGLE_P,
	ROLL_ANGLE_I,
	ROLL_ANGLE_D,
	ROLL_ANGLE_F,
	ROLL_ANGLE_PM,
	ROLL_ANGLE_IM,
	ROLL_ANGLE_DM, 
	ROLL_ANGLE_OM, 
    
	NUM_OF_LIST
};

typedef struct{
	uint8_t TFInsertState;						//这一刻TF插入的状态
	uint8_t TFInsertLastState;				//上一刻TF卡插入的状态
	uint8_t TFInsertFlag;							//TF卡插入检测
	uint8_t TFError;									//TF错误标志
}parameterStruct_t;

#define TFCARD_NUM_LIST NUM_OF_LIST-21
#define TFCARD_INSERT 0
#define TFCARD_OUT 1
#define TFCARD_INSERT_IO	PDin(10)

#define USE_DIGITAL_IMU
#define RC_SBUS
#define DSHOT_USE
#define MAG9250_ENABLE

#define DIMU_FLAG 	(1 << 0)
#define IMU_FLAG 		(1 << 1)
#define SENSOR_FLAG (1 << 2)
#define UKF_INIT_FLAG 	(1 << 3)
#define SUPERVISOR_FLAG (1 << 4)

#define waitForFlag(xFLAG)	xTaskNotifyWait(0x00000000,0xFFFFFFFF,&xFLAG,portMAX_DELAY);					//最久等待500ms

extern parameterStruct_t parameterRunData;

void tFCardUpdate(void);
uint8_t parameterWriteDataFormFlash(uint8_t robotId);
uint8_t parameterReadDataFromTFCard(uint8_t robotId);

#endif




