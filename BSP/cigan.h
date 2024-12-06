//*******************************
//STM32F407 板载LED配置
//File: led.h
//Author: xxpcb(wxgzh:码农爱学习)
//Vesion: V1.0
//Date: 2020/05/30
//*******************************

#ifndef __CIGAN_H
#define __CIGAN_H
#include "bsp_gpio.h"

#define digitalHigh(p,i)     {p->BSRRH=i;}  //引脚输出高电平       
#define digitalLow(p,i)      {p->BSRRL=i;}   //引脚输出低电平
#define digitalToggle(p,i)   {p->ODR ^=i;} //反转

#define cigan_on			PAin(4) = 1;
#define cigan_off			PAin(4) = 0;
void cigan_Init(void);//初始化		 
void EXTI_Configuration(void);
#endif
