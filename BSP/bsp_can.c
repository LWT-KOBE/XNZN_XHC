#include "bsp_can.h"

/******************************外部调用函数************************************/
void BSP_CAN_Mode_Init(BSP_CAN_TypeDef *BSP_CANx,u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode,u8 PreemptionPriority,u8 SubPriority);
//void BSP_CAN_Mode_Init(CAN_TypeDef *CANx,u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode,u8 PreemptionPriority,u8 SubPriority);
 u8	 BSP_CAN_Send_Msg(BSP_CAN_TypeDef *BSP_CANx,CanTxMsg *TxMessage);
 u8	 BSP_CAN_Receive_Msg(BSP_CAN_TypeDef *BSP_CANx,CanRxMsg *RxMessage);
/*****************************************************************************/

const CAN_FilterInitTypeDef  CAN1_FilterInitStructure = {
	.CAN_FilterNumber = 0,										/*过滤器0*/					
	.CAN_FilterMode = CAN_FilterMode_IdMask,	/*标识符屏蔽位模式*/	
	.CAN_FilterScale = CAN_FilterScale_32bit,	/*32位的模式*/				
	.CAN_FilterIdHigh = 0x0000,								/*32位MASK*/				
	.CAN_FilterIdLow = 0x0000,
	.CAN_FilterMaskIdHigh = 0x0000,
	.CAN_FilterMaskIdLow = 0x0000,
	.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0,	/*过滤器0关联到FIFO0*/	
	.CAN_FilterActivation = ENABLE,								/*激活过滤器*/					
};

const CAN_FilterInitTypeDef  CAN2_FilterInitStructure = {
	.CAN_FilterNumber = 20,										/*过滤器14*/
	.CAN_FilterMode = CAN_FilterMode_IdMask,	/*标识符屏蔽位模式*/	
	.CAN_FilterScale = CAN_FilterScale_32bit,	/*32位的模式*/				
	.CAN_FilterIdHigh = 0x0000,								/*32位MASK*/				
	.CAN_FilterIdLow = 0x0000,
	.CAN_FilterMaskIdHigh = 0x0000,
	.CAN_FilterMaskIdLow = 0x0000,
	//.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0,	/*过滤器0关联到FIFO0*/
	.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0,	/*过滤器0关联到FIFO0*/
	.CAN_FilterActivation = ENABLE,								/*激活过滤器*/
};

/*
***************************************************
函数名：BSP_SPI_RCBSP_CAN_RCC_InitC_Init
功能：配置CAN外设时钟
入口参数：	CANx：CAN号
返回值：无
应用范围：内部调用
备注：
***************************************************
*/
static void BSP_CAN_RCC_Init(CAN_TypeDef* CANx){
	uint32_t RCC_CANx;
	
	if(CANx == CAN1)
		RCC_CANx = RCC_APB1Periph_CAN1;
	else if(CANx == CAN2)
		RCC_CANx = RCC_APB1Periph_CAN1 | RCC_APB1Periph_CAN2;
	
	RCC_APB1PeriphClockCmd(RCC_CANx,ENABLE);
}

/*
***************************************************
函数名：CANx_TO_GPIO_AF
功能：从CAN号输出GPIO_AF
入口参数：	CANx：CAN号
返回值：GPIO_AF:复用的CAN模式
应用范围：内部调用
备注：
***************************************************
*/
static uint8_t CANx_TO_GPIO_AF(CAN_TypeDef *CANx){
	uint8_t GPIO_AF;
	if(CANx == CAN1)			
		GPIO_AF = GPIO_AF_CAN1;
	else if(CANx == CAN2)	
		GPIO_AF = GPIO_AF_CAN2;
	
	return GPIO_AF;
}

/*
***************************************************
函数名：GPIO_TO_NVIC_IRQChannel
功能：输出NVIC中断通道
入口参数：	GPIO：引脚号
返回值：NVIC_IRQChannel
应用范围：内部调用
备注：
***************************************************
*/
static uint8_t	CAN_TO_NVIC_IRQChannel(CAN_TypeDef* CANx){
	uint8_t NVIC_IRQChannel;
	
	if(CANx == CAN1)			
		NVIC_IRQChannel = CAN1_RX0_IRQn;
	else if(CANx == CAN2)	
		NVIC_IRQChannel = CAN2_RX0_IRQn;
	
	return NVIC_IRQChannel;	
}

/*
***************************************************
函数名：BSP_CAN_Mode_Init
功能：CAN初始化
入口参数：	BSP_CANx：CAN号
					tsjw：重新同步跳跃时间单元.范围:CAN_SJW_1tq~ CAN_SJW_4tq
					tbs2：时间段2的时间单元.   范围:CAN_BS2_1tq~CAN_BS2_8tq;
					tbs1：时间段1的时间单元.   范围:CAN_BS1_1tq ~CAN_BS1_16tq
					brp ：波特率分频器.				范围:1~1024; tq=(brp)*tpclk1
					mode：CANx模式							范围:CAN_Mode_Normal,普通模式;CAN_Mode_LoopBack,回环模式;
					PreemptionPriority：抢占优先级
					SubPriority：子优先级
返回值：无
应用范围：外部调用
备注：已解决初始化CAN2必须先初始化CAN1
BSP_CAN_Mode_Init(&can1,CAN_SJW_1tq,CAN_BS2_5tq,CAN_BS1_9tq,3,CAN_Mode_Normal,Preemption,Sub);

***************************************************
*/
//CANbps= Fpclk/((BRP+1)*((Tseg1+1)+(Tseg2+1)+1)  
//所以这里CANbps=APB1总线频率45M/3/(1+5+9))=1000k bps
//总体配置方向: Tseg1>=Tseg2  Tseg2>=tq; Tseg2>=2TSJW	



void BSP_CAN_Mode_Init(BSP_CAN_TypeDef *BSP_CANx,u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode,u8 PreemptionPriority,u8 SubPriority){
	CAN_InitTypeDef		CAN_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStruct;
	
	CAN_FilterInitStruct = BSP_CANx->CAN_FilterInitStructure;
	
	/*************初始化CANx时钟***************/
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//使能CAN1时钟
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//使能CAN1时钟
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//使能CAN1时钟
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);//使能CAN2时钟
	BSP_CAN_RCC_Init(BSP_CANx->CANx);
	
	/*************初始化CANx引脚***************/
	BSP_GPIO_Init(BSP_CANx->CANx_RX,GPIO_Mode_AF_PP);	//初始化CANx_RX引脚为复用推挽
	BSP_GPIO_Init(BSP_CANx->CANx_TX,GPIO_Mode_AF_PP);	//初始化CANx_TX引脚为复用推挽
	
	/*************引脚复用映射配置***************/
	GPIO_Pin_TO_PinAFConfig(BSP_CANx->CANx_RX,CANx_TO_GPIO_AF(BSP_CANx->CANx));	//CANx_RX引脚复用为 CANx
	GPIO_Pin_TO_PinAFConfig(BSP_CANx->CANx_TX,CANx_TO_GPIO_AF(BSP_CANx->CANx));	//CANx_TX引脚复用为 CANx
	
	/****************CAN单元设置*****************/
	CAN_InitStructure.CAN_TTCM=DISABLE;		//非时间触发通信模式   
	CAN_InitStructure.CAN_ABOM=ENABLE;		//软件自动离线管理	  
	CAN_InitStructure.CAN_AWUM=DISABLE;		//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
	CAN_InitStructure.CAN_NART=DISABLE;		//禁止报文自动传送 
	CAN_InitStructure.CAN_RFLM=DISABLE;		//报文不锁定,新的覆盖旧的  
	CAN_InitStructure.CAN_TXFP=DISABLE;		//优先级由报文标识符决定 
	CAN_InitStructure.CAN_Mode= mode;	 		//模式设置 
	CAN_InitStructure.CAN_SJW=tsjw;				//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1tq~CAN_SJW_4tq
	CAN_InitStructure.CAN_BS1=tbs1; 			//Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2=tbs2;				//Tbs2范围CAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler=brp;  //分频系数(Fdiv)为brp+1	
	CAN_Init(BSP_CANx->CANx, &CAN_InitStructure);   // 初始化CANx 
	
	/****************CAN配置过滤器*****************/
	//使用CAN2得加上这句
	CAN_SlaveStartBank(14);
	CAN_FilterInitStruct.CAN_FilterNumber=0;	  //过滤器0
	CAN_FilterInitStruct.CAN_FilterMode=CAN_FilterMode_IdMask; 
	CAN_FilterInitStruct.CAN_FilterScale=CAN_FilterScale_32bit; //32位 
	CAN_FilterInitStruct.CAN_FilterIdHigh=0x0000;////32位ID
	CAN_FilterInitStruct.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStruct.CAN_FilterMaskIdHigh=0x0000;//32位MASK
	CAN_FilterInitStruct.CAN_FilterMaskIdLow=0x0000;
	CAN_FilterInitStruct.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
	CAN_FilterInitStruct.CAN_FilterActivation=ENABLE; //激活过滤器0

	CAN_FilterInit(&CAN_FilterInitStruct);   
	
	/****************CAN配置消息中断*****************/
	CAN_ITConfig(BSP_CANx->CANx,CAN_IT_FMP0,ENABLE);	//FIFO0消息挂号中断允许
	
	
	NVIC_InitStructure.NVIC_IRQChannel = CAN_TO_NVIC_IRQChannel(BSP_CANx->CANx);	//CANx接收中断
	//NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn | CAN2_RX0_IRQn;	//CANx接收中断
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PreemptionPriority;		//抢占优先级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = SubPriority;									//子优先级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//使能CANx接收中断通道
  NVIC_Init(&NVIC_InitStructure);	//配置
}

//void BSP_CAN_Mode_Init(CAN_TypeDef *CANx,u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode,u8 PreemptionPriority,u8 SubPriority){
//	CAN_InitTypeDef		CAN_InitStructure;
//	NVIC_InitTypeDef  NVIC_InitStructure;
//	GPIO_InitTypeDef 	   GPIO_InitStructure;		//GPIO物理接口
//	CAN_FilterInitTypeDef CAN_FilterInitStruct;
//	
//	//CAN_FilterInitTypeDef  CAN1_FilterInitStructure;
//	
//	
//	CAN_FilterInitTypeDef  CAN1_FilterInitStructure = {
//	.CAN_FilterNumber = 0,										/*过滤器0*/					
//	.CAN_FilterMode = CAN_FilterMode_IdMask,	/*标识符屏蔽位模式*/	
//	.CAN_FilterScale = CAN_FilterScale_32bit,	/*32位的模式*/				
//	.CAN_FilterIdHigh = 0x0000,								/*32位MASK*/				
//	.CAN_FilterIdLow = 0x0000,
//	.CAN_FilterMaskIdHigh = 0x0000,
//	.CAN_FilterMaskIdLow = 0x0000,
//	.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0,	/*过滤器0关联到FIFO0*/	
//	.CAN_FilterActivation = ENABLE,								/*激活过滤器*/					
//	};
//	/*************初始化CANx时钟***************/
//	BSP_CAN_RCC_Init(CANx);
//	
//	/*************初始化CANx引脚***************/
//	
//	/*************引脚复用映射配置***************/
//	//PD0 RX PD1 TX
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1,ENABLE);
//	
//	GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_CAN1);
//	GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_CAN1);
//	
//	//Configure CAN1 TX
//	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;
//	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
//	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
//	GPIO_InitStructure.GPIO_Speed=GPIO_Fast_Speed;
//	
//	GPIO_Init(GPIOD,&GPIO_InitStructure);
//	
//	//Configure CAN1 RX
//	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
//	GPIO_Init(GPIOD,&GPIO_InitStructure);
//	
//	/****************CAN单元设置*****************/
//	CAN_InitStructure.CAN_TTCM=DISABLE;		//非时间触发通信模式   
//	CAN_InitStructure.CAN_ABOM=ENABLE;		//软件自动离线管理	  
//	CAN_InitStructure.CAN_AWUM=DISABLE;		//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
//	CAN_InitStructure.CAN_NART=DISABLE;		//禁止报文自动传送 
//	CAN_InitStructure.CAN_RFLM=DISABLE;		//报文不锁定,新的覆盖旧的  
//	CAN_InitStructure.CAN_TXFP=DISABLE;		//优先级由报文标识符决定 
//	CAN_InitStructure.CAN_Mode= mode;	 		//模式设置 
//	CAN_InitStructure.CAN_SJW=tsjw;				//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1tq~CAN_SJW_4tq
//	CAN_InitStructure.CAN_BS1=tbs1; 			//Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq
//	CAN_InitStructure.CAN_BS2=tbs2;				//Tbs2范围CAN_BS2_1tq ~	CAN_BS2_8tq
//	CAN_InitStructure.CAN_Prescaler=brp;  //分频系数(Fdiv)为brp+1	
//	CAN_Init(CANx, &CAN_InitStructure);   // 初始化CANx 
//	
//		/****************CAN配置过滤器*****************/
//	CAN_FilterInitStruct.CAN_FilterNumber=0;	  //过滤器0
//	CAN_FilterInitStruct.CAN_FilterMode=CAN_FilterMode_IdMask; 
//	CAN_FilterInitStruct.CAN_FilterScale=CAN_FilterScale_32bit; //32位 
//	CAN_FilterInitStruct.CAN_FilterIdHigh=0x0000;////32位ID
//	CAN_FilterInitStruct.CAN_FilterIdLow=0x0000;
//	CAN_FilterInitStruct.CAN_FilterMaskIdHigh=0x0000;//32位MASK
//	CAN_FilterInitStruct.CAN_FilterMaskIdLow=0x0000;
//	CAN_FilterInitStruct.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
//	CAN_FilterInitStruct.CAN_FilterActivation=ENABLE; //激活过滤器0

//	CAN_FilterInit(&CAN_FilterInitStruct);   
//	
//	/****************CAN配置消息中断*****************/
//	CAN_ITConfig(CANx,CAN_IT_FMP0,ENABLE);	//FIFO0消息挂号中断允许
//	NVIC_InitStructure.NVIC_IRQChannel = CAN_TO_NVIC_IRQChannel(CANx);	//CANx接收中断
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PreemptionPriority;		//抢占优先级
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = SubPriority;									//子优先级
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//使能CANx接收中断通道
//	NVIC_Init(&NVIC_InitStructure);	//配置
//}


//CAN2初始化
void CAN2_Mode_Init(uint8_t tsjw,uint8_t tbs2,uint8_t tbs1,uint16_t brp,uint8_t mode)
{
	GPIO_InitTypeDef		GPIO_InitStructure; 
	CAN_InitTypeDef			CAN_InitStructure;
	CAN_FilterInitTypeDef	CAN_FilterInitStructure;
		
	NVIC_InitTypeDef		NVIC_InitStructure;
	
	CAN_DeInit(CAN2);
	CAN_StructInit(&CAN_InitStructure);
	
	//使能相关时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能PORTB时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//使能CAN1时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);//使能CAN2时钟
	
	//初始化GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5| GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化PB5，PB6
	
	//引脚复用映射配置
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_CAN2); //GPIOB5复用为CAN2
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_CAN2); //GPIOB6复用为CAN2
	
	//CAN单元设置
	CAN_InitStructure.CAN_TTCM=DISABLE;	//非时间触发通信模式
	CAN_InitStructure.CAN_ABOM=DISABLE;	//启用软件自动离线管理 ENABLE
	CAN_InitStructure.CAN_AWUM=DISABLE;	//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
	CAN_InitStructure.CAN_NART=ENABLE;	//禁止报文自动传送
	CAN_InitStructure.CAN_RFLM=DISABLE;	//报文不锁定,新的覆盖旧的
	CAN_InitStructure.CAN_TXFP=DISABLE;	//优先级由报文标识符决定
	
	CAN_InitStructure.CAN_Mode= mode;//模式设置
	CAN_InitStructure.CAN_SJW=tsjw;	//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1tq~CAN_SJW_4tq
	CAN_InitStructure.CAN_BS1=tbs1; //Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2=tbs2;//Tbs2范围CAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler=brp;  //分频系数(Fdiv)为brp+1
	
	CAN_Init(CAN2, &CAN_InitStructure);// 初始化CAN2
	
	//配置过滤器
	//使用CAN2得加上这句
	CAN_SlaveStartBank(15);
	CAN_FilterInitStructure.CAN_FilterNumber=15;//过滤器15
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;//32位
	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32位MASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //激活过滤器0
	CAN_FilterInit(&CAN_FilterInitStructure);//滤波器初始化
	

	
	CAN_ITConfig(CAN2,CAN_IT_FMP0,ENABLE);//FIFO0消息挂起中断允许   
	
	
	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;// 主优先级为4
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;// 次优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);

}

/*
***************************************************
函数名：BSP_CAN_Send_Msg
功能：CAN发送一组数据
入口参数：	BSP_CANx：CAN号
					TxMessage：数据指针
返回值：	0：成功;
				1：失败
应用范围：外部调用
备注：
***************************************************
*/
u8 BSP_CAN_Send_Msg(BSP_CAN_TypeDef *BSP_CANx,CanTxMsg *TxMessage){	
  u8 mbox;
  u16 i=0;        
  mbox= CAN_Transmit(BSP_CANx->CANx, TxMessage);   
	
	//等待发送结束
  while(CAN_TransmitStatus(BSP_CANx->CANx, mbox)==CAN_TxStatus_Failed){
		i++;	
		if(i>=0xFFF)
			return 1;
	}
  return 0;		
}

/*
***************************************************
函数名：BSP_CAN_Receive_Msg
功能：CAN发送一组数据
入口参数：	BSP_CANx：CAN号
					RxMessage：接收数据指针
返回值：	0：无数据;
				其它：有数据
应用范围：外部调用
备注：
***************************************************
*/
u8 BSP_CAN_Receive_Msg(BSP_CAN_TypeDef *BSP_CANx,CanRxMsg *RxMessage){		   		   
	if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)	//没有接收到数据,直接退出 
		return 0;		
	CAN_Receive(BSP_CANx->CANx, CAN_FIFO0, RxMessage);//读取数据	
	return RxMessage->DLC;	
}
