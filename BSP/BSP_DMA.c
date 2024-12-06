//#include "BSP_DMA.h"
//#include "BSP_DMA_Define.h"

///*
//***************************************************
//函数名：BSP_DMA_Init
//功能：DMAx的各通道配置
//入口参数：	*BSP_DMA_Streamx_chx：DMA数据流通道结构体指针
//					par：	外设地址
//					mar：	存储器地址
//					ndtr：数据传输量 
//返回值：无
//应用范围：外部调用
//备注：
//***************************************************
//*/
//void BSP_DMA_Init(BSP_DMA_TypeDef *BSP_DMA_Streamx_chx,uint32_t par,uint32_t mar,uint16_t ndtr){
//	DMA_InitTypeDef  DMA_InitStructure;
//	//得到当前stream是属于DMA2还是DMA1
//	if((uint32_t)BSP_DMA_Streamx_chx->DMA_Streamx > (uint32_t)DMA2)
//		//DMA2时钟使能
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE); 	
//	else
//		//DMA1时钟使能
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE); 
//	DMA_DeInit(BSP_DMA_Streamx_chx->DMA_Streamx);
//	//等待DMA可配置
//	while (DMA_GetCmdStatus(BSP_DMA_Streamx_chx->DMA_Streamx) != DISABLE);
//	/* 配置 DMA Stream */
//	//通道选择
//	DMA_InitStructure.DMA_Channel = BSP_DMA_Streamx_chx->DMA_channel;  
//	//DMA外设地址
//	DMA_InitStructure.DMA_PeripheralBaseAddr = par;
//	//DMA 存储器0地址
//	DMA_InitStructure.DMA_Memory0BaseAddr = mar;
//	//存储器到外设模式
//	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
//	//数据传输量
//	DMA_InitStructure.DMA_BufferSize = ndtr; 
//	//外设非增量模式
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	//存储器增量模式
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	//外设数据长度:8位
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
//	//存储器数据长度:8位
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;
//	//使用普通模式
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;/*循环模式*/
//	//高等优先级
//	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
//	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
//	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
//	//存储器突发单次传输
//	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
//	//外设突发单次传输
//	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
//	//初始化DMA Streams
//	DMA_Init(BSP_DMA_Streamx_chx->DMA_Streamx, &DMA_InitStructure);
//	DMA_Cmd(BSP_DMA_Streamx_chx->DMA_Streamx,ENABLE);//开启DMA传输

//}

//void TestAdcDMA(BSP_DMA_TypeDef *BSP_DMA_Streamx_chx,uint32_t par,uint32_t mar,uint16_t ndtr)
//{
//	DMA_InitTypeDef DMA_InitStructure;
//	//得到当前stream是属于DMA2还是DMA1
//	if((uint32_t)BSP_DMA_Streamx_chx->DMA_Streamx > (uint32_t)DMA2)
//		//DMA2时钟使能
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE); 	
//	else
//		//DMA1时钟使能
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE); 
//	DMA_DeInit(BSP_DMA_Streamx_chx->DMA_Streamx);
//	
//	
//	
//	
//	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);/*DMA2的时钟使能*/
//	while(DMA_GetCmdStatus(BSP_DMA_Streamx_chx->DMA_Streamx) != DISABLE);/*等待DMA可以配置*/


//	//通道选择
//	DMA_InitStructure.DMA_Channel = BSP_DMA_Streamx_chx->DMA_channel;  
//	
//	
//	//DMA外设地址
//	DMA_InitStructure.DMA_PeripheralBaseAddr = par;
//	//DMA 存储器0地址
//	DMA_InitStructure.DMA_Memory0BaseAddr = mar;
//	//数据传输量
//	DMA_InitStructure.DMA_BufferSize = ndtr; 
//	
//	//DMA_InitStructure.DMA_BufferSize = 1;/*数据传输的数量为1*/
//	
//	//DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t)&AdcValue;/*存取器地址*/
//	
//	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;/*方向从外设到内存*/
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;/*地址不增加*/
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;/*地址不增加*/
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;/*数据长度半字*/
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;/*数据长度半字*/
//	DMA_InitStructure.DMA_Priority = DMA_Priority_High;/*高优先级*/
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;/*循环模式*/
//	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;/*禁止FIFO*/
//	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;/*FIFO的值*/
//	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;/*单次传输*/
//	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;/*单次传输*/
//	DMA_Init(BSP_DMA_Streamx_chx->DMA_Streamx,&DMA_InitStructure);/**/
//	DMA_Cmd(BSP_DMA_Streamx_chx->DMA_Streamx,ENABLE);//开启DMA传输
//	
//	
//	//BSP_DMA_Init(&BSP_DMA_ADC1,(uint32_t)ADC1_BASE+0x4C,(uint32_t)&AdcValue,1);
//	
//}





















///*
//***************************************************
//函数名：BSP_DMA_DSHOT_Init
//功能：使用DSHOT的DMAx的各通道配置
//入口参数：	*BSP_DMA_Streamx_chx：DMA数据流通道结构体指针
//					par：	外设地址
//					mar：	存储器地址
//					ndtr：数据传输量 
//返回值：无
//应用范围：外部调用
//备注：
//***************************************************
//*/
//void BSP_DMA_DSHOT_Init(BSP_DMA_TypeDef *BSP_DMA_Streamx_chx,uint32_t par,uint32_t mar,uint16_t ndtr){
//	DMA_InitTypeDef  DMA_InitStructure;
//	//得到当前stream是属于DMA2还是DMA1
//	if((uint32_t)BSP_DMA_Streamx_chx->DMA_Streamx > (uint32_t)DMA2)
//		//DMA2时钟使能
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE); 
//	else
//		//DMA1时钟使能 
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);
//	DMA_DeInit(BSP_DMA_Streamx_chx->DMA_Streamx);
//	//等待DMA可配置 
//	while (DMA_GetCmdStatus(BSP_DMA_Streamx_chx->DMA_Streamx) != DISABLE);
//	 /* 配置 DMA Stream */
//	//通道选择
//	DMA_InitStructure.DMA_Channel = BSP_DMA_Streamx_chx->DMA_channel;  
//	//DMA外设地址
//	DMA_InitStructure.DMA_PeripheralBaseAddr = par;
//	//DMA 存储器0地址
//	DMA_InitStructure.DMA_Memory0BaseAddr = mar;
//	//存储器到外设模式
//	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
//	//数据传输量
//	DMA_InitStructure.DMA_BufferSize = ndtr; 
//	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable; 
//	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
//	//外设非增量模式
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	//存储器增量模式
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	//外设数据长度:16位
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;		
//	//存储器数据长度:16位
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;					
//	//使用普通模式
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; 
//	//高优先级
//	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
//	//存储器突发单次传输
//	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
//	//外设突发单次传输
//	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
//	//初始化DMA Stream
//	DMA_Init(BSP_DMA_Streamx_chx->DMA_Streamx, &DMA_InitStructure);
//	//使能DMA中断，数据传输完毕触发	
//	DMA_ITConfig(BSP_DMA_Streamx_chx->DMA_Streamx, DMA_IT_TC, ENABLE);
//}
///*
//***************************************************
//函数名：BSP_DMA_SK6812_Init
//功能：使用SK6812灯条的DMAx的各通道配置
//入口参数：	*BSP_DMA_Streamx_chx：DMA数据流通道结构体指针
//					par：	外设地址
//					mar：	存储器地址
//					ndtr：数据传输量 
//返回值：无
//应用范围：外部调用
//备注：
//***************************************************
//*/
//void BSP_DMA_SK6812_Init(BSP_DMA_TypeDef *BSP_DMA_Streamx_chx,uint32_t par,uint32_t mar,uint16_t ndtr){
//  DMA_InitTypeDef  DMA_InitStructure;
//	//得到当前stream是属于DMA2还是DMA1
//	if((uint32_t)BSP_DMA_Streamx_chx->DMA_Streamx > (uint32_t)DMA2)
//		//DMA2时钟使能
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);
//	else
//		//DMA1时钟使能 
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);
//	DMA_DeInit(BSP_DMA_Streamx_chx->DMA_Streamx);
//	//等待DMA可配置 
//	while (DMA_GetCmdStatus(BSP_DMA_Streamx_chx->DMA_Streamx) != DISABLE);
//	 /* 配置 DMA Stream */
//	//通道选择
//	DMA_InitStructure.DMA_Channel = BSP_DMA_Streamx_chx->DMA_channel;  
//	//DMA外设地址
//	DMA_InitStructure.DMA_PeripheralBaseAddr = par;
//	//DMA 存储器0地址
//	DMA_InitStructure.DMA_Memory0BaseAddr = mar;
//	//存储器到外设模式
//	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
//	//数据传输量
//	DMA_InitStructure.DMA_BufferSize = ndtr; 
//	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable; 
//	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
//	//外设非增量模式
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	//存储器增量模式
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	//外设数据长度:32位
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;		
//	//存储器数据长度:32位
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;					
//	//使用普通模式 
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
//	//高优先级
//	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
//	//存储器突发单次传输
//	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
//	//外设突发单次传输
//	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
//	//初始化DMA Stream
//	DMA_Init(BSP_DMA_Streamx_chx->DMA_Streamx, &DMA_InitStructure);
//	//使能DMA中断，数据传输完毕触发
//	DMA_ITConfig(BSP_DMA_Streamx_chx->DMA_Streamx, DMA_IT_TC, ENABLE);	
//}
///*
//***************************************************
//函数名：BSP_DMA_TX_Init
//功能：使用串口发送的DMAx的各通道配置
//入口参数：	*BSP_DMA_Streamx_chx：DMA数据流通道结构体指针
//			*USARTx：串口号
//			par：	外设地址
//			ndtr：数据传输量 
//返回值：无
//应用范围：外部调用
//备注：
//***************************************************
//*/
//void BSP_DMA_TX_Init(BSP_DMA_TypeDef *BSP_DMA_Streamx_chx,USART_TypeDef	*USARTx,uint32_t par,uint16_t ndtr){
//	DMA_InitTypeDef  DMA_InitStructure;
//	//得到当前stream是属于DMA2还是DMA1
//	if((uint32_t)BSP_DMA_Streamx_chx->DMA_Streamx > (uint32_t)DMA2)
//		//DMA2时钟使能
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE); 
//	else
//		//DMA1时钟使能
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE); 
//	DMA_DeInit(BSP_DMA_Streamx_chx->DMA_Streamx);
//	//等待DMA可配置
//	while (DMA_GetCmdStatus(BSP_DMA_Streamx_chx->DMA_Streamx) != DISABLE);
//	/* 配置 DMA Stream */
//	//通道选择
//	DMA_InitStructure.DMA_Channel = BSP_DMA_Streamx_chx->DMA_channel;  
//	//DMA外设地址
//	DMA_InitStructure.DMA_PeripheralBaseAddr = par;
//	//DMA 存储器0地址
//	DMA_InitStructure.DMA_Memory0BaseAddr = marArrayTX(USARTx,ndtr);
//	//存储器到外设模式
//	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
//	//数据传输量
//	DMA_InitStructure.DMA_BufferSize = 0; 
//	//外设非增量模式
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	//存储器增量模式
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	//外设数据长度:8位
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//	//存储器数据长度:8位
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//	//使用普通模式
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; 
//	//中等优先级
//	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
//	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
//	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
//	//存储器突发单次传输
//	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
//	//外设突发单次传输
//	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
//	//初始化DMA Stream
//	DMA_Init(BSP_DMA_Streamx_chx->DMA_Streamx, &DMA_InitStructure);
//}
///*
//***************************************************
//函数名：BSP_DMA_RX_Init
//功能：使用串口接收的DMAx的各通道配置
//入口参数：	*BSP_DMA_Streamx_chx：DMA数据流通道结构体指针
//			*USARTx：串口号
//			par：	外设地址
//			ndtr：数据传输量 
//返回值：无
//应用范围：外部调用
//备注：
//***************************************************
//*/
//void BSP_DMA_RX_Init(BSP_DMA_TypeDef *BSP_DMA_Streamx_chx,USART_TypeDef	*USARTx,uint32_t par,uint16_t ndtr){
//	DMA_InitTypeDef  DMA_InitStructure;
//	//得到当前stream是属于DMA2还是DMA1
//	if((uint32_t)BSP_DMA_Streamx_chx->DMA_Streamx > (uint32_t)DMA2)
//		//DMA2时钟使能
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE); 
//	else
//		//DMA1时钟使能
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE); 
//	DMA_DeInit(BSP_DMA_Streamx_chx->DMA_Streamx);
//	//等待DMA可配置 
//	while (DMA_GetCmdStatus(BSP_DMA_Streamx_chx->DMA_Streamx) != DISABLE);
//	/* 配置 DMA Stream */
//	//通道选择
//	DMA_InitStructure.DMA_Channel = BSP_DMA_Streamx_chx->DMA_channel;  
//	//DMA外设地址
//	DMA_InitStructure.DMA_PeripheralBaseAddr = par;
//	//DMA 存储器0地址
//	DMA_InitStructure.DMA_Memory0BaseAddr = marArrayRX(USARTx,ndtr);
//	//外设到存储器模式
//	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
//	//数据传输量
//	DMA_InitStructure.DMA_BufferSize = ndtr; 
//	//外设非增量模式
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	//存储器增量模式
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	//外设数据长度:8位
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//	//存储器数据长度:8位
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//	//使用普通模式 
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
//	//中等优先级
//	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
//	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
//	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
//	//存储器突发单次传输
//	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
//	//外设突发单次传输
//	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
//	//初始化DMA Stream
//	DMA_Init(BSP_DMA_Streamx_chx->DMA_Streamx, &DMA_InitStructure);
//}

///*
//DAM1请求映射：
//-----------------------------------------------------------------------------------------------------------------------------------------
//外设请求	|	 DMA1_Stream0	|	 DMA1_Stream1	|	 DMA1_Stream2	|	 DMA1_Stream3	|	 DMA1_Stream4	|	 DMA1_Stream5	|	 DMA1_Stream6	|	 DMA1_Stream7	|
//-----------------------------------------------------------------------------------------------------------------------------------------
// 通道0	|    SPI3_RX    |								|			SPI3_RX		|			SPI2_RX		|			SPI2_TX		|			SPI3_TX		|								|		 SPI3_TX		|
//-----------------------------------------------------------------------------------------------------------------------------------------
// 通道1	|		 I2C1_RX		|								|			TIM7_UP		|								|			TIM7_UP		|    I2C1_RX    |    I2C1_TX    |    I2C1_TX    |
//-----------------------------------------------------------------------------------------------------------------------------------------
// 通道2	|		 TIM4_CH1	 	|								|  I2S3_EXT_RX  |		 TIM4_CH2	 	|  I2S2_EXT_TX  |  I2S3_EXT_TX  |    TIM4_UP    |    TIM4_CH3   |
//-----------------------------------------------------------------------------------------------------------------------------------------
// 通道3	|  I2S3_EXT_RX  |	 TIM2_CH3/UP	|		 I2C3_RX		|  I2S2_EXT_RX  |		 I2C3_RX		|    TIM2_CH1   |  TIM2_CH2/CH4 |  TIM2_CH4/UP  |
//-----------------------------------------------------------------------------------------------------------------------------------------
// 通道4	|		 UART5_RX		|   USART3_RX   |		 UART4_RX		|   USART3_TX   |		 UART4_TX		|   USART2_RX   |   USART2_TX   |    UART5_TX   |
//-----------------------------------------------------------------------------------------------------------------------------------------
// 通道5	|		 UART8_TX		|    UART7_TX   |	 TIM3_CH4/UP	|    UART7_RX   | TIM3_CH1/TRIG |    TIM3_CH2   |    UART8_RX   |    TIM3_CH3   |
//-----------------------------------------------------------------------------------------------------------------------------------------
// 通道6	|	 TIM5_CH3/UP	|	TIM5_CH4/TRIG	|		 TIM5_CH1	 	|	TIM5_CH4/TRIG	|		 TIM5_CH2	 	|								|    TIM5_UP    |								|
//-----------------------------------------------------------------------------------------------------------------------------------------
// 通道7	|								|    TIM6_UP    |		 I2C2_RX		|		 I2C2_RX		|   USART3_TX   |      DAC1     |      DAC2     |		 I2C2_TX		|
//-----------------------------------------------------------------------------------------------------------------------------------------

//DMA2请求映射：
//-----------------------------------------------------------------------------------------------------------------------------------------
//外设请求	|	 DMA2_Stream0	|	 DMA2_Stream1	|	 DMA2_Stream2	|	 DMA2_Stream3	|	 DMA2_Stream4	|	 DMA2_Stream5	|	 DMA2_Stream6	|	 DMA2_Stream7	|
//-----------------------------------------------------------------------------------------------------------------------------------------
// 通道0	|     ADC1      |								| TIM8_CH1/2/3  |								|     ADC1      |								| TIM1_CH1/2/3  |								|
//-----------------------------------------------------------------------------------------------------------------------------------------
// 通道1	|								|     DCMI      |      ADC2     |      ADC2     |								|    SPI6_TX    |    SPI6_RX    |      DCMI     |
//-----------------------------------------------------------------------------------------------------------------------------------------
// 通道2	|		  ADC3  	 	|		  ADC3  	 	|								|		 SPI5_RX	 	|    SPI5_TX    |    CRYP_OUT   |    CRYP_IN    |     HASH_IN   |
//-----------------------------------------------------------------------------------------------------------------------------------------
// 通道3	|    SPI1_RX    |								|		 SPI1_RX		|    SPI1_TX    |								|    SPI1_TX    |								|								|
//-----------------------------------------------------------------------------------------------------------------------------------------
// 通道4	|		 SPI4_RX 		|    SPI4_TX    |	  USART1_RX 	|      SDIO     |								|   USART1_RX   |      SDIO     |   USART1_TX   |
//-----------------------------------------------------------------------------------------------------------------------------------------
// 通道5	|								|   USART6_RX   |	  USART6_RX 	|    SPI4_RX    |    SPI4_TX    |								|   USART6_TX   |   USART6_TX   |
//-----------------------------------------------------------------------------------------------------------------------------------------
// 通道6	|	  TIM1_TRIG 	|    TIM1_CH1   |		 TIM1_CH2	 	|	   TIM1_CH1   |TIM1C4/TRIG/COM|    TIM1_UP    |    TIM1_CH3   |								|
//-----------------------------------------------------------------------------------------------------------------------------------------
// 通道7	|								|    TIM8_UP    |		 TIM8_CH1  	|		 TIM8_CH2		|		 TIM8_CH3		|    SPI5_RX    |    SPI5_TX    |TIM8C4/TRIG/COM|
//-----------------------------------------------------------------------------------------------------------------------------------------

//*/
