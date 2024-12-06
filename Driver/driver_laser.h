#ifndef __DRIVER_LASER_H
#define __DRIVER_LASER_H

#include "BSP.h"
/*- Utility  ----------------------*/
#include "util.h"
#include "cpu_utils.h"
#include "stdbool.h"

#define LASER_USARTX					USART2			//OPENMV串口号
#define LASER_USARTX_RX_PIN			BSP_GPIOA3	//OPENMV接收引脚
#define LASER_USARTX_TX_PIN			BSP_GPIOA2		//SLAVE_SENSOR发送引脚

#define LASER_USART_PRE_PRIORITY 3						//OPENMV_USART中断抢占优先级
#define LASER_USART_SUB_PRIORITY 0
#define LASER_USART_BOUND		 115200


typedef struct
{
    unsigned char buf[256];
	  int length;
	  bool valid;
	  int type;
}usart_buf_t;

void Clear_L1Mod_Usart(void);

void test11(void);

void Usart_Write_Bytes(USART_TypeDef* USARTx, unsigned char *pdata, int length);

void Laser_Recive(uint8_t *rebuf);
void Driver_Laser_Init(USART_TypeDef* USARTx, BSP_GPIOSource_TypeDef *USART_RX, BSP_GPIOSource_TypeDef *USART_TX, \
														uint32_t baudRate, uint8_t PreemptionPriority, uint8_t SubPriority);
void HEX_Conti_Meas_Cmd(void) ;            
void HEX_FastConti_Meas_Cmd(void);          

extern uint16_t Laser_distance;
extern usart_buf_t g_L1Mod_node;

#endif
