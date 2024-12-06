#ifndef __DRIVE_WT931_H
#define __DRIVE_WT931_H
#include "Util.h"
#include "bsp.h"
#include "BSP_GPIO.h"
#include "stdbool.h"

#define MPU_WT931_USARTX										USART6		  //串口号
#define MPU_WT931_USARTX_RX_PIN							BSP_GPIOC7	//接收引脚
#define MPU_WT931_USARTX_TX_PIN							BSP_GPIOC6	//发送引脚
#define MPU_WT931_USART_PreemptionPriority 	3						//中断抢占优先级
#define MPU_WT931_USART_SubPriority 				0						//中断响应优先级

typedef enum{
	NOT_READY = 0,
	ACC_READY,		//加速度通过校验和
	GYRO_READY,		//角速度通过校验和
	ANGLE_READAY,	//角度通过过校验和
}imuSumFlag_e;

typedef struct{
	float x; 
	float y; 
	float z; 
	float T;
}mpuDirection_t;

typedef struct{
    
	bool initFlag;        
   	uint32_t loops;
    formatTrans32Struct_t yaw, pitch, roll;
	formatTrans16Struct_t CNTR;        
	mpuDirection_t stcAcc;
	mpuDirection_t stcGyro;
	mpuDirection_t stcAngle;
}wt931Data_t;


extern wt931Data_t wt931Data;




void Driver_WT931_Init(USART_TypeDef *USARTx,BSP_GPIOSource_TypeDef *USART_RX,BSP_GPIOSource_TypeDef *USART_TX,u8 PreemptionPriority,u8 SubPriority);
void dataToCopy(uint8_t array[]);
void Driver_WT931ReadDMA(uint8_t *arrayWT931Receive);
void caliInitWT931(uint8_t * sendData);


#endif



