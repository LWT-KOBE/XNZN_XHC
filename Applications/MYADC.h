#ifndef __MYADC_H
#define __MYADC_H
 
#include "stm32f4xx.h"                  // Device header
extern uint16_t* ADC_Value; 

extern u16 garry_ch0[10] ;
extern u16 gGetAdcCounter;
void MyADC_Init(void);
u16* MyADC_GetValue(void);
void MyDMA_Init(uint32_t SAddress, uint32_t DAddress, uint16_t Size);
void GetAdcAverage(void);
#endif


