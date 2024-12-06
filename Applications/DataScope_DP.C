#include "application.h"

/**************************************************************************
**************************************************************************/
unsigned char DataScope_OutPut_Buffer[42] = {0};	   //串口发送缓冲区
unsigned char i,temp;          //计数变量
unsigned char Send_Count; //串口需要发送的数据个数

/***************************************/
static void DTUInit(void);
/***************************************/
deviceInitClass DTUClass = {
	DTUInit,
};
/*
***************************************************
函数名：Driver_DTU_Init
功能：无线数传初始化
入口参数：	I can understand it at one glance.
返回值：无
应用范围：外部调用
备注：
***************************************************
*/
static void Driver_DTU_Init(USART_TypeDef *USARTx,\
							BSP_GPIOSource_TypeDef *USART_RX,\
							BSP_GPIOSource_TypeDef *USART_TX,\
							uint8_t PreemptionPriority,uint8_t SubPriority){
	BSP_USART_TypeDef WIRELESS_USART;
	WIRELESS_USART.USARTx = USARTx;
	WIRELESS_USART.USART_RX = USART_RX;
	WIRELESS_USART.USART_TX = USART_TX;
	//波特率为500000
	WIRELESS_USART.USART_InitStructure.USART_BaudRate = 9600;	
	//字长为8位数据格式
	WIRELESS_USART.USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	//一个停止位
	WIRELESS_USART.USART_InitStructure.USART_StopBits = USART_StopBits_1;
	//无校验位
	WIRELESS_USART.USART_InitStructure.USART_Parity = USART_Parity_No;
	//接收/发送模式
	WIRELESS_USART.USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	//无硬件数据流控制
	WIRELESS_USART.USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None,		
	
	BSP_USART_Init(&WIRELESS_USART,PreemptionPriority,SubPriority);
	BSP_USART_RX_DMA_Init(&WIRELESS_USART);																	
}
/****************************************************
函数名：DTUInit
功能：数传串口调用初始化
入口参数：无.
返回值：无
应用范围：外部调用
备注：
****************************************************/
static void DTUInit(void){
//	Driver_DTU_Init(WIRELESS_USARTX,\
//	WIRELESS_USARTX_RX_PIN,\
//	WIRELESS_USARTX_TX_PIN,\
//	WIRELESS_USART_PreemptionPriority,\
//	WIRELESS_USART_SubPriority);
}








//函数说明：将单精度浮点数据转成4字节数据并存入指定地址 
//附加说明：用户无需直接操作此函数 
//target:目标单精度数据
//buf:待写入数组
//beg:指定从数组第几个元素开始写入
//函数无返回 
void Float2Byte(float *target,unsigned char *buf,unsigned char beg)
{
    unsigned char *point;
    point = (unsigned char*)target;	  //得到float的地址
    buf[beg]   = point[0];
    buf[beg+1] = point[1];
    buf[beg+2] = point[2];
    buf[beg+3] = point[3];
}
 
 
//函数说明：将待发送通道的单精度浮点数据写入发送缓冲区
//Data：通道数据
//Channel：选择通道（1-10）
//函数无返回 
void DataScope_Get_Channel_Data(float Data,unsigned char Channel)
{
	if ( (Channel > 10) || (Channel == 0) ) return;  //通道个数大于10或等于0，直接跳出，不执行函数
  else
  {
     switch (Channel)
		{
      case 1:  Float2Byte(&Data,DataScope_OutPut_Buffer,1); break;
      case 2:  Float2Byte(&Data,DataScope_OutPut_Buffer,5); break;
		  case 3:  Float2Byte(&Data,DataScope_OutPut_Buffer,9); break;
		  case 4:  Float2Byte(&Data,DataScope_OutPut_Buffer,13); break;
		  case 5:  Float2Byte(&Data,DataScope_OutPut_Buffer,17); break;
		  case 6:  Float2Byte(&Data,DataScope_OutPut_Buffer,21); break;
		  case 7:  Float2Byte(&Data,DataScope_OutPut_Buffer,25); break;
		  case 8:  Float2Byte(&Data,DataScope_OutPut_Buffer,29); break;
		  case 9:  Float2Byte(&Data,DataScope_OutPut_Buffer,33); break;
		  case 10: Float2Byte(&Data,DataScope_OutPut_Buffer,37); break;
		}
  }	 
}


//函数说明：生成 DataScopeV1.0 能正确识别的帧格式
//Channel_Number，需要发送的通道个数
//返回发送缓冲区数据个数
//返回0表示帧格式生成失败 
unsigned char DataScope_Data_Generate(unsigned char Channel_Number)
{
	if ( (Channel_Number > 10) || (Channel_Number == 0) ) { return 0; }  //通道个数大于10或等于0，直接跳出，不执行函数
  else
  {	
	 DataScope_OutPut_Buffer[0] = '$';  //帧头
		
	 switch(Channel_Number)   
   { 
		 case 1:   DataScope_OutPut_Buffer[5]  =  5; return  6;  
		 case 2:   DataScope_OutPut_Buffer[9]  =  9; return 10;
		 case 3:   DataScope_OutPut_Buffer[13] = 13; return 14; 
		 case 4:   DataScope_OutPut_Buffer[17] = 17; return 18;
		 case 5:   DataScope_OutPut_Buffer[21] = 21; return 22;  
		 case 6:   DataScope_OutPut_Buffer[25] = 25; return 26;
		 case 7:   DataScope_OutPut_Buffer[29] = 29; return 30; 
		 case 8:   DataScope_OutPut_Buffer[33] = 33; return 34; 
		 case 9:   DataScope_OutPut_Buffer[37] = 37; return 38;
     case 10:  DataScope_OutPut_Buffer[41] = 41; return 42; 
   }	 
  }
	return 0;
}



void DataScope(void)
{   
		DataScope_Get_Channel_Data( getGimbalData()->pitchAngleFbd*1000, 1 );      
		DataScope_Get_Channel_Data( getGimbalData()->pitchAngleRef*1000, 2 );         
		DataScope_Get_Channel_Data(getGimbalData()->yawAngleFbd*1000, 3 );  //黄色            
		DataScope_Get_Channel_Data(getGimbalData()->yawAngleRef*1000 , 4 );   //紫色
//		DataScope_Get_Channel_Data(500, 5 ); 
//		DataScope_Get_Channel_Data(Target_Position2-(Pitch+48)*10, 6 );
//		DataScope_Get_Channel_Data(0, 7 );
//		DataScope_Get_Channel_Data( 0, 8 ); 
//		DataScope_Get_Channel_Data(0, 9 );  
//		DataScope_Get_Channel_Data( 0 , 10);
		Send_Count = DataScope_Data_Generate(6);
		for( i = 0 ; i < Send_Count; i++) 
		{
		while((USART2->SR&0X40)==0);  
		USART2->DR = DataScope_OutPut_Buffer[i]; 
		}
}








