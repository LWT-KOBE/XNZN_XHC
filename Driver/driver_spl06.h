#ifndef SPL06_01_H
#define SPL06_01_H

#include "BSP.h"
#include "std_lib.h"
#include "util.h"
#include "d_imu.h"


//寄存器定义
#define PRESSURE_REG 0X00
#define TEMP_REG 0X03
#define PRS_CFG_REG 0x06 //气压测量速率配置
#define TMP_CFG_REG 0x07 //温度测量速度配置
#define MEAS_CFG_REG 0x08 //测量配置与传感器配置
#define CFG_REG 0x09 //中断/FIFO/SPI线数等配置
#define INT_STS_REG 0X0A //中断状态标志位
#define FIFO_STS_REG 0X0B //FIFO状态
#define RESET_REG 0X0C
#define ID_REG 0x0D
#define COEF_REG 0x10


#define PRESSURE_RATE_1_TIMES 0 //采样率 
#define PRESSURE_RATE_2_TIMES 1
#define PRESSURE_RATE_4_TIMES 2
#define PRESSURE_RATE_8_TIMES 3
#define PRESSURE_RATE_16_TIMES 4
#define PRESSURE_RATE_32_TIMES 5
#define PRESSURE_RATE_64_TIMES 6
#define PRESSURE_RATE_128_TIMES 7

#define TEMPERATURE_RATE_1_TIMES 0 //采样率
#define TEMPERATURE_RATE_2_TIMES 1
#define TEMPERATURE_RATE_4_TIMES 2
#define TEMPERATURE_RATE_8_TIMES 3
#define TEMPERATURE_RATE_16_TIMES 4
#define TEMPERATURE_RATE_32_TIMES 5
#define TEMPERATURE_RATE_64_TIMES 6
#define TEMPERATURE_RATE_128_TIMES 7
#define TEMPERATURE_RATE_TMP_EXT_INTERNAL 0  //集成电路上的温度计
#define TEMPERATURE_RATE_TMP_EXT_EXTERNAL 1  //传感器MEMS气压芯片上温度计


#define MEAS_CFG_COEF_RDY 0X80 // 传感器内部校准值可读，在启动后
#define MEAS_CFG_SENSOR_RDY 0X40 // 传感器已初始化完成，在启动后
#define MEAS_CFG_TMP_RDY 0x20 //温度值已经准备就绪，可以进行读取，该标志位读取后自动清0
#define MEAS_CFG_PRS_RDY 0x10 //气压值已经准备就绪，可以进行读取，该标志位
#define MEAS_CFG_MEAS_CTR_STANDBY 0 //模式配置 挂起模式
#define MEAS_CFG_MEAS_CTR_COMMAND_PRS 0x01 //模式配置 命令模式下启动气压采集
#define MEAS_CFG_MEAS_CTR_COMMAND_TMP 0x02 //模式配置 命令模式下启动温度采集
#define MEAS_CFG_MEAS_CTR_BACKGROUND_PRS 0x05 //模式配置 后台模式只读取气压值
#define MEAS_CFG_MEAS_CTR_BACKGROUND_TMP 0X06 //模式配置 后台模式只读取温度值
#define MEAS_CFG_MEAS_CTR_BACKGROUND_PSR_TMP 0X07 //模式配置 后台模式同时读取温度值和气压值

#define CFG_INT_LEVEL_ACTIVE_LOW 0//中断低电平有效
#define CFG_INT_LEVEL_ACTIVE_HIGH 1//中断高电平有效
#define CFG_INT_FIFO 0X40    //当FIFO满使能中断 
#define CFG_INT_PRS 0X20    //当气压计读取完毕使能中断 
#define CFG_INT_TMP 0X10    //当温度读取完毕使能中断 
#define CFG_T_SHIFT 0X08    //允许数据被覆盖，可以进行下一笔采集
#define CFG_P_SHIFT 0X04    //允许数据被覆盖，可以进行下一笔采集
#define CFG_FIF 0X02    //使能FIFO
#define CFG_SPI_3_WIRE 1    //3线SPI
#define CFG_SPI_4_WIRE 0    //4线SPI

#define INT_STS_FIFO_FULL  0X04 //FIFO满中断状态
#define INT_STS_FIFO_TMP   0X02  //温度测量完成标志位
#define INT_STS_FIFO_PRS  0X01  //气压测量完成标志位

#define FIFO_STS_FULL  0X02 //FIFO满
#define FIFO_STS_EMPTY   0X01 //FIFO空满

#define RESET_FIFO_FLUSH 0X80 //FIFO清0
#define RESET_SOFT 0X09//软件复位

#define PRODUCT_ID 0X10//产品ID



#define s32 int32
#define int16 short
#define int32 int
#define uint8 unsigned char
	

#define SPL06_CS_H GPIO_SetBits(GPIOA, GPIO_Pin_4)
#define SPL06_CS_L GPIO_ResetBits(GPIOA, GPIO_Pin_4)




//extern spl06Struct_t Spl06Data;


unsigned char spl0601_init(void);
void Drv_Spl0601_Read ( void );

float user_spl0601_get_presure(void);
float user_spl0601_get_temperature(void);
#endif

