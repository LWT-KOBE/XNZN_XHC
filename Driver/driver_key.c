#include "driver.h"
  /**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
/**************************************************************************
函数功能：按键初始化
入口参数：无
返回  值：无 
**************************************************************************/
void KEY_Init(void)
{
    BSP_GPIO_Init(BSP_GPIOA8,GPIO_Mode_IPU);//上拉输入
    BSP_GPIO_Init(BSP_GPIOD12,GPIO_Mode_IPU);//上拉输入
    BSP_GPIO_Init(BSP_GPIOC9,GPIO_Mode_IPU);//上拉输入
    
    BSP_GPIO_EXIT_Init(BSP_GPIOA8,EXTI_Trigger_Falling,0x03,0x03); //中断线配置      
    BSP_GPIO_EXIT_Init(BSP_GPIOC9,EXTI_Trigger_Falling,0x03,0x03); //中断线配置      
    BSP_GPIO_EXIT_Init(BSP_GPIOD12,EXTI_Trigger_Falling,0x03,0x03); //中断线配置      

} 

