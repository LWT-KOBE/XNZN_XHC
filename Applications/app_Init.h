#ifndef __APP_INIT_H
#define __APP_INIT_H

#include "FreeRTOS_board.h"

#define INIT_PRIORITIES 2
#define INIT_SIZE 512
#define USB_USART_PreemptionPriority        0X02
#define USB_USART_SubPriority               0X00
typedef struct {
	EventGroupHandle_t eventGroups;
}taskInit_t;

void appInit(void *Parameters);

extern taskInit_t taskInitData;

#endif 





