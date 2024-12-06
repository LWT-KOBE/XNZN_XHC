#include "driver.h"
#include <string.h>
#include "application.h"

BSP_USART_TypeDef MPU_WT931;
wt931Data_t wt931Data;

uint8_t setCaliSelf[5] = {0xff,0xaa,0x63,0x00,0x00};     //自动校准
uint8_t delCaliSelf[5] = {0xff,0xaa,0x63,0x01,0x00};


void Driver_WT931_Init(USART_TypeDef *USARTx,BSP_GPIOSource_TypeDef *USART_RX,BSP_GPIOSource_TypeDef *USART_TX,u8 PreemptionPriority,u8 SubPriority){
//	static uint8_t caliFlag = DISABLE;
	BSP_USART_TypeDef MPU_WT931;
	MPU_WT931.USARTx = USARTx;
	MPU_WT931.USART_RX = USART_RX;
	MPU_WT931.USART_TX = USART_TX;
	MPU_WT931.USART_InitStructure.USART_BaudRate = 921600;									/*波特率为500000*/
	MPU_WT931.USART_InitStructure.USART_WordLength = USART_WordLength_8b;	/*字长为8位数据格式*/
	MPU_WT931.USART_InitStructure.USART_StopBits = USART_StopBits_1;				/*一个停止位*/
	MPU_WT931.USART_InitStructure.USART_Parity = USART_Parity_No;				/*无校验位*/
	MPU_WT931.USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;							/*接收/发送模式*/	
	MPU_WT931.USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	/*无硬件数据流控制*/	
	BSP_USART_Init(&MPU_WT931,PreemptionPriority,SubPriority);
	BSP_USART_RX_DMA_Init(&MPU_WT931);							
//	
//	if(caliFlag == DISABLE){			//设置时使用
//		caliInitWT931(delCaliSelf);
//		caliFlag = ENABLE;
//	}
}
void caliInitWT931(uint8_t * sendData){
	static uint8_t sendNum = 0;
	while(sendData[sendNum] != '\0'){
		while(USART_GetFlagStatus(UART8,USART_FLAG_TC )==RESET){
			USART_SendData(UART8,sendData[sendNum]);
			sendNum ++;
		}
	}
}

void Driver_WT931ReadDMA(uint8_t *arrayWT931Receive){
	
	if(arrayWT931Receive[0] != 0x55){
	}	//若起始和末尾校验失败，则不进行下一步操作
	else{
		dataToCopy(arrayWT931Receive);        		
 	} 
}

      

void dataToCopy(uint8_t array[]){    
uint8_t dataSum = 0;

        if(array[1] == 0x51){
                for(uint8_t i = 0; i<10 ; i++)
                    dataSum += array[i];
                if(dataSum != array[10]){
                }
                else{
                    wt931Data.stcAcc.x = (float)((short)((((int16_t)array[3])<<8)|((int16_t)array[2])))/32768*16;   
                    wt931Data.stcAcc.y = (float)((short)((((int16_t)array[5])<<8)|((int16_t)array[4])))/32768*16;   
                    wt931Data.stcAcc.z = (float)((short)((((int16_t)array[7])<<8)|((int16_t)array[6])))/32768*16;
                    wt931Data.stcAcc.T = (float)((short)((((int16_t)array[9])<<8)|((int16_t)array[8])))/100;
                }
                dataSum = 0;
            }
            if(array[12] == 0x52){
                for(uint8_t i = 11; i<21 ; i++)
                    dataSum += array[i];
                if(dataSum != array[21]){
                }
                else{
                    wt931Data.stcGyro.x = (float)((short)((((int16_t)array[14])<<8)|((int16_t)array[13])))/32768*2000;   
                    wt931Data.stcGyro.y = (float)((short)((((int16_t)array[16])<<8)|((int16_t)array[15])))/32768*2000;   
                    wt931Data.stcGyro.z = (float)((short)((((int16_t)array[18])<<8)|((int16_t)array[17])))/32768*2000;
                    wt931Data.stcGyro.T = (float)((short)((((int16_t)array[20])<<8)|((int16_t)array[19])))/100;            
                }
                dataSum = 0;
            }
            if(array[23] == 0x53){
                for(uint8_t i = 22; i<32 ; i++)
                    dataSum += array[i];
                if(dataSum != array[32]){
                }
                else{
                    wt931Data.stcAngle.x = (float)((short)((((int16_t)array[25])<<8)|((int16_t)array[24])))/32768*180;   //pitch轴
                    wt931Data.stcAngle.y = (float)((short)((((int16_t)array[27])<<8)|((int16_t)array[26])))/32768*180;   //yaw轴
                    wt931Data.stcAngle.z = (float)((short)((((int16_t)array[29])<<8)|((int16_t)array[28])))/32768*180;  //roll轴	
                    wt931Data.stcAngle.T = (float)((short)((((int16_t)array[31])<<8)|((int16_t)array[30])))/100;        //tempreature

                    wt931Data.pitch.float_temp = wt931Data.stcAngle.x;
                    wt931Data.yaw.float_temp = wt931Data.stcAngle.z;
                    wt931Data.roll.float_temp = wt931Data.stcAngle.y;
					
                    imuSensorData.pitch.float_temp = wt931Data.stcAngle.x;
                    imuSensorData.yaw.float_temp = wt931Data.stcAngle.z;
                    imuSensorData.roll.float_temp = wt931Data.stcAngle.y;					
					imuSensorData.temp.float_temp=wt931Data.stcAngle.T;
					
                    digitalIncreasing(&wt931Data.CNTR.s16_temp);                   
                    
                }
                dataSum = 0;
            }
    
}




