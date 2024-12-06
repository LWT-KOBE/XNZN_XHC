#ifndef __DRIVER_SLAVE_SENSOR_H
#define __DRIVER_SLAVE_SENSOR_H

#include "bsp.h"
#include "d_imu.h"

/**********************************SLAVE_SENSOR始化串口****************************************/
#define SLAVE_SENSOR_USARTX										USART3			//SLAVE_SENSOR串口号
#define SLAVE_SENSOR_USARTX_RX_PIN						BSP_GPIOD9	//SLAVE_SENSOR接收引脚
#define SLAVE_SENSOR_USARTX_TX_PIN						BSP_GPIOD8	//SLAVE_SENSOR发送引脚
#define SLAVE_SENSOR_USART_PRE 								3						//SLAVE_SENSOR_USART中断抢占优先级
#define SLAVE_SENSOR_USART_SUB 								0						//SLAVE_SENSOR_USART中断响应优先级

void driver_slaveSensorInit(USART_TypeDef* USARTx,BSP_GPIOSource_TypeDef *USART_RX,BSP_GPIOSource_TypeDef *USART_TX,u32 baudRate,u8 PreemptionPriority,u8 SubPriority);


#endif
