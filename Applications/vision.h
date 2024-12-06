#ifndef __VISION_H
#define __VISION_H

#include "bsp.h"
#include "util.h"

#define VISION_USARTX						USART3				//串口号
#define VISION_USARTX_TX_PIN				BSP_GPIOB10			//发送引脚
#define VISION_USARTX_RX_PIN				BSP_GPIOB11			//接收引脚
#define VISION_USART_BAUD_RATE				460800
#define VISION_USART_PRE_PRIORITY			3					//SBUS_USART中断抢占优先级
#define VISION_USART_SUB_PRIORITY			0					//SBUS_USART中断响应优先级

#define VISION_GAIN								 30						//视觉增益2.5

typedef enum{
	TX2_STOP = 0,							//停止工作		
	TX2_DISTINGUISH_ARMOR ,		//识别装甲板
	TX2_DISTINGUISH_BUFF 			//识别大小符	
} visionWorkMode_e;

typedef enum{
	SMALL_BULLET = 0,
	BIG_BULLET
} bulletType_e;

typedef enum{
	ENEMY_RED = 0,
	ENEMY_BLUE
} enemyType_e;

typedef enum{
	SPEED0,
	SPEED1,
	SPEED2,
	SPEED3,
}speedLevelType_e;

typedef struct{
	formatTrans32Struct_t x;
	formatTrans32Struct_t y;
	formatTrans32Struct_t z;
}point3FStruct_t;

typedef struct{
	formatTrans16Struct_t armorCoordinate[3];        //装甲板坐标信息，从tx2接收，发送到主控
	formatTrans16Struct_t armorCoordinateReal[3];	//装甲板实际坐标信息，从tx2接收，发送到主控
	uint8_t distingushState;
	formatTrans16Struct_t CNTR;				//TX2回传的包序号，从tx2接收，发送到主控
	
	uint8_t mainControlCmd;					//主控对TX2的总命令
	uint32_t checkSeq;
	
	uint32_t visionErrorCount;				// 误判或丢失目标 次数
	uint32_t visionLastErrorCount;
	uint32_t intervalNum;
	uint16_t counter;
	speedLevelType_e speedLevel;
	errorScanStruct_t visionError;
	uint8_t shootSpeed;
} visionStruct_t;

extern visionStruct_t visionData;

void visionReceive(u8 *array);
void visionControlInit(void);
void visionSendData(USART_TypeDef *USARTx);

#endif
