#ifndef __ENCODER_H
#define __ENCODER_H
//#include <sys.h>	 
//#include "system.h"
#include "board.h"

// No larger than 65535, because the timer of STM32F103 is 16 bit
//不可大于65535，因为STM32F103的定时器是16位的
#define ENCODER_TIM_PERIOD (u16)(65535)   

void Encoder_Init_TIM2(void);
void Encoder_Init_TIM3(void);
void Encoder_Init_TIM4(void);
void Encoder_Init_TIM5(void);
	
int Read_Encoder(u8 TIMX);
float Moto_Speed(int encoder_cnt,uint16_t ppr,uint16_t ratio,uint16_t cnt_time);
long Num_Encoder_Cnt(float num,uint16_t ppr,float ratio);
float Rpm_Encoder_Cnt(float rpm,uint16_t ppr,uint16_t ratio,uint16_t cnt_time);

void TIM2_IRQHandler(void);
void TIM3_IRQHandler(void);
void TIM4_IRQHandler(void);
void TIM5_IRQHandler(void);

#endif
