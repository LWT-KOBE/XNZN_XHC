/*******************************************************************************
* 文件名: LobotServoController.h
* 作者: 深圳乐幻索尔科技
* 日期：20160806
* LSC系列舵机控制板二次开发示例
*******************************************************************************/

#ifndef LOBOTSERVOCONTROLLER_H_
#define LOBOTSERVOCONTROLLER_H_

#include "stm32f4xx.h"
#include "FreeRTOS_board.h"
#include "stdbool.h"
#include "bsp.h"
#include "util.h"

#define FRAME_HEADER 0x55             //帧头
#define CMD_SERVO_MOVE 0x03           //舵机移动指令
#define CMD_ACTION_GROUP_RUN 0x06     //运行动作组指令
#define CMD_ACTION_GROUP_STOP 0x07    //停止动作组指令
#define CMD_ACTION_GROUP_SPEED 0x0B   //设置动作组运行速度
#define CMD_GET_BATTERY_VOLTAGE 0x0F  //获取电池电压指令

//宏函数 获得A的低八位
#define GET_LOW_BYTE(A) ((uint8_t)(A))

//宏函数 获得A的高八位
#define GET_HIGH_BYTE(A) ((uint8_t)((A) >> 8))

/**********************************Lobot初始化串口****************************************/
//Lobot串口号
#define Lobot_USARTX							USART3
//Lobot接收引脚
#define Lobot_USARTX_RX_PIN						BSP_GPIOD9	
//DTU发送引脚
#define Lobot_USARTX_TX_PIN						BSP_GPIOD8	
//Lobot_USART中断抢占优先级
#define Lobot_USART_PreemptionPriority 			3					
//Lobot_USART中断响应优先级
#define Lobot_USART_SubPriority 				0	







extern bool isUartRxCompleted;
extern uint8_t LobotRxBuf[16];
extern uint16_t batteryVolt;
extern void receiveHandle(void);

typedef struct _lobot_servo_ {  //舵机ID,舵机目标位置
	uint8_t ID;
	uint16_t Position;
} LobotServo;


void moveServo(uint8_t servoID, uint16_t Position, uint16_t Time);
void moveServosByArray(LobotServo servos[], uint8_t Num, uint16_t Time);
void moveServos(uint8_t Num, uint16_t Time, ...);
void runActionGroup(uint8_t numOfAction, uint16_t Times);
void stopActionGroup(void);
void setActionGroupSpeed(uint8_t numOfAction, uint16_t Speed);
void setAllActionGroupSpeed(uint16_t Speed);
void getBatteryVoltage(void);
void Driver_LobotReadDMA(uint8_t *arrayLobotReceive);



#endif
