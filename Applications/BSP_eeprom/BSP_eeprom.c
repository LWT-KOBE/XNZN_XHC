#include "BSP_eeprom.h"
#include <stdlib.h>
#include <string.h>
#include "application.h"
#include "stm32f4xx.h"
void EEPROM_GPIO_Init(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APBxPeriph_EEPROM_IO, ENABLE);

	GPIO_InitStructure.GPIO_Pin = EEPROM_SCL_PIN;			//SCL
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//上拉
	GPIO_Init(EEPROM_IO, &GPIO_InitStructure);//初始化GPIO		

	GPIO_InitStructure.GPIO_Pin = EEPROM_SDA_PIN;			//SDA	OD
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//下拉
	GPIO_Init(EEPROM_IO, &GPIO_InitStructure);//初始化GPIO			
	
	
	
//写保护	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				//WP PA8
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO		
	WPS = 1;//24C02写保护

//	GPIO_InitStructure.GPIO_Pin = EEPROM_WP_PIN;			//WP 
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(EEPROM_IO, &GPIO_InitStructure);
//	GPIO_SetBits(EEPROM_IO, EEPROM_WP_PIN); //写保护
	
}


void EEPROM_delay(void)
{
	delay_us(100);
}

bool EEPROM_Start(void)
{
	SET_EEPROM_SDA;
	SET_EEPROM_SCL;
	EEPROM_delay();
	if(!SDA_read)return false; //SDA线为低电平则总线忙,退出
	RESET_EEPROM_SDA;
	EEPROM_delay();
	if(SDA_read) return false; //SDA线为高电平则总线出错,退出
	RESET_EEPROM_SDA;
	EEPROM_delay();
	return true;
}

void EEPROM_Stop(void)
{
	RESET_EEPROM_SCL;
	EEPROM_delay();
	RESET_EEPROM_SDA;
	EEPROM_delay();
	SET_EEPROM_SCL;
	EEPROM_delay();
	SET_EEPROM_SDA;
	EEPROM_delay();
}

void EEPROM_Ack(void)
{
	RESET_EEPROM_SCL;
	EEPROM_delay();
	RESET_EEPROM_SDA;
	EEPROM_delay();
	SET_EEPROM_SCL;
	EEPROM_delay();
	RESET_EEPROM_SCL;
	EEPROM_delay();
}

void EEPROM_NoAck(void)
{
	RESET_EEPROM_SCL;
	EEPROM_delay();
	SET_EEPROM_SDA;
	EEPROM_delay();
	SET_EEPROM_SCL;
	EEPROM_delay();
	RESET_EEPROM_SCL;
	EEPROM_delay();
}

bool EEPROM_WaitAck(void)   //返回为:=1有ACK,=0无ACK
{
	RESET_EEPROM_SCL;
	EEPROM_delay();
	SET_EEPROM_SDA;
	EEPROM_delay();
	SET_EEPROM_SCL;
	EEPROM_delay();
	if(SDA_read)
	{
		RESET_EEPROM_SCL;
		return false;
		//      return TRUE;
	}
	RESET_EEPROM_SCL;
	return true;
}

void EEPROM_SendByte(u8 SendByte) //数据从高位到低位//
{
	u8 i = 8;
	while(i--)
	{
		RESET_EEPROM_SCL;
		EEPROM_delay();
		
		if(SendByte & 0x80)
			SET_EEPROM_SDA;
		else
			RESET_EEPROM_SDA;
		SendByte <<= 1;
		EEPROM_delay();
		SET_EEPROM_SCL;
		EEPROM_delay();
	}
	RESET_EEPROM_SCL;
}

u8 EEPROM_ReceiveByte(void)  //数据从高位到低位//
{
	u8 i = 8;
	u8 ReceiveByte = 0;

	SET_EEPROM_SDA;
	while(i--)
	{
		ReceiveByte <<= 1;
		RESET_EEPROM_SCL;
		EEPROM_delay();
		SET_EEPROM_SCL;
		EEPROM_delay();
		if(SDA_read)
		{
			ReceiveByte |= 0x01;
		}
	}
	RESET_EEPROM_SCL;
	return ReceiveByte;
}

bool EEPROM_Write_Byte(u8 WriteAddr, u8 WriteData)
{
//	GPIO_ResetBits(EEPROM_IO, EEPROM_WP_PIN);
	if (!EEPROM_Start()) return false;
	EEPROM_SendByte(EEPROM_SLAVE_ADDRESS7);//设置器件地址
	if (!EEPROM_WaitAck())
	{
		EEPROM_Stop();
		return false;
	}
	EEPROM_SendByte(WriteAddr);   //设置段内地址
	EEPROM_WaitAck();

	EEPROM_SendByte(WriteData);
	EEPROM_WaitAck();
	EEPROM_Stop();
	//注意：因为这里要等待EEPROM写完，可以采用查询或延时方式(10ms)
	//Systick_Delay_1ms(10);
//	delay_ms(10);
//	GPIO_SetBits(EEPROM_IO, EEPROM_WP_PIN);
	return true;
}



bool EEPROM_Write2(u8 WriteAddr, u8 WriteData)
{
	if (!EEPROM_Start()) return false;
	EEPROM_SendByte(0x10);//设置器件地址+段地址
	if (!EEPROM_WaitAck())
	{
		EEPROM_Stop();
		return false;
	}
	EEPROM_SendByte(WriteAddr);   //设置段内地址
	EEPROM_WaitAck();

	EEPROM_SendByte(WriteData);
	EEPROM_WaitAck();
	EEPROM_Stop();
	//注意：因为这里要等待EEPROM写完，可以采用查询或延时方式(10ms)
	//Systick_Delay_1ms(10);
	return true;
}

void EepromWriteFloat(uint8_t addr_, float dat_)
{

	uint8_t buf[4] = {0, 0, 0, 0};
	uint8_t i = 0;

	memcpy(&buf[0], &dat_, 4);
	for(i = 0; i < 4; i++)
	{
		EEPROM_Write_Byte((addr_ + i), buf[i]); //写入EEPROM第1个字节
		delay_ms(2);
	}

}

//读出1串数据
u8 EEPROM_Read_Byte(u8 WriteAddr)
{
	u8 tempDat = 0;
	if (!EEPROM_Start()) return false;

	EEPROM_SendByte(EEPROM_SLAVE_ADDRESS7);//设置器件地址+段地址

	if (!EEPROM_WaitAck())
	{
		EEPROM_Stop();
		return false;
	}

	EEPROM_SendByte(WriteAddr);   //设置低起始地址
	EEPROM_WaitAck();
	EEPROM_Start();
	EEPROM_SendByte(EEPROM_SLAVE_ADDRESS7 | 0x01);
	EEPROM_WaitAck();
	tempDat = EEPROM_ReceiveByte();
	EEPROM_NoAck();

	EEPROM_Stop();
	return tempDat;
}

u16 EEPROM_Read_u16(u8 WriteAddr)
{
	return ((EEPROM_Read_Byte(WriteAddr) << 8) + (EEPROM_Read_Byte(WriteAddr + 1) & 0xff));
}

//读出1串数据
u8 EEPROM_Read2(u8 WriteAddr)
{
	u8 tempDat = 0;


	if (!EEPROM_Start()) return false;

	EEPROM_SendByte(0x10);//设置器件地址+段地址

	if (!EEPROM_WaitAck())
	{
		EEPROM_Stop();
		return false;
	}

	EEPROM_SendByte(WriteAddr);   //设置低起始地址
	EEPROM_WaitAck();
	EEPROM_Start();
	EEPROM_SendByte(0x10 | 0x01);
	EEPROM_WaitAck();
	tempDat = EEPROM_ReceiveByte();
	EEPROM_NoAck();

	EEPROM_Stop();

	return tempDat;
}
///////////////////////////////////////////////

bool EEPROM_Write_u16(u8 WriteAddr, u16 WriteData)
{
	EEPROM_Write_Byte(WriteAddr + 1, WriteData & 0xff);
	delay_ms(5);
	EEPROM_Write_Byte(WriteAddr, WriteData >> 8);
	return true;
}

float EepromReadFloat(u8 WriteAddr)
{
	u8 datbuf[4];
	u8 i = 0;
	u8 len = 4;
	float data_ = 0;
	memset(&datbuf[0], 0, 4);//数组清空
	for(i = 0; i < len; i++)
	{
		datbuf[i] = EEPROM_Read_Byte(WriteAddr + i);
		delay_ms(2);
	}
	memcpy(&data_, &datbuf[0], 4);
	return data_;
}


