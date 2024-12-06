#include "driver_Odrive.h"

OdriveStruct_t OdriveData; 
ODCanDataRecv_t OdReceivedData;


/*电机命令CAN发送_数据帧*/
void OdriveSendData(CAN_TypeDef *CANx, uint32_t ID_CAN,uint32_t CMD_CAN, uint8_t len,ODCANSendStruct_t* CanSendData) {
	CanTxMsg *txMessage;
	uint8_t mbox;
	uint8_t count;
	uint16_t i = 0;
	txMessage = (CanTxMsg*)aqCalloc(8,sizeof(CanTxMsg));
	txMessage->StdId = (ID_CAN<<5)+CMD_CAN;	
	txMessage->IDE = CAN_Id_Standard;
	txMessage->RTR = CAN_RTR_Data;
	txMessage->DLC = len;
	for (count = 0; count < len; count++) {
		txMessage->Data[count] = (uint8_t)CanSendData->data[count];
	}
	mbox = CAN_Transmit(CANx, txMessage);
	while (CAN_TransmitStatus(CANx,mbox) == 0x00) {
		i++;
		if (i >= 0xFFF)break;
	}
	aqFree(txMessage,8,sizeof(CanTxMsg));
}


/*电机命令CAN发送_远程帧*/
void OdriveSend_RemoteCmd(CAN_TypeDef *CANx, uint32_t ID_CAN,uint32_t CMD_CAN) {
	CanTxMsg *txMessage;
	uint8_t mbox;
	uint16_t i = 0;
	txMessage = (CanTxMsg*)aqCalloc(8,sizeof(CanTxMsg));
	
	
	txMessage->StdId = (ID_CAN<<5)+CMD_CAN;

	txMessage->ExtId=0x12; 	 // 设置扩展标示符（29位） 
	
	txMessage->IDE = CAN_Id_Standard;
	txMessage->RTR = CAN_RTR_Remote;
	mbox = CAN_Transmit(CANx, txMessage);
	while (CAN_TransmitStatus(CANx,mbox) == 0x00) {
		i++;
		if (i >= 0xFFF)break;
	}
	aqFree(txMessage,8,sizeof(CanTxMsg));
}


/*位置闭环*/
void ODSendInputPosData(CAN_TypeDef *CANx, uint32_t ID_CAN,uint32_t CMD_CAN, uint8_t len,OdriveStruct_t* Spetsnaz,uint8_t axis,ODCANSendStruct_t* CanSendData) {



	CanSendData->data[0] = Spetsnaz->SetPos.InputPos[axis].u8_temp[0];
	CanSendData->data[1] = Spetsnaz->SetPos.InputPos[axis].u8_temp[1];
	CanSendData->data[2] = Spetsnaz->SetPos.InputPos[axis].u8_temp[2];
	CanSendData->data[3] = Spetsnaz->SetPos.InputPos[axis].u8_temp[3];
	
	
	CanSendData->data[4] = Spetsnaz->SetPos.InputVel[axis].u8_temp[0];
	CanSendData->data[5] = Spetsnaz->SetPos.InputVel[axis].u8_temp[1];
	CanSendData->data[6] = Spetsnaz->SetPos.InputCur[axis].u8_temp[0];
	CanSendData->data[7] = Spetsnaz->SetPos.InputCur[axis].u8_temp[1];
	
	
	OdriveSendData(CANx,ID_CAN,CMD_CAN,len,CanSendData); 
	
	
}

/*速度闭环*/
void ODSendInputVelData(CAN_TypeDef *CANx, uint32_t ID_CAN,uint32_t CMD_CAN, uint8_t len,OdriveStruct_t* Spetsnaz,uint8_t axis,ODCANSendStruct_t* CanSendData) {



	CanSendData->data[0] = Spetsnaz->SetVel.InputVel[axis].u8_temp[0];
	CanSendData->data[1] = Spetsnaz->SetVel.InputVel[axis].u8_temp[1];
	CanSendData->data[2] = Spetsnaz->SetVel.InputVel[axis].u8_temp[2];
	CanSendData->data[3] = Spetsnaz->SetVel.InputVel[axis].u8_temp[3];
	
	
	CanSendData->data[4] = Spetsnaz->SetVel.InputCur[axis].u8_temp[0];
	CanSendData->data[5] = Spetsnaz->SetVel.InputCur[axis].u8_temp[1];
	CanSendData->data[6] = Spetsnaz->SetVel.InputCur[axis].u8_temp[2];
	CanSendData->data[7] = Spetsnaz->SetVel.InputCur[axis].u8_temp[3];
	
	
	OdriveSendData(CANx,ID_CAN,CMD_CAN,len,CanSendData); 
	
	
}

/*力矩闭环*/
void ODSendInputCurData(CAN_TypeDef *CANx, uint32_t ID_CAN,uint32_t CMD_CAN, uint8_t len,OdriveStruct_t* Spetsnaz,uint8_t axis,ODCANSendStruct_t* CanSendData) {



	CanSendData->data[0] = Spetsnaz->SetCur[axis].u8_temp[0];
	CanSendData->data[1] = Spetsnaz->SetCur[axis].u8_temp[1];
	CanSendData->data[2] = Spetsnaz->SetCur[axis].u8_temp[2];
	CanSendData->data[3] = Spetsnaz->SetCur[axis].u8_temp[3];
	
		
	
	OdriveSendData(CANx,ID_CAN,CMD_CAN,len,CanSendData); 
	
	
}

/*状态参数配置*/
void set_axis_requested_state(CAN_TypeDef *CANx, uint32_t ID_CAN,uint32_t CMD_CAN, uint8_t len,OdriveStruct_t* Spetsnaz,uint8_t axis,ODCANSendStruct_t* CanSendData) {
	
	CanSendData->data[0] = Spetsnaz->AxisState;		
	OdriveSendData(CANx,ID_CAN,CMD_CAN,len,CanSendData); 
	
	
}

/*

typedef enum{
	CONTROL_MODE_VOLTAGE_CONTROL     = 0,//电压控制模式
	CONTROL_MODE_TORQUE_CONTROL      = 1,//力矩控制模式
	CONTROL_MODE_VELOCITY_CONTROL    = 2,//速度控制模式
	CONTROL_MODE_POSITION_CONTROL    = 3,//位置控制模式
} ODControlModeStruct_t;

typedef enum{
	INPUT_MODE_INACTIVE              = 0,// 不活跃的输入模式
	INPUT_MODE_PASSTHROUGH           = 1,//通用设置模式
	INPUT_MODE_VEL_RAMP              = 2,//速度斜坡模式
	INPUT_MODE_POS_FILTER            = 3,//位置过滤模式？
	INPUT_MODE_MIX_CHANNELS          = 4,//混合输入模式？
	INPUT_MODE_TRAP_TRAJ             = 5,//梯形轨迹模式
	INPUT_MODE_TORQUE_RAMP           = 6,//力矩斜坡模式
	INPUT_MODE_MIRROR                = 7,//镜像模式
} ODInputModeStruct_t;




*/

/*配置电机的输入模式和闭环模式*/
void ODSendControllerModes(CAN_TypeDef *CANx, uint32_t ID_CAN,uint32_t CMD_CAN, uint8_t len,OdriveStruct_t* Spetsnaz,uint8_t axis,ODCANSendStruct_t* CanSendData) {



	CanSendData->data[0] = Spetsnaz->ControlMode[axis].u8_temp[0];
	CanSendData->data[1] = Spetsnaz->ControlMode[axis].u8_temp[1];
	CanSendData->data[2] = Spetsnaz->ControlMode[axis].u8_temp[2];
	CanSendData->data[3] = Spetsnaz->ControlMode[axis].u8_temp[3];
	
	
	CanSendData->data[4] = Spetsnaz->InputMode[axis].u8_temp[0];
	CanSendData->data[5] = Spetsnaz->InputMode[axis].u8_temp[1];
	CanSendData->data[6] = Spetsnaz->InputMode[axis].u8_temp[2];
	CanSendData->data[7] = Spetsnaz->InputMode[axis].u8_temp[3];
	
	
	OdriveSendData(CANx,ID_CAN,CMD_CAN,len,CanSendData); 
	
	
}





/*读取心跳信号 返回数据为位置/力矩*/
void ODReadHeartBeatData(CanRxMsg* CanRevData,ODCanDataRecv_t* Spetsnaz,uint8_t axis) {
		
	Spetsnaz->pos_estimate[axis].u8_temp[0] = CanRevData->Data[0];	
	Spetsnaz->pos_estimate[axis].u8_temp[1] = CanRevData->Data[1];	
	Spetsnaz->pos_estimate[axis].u8_temp[2] = CanRevData->Data[2];	
	Spetsnaz->pos_estimate[axis].u8_temp[3] = CanRevData->Data[3];
	
	
	Spetsnaz->Iq_measured[axis].u8_temp[0] = CanRevData->Data[4];	
	Spetsnaz->Iq_measured[axis].u8_temp[1] = CanRevData->Data[5];	
	Spetsnaz->Iq_measured[axis].u8_temp[2] = CanRevData->Data[6];	
	Spetsnaz->Iq_measured[axis].u8_temp[3] = CanRevData->Data[7];	

}

/*读取电压/电流CAN数据*/
void ODReadVbusData(CanRxMsg* CanRevData,ODCanDataRecv_t* Spetsnaz) {
		
	
	Spetsnaz->vbus_voltage.u8_temp[0] = CanRevData->Data[0];	
	Spetsnaz->vbus_voltage.u8_temp[1] = CanRevData->Data[1];	
	Spetsnaz->vbus_voltage.u8_temp[2] = CanRevData->Data[2];	
	Spetsnaz->vbus_voltage.u8_temp[3] = CanRevData->Data[3];
	
	
	Spetsnaz->ibus.u8_temp[0] = CanRevData->Data[4];	
	Spetsnaz->ibus.u8_temp[1] = CanRevData->Data[5];	
	Spetsnaz->ibus.u8_temp[2] = CanRevData->Data[6];	
	Spetsnaz->ibus.u8_temp[3] = CanRevData->Data[7];	
	

	
}

/*读取温度CAN数据*/
void ODReadTempData(CanRxMsg* CanRevData,ODCanDataRecv_t* Spetsnaz,uint8_t axis) {
		
	Spetsnaz->temperature[axis].u8_temp[0] = CanRevData->Data[0];	
	Spetsnaz->temperature[axis].u8_temp[1] = CanRevData->Data[1];	
	Spetsnaz->temperature[axis].u8_temp[2] = CanRevData->Data[2];	
	Spetsnaz->temperature[axis].u8_temp[3] = CanRevData->Data[3];
	
	
}

/*
	OdReceivedData.MotorError.u32_temp 电机错误对应值

            ERROR_NONE                       = 0x00000000,    	//0
            ERROR_PHASE_RESISTANCE_OUT_OF_RANGE = 0x00000001,	//1
            ERROR_PHASE_INDUCTANCE_OUT_OF_RANGE = 0x00000002,	//2
            ERROR_ADC_FAILED                 = 0x00000004, 		//4
            ERROR_DRV_FAULT                  = 0x00000008, 		//8
            ERROR_CONTROL_DEADLINE_MISSED    = 0x00000010, 		//16
            ERROR_NOT_IMPLEMENTED_MOTOR_TYPE = 0x00000020, 		//32
            ERROR_BRAKE_CURRENT_OUT_OF_RANGE = 0x00000040,		//64	
            ERROR_MODULATION_MAGNITUDE       = 0x00000080,		//128
            ERROR_BRAKE_DEADTIME_VIOLATION   = 0x00000100,		//256
            ERROR_UNEXPECTED_TIMER_CALLBACK  = 0x00000200,		//512
            ERROR_CURRENT_SENSE_SATURATION   = 0x00000400,		//1024
            ERROR_CURRENT_LIMIT_VIOLATION    = 0x00001000,		//4096
            ERROR_BRAKE_DUTY_CYCLE_NAN       = 0x00002000,		//8192
            ERROR_DC_BUS_OVER_REGEN_CURRENT  = 0x00004000,		//16384
            ERROR_DC_BUS_OVER_CURRENT        = 0x00008000,		//32768

*/
/*读取电机错误值*/
void ODReadMotorErrorData(CanRxMsg* CanRevData,ODCanDataRecv_t* Spetsnaz,uint8_t axis) {
		
	Spetsnaz->MotorError[axis].u8_temp[0] = CanRevData->Data[0];	
	Spetsnaz->MotorError[axis].u8_temp[1] = CanRevData->Data[1];	
	Spetsnaz->MotorError[axis].u8_temp[2] = CanRevData->Data[2];	
	Spetsnaz->MotorError[axis].u8_temp[3] = CanRevData->Data[3];
	
}

/*
	OdReceivedData.EncoderError.u32_temp 电机错误对应值

        enum Error {
            ERROR_NONE                       = 0x00000000, //0
            ERROR_UNSTABLE_GAIN              = 0x00000001, //1
            ERROR_CPR_POLEPAIRS_MISMATCH     = 0x00000002, //2
            ERROR_NO_RESPONSE                = 0x00000004, //4
            ERROR_UNSUPPORTED_ENCODER_MODE   = 0x00000008, //8
            ERROR_ILLEGAL_HALL_STATE         = 0x00000010, //16
            ERROR_INDEX_NOT_FOUND_YET        = 0x00000020, //32
            ERROR_ABS_SPI_TIMEOUT            = 0x00000040, //64
            ERROR_ABS_SPI_COM_FAIL           = 0x00000080, //128
            ERROR_ABS_SPI_NOT_READY          = 0x00000100, //256
        };
*/
/*读取编码器错误值*/
void ODReadEncodeErrorData(CanRxMsg* CanRevData,ODCanDataRecv_t* Spetsnaz,uint8_t axis) {
		
	Spetsnaz->EncoderError[axis].u8_temp[0] = CanRevData->Data[0];	
	Spetsnaz->EncoderError[axis].u8_temp[1] = CanRevData->Data[1];	
	Spetsnaz->EncoderError[axis].u8_temp[2] = CanRevData->Data[2];	
	Spetsnaz->EncoderError[axis].u8_temp[3] = CanRevData->Data[3];
	
}



/*读取编码器CAN数据*/
void ODReadEncoderCountData(CanRxMsg* CanRevData,ODCanDataRecv_t* Spetsnaz,uint8_t axis) {
		
	Spetsnaz->shadow_count[axis].u8_temp[0] = CanRevData->Data[0];	
	Spetsnaz->shadow_count[axis].u8_temp[1] = CanRevData->Data[1];	
	Spetsnaz->shadow_count[axis].u8_temp[2] = CanRevData->Data[2];	
	Spetsnaz->shadow_count[axis].u8_temp[3] = CanRevData->Data[3];
	
	
	Spetsnaz->count_in_cpr[axis].u8_temp[0] = CanRevData->Data[4];	
	Spetsnaz->count_in_cpr[axis].u8_temp[1] = CanRevData->Data[5];	
	Spetsnaz->count_in_cpr[axis].u8_temp[2] = CanRevData->Data[6];	
	Spetsnaz->count_in_cpr[axis].u8_temp[3] = CanRevData->Data[7];	

}

/*读取位置/速度CAN数据*/
void ODReadEncodeEstimatesData(CanRxMsg* CanRevData,ODCanDataRecv_t* Spetsnaz,uint8_t axis) {
		
//	Spetsnaz->pos_estimate[axis].u8_temp[0] = CanRevData->Data[0];	
//	Spetsnaz->pos_estimate[axis].u8_temp[1] = CanRevData->Data[1];	
//	Spetsnaz->pos_estimate[axis].u8_temp[2] = CanRevData->Data[2];	
//	Spetsnaz->pos_estimate[axis].u8_temp[3] = CanRevData->Data[3];
	
	
	Spetsnaz->vel_estimate[axis].u8_temp[0] = CanRevData->Data[4];	
	Spetsnaz->vel_estimate[axis].u8_temp[1] = CanRevData->Data[5];	
	Spetsnaz->vel_estimate[axis].u8_temp[2] = CanRevData->Data[6];	
	Spetsnaz->vel_estimate[axis].u8_temp[3] = CanRevData->Data[7];	

}


