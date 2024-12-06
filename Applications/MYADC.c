#include "MyADC.h"           
#include "app.h" 
uint16_t MyADC_Buff[2];
u16 garry_ch0[10] = {0};
uint16_t* ADC_Value;
u16 gGetAdcCounter = 0;

/**
  * @摘要  		初始化ADC和相应的GPIO
  * @参数  		无
  * @返回值  	无
  * @说明  		无
  */
void MyADC_Init(void)
{
	//开启GPIOA的时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	//定义GPIO初始化结构体
	GPIO_InitTypeDef GPIO_InitStructure;
	//初始化上述结构体
	GPIO_StructInit(&GPIO_InitStructure);
	//配置GPIO模式为模拟输入模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	//配置GPIO输出类型为推挽输出（此处无用）
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//指定GPIO引脚为Pin4hePin5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	//配置GPIO输入类型为浮空输入（此处无用）
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	//配置GPIO的速度为快速50MHZ
	GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;
	//初始化对应的GPIO
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	//开启ADC1的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	//定义所有ADC共用配置的初始化结构体
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	//初始化上述结构体
	ADC_CommonStructInit(&ADC_CommonInitStructure);
	//配置为独立模式
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	//配置ADC时钟的预分频器的值为4，即21MHZ（<30MHZ）
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
	//失能DMA（这是多ADC模式下的DMA，不是这里要用的DMA）
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	//配置多ADC模式下的两个ADC采样间隔（此处无用）
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_8Cycles;
	//初始化所有ADC共用配置
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	//定义ADC初始化结构体
	ADC_InitTypeDef ADC_InitStructure;
	//初始化上述结构体
	ADC_StructInit(&ADC_InitStructure);
	//配置ADC分辨率为12位
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	//配置ADC为单次转换模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	//配置ADC为扫描模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	//配置ADC数据对齐方式为右对齐
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	//配置ADC外部触发为无，即关闭外部触发
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	//配置ADC外部触发源为TIM2的TRGO输出（此处无用）
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_TRGO;
	//配置ADC转换通道数为2
	ADC_InitStructure.ADC_NbrOfConversion = 2;
	//初始化对应的ADC
	ADC_Init(ADC1, &ADC_InitStructure);
	
	//配置每一个规则通道的排序和采样周期
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_56Cycles);
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 2, ADC_SampleTime_56Cycles);
	
	//开启ADC1的DMA通道
	ADC_DMACmd(ADC1, ENABLE);
	//允许ADC一次传输后发出新的DMA请求
	ADC_DMARequestAfterLastTransferCmd(ADC1,ENABLE);
	
	//初始化DMA
	MyDMA_Init((uint32_t)&(ADC1->DR), (uint32_t)MyADC_Buff, 2);
	
	
	//开启ADC1，此时ADC1进入等待触发的状态
	ADC_Cmd(ADC1, ENABLE);
}
 
/**
  * @摘要  		软件触发ADC并读取采样值
  * @参数  		无
  * @返回值  	指向ADC采样值的指针
  * @说明  		无
  */
u16* MyADC_GetValue(void)
{
	//软件触发ADC1，此时ADC1进入采样转换状态
	ADC_SoftwareStartConv(ADC1);
	
	//ADC采样完成后会硬件自动触发DMA进行数据转运
	
	//等待DMA转运完成，DMA转运完成则ADC转换一定完成了
	while(DMA_GetFlagStatus(DMA2_Stream0, DMA_FLAG_TCIF0) == RESET);
	//清除DMA转运完成标志
	DMA_ClearFlag(DMA2_Stream0, DMA_FLAG_TCIF0);
	//重新开启DMA，此时DMA等待硬件触发
	DMA_Cmd(DMA2_Stream0, ENABLE);
	
	//返回ADC1的采样值
	return MyADC_Buff;
}

 
/**
  * @摘要  		初始化DMA
  * @参数  		SAddress: 源地址
  * @参数  		DAddress: 目的地址
  * @参数  		Size: 要传输的数据个数
  * @返回值  	无
  * @说明  		无
  */
void MyDMA_Init(uint32_t SAddress, uint32_t DAddress,uint16_t Size)
{
	//开启DMA2的时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	
	//定义DMA初始化结构体
	DMA_InitTypeDef DMA_InitStructure;
	//初始化上述结构体
	DMA_StructInit(&DMA_InitStructure);
	//配置DMA通道为通道0
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	//配置DMA传输的存储器地址（即目的地址）
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)DAddress;
	//配置存储器接收数据的单位为半字
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	//配置存储器地址指针为自增模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	//配置DMA传输的外设（即源地址）
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SAddress;
	//配置外设发送数据的单位为半字
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	//配置外设地址指针为非自增模式
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	//配置存储器突发传输配置为单次传输
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	//配置外设突发传输配置为单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	//配置DMA传输方向为外设到存储器
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	//配置DMA要传输的数据个数
	DMA_InitStructure.DMA_BufferSize = Size;
	//配置DMA为非循环模式
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	//配置DMA为非直接模式
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
	//配置FIFO阈值为全满
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
	//配置该通道的DMA优先级为中
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	
	//等待DMA失能，以确保初始化可以成功
	while(DMA_GetCmdStatus(DMA2_Stream0) == ENABLE);
	//初始化DMA
	DMA_Init(DMA2_Stream0, &DMA_InitStructure);
	
	//开启DMA，此时DMA等待硬件触发
	DMA_Cmd(DMA2_Stream0, ENABLE);
}
void GetAdcAverage(void)
{
	u32 sum1 = 0;
//	u32 sum2 = 0;
	u8 i = 0;
	for(i = 0; i < 10; i++)
	{
		sum1 += garry_ch0[i];
		//sum2 += garry_ch1[i];

	}
	gCarBatPower = sum1 / 10; //电源电压 
}

