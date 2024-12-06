#ifndef __WIRED_CONTROL_H
#define __WIRED_CONTROL_H

#include "stm32f4xx.h"
#include "FreeRTOS_board.h"
#include "stdbool.h"
#include "bsp.h"

//#define IMU_SLAVE


#define SLAVE_USARTX				USART1			//串口号
#define SLAVE_USARTX_TX_PIN			BSP_GPIOA9		//发送引脚
#define SLAVE_USARTX_RX_PIN			BSP_GPIOA10		//接收引脚
#define SLAVE_USART_BAUD_RATE		512000
#define SLAVE_USART_PRE_PRIORITY	3				//SBUS_USART中断抢占优先级
#define SLAVE_USART_SUB_PRIORITY	0				//SBUS_USART中断响应优先级

#define MASTER_USARTX				USART6			//串口号
#define MASTER_USARTX_TX_PIN			BSP_GPIOC6		//发送引脚
#define MASTER_USARTX_RX_PIN			BSP_GPIOC7		//接收引脚
#define MASTER_USART_BAUD_RATE		512000
#define MASTER_USART_PRE_PRIORITY	3				//SBUS_USART中断抢占优先级
#define MASTER_USART_SUB_PRIORITY	0				//SBUS_USART中断响应优先级


#define IMU_MODULAR_BEGIN				0xAD
#define IMU_MODULAR_ADDRESS				0x40

#define MAIN_CONTROL_BEGIN				0x3F
#define MAIN_CONTROL_ADDRESS			0x10

#define VISION_BEGIN					0xD4
#define VISION_ADDRESS					0x20

enum {
	TRANS_ONLY_INS = 0x00,
	TRANS_ADD_ANGLE = 0x01,
	TRANS_ADD_IMU_SENSOR = 0x02,	
	TRANS_ADD_MAG = 0x03	

};

typedef struct {
	formatTrans16Struct_t lightBarsState;
	bool imuCali;
	bool accCali;
	bool canForward;
} controlCommadStruct_t;

typedef struct {
	controlCommadStruct_t cmd;
	errorScanStruct_t wiredError;
	
	formatTrans16Struct_t pitchCmd;
	formatTrans16Struct_t yawCmd;
	formatTrans16Struct_t pitchRecv[3];
	formatTrans16Struct_t yawRecv[3];
	uint8_t transType;
	
	formatTrans16Struct_t fricWheelCmd[2];
	formatTrans16Struct_t fricWheelRecv[2][3];
	
	uint32_t checkSeq;
	uint32_t loops;
} wiredControlStruct_t;


typedef struct{
	uint32_t slaveErrorCount;
	uint32_t slaveLastErrorCount;
	uint32_t intervalNum;	
	bool initFlag;
	uint8_t canForwardIndexPtr;
	
	formatTrans32Struct_t yaw, pitch, roll;
	formatTrans32Struct_t normalizedAcc[3];
	formatTrans32Struct_t normalizedGyo[3];
	formatTrans32Struct_t normalizedMag[3];
	formatTrans32Struct_t temp;
	uint8_t Nd08Slave1_OK;
	uint8_t Nd08Slave2_OK;

} slaveSensorStruct_t;

extern slaveSensorStruct_t slaveSensorData;


void slaveSensorRead(uint8_t *array);

extern wiredControlStruct_t wiredControlData;

void wiredCommandReceive(u8 *array);
void Sk6812Cmd(u8 array);
void wiredControlInit(void);
void commandUpate(void);
void wiredSendData(USART_TypeDef *USARTx);
void wiredTransTypeSwitch(uint16_t state, uint8_t value);

#endif
