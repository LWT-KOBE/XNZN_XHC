#include "Driver_RMMotor.h"

#define CHASSIS_EC60

/*******************************外部使用的变量*********************************/
motorCanDataRecv_t	 fricWheelData[2];											//英雄摩擦轮
motorCanDataRecv_t	 wheelData[4];													//底盘电机
motorCanDataRecv_t	 pokeData;															//拨弹
motorCanDataRecv_t	 lidData;																//补给
motorCanDataRecv_t	 turntableData;  											  //英雄拨弹转盘
motorCanDataRecv_t	 bigPokeData;     										  //大发射机构拨弹
rmmotorCanDataRecv_t pitchMotorData,yawMotorData; 
currentCanDataRecv_t currentDate;
motorMeasureData_t   motorViewPitData,motorViewYawData;
motorMeasureData_t   motorGM3510PitData;

/****************************机器人类型电机变量*********************************/
motorCanDataRecv_t	 pawMotorData[2];												//工程爪子
motorCanDataRecv_t	 deformMotorData[2];										//工程横向变形机构
motorCanDataRecv_t	 liftMotorData[2];											//工程抬升机构
rmmotorTarData_t 	 pawMotorTarData[2];
/*****************************************************************************/                 

/*
***************************************************
函数名：driver_gimbal_readdata
功能：读取云台电机数据
入口参数：	can_rx_msg：CAN接收数据指针变量
					gimbal_data：存放云台电机接收到数据的指针变量
返回值：无
应用范围：外部调用
备注：
***************************************************
*/
void gimbal_readdata(CanRxMsg *can_rx_msg,gimbalCanDataRecv_t *gimbal_data){
	gimbal_data->encoderAngle = ((uint16_t)can_rx_msg->Data[0] << 8) | can_rx_msg->Data[1];
	gimbal_data->realcurrent 	= (( int16_t)can_rx_msg->Data[2] << 8) | can_rx_msg->Data[3];
	gimbal_data->current = (int16_t)(can_rx_msg->Data[4] << 8) | can_rx_msg->Data[5];
}
/*
***************************************************
函数名：chassis_motor_readdata
功能：读取地盘电机数据
入口参数：	can_rx_msg：CAN接收数据指针变量
					wheelData：存放地盘电机接收到数据的指针变量
返回值：无
应用范围：外部调用
备注：
***************************************************
*/ 
void rmmotor_readdata(CanRxMsg *can_rx_msg,motorCanDataRecv_t *motorData){
	motorData->rawangle = ((uint16_t)can_rx_msg->Data[0] << 8) | can_rx_msg->Data[1];
	motorData->speed 	 = (( int16_t)can_rx_msg->Data[2] << 8) | can_rx_msg->Data[3];
	motorData->currunt  = (( int16_t)can_rx_msg->Data[4] << 8) | can_rx_msg->Data[5];
	motorData->temperature = ( int8_t)can_rx_msg->Data[6];
}

void gimbal_motor6020_readdata(CanRxMsg *can_rx_msg,motorCanDataRecv_t *motor6020Data){
	motor6020Data->rawangle = ((uint16_t)can_rx_msg->Data[0] << 8) | can_rx_msg->Data[1];
	motor6020Data->speed 	 = (( int16_t)can_rx_msg->Data[2] << 8) | can_rx_msg->Data[3];
	motor6020Data->currunt  = (( int16_t)can_rx_msg->Data[4] << 8) | can_rx_msg->Data[5];
	motor6020Data->temperature = ( int8_t)can_rx_msg->Data[6];
}

void rmmotor_senddata(CAN_TypeDef *CANx, u32 ID_CAN, motorSerialNumber_t *rmmotorCanData){
	u8 mbox;
  u16 i=0;
	
	CanTxMsg txMessage;
	txMessage.StdId = ID_CAN;
	txMessage.IDE = CAN_Id_Standard;
	txMessage.RTR = CAN_RTR_Data;
	txMessage.DLC = 0x08;
	
	txMessage.Data[0] = (uint8_t)(rmmotorCanData->currunt1>>8);
	txMessage.Data[1] = (uint8_t) rmmotorCanData->currunt1;
	txMessage.Data[2] = (uint8_t)(rmmotorCanData->currunt2>>8);
	txMessage.Data[3] = (uint8_t) rmmotorCanData->currunt2;
	txMessage.Data[4] = (uint8_t)(rmmotorCanData->currunt3>>8);
	txMessage.Data[5] = (uint8_t) rmmotorCanData->currunt3;
	txMessage.Data[6] = (uint8_t)(rmmotorCanData->currunt4>>8);
	txMessage.Data[7] = (uint8_t) rmmotorCanData->currunt4;
	
	        
  mbox= CAN_Transmit(CANx, &txMessage);   
	
	//等待发送结束
  while(CAN_TransmitStatus(CANx, mbox)==CAN_TxStatus_Failed){
		i++;	
		if(i>=0xFFF)
		break;
	}
}

int16_t getRelativePos(int16_t rawEcd, int16_t centerOffset,rmmotorCanDataRecv_t *gimbalMotor){
  int16_t tmp = 0;

	  if (centerOffset >= 4096){
		if (rawEcd > centerOffset - 4096)
		  tmp = rawEcd - centerOffset;
		else
		  tmp = rawEcd + 8192 - centerOffset;
	  }
	  else{
		if (rawEcd > centerOffset + 4096)
		  tmp = rawEcd - 8192 - centerOffset;
		else
		  tmp = rawEcd - centerOffset;
	  }
  return tmp;
}

/*
***************************************************
函数名：gimbal_readData
功能：读取云台电机数据
入口参数：	can_rx_msg：CAN接收数据指针变量
					gimbal_data：存放云台电机接收到数据的指针变量
返回值：无
应用范围：外部调用
备注：
***************************************************
*/
void gimbal_readData(CanRxMsg *can_rx_msg,rmmotorCanDataRecv_t *gimbal_data){                                       

		gimbal_data->RMD_L_7015Data.molotovDataSheet.motorEncoder = ((uint16_t)can_rx_msg->Data[7] << 8) | can_rx_msg->Data[6];
		gimbal_data->RMD_L_7015Data.molotovDataSheet.motorSpeed   = ((int16_t)can_rx_msg->Data[5] << 8) | can_rx_msg->Data[4];
		gimbal_data->RMD_L_7015Data.molotovDataSheet.temperature  = (int8_t)can_rx_msg->Data[1];
		gimbal_data->RMD_L_7015Data.molotovDataSheet.iq			  = ((int16_t)can_rx_msg->Data[3] << 8) | can_rx_msg->Data[2];
}

/*
***************************************************
函数名：gimbal_readSlaveData
功能：向云台电机数据结构体中写入数据
入口参数：	dataNumber : 数据对应的序号（具体查看头文件）
          writeData：要写入的数据
					gimbal_data：存放云台电机接收到数据的指针变量
返回值：无
应用范围：外部调用
备注：
***************************************************
*/
void gimbal_readSlaveData(uint8_t dataNumber,vs16 writeData,rmmotorCanDataRecv_t *gimbal_data){                                       

			switch(dataNumber){
				case CODEBOARD_VALUE:    gimbal_data -> GM6020Data . rawangle = writeData;break;
				case REALTORQUE_CURRENT: gimbal_data -> GM6020Data . currunt = writeData;break;
				case TURN_SPEED:         gimbal_data -> GM6020Data . speed = writeData;break;
				case TEMPER:             gimbal_data -> GM6020Data . temperature = writeData;break;
			}

	}



/*
***************************************************
gimbal_chooseData
功能：云台电机数据选择
入口参数：	dataNumber : 数据对应的序号（具体查看头文件）
					gimbal_data：存放云台电机接收到数据的指针变量
返回值：无
应用范围：外部调用
备注：
***************************************************
*/
vs16 gimbal_chooseData(uint8_t dataNumber,rmmotorCanDataRecv_t *gimbal_data){                                       
	vs16 dataChoose;
			switch(dataNumber){
				
//				case CODEBOARD_VALUE:    dataChoose = gimbal_data -> GM6020Data . rawangle;break;
//				case REALTORQUE_CURRENT: dataChoose = gimbal_data -> GM6020Data . currunt;break;
//				case TURN_SPEED:         dataChoose = gimbal_data -> GM6020Data . speed;break;
//				case TEMPER:             dataChoose = gimbal_data -> GM6020Data . temperature;break;				
							
				case CODEBOARD_VALUE: dataChoose = gimbal_data->RMD_L_7015Data.molotovDataSheet.motorEncoder;break;
				case TORQUE_CURRENT: dataChoose = gimbal_data->RMD_L_7015Data.molotovDataSheet.iq;break;
			
			}
    return  dataChoose;
}









