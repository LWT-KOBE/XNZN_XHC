#include "cigan.h" 
#include "app.h"

void cigan_Init(void)
{    	 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIOD时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOE时钟
	
	//GPIOD2初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化GPIO
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO

	GPIO_SetBits(GPIOB,GPIO_Pin_3);//
	GPIO_SetBits(GPIOD,GPIO_Pin_2);//
}


void EXTI_Configuration(void)
{
        //配置外部中断
		EXTI_InitTypeDef EXTI_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
        
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); //使能SYSCFG时钟

    // 配置中断源为GPIOA的Pin6
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource6);//speedA
    EXTI_InitStructure.EXTI_Line = EXTI_Line6; //选择外部中断线路2
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; //设置为中断模式
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; //设置触发方式为下降沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE; //使能外部中断线路
    EXTI_Init(&EXTI_InitStructure);
	
    // 配置中断源为GPIOA的Pin6
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource1);//speedB
    EXTI_InitStructure.EXTI_Line = EXTI_Line1; //选择外部中断线路2
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; //设置为中断模式
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; //设置触发方式为下降沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE; //使能外部中断线路
    EXTI_Init(&EXTI_InitStructure);	

    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn; //选择中断通道为EXTI0_IRQn
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x04; //抢占优先级为0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x04; //子优先级为1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能中断通道
    NVIC_Init(&NVIC_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn; //选择中断通道为EXTI0_IRQn
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x04; //抢占优先级为0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x04; //子优先级为1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能中断通道
    NVIC_Init(&NVIC_InitStructure);
}


//////外部中断0服务程序 
void EXTI9_5_IRQHandler(void)
{

	if(EXTI_GetITStatus(EXTI_Line6)!=RESET)			//标志位被值位（产生中断）SPEEDA
	{	
		PulseCntA++;  
		EXTI_ClearITPendingBit(EXTI_Line6); //清除LINE0上的中断标志位 
	}
	
}

void EXTI1_IRQHandler(void)
{

	if(EXTI_GetITStatus(EXTI_Line1)!=RESET)			//标志位被值位（产生中断）SPEEDA
	{	
		PulseCntB++; 
		EXTI_ClearITPendingBit(EXTI_Line1); //清除LINE0上的中断标志位 
	}
	
}





