#include "application.h"

visionStruct_t visionData;
void visionReadCommand(uint8_t *array){
	uint8_t index_ptr = 4;
	uint8_t index = 0;

	visionData.distingushState = array[index_ptr++];
	for(uint8_t j = 0; j < 3; j++){
		for(index = 0; index < 2; index++){
			visionData.armorCoordinate[j].u8_temp[index] = array[index_ptr++];
		}
	}
	for(uint8_t j = 0; j < 3; j++){
		for(index = 0; index < 2; index++){
			visionData.armorCoordinateReal[j].u8_temp[index] = array[index_ptr++];
		}
	}
	for(index = 0; index < 2; index++)
		visionData.CNTR.u8_temp[index] = array[index_ptr++];
	wiredTransTypeSwitch(TRANS_ADD_VISION, ENABLE);
}

void visionReceive(uint8_t *array){
	if(array[0] == IMU_MODULAR_BEGIN && array[1] == IMU_MODULAR_ADDRESS){
		if(!Verify_CRC8_Check_Sum(array, 4) && !Verify_CRC16_Check_Sum(array, array[2])){					
			//校验失败则不导入数据
			//暂时不进行操作，后续会向主机报错
		}
		else{
			//读取主机命令
			visionReadCommand(array);
			digitalIncreasing(&visionData.checkSeq);
			digitalIncreasing(&visionData.visionError.errorCount);
		}
	}
}

void visionSendData(USART_TypeDef *USARTx){
	uint8_t *array = (uint8_t *)USART_TO_ArrayTX(USARTx);
	uint8_t index_ptr = 0;
	uint8_t index = 0;
	array[index_ptr++] = VISION_BEGIN;  	//0xD4	0
	array[index_ptr++] = VISION_ADDRESS;	//0x20	1
	array[index_ptr++] = 0x00;
	array[index_ptr++] = 0x00;		//CRC8	3
	
	array[index_ptr++] = visionData.mainControlCmd;	//4
	for(index = 0; index < 4; index++){
		array[index_ptr++] = navUkfData.pitch.u8_temp[index];	//5-8
	}
	for(index = 0; index < 4; index++){
		array[index_ptr++] = navUkfData.yaw.u8_temp[index];		//9-12
	}
	for(index = 0; index < 2; index++){
		array[index_ptr++] = runData.CNTR.u8_temp[index];		//13-14
	}
	array[index_ptr++] = visionData.shootSpeed;	//15	
	//填装校验位
	array[2] = index_ptr + 2;
	Append_CRC8_Check_Sum(array, 4);
	Append_CRC16_Check_Sum(array, array[2]);
	BSP_USART_DMA_SendData(USARTx, array, (index_ptr + 2));
}

void visionControlInit(void){
	driver_dataLinkInit(VISION_USARTX,VISION_USARTX_RX_PIN,VISION_USARTX_TX_PIN,\
											VISION_USART_BAUD_RATE,VISION_USART_PRE_PRIORITY,DATA_LINK_USART_SUB_PRIORITY);
}
