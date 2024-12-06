/**************************************************************************
作者：平衡小车之家 
淘宝店铺：http://shop114407458.taobao.com/
**************************************************************************/

#ifndef __DATA_PRTOCOL_H
#define __DATA_PRTOCOL_H
#include "stm32f4xx.h"
#include "FreeRTOS_board.h"
#include "stdbool.h"
#include "bsp.h"
#include "util.h" 
 
extern unsigned char DataScope_OutPut_Buffer[42];	   //待发送帧数据缓存区
void DataScope_Get_Channel_Data(float Data,unsigned char Channel);    // 写通道数据至 待发送帧数据缓存区
unsigned char DataScope_Data_Generate(unsigned char Channel_Number);  // 发送帧数据生成函数 
void DataScope(void);//上位机显示函数 
 
 /**********************************DTU初始化串口****************************************/
//DTU串口号
#define WIRELESS_USARTX								USART2
//DTU接收引脚
#define WIRELESS_USARTX_RX_PIN						BSP_GPIOA3	//接收机接收引脚
//DTU发送引脚
#define WIRELESS_USARTX_TX_PIN						BSP_GPIOA2	//接收机发送引脚
//WIRELESS_USART中断抢占优先级
#define WIRELESS_USART_PreemptionPriority 			3					
//WIRELESS_USART中断响应优先级
#define WIRELESS_USART_SubPriority 					0	

 
 
 
#endif 



