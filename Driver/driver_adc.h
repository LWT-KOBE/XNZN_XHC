#ifndef _DRIVER_ADC_H_
#define _DRIVER_ADC_H_
#include "stm32f4xx.h"

extern __IO uint16_t AdcValue;

void Drv_AdcInit(void);

#endif
