#include "config.h"
#include "control.h"
#include "rc.h"
#include "imu.h"
#include "gimbal.h"
#include "supervisor.h"
#include "Driver_USBVCP.h"
remoteControlStruct_t remoteControlData;

//把绝对值从660转化到500的函数
void rcSbusScale(sbusStruct_t *raw,sbusStruct_t *real){					
	uint8_t i;
	f32_t ch[CH_NUMBER];
	for(i = 0;i < CH_NUMBER;i++){  
		ch[i] = (f32_t)*(&(raw->CH0)+i) / 660 * 500;
		ch[i] = constrainFloat(ch[i],-500,500);
		*(&(real->CH0) + i) = (int16_t)ch[i];
	}
}

//把绝对值从660转化到500的函数
void rcDt7Scale(dt7Struct_t *raw,dt7Struct_t *real){						
	uint8_t i;
	f32_t ch[CH_NUMBER];
	for(i = 0;i < CH_NUMBER;i++){  
		ch[i] = (f32_t)*(&(raw->CH0)+i) / 660 * 500;		
		ch[i] = constrainFloat(ch[i],-500.0f,500.0f);
		*(&(real->CH0) + i) = (int16_t)ch[i];
	}
	real->S1 = raw->S1;
	real->S2 = raw->S2;
}


//云台速度摇杆赋值
static void gimbalOperationFunc(int16_t pitCtrl, int16_t yawCtrl){
    
  remoteControlData.pitchGyroTarget =  pitCtrl * 0.003f * 2;
  remoteControlData.yawGyroTarget   =  yawCtrl * 0.003f * 2;
 
//  remoteControlData.pitchGyroTarget =  pitCtrl * 0.0012f * 2;
//  remoteControlData.yawGyroTarget   =  yawCtrl * 0.0012f * 2;

    
}

void getGimbalCtrlDate(void){

		getGimbalData()->yawMotorAngle =ENCODER_ANGLE_RATIO13 * getRelativePos(gimbal_chooseData(CODEBOARD_VALUE,&yawMotorData),\
                                                                          getConfigData()->yawCenter,\
                                                                          &yawMotorData);    

		getGimbalData()->pitchMotorAngle =ENCODER_ANGLE_RATIO13 * getRelativePos(gimbal_chooseData(CODEBOARD_VALUE,&pitchMotorData),\
                                                                          getConfigData()->pitchCenter,\
                                                                          &pitchMotorData);  
	
     //云台摇杆操作数据处理
	remoteCtrlGimbalHook();	        
	//pitch轴角度
	getGimbalData()->pitchGyroAngle = AQ_PITCH;
	//yaw轴角度		
	getGimbalData()->yawGyroAngle   = AQ_YAW;    
      
	getGimbalData()->pitchAngleFbd =  getGimbalData()->pitchGyroAngle;
	getGimbalData()->yawAngleFbd   =  getGimbalData()->yawGyroAngle -  getGimbalData()->yawAngleSave;    
}







void remoteCtrlGimbalHook(void){
  gimbalOperationFunc(RC_PITCH, RC_RUDD);
}


void rcUpdateTask(void){
#if UAV_SBUS 
	if(remoteControlData.rcIspReady){
		Driver_SBUS_Decode_RemoteData(&remoteControlData.sbusValue.rcRawData,Array_USART1_RX);
		digitalLo(&remoteControlData.rcIspReady);
	}
	rcSbusScale(&remoteControlData.sbusValue.rcRawData,&remoteControlData.sbusValue.rcRealData);
#else
	if(remoteControlData.rcIspReady){
		Driver_RMDT7_Decode_RemoteData(&remoteControlData.dt7Value,Array_USART1_RX);
		digitalLo(&remoteControlData.rcIspReady);
	}
	rcDt7Scale(&remoteControlData.dt7Value.rcRawData,&remoteControlData.dt7Value.rcRealData);
#endif
	digitalIncreasing(&remoteControlData.loops);
}
void rcInit(void){
	DT7IintClass.Init();
	remoteControlData.initFlag = true;
    usbVCP_Printf("RCInit Successfully \r\n");
}
 



