#include "driver_ADIS16470.h"
#include "math.h"
#include "util.h"
#include "clockcount.h"
#include "nav_para.h"

BSP_SPI_TypeDef ADIS16470_SPI_Base = ADIS16470_SPI_DEFAULT;
BSP_SPI_TypeDef *ADIS16470_SPI = &ADIS16470_SPI_Base;

adis16470Data_t adis16470Data;

/*--------------------------------------------- 
-----------------------------------------------
//		警告：公共代码区域，故障责任重大				   //
//			除特定区域可以自由可以更改外，		  		 //
//	其余部分更改，请与公共区域负责人取得更改权限  // 
-----------------------------------------------
---------------------------------------------*/

void delay_someTime(float s){
	adis16470Data.time[1] = getClockCount();
	adis16470Data.executionTime = 0.0f;
	while(adis16470Data.executionTime <= s){
		adis16470Data.time[0] = getClockCount();
		adis16470Data.executionTime = (float)(adis16470Data.time[0] - adis16470Data.time[1]);
	}
}

/*
***************************************************
函数名：		driver_ADIS16470_SPI_Write
功能：			SPI方式向ADIS16470的指定寄存器写入一个字节数据
入口参数：	regAddr：ADIS16470的寄存器地址
					dat：要写入的数据
返回值：		无
应用范围：	外部调用
备注：
***************************************************
*/
void driver_ADIS16470_SPI_Write(u8 regAddr,u16 dat){
	u16 temp = 0;
	temp = regAddr++ << 8 | (u8)dat ;
	ADIS16470_CS = 0;
	BSP_SPI_ReadWriteByte(ADIS16470_SPI,temp | 0x8000);	
	ADIS16470_CS = 1;
	delay_someTime(0.00002f);
	temp = regAddr << 8 | (u8)(dat >> 8) ;
	ADIS16470_CS = 0;
	BSP_SPI_ReadWriteByte(ADIS16470_SPI,temp | 0x8000);	
	ADIS16470_CS = 1;
	delay_someTime(0.00002f);
}

/*
***************************************************
函数名：		driver_ADIS16470_SPI_Read
功能：			SPI方式从ADIS16470的指定寄存器读出一个字节数据
入口参数：	regAddr：ADIS16470的寄存器地址
返回值：		dat：读出的数据
应用范围：	外部调用
备注：
***************************************************
*/
u16 driver_ADIS16470_SPI_Read(u8 regAddr){
	u16 temp = 0;
	ADIS16470_CS = 0;
	BSP_SPI_ReadWriteByte(ADIS16470_SPI,regAddr << 8);
	ADIS16470_CS = 1;
	delay_someTime(0.00002f);
	ADIS16470_CS = 0;
	temp = BSP_SPI_ReadWriteByte(ADIS16470_SPI,temp);	
	ADIS16470_CS = 1;
	delay_someTime(0.00002f);
	return temp;
}

u16 dataCNTR_Check(void){
	static u16 last_CNTR;
	u16 value = abs((s16)adis16470Data.dataCNTR.s16_temp - (s16)last_CNTR);
	last_CNTR = adis16470Data.dataCNTR.s16_temp ;
	return value;
}

u16 readADIS16471Data(void){
	u16 tempLow = 0;
	u16 tempHigh = 0;
	/*角速度*/
	tempLow = driver_ADIS16470_SPI_Read(xAxisGyroLowWordReg);
	tempHigh = driver_ADIS16470_SPI_Read(xAxisGyroHighWordReg);
	adis16470Data.gyroscope_h.x.float_temp = (int32_t)(tempLow | tempHigh << 16) * 0.1f / pow(2,16);
	tempLow = driver_ADIS16470_SPI_Read(yAxisGyroLowWordReg);
	tempHigh = driver_ADIS16470_SPI_Read(yAxisGyroHighWordReg);
	adis16470Data.gyroscope_h.y.float_temp = (int32_t)(tempLow | tempHigh << 16) * 0.1f / pow(2,16);
	tempLow = driver_ADIS16470_SPI_Read(zAxisGyroLowWordReg);
	tempHigh = driver_ADIS16470_SPI_Read(zAxisGyroHighWordReg);
	adis16470Data.gyroscope_h.z.float_temp = (int32_t)(tempLow | tempHigh << 16) * 0.1f / pow(2,16);
	
	/*加速度*/
	tempLow = driver_ADIS16470_SPI_Read(xAxisAccLowWordReg);
	tempHigh = driver_ADIS16470_SPI_Read(xAxisAccHighWordReg);
	adis16470Data.acceleration_h.x.float_temp = (int32_t)(tempLow | tempHigh << 16) * 0.00125f / pow(2,16) * GRAVITY;
	tempLow = driver_ADIS16470_SPI_Read(yAxisAccLowWordReg);
	tempHigh = driver_ADIS16470_SPI_Read(yAxisAccHighWordReg);
	adis16470Data.acceleration_h.y.float_temp = (int32_t)(tempLow | tempHigh << 16) * 0.00125f / pow(2,16) * GRAVITY;
	tempLow = driver_ADIS16470_SPI_Read(zAxisAccLowWordReg);
	tempHigh = driver_ADIS16470_SPI_Read(zAxisAccHighWordReg);
	adis16470Data.acceleration_h.z.float_temp = (int32_t)(tempLow | tempHigh << 16) * 0.00125f / pow(2,16) * GRAVITY;
	
	/*温度*/
	tempLow = driver_ADIS16470_SPI_Read(temperatureReg);
	adis16470Data.temperature.s16_temp = (int16_t)tempLow * 0.1f;
	
	/*CNTR*/
	adis16470Data.dataCNTR.u16_temp = driver_ADIS16470_SPI_Read(data_CNTR);
	return dataCNTR_Check();
}

/*
***************************************************
函数名：		brustReadADI16470Data
功能：			突发模式读取数据
入口参数：	无
返回值：		无
应用范围：	外部调用
备注：
***************************************************
*/
u16 brustReadADI16470Data(void){
	u16 temp = 0;
	ADIS16470_CS = 0;
	BSP_SPI_ReadWriteByte(ADIS16470_SPI,BRUSTREAD_CMD);
	adis16470Data.errorFlag.u16_temp = BSP_SPI_ReadWriteByte(ADIS16470_SPI,temp);
	for(u8 index=0;index < 6;index++)
		*(&(adis16470Data.gyroscope.x.s16_temp)+index) = (int16_t)BSP_SPI_ReadWriteByte(ADIS16470_SPI,temp);
	adis16470Data.temperature.s16_temp = (int16_t)BSP_SPI_ReadWriteByte(ADIS16470_SPI,temp);
	adis16470Data.dataCNTR.u16_temp = BSP_SPI_ReadWriteByte(ADIS16470_SPI,temp);
	adis16470Data.checkSum = BSP_SPI_ReadWriteByte(ADIS16470_SPI,temp);
	ADIS16470_CS = 1;
	
	return dataCNTR_Check();
}

/*
***************************************************
函数名：		ADIS16470Check
功能：			检测是否与ADIS16470连接
入口参数：	无
返回值：		0：检测到
					1：未检测到
应用范围：	外部调用
备注：
***************************************************
*/
u8 ADIS16470Check(void){
	u16 data;
	data = driver_ADIS16470_SPI_Read(PROD_IDReg);
	if( data == 0x4056 )
		return 1;
	else
		return 0;
}

/*
***************************************************
函数名：		driver_ADIS16470_Init
功能：			ADIS16470初始化配置
入口参数：	无
返回值：		0：初始化失败
					1：初始化成功
应用范围：	外部调用
备注：
***************************************************
*/

void driver_ADIS16470_Hardware_Init(void){
	BSP_SPI_Init(ADIS16470_SPI);
}

bool driver_ADIS16470_Init(void){
	u16 waitCheckTime = 0;
	u8 ask = 0;
	while(!ask && waitCheckTime < 5000){
		ask = ADIS16470Check();
		waitCheckTime++;
	};
	if(!ask)
		return false;
	driver_ADIS16470_SPI_Write(0x68,0x0080);							//软件复位	
	return true;																					//初始化成功
}
