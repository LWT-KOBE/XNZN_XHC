#include "bsp_iic.h"
#include "driver_ist8310.h"


unsigned char I2C_Err=0;
u32 ulTimeOut_Time ;

/*
***************************************************
函数名：BSP_I2C_RCC_Init
功能：配置I2C外设时钟
入口参数：	BSP_I2Cx：I2C号
返回值：无
应用范围：内部调用
备注：
***************************************************
*/
void BSP_I2C_RCC_Init(I2C_TypeDef *I2Cx){
	if(I2Cx == I2C1)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	else if(I2Cx == I2C2)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);
	else if(I2Cx == I2C3)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C3,ENABLE);

}

/*
***************************************************
函数名：I2Cx_TO_GPIO_AF
功能：从I2C号输出GPIO_AF
入口参数：	I2Cx：I2C号
返回值：GPIO_AF:复用的I2C模式
应用范围：内部调用
备注：
***************************************************
*/
uint8_t I2Cx_TO_GPIO_AF(I2C_TypeDef *I2Cx){
	uint8_t GPIO_AF;
	if(I2Cx == I2C1)
		GPIO_AF = GPIO_AF_I2C1;
	else if(I2Cx == I2C2)	
		GPIO_AF = GPIO_AF_I2C2;
	else if(I2Cx == I2C3)	
		GPIO_AF = GPIO_AF_I2C3;

	return GPIO_AF;
}

void BSP_I2C_Init(BSP_I2C_TypeDef* BSP_I2Cx){
	
	I2C_InitTypeDef I2C_InitStructure; 	
	GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_ClocksTypeDef   rcc_clocks;	
	
	/*************初始化I2C时钟***************/
	BSP_I2C_RCC_Init(BSP_I2Cx->I2Cx);
	
	/*************初始化I2C引脚***************/
	
	BSP_GPIO_RCC_Init(BSP_I2Cx->I2C_SCL);	//开启GPIO端口时钟;
	BSP_GPIO_RCC_Init(BSP_I2Cx->I2C_SDA);	//开启GPIO端口时钟;	
	GPIO_InitStructure.GPIO_Pin=BSP_I2Cx->I2C_SCL->Pin | BSP_I2Cx->I2C_SDA->Pin;   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;           //开启复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;    //设置为开漏输出
	GPIO_InitStructure.GPIO_PuPd =GPIO_PuPd_NOPULL;;   //带上拉电阻
	GPIO_Init(BSP_I2Cx->I2C_SCL->GPIOx,&GPIO_InitStructure);		
	GPIO_Init(BSP_I2Cx->I2C_SDA->GPIOx,&GPIO_InitStructure);				
	
	/*************设置引脚复用I2C外设***************/
	GPIO_Pin_TO_PinAFConfig(BSP_I2Cx->I2C_SCL ,I2Cx_TO_GPIO_AF(BSP_I2Cx->I2Cx));	
	GPIO_Pin_TO_PinAFConfig(BSP_I2Cx->I2C_SDA,I2Cx_TO_GPIO_AF(BSP_I2Cx->I2Cx));
	
	/**************初始化I2Cx模式**************/
	
	 I2C_DeInit(BSP_I2Cx->I2Cx); //将外设IIC的各个寄存器恢复到复位以后的值	 
	 I2C_InitStructure.I2C_ClockSpeed=400000;  //标准模式 时钟频率为100KHZ
	 I2C_InitStructure.I2C_Mode=I2C_Mode_I2C;   //选中I2C模式
	 I2C_InitStructure.I2C_DutyCycle=I2C_DutyCycle_2;
	 I2C_InitStructure.I2C_OwnAddress1=0X00;      //当I2C出于从模式时,自身的地址
	 I2C_InitStructure.I2C_Ack=I2C_Ack_Enable;
	 I2C_InitStructure.I2C_AcknowledgedAddress=I2C_AcknowledgedAddress_7bit;
	 I2C_Init(BSP_I2Cx->I2Cx, &I2C_InitStructure);	 
	 I2C_AcknowledgeConfig(BSP_I2Cx->I2Cx,ENABLE);   //在接收到一个字节后返回一个应答ACK
	 
	/**************使能I2Cx外设*******************/	 
	 I2C_Cmd(BSP_I2Cx->I2Cx,ENABLE);  //开启外设IIC模块		


	/*超时设置*/
    RCC_GetClocksFreq(&rcc_clocks);
    ulTimeOut_Time = (rcc_clocks.SYSCLK_Frequency /10000); 

}


/*
 * BSP_I2C_WriteOneByte
 * 描述  ：写一个字节到I2C设备寄存器中
 * 输入  ：REG_Address 接收数据的IIC设备寄存器的地址 
 *        REG_data 待写入的数据
		  Start_Address IIC设备硬件地址
		  BSP_I2Cx 硬件i2c外设
 * 输出  ：无
 * 返回  ：无
 * 调用  ：内部调用
 */	


void BSP_I2C_WriteOneByte(BSP_I2C_TypeDef* BSP_I2Cx,uint8_t Start_Address, uint8_t REG_Address,uint8_t REG_data){
	
	u32 tmr;

    tmr = ulTimeOut_Time;
    while(I2C_GetFlagStatus(BSP_I2Cx->I2Cx, I2C_FLAG_BUSY));
    while((--tmr)&&I2C_GetFlagStatus(BSP_I2Cx->I2Cx, I2C_FLAG_BUSY));
    if(tmr==0) I2C_Err = 1;
	
	
	
	I2C_GenerateSTART(BSP_I2Cx->I2Cx,ENABLE);
	
	while((--tmr)&&(!I2C_CheckEvent(BSP_I2Cx->I2Cx,I2C_EVENT_MASTER_MODE_SELECT)));
    if(tmr==0) I2C_Err = 1;

	
	I2C_Send7bitAddress(BSP_I2Cx->I2Cx,Start_Address,I2C_Direction_Transmitter);

	while((--tmr)&&(!I2C_CheckEvent(BSP_I2Cx->I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)));
    if(tmr==0) I2C_Err = 1;

	I2C_SendData(BSP_I2Cx->I2Cx,REG_Address);

	while((--tmr)&&(!I2C_CheckEvent(BSP_I2Cx->I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED)));
    if(tmr==0) I2C_Err = 1;

	I2C_SendData(BSP_I2Cx->I2Cx,REG_data);

	while((--tmr)&&(!I2C_CheckEvent(BSP_I2Cx->I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED)));
    if(tmr==0) I2C_Err = 1;

	I2C_GenerateSTOP(BSP_I2Cx->I2Cx,ENABLE);	
	
		
	
}

/*
 * 函数名：BSP_I2C_ReadOneByte
 * 描述  ：从IIC设备寄存器中读取一个字节
 * 输入  ：REG_Address 读取数据的寄存器的地址
		  Start_Address IIC设备硬件地址
		  BSP_I2Cx 硬件i2c外设
 * 输出  ：无
 * 返回  ：无
 * 调用  ：内部调用 
*/

uint8_t BSP_I2C_ReadOneByte(BSP_I2C_TypeDef* BSP_I2Cx,uint8_t Start_Address, uint8_t REG_Address){

	uint8_t REG_data;
	//uint8_t I2C_Hardware_Count=0;

    u32 tmr;

    tmr = ulTimeOut_Time;
    while((--tmr)&&I2C_GetFlagStatus(BSP_I2Cx->I2Cx, I2C_FLAG_BUSY));
    if(tmr==0) I2C_Err = 1;	
	

	while((--tmr)&&(I2C_GetFlagStatus(BSP_I2Cx->I2Cx,I2C_FLAG_BUSY)));
    if(tmr==0) I2C_Err = 1;

	I2C_GenerateSTART(BSP_I2Cx->I2Cx,ENABLE);//起始信号

	while((--tmr)&&(!I2C_CheckEvent(BSP_I2Cx->I2Cx,I2C_EVENT_MASTER_MODE_SELECT)));
    if(tmr==0) I2C_Err = 1;

	I2C_Send7bitAddress(BSP_I2Cx->I2Cx,Start_Address,I2C_Direction_Transmitter);//发送设备地址+写信号

	while((--tmr)&&(!I2C_CheckEvent(BSP_I2Cx->I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)));//
    if(tmr==0) I2C_Err = 1;

	I2C_Cmd(BSP_I2Cx->I2Cx,ENABLE);

	I2C_SendData(BSP_I2Cx->I2Cx,REG_Address);//发送存储单元地址，从0开始

	while((--tmr)&&(!I2C_CheckEvent(BSP_I2Cx->I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED)));
    if(tmr==0) I2C_Err = 1;

	I2C_GenerateSTART(BSP_I2Cx->I2Cx,ENABLE);//起始信号

	while((--tmr)&&(!I2C_CheckEvent(BSP_I2Cx->I2Cx,I2C_EVENT_MASTER_MODE_SELECT)));
    if(tmr==0) I2C_Err = 1;

	I2C_Send7bitAddress(BSP_I2Cx->I2Cx,Start_Address,I2C_Direction_Receiver);//发送设备地址+读信号

	while((--tmr)&&(!I2C_CheckEvent(BSP_I2Cx->I2Cx,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)));
    if(tmr==0) I2C_Err = 1;

	I2C_AcknowledgeConfig(BSP_I2Cx->I2Cx,DISABLE);

	I2C_GenerateSTOP(BSP_I2Cx->I2Cx,ENABLE);

	while((--tmr)&&(!(I2C_CheckEvent(BSP_I2Cx->I2Cx,I2C_EVENT_MASTER_BYTE_RECEIVED))));
    if(tmr==0) I2C_Err = 1;

	REG_data=I2C_ReceiveData(BSP_I2Cx->I2Cx);//读出寄存器数据
	
	//vTaskDelay(1);

	return REG_data;	
	
}


/***************常用I2C配置*******************/
BSP_I2C_TypeDef BSP_I2C1 = {
	.I2Cx = I2C1,
	.I2C_SCL = BSP_GPIOB8,
	.I2C_SDA = BSP_GPIOB9,

};

BSP_I2C_TypeDef BSP_I2C2 = {
	.I2Cx = I2C2,
	.I2C_SCL = BSP_GPIOB10,
	.I2C_SDA = BSP_GPIOB11,

};

BSP_I2C_TypeDef BSP_I2C3 = {
	.I2Cx = I2C3,
	.I2C_SCL = BSP_GPIOA8,
	.I2C_SDA = BSP_GPIOC9,

};

