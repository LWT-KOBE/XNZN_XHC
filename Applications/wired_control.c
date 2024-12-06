#include "board.h"

wiredControlStruct_t wiredControlData;

slaveSensorStruct_t slaveSensorData;

/*--------------------------------------------- 
-----------------------------------------------
//		警告：公共代码区域，故障责任重大		 //
//			除特定区域可以自由可以更改外，		 //
//	其余部分更改，请与公共区域负责人取得更改权限  // 
-----------------------------------------------
---------------------------------------------*/

void wiredTransTypeSwitch(uint16_t state, uint8_t value){
	if(value)
		wiredControlData.transType |= state;
	else
		wiredControlData.transType &= ~state;
}


//从机发送的数据
void wiredSendData(USART_TypeDef *USARTx){ 
	uint8_t *array = (uint8_t *)USART_TO_ArrayTX(USARTx);
	uint8_t index_ptr = 0;
	uint8_t index = 0;
	//帧头，长度4，1kHz
	array[index_ptr++] = MAIN_CONTROL_BEGIN;
	array[index_ptr++] = MAIN_CONTROL_ADDRESS + wiredControlData.transType;
	array[index_ptr++] = 0x00;
	array[index_ptr++] = 0x00;
	
	//陀螺仪状态标志位，长度3，1kHz
	supervisrCheckModule();
	array[index_ptr++] = getsupervisorData()->modularState;

	
	//角度数据，长度14，500Hz
	if(wiredControlData.transType & TRANS_ADD_ANGLE){
		wiredTransTypeSwitch(TRANS_ADD_ANGLE, DISABLE);
		
		//32位数据，串口只能发8位数据，故进行四次for循环
		for(index = 0; index < 4; index++){ 
			array[index_ptr++] = imuSensorData.pitch.u8_temp[index];
		}		
		for(index = 0; index < 4; index++){
			array[index_ptr++] = imuSensorData.roll.u8_temp[index];
		}
		for(index = 0; index < 4; index++){
			array[index_ptr++] = imuSensorData.yaw.u8_temp[index];	
		}		

		
	}
	
	//陀螺仪的数据，包括加速度，角速度，温度，长度28，500Hz
	if(wiredControlData.transType & TRANS_ADD_IMU_SENSOR){
		//wiredTransTypeSwitch(TRANS_ADD_IMU_SENSOR, DISABLE);    
		
		//角速度
		for(index = 0; index < 4; index++){  //32位
			array[index_ptr++] = imuSensorData.normalizedGyo[0].u8_temp[index];
		}		
		for(index = 0; index < 4; index++){//32位
			array[index_ptr++] = imuSensorData.normalizedGyo[1].u8_temp[index];
		}
		for(index = 0; index < 4; index++){//32位
			array[index_ptr++] = imuSensorData.normalizedGyo[2].u8_temp[index];
		}

		//加速度
		for(index = 0; index < 4; index++){  //32位
			array[index_ptr++] = imuSensorData.normalizedAcc[0].u8_temp[index];
		}		
		for(index = 0; index < 4; index++){//32位
			array[index_ptr++] = imuSensorData.normalizedAcc[1].u8_temp[index];
		}
		for(index = 0; index < 4; index++){//32位
			array[index_ptr++] = imuSensorData.normalizedAcc[2].u8_temp[index];
		}		
	
		//温度
		for(index = 0; index < 4; index++){  //32位
			array[index_ptr++] = imuSensorData.temp.u8_temp[index];
		}	

		
	}	
	
	//磁力计数据，长度12，500Hz
	if(wiredControlData.transType & TRANS_ADD_MAG){
		//wiredTransTypeSwitch(TRANS_ADD_MAG, DISABLE);
		
		for(index = 0; index < 4; index++){  //32位
			array[index_ptr++] = imuSensorData.normalizedMag[0].u8_temp[index];
		}		
		for(index = 0; index < 4; index++){//32位
			array[index_ptr++] = imuSensorData.normalizedMag[1].u8_temp[index];
		}
		for(index = 0; index < 4; index++){//32位
			array[index_ptr++] = imuSensorData.normalizedMag[2].u8_temp[index];
		}
	}	

		
	//填装校验位
	array[2] = index_ptr + 2;
	Append_CRC8_Check_Sum(array, 4);
	Append_CRC16_Check_Sum(array, array[2]);
	BSP_USART_DMA_SendData(USARTx, array, (index_ptr + 2));
}



//读取从机的IMU状态信息
void modularDataDecoding(uint8_t array){

	wiredTransTypeSwitch(MODULAR_IMU_LOSS, array & 0x01);
	wiredTransTypeSwitch(MODULAR_IMU_CALI, array & 0x02);
}

//读取从机IMU数据
static void slaveImuDataRead(uint8_t *array){
	uint8_t index_ptr = 4;
	uint8_t index = 0;
	
	//从机陀螺仪状态标志位，长度3，1kHz
	modularDataDecoding(array[index_ptr++]);
	
	//从机角度数据，长度14，500Hz
	if(array[1] & TRANS_ADD_ANGLE){
		for(index=0;index < 4;index++)
			slaveSensorData.pitch.u8_temp[index] = array[index_ptr++];
		for(index=0;index < 4;index++)
			slaveSensorData.roll.u8_temp[index] = array[index_ptr++];
		for(index=0;index < 4;index++)
			slaveSensorData.yaw.u8_temp[index] = array[index_ptr++];

	
		
	}
	
	//从机陀螺仪的数据，包括加速度，角速度，温度，长度28，500Hz
	if(array[1] & TRANS_ADD_IMU_SENSOR){
		
		//角速度		
		for(index=0;index < 4;index++)
			slaveSensorData.normalizedGyo[0].u8_temp[index] = array[index_ptr++];
		for(index=0;index < 4;index++)
			slaveSensorData.normalizedGyo[1].u8_temp[index] = array[index_ptr++];
		for(index=0;index < 4;index++)
			slaveSensorData.normalizedGyo[2].u8_temp[index] = array[index_ptr++];

		//加速度		
		for(index=0;index < 4;index++)
			slaveSensorData.normalizedAcc[0].u8_temp[index] = array[index_ptr++];
		for(index=0;index < 4;index++)
			slaveSensorData.normalizedAcc[1].u8_temp[index] = array[index_ptr++];
		for(index=0;index < 4;index++)
			slaveSensorData.normalizedAcc[2].u8_temp[index] = array[index_ptr++];		
		
		//温度
		for(index=0;index < 4;index++)
			slaveSensorData.temp.u8_temp[index] = array[index_ptr++];			
		
	
		
	}	
	
	//从机磁力计数据，长度12，500Hz	
	if(array[1] & TRANS_ADD_MAG){

		
		for(index=0;index < 4;index++)
			slaveSensorData.normalizedMag[0].u8_temp[index] = array[index_ptr++];
		for(index=0;index < 4;index++)
			slaveSensorData.normalizedMag[1].u8_temp[index] = array[index_ptr++];
		for(index=0;index < 4;index++)
			slaveSensorData.normalizedMag[2].u8_temp[index] = array[index_ptr++];	
	}


	
	
}

//主机读取从机发过来的数据
void slaveSensorRead(uint8_t *array){
	if(array[0] == MAIN_CONTROL_BEGIN && (array[1] & MAIN_CONTROL_ADDRESS) ){
		if(!Verify_CRC8_Check_Sum(array, 4) && !Verify_CRC16_Check_Sum(array, array[2])){					
			// 校验失败则不导入数据
			//暂时不进行操作
		}
		else{
			//读取从机imu数据
			slaveImuDataRead(array);
		}
	}
}

//从机串口初始化
void driver_SlaveInit(USART_TypeDef* USARTx,BSP_GPIOSource_TypeDef *USART_RX,BSP_GPIOSource_TypeDef *USART_TX,
												u32 baudRate,u8 PreemptionPriority,u8 SubPriority){
	BSP_USART_TypeDef SLAVE_USART;
	SLAVE_USART.USARTx = USARTx;
	SLAVE_USART.USART_RX = USART_RX;
	SLAVE_USART.USART_TX = USART_TX;
	SLAVE_USART.USART_InitStructure.USART_BaudRate = baudRate;
	SLAVE_USART.USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	SLAVE_USART.USART_InitStructure.USART_StopBits = USART_StopBits_1;
	SLAVE_USART.USART_InitStructure.USART_Parity = USART_Parity_No;
	SLAVE_USART.USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	SLAVE_USART.USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	
	BSP_USART_Init(&SLAVE_USART,PreemptionPriority,SubPriority);
	BSP_USART_RX_DMA_Init(&SLAVE_USART);
	BSP_USART_TX_DMA_Init(&SLAVE_USART);
}

//主机串口初始化
void driver_MasterInit(USART_TypeDef* USARTx,BSP_GPIOSource_TypeDef *USART_RX,BSP_GPIOSource_TypeDef *USART_TX,
												u32 baudRate,u8 PreemptionPriority,u8 SubPriority){
	BSP_USART_TypeDef MASTER_USART;
	MASTER_USART.USARTx = USARTx;
	MASTER_USART.USART_RX = USART_RX;
	MASTER_USART.USART_TX = USART_TX;
	MASTER_USART.USART_InitStructure.USART_BaudRate = baudRate;
	MASTER_USART.USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	MASTER_USART.USART_InitStructure.USART_StopBits = USART_StopBits_1;
	MASTER_USART.USART_InitStructure.USART_Parity = USART_Parity_No;
	MASTER_USART.USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	MASTER_USART.USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	
	BSP_USART_Init(&MASTER_USART,PreemptionPriority,SubPriority);
	BSP_USART_RX_DMA_Init(&MASTER_USART);
	BSP_USART_TX_DMA_Init(&MASTER_USART);
}









void wiredControlInit(void){
	
#ifdef IMU_SLAVE
	
	driver_SlaveInit(MASTER_USARTX, MASTER_USARTX_RX_PIN, MASTER_USARTX_TX_PIN,\
					 MASTER_USART_BAUD_RATE, MASTER_USART_PRE_PRIORITY, MASTER_USART_SUB_PRIORITY);	
	//SlaveBleInit_1();
#else
	driver_MasterInit(SLAVE_USARTX, SLAVE_USARTX_RX_PIN, SLAVE_USARTX_TX_PIN,\
					 SLAVE_USART_BAUD_RATE, SLAVE_USART_PRE_PRIORITY, SLAVE_USART_SUB_PRIORITY);	
	
	//MasterBleInit();
	
#endif
	
}
