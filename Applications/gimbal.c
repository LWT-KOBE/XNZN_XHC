#include "application.h"
#include "driver.h"
#include "ramp.h"
#include "math.h"

/*
***************************************************
函 数 名：	gimbalUpdate
功		能：云台任务更新
入口参数：	gimbalData.visionYawCmd				横向坐标系反馈
					gimbalData.visionPitchCmd			纵向坐标系反馈
					gimbalData.autoMode		切换自动任务标志
返 回 值：	无
应用范围：	外部调用
备		注：
***************************************************
*********************************************************/

gimbalStruct_t gimbalData;
gimbalStruct_t* getGimbalData(){
    return &gimbalData;
}

ramp_t yawVisionRamp = RAMP_GEN_DAFAULT;											//云台斜坡初始化
ramp_t pitchVisionRamp = RAMP_GEN_DAFAULT;

static ramp_t yawRamp = RAMP_GEN_DAFAULT;											//云台斜坡初始化
static ramp_t pitchRamp = RAMP_GEN_DAFAULT;

int8_t getInstallDirect(uint8_t installPara,bool type){
	int8_t res;
	if(type)
		res = (installPara & 0x02)?-1:1;
	else
		res = (installPara & 0x01)?-1:1;
	return res;
}


u8 acc;
void gimbalUpdate(void){
	getGimbalCtrlDate();															
	
	switch (robotMode){
		case MODE_INIT:           
			gimbalInitHandle();   
			break;//初始化模式
		case MODE_RC: {
			if(gimbalData.ctrlMode == GIMBAL_INIT){
				gimbalInitHandle();
			}
			else if(gimbalData.ctrlMode == GIMBAL_NORMAL){
				gimbalFollowHandle();
			}
			else if(gimbalData.ctrlMode == GIMBAL_STOP){
				gimbalStopHandle();
			}
			break;
		}
		case MODE_STOP:{	 
			gimbalStopHandle();	 
			break;//丢控模式
		}
		case MODE_RELAX:{        
			gimbalRelaxHandle();  
			break;//解除控制权
		}
		default:                                        
			break;
	}

	gimbalData.time[0] = getClockCount();
	gimbalData.intervalTime = (float)(gimbalData.time[0] - gimbalData.time[1]);
	gimbalData.time[1] = gimbalData.time[0];
	
	
	if(isnan(gimbalData.yawAngleOut)||isnan(gimbalData.pitchAngleOut)){
		pidZeroState(gimbalData.pitchAnglePID);
		pidZeroState(gimbalData.pitchSpeedPID);
		pidZeroState(gimbalData.yawAnglePID);
		pidZeroState(gimbalData.yawSpeedPID);

		digitalClan(&gimbalData.yawAngleOut);
		digitalClan(&gimbalData.pitchAngleOut);
		digitalClan(&gimbalData.yawSpeedOut);
		digitalClan(&gimbalData.pitchSpeedOut);		
		
		gimbalData.pitchAngleRef = gimbalData.pitchAngleFbd;				
		gimbalData.yawAngleRef = gimbalData.yawAngleFbd;	
	}
	

	gimbalData.yawAngleOut   = pidUpdate(gimbalData.yawAnglePID, gimbalData.yawAngleRef, gimbalData.yawAngleFbd,gimbalData.intervalTime); 
	gimbalData.yawSpeedRef   = -gimbalData.yawAngleOut;
	

	gimbalData.yawAngleOut   = pidUpdate(gimbalData.yawAnglePID, gimbalData.yawAngleRef, gimbalData.yawAngleFbd,gimbalData.intervalTime); 
	gimbalData.yawSpeedRef   = -gimbalData.yawAngleOut;
	
	gimbalData.pitchAngleOut = pidUpdate(gimbalData.pitchAnglePID, gimbalData.pitchAngleRef, gimbalData.pitchAngleFbd,gimbalData.intervalTime);
	gimbalData.pitchSpeedRef = -gimbalData.pitchAngleOut;
	
	
	gimbalData.pitchSpeedFbd = IMU_RATEY;                    // 速度环反馈用imu角速度速度
	gimbalData.yawSpeedFbd   = IMU_RATEZ;
	
	gimbalData.pitchSpeedOut = -getInstallDirect(PITCH_INSTALL_CONFIG,INSTALL_TURN) * \
								pidUpdate(gimbalData.pitchSpeedPID,gimbalData.pitchSpeedRef,gimbalData.pitchSpeedFbd,gimbalData.intervalTime);
	gimbalData.yawSpeedOut   = getInstallDirect(YAW_INSTALL_CONFIG,INSTALL_TURN) * \
							   pidUpdate(gimbalData.yawSpeedPID,gimbalData.yawSpeedRef,gimbalData.yawSpeedFbd,gimbalData.intervalTime);
	
	if(robotMode == MODE_RELAX){
		digitalClan(&gimbalData.yawSpeedOut);
		digitalClan(&gimbalData.pitchSpeedOut);
		digitalClan(&gimbalData.rollSpeedOut);
	}
}


float shiftAngle = 0;
static void gimbalFollowHandle(void){
	digitalClan(&gimbalData.initFinishFlag);
	digitalClan(&gimbalData.motorFlag);

	gimbalData.pitchAngleFbd = gimbalData.pitchGyroAngle;
	gimbalData.yawAngleFbd = gimbalData.yawGyroAngle - gimbalData.yawAngleSave;
	gimbalData.yawAngleRef += remoteControlData.yawGyroTarget ;
	gimbalData.pitchAngleRef += remoteControlData.pitchGyroTarget;


	//pitch轴角度限幅
	//gimbalData.pitchAngleRef = constrainFloat(gimbalData.pitchAngleRef,getConfigData()->pitchMinRange,getConfigData()->pitchMaxRange); 
}

static void gimbalInitHandle(void){
	static f32_t pitchRampAngle;
	static f32_t yawRampAngle;
	
	static TickType_t xLastWakeTime = 0;
    //每一次进入初始化模式都获取当前角度并初始化斜坡函数
	if(lastRobotMode != robotMode||gimbalData.ctrlMode != gimbalData.lastCtrlMode){																				
		xLastWakeTime = xTaskGetTickCount();
		gimbalRampInit();
        //获取pitch轴imu当前角度
		pitchRampAngle = getInstallDirect(PITCH_INSTALL_CONFIG,INSTALL_ENCODER) * gimbalData.pitchMotorAngle;  				
        //获取yaw码盘当前角度
		yawRampAngle   = getInstallDirect(YAW_INSTALL_CONFIG,INSTALL_ENCODER) * gimbalData.yawMotorAngle;   					

	}
    //设置pitch轴反馈//pitch轴回中
	gimbalData.pitchAngleFbd = getInstallDirect(PITCH_INSTALL_CONFIG,INSTALL_ENCODER) * gimbalData.pitchMotorAngle;	
	gimbalData.pitchAngleRef = pitchRampAngle * (1 - LinearRampCalc(&pitchRamp,2));																			
    //设置yaw轴反馈//yaw轴回中
	gimbalData.yawAngleFbd = getInstallDirect(YAW_INSTALL_CONFIG,INSTALL_ENCODER) * gimbalData.yawMotorAngle;   		
	gimbalData.yawAngleRef = yawRampAngle * (1 - LinearRampCalc(&yawRamp,2));       																		

	//等待pitch、yaw轴回中
	if(((gimbalData.yawMotorAngle < 0.5f && gimbalData.yawMotorAngle > -0.5f)\
	&&(gimbalData.pitchMotorAngle < 0.5f && gimbalData.pitchMotorAngle > -0.5f))\
	|| xTaskGetTickCount() - xLastWakeTime > getConfigData()->backCenterTime * 2){
        //设置云台模式为跟随模式
		gimbalData.ctrlMode  = GIMBAL_NORMAL;

		
		//保存从初始化模式到跟随模式的陀螺仪角度	
		gimbalData.pitchAngleSave = gimbalData.pitchGyroAngle;		
		gimbalData.yawAngleSave = gimbalData.yawGyroAngle;
		//期望值清零
		digitalClan(&gimbalData.yawAngleRef);                    	
		digitalClan(&gimbalData.pitchAngleRef);
		
		//云台回中完成标志置一
		digitalHi(&gimbalData.initFinishFlag);                   	
	}
}


static void gimbalStopHandle(void){
	static f32_t pitchRampAngle;
	static f32_t yawRampAngle;
	static TickType_t xLastWakeTime = 0;
	//每一次进入初始化模式都获取当前角度并初始化斜坡函数
	if(lastRobotMode != robotMode || gimbalData.ctrlMode != gimbalData.lastCtrlMode){																						
		xLastWakeTime = xTaskGetTickCount();
		gimbalRampInit();
		//获取pitch码盘当前角度
		pitchRampAngle = getInstallDirect(PITCH_INSTALL_CONFIG,INSTALL_ENCODER) * gimbalData.pitchMotorAngle;  			
		//获取yaw码盘当前角度			
		yawRampAngle   = getInstallDirect(YAW_INSTALL_CONFIG,INSTALL_ENCODER) * gimbalData.yawMotorAngle;    						

	}
	//设置pitch轴反馈	
	gimbalData.pitchAngleFbd = getInstallDirect(PITCH_INSTALL_CONFIG,INSTALL_ENCODER) * gimbalData.pitchMotorAngle;				
	//设置yaw轴反馈
	gimbalData.yawAngleFbd = getInstallDirect(YAW_INSTALL_CONFIG,INSTALL_ENCODER) * gimbalData.yawMotorAngle;  						

	if(gimbalData.motorFlag){
		//pitch轴回中
		gimbalData.pitchAngleRef = pitchRampAngle + (gimbalData.pitchAngleStop - pitchRampAngle) * LinearRampCalc(&pitchRamp,1);			
		//yaw轴回中	  
		gimbalData.yawAngleRef = yawRampAngle + (gimbalData.yawAngleStop - yawRampAngle) * LinearRampCalc(&yawRamp,1);					

	}
	else{
		digitalClan(&gimbalData.pitchAngleStop);
		digitalClan(&gimbalData.yawAngleStop);
        //pitch轴回中
		gimbalData.pitchAngleRef = pitchRampAngle * (1 - LinearRampCalc(&pitchRamp,2));													
		//yaw轴回中
		gimbalData.yawAngleRef = yawRampAngle * (1 - LinearRampCalc(&yawRamp,2));

	}

  	//等待pitch、yaw轴回中
	if(((gimbalData.yawMotorAngle < gimbalData.yawAngleStop + 2.0f && \
        gimbalData.yawMotorAngle > gimbalData.yawAngleStop - 2.0f)\
		&& (gimbalData.pitchMotorAngle < gimbalData.pitchAngleStop + 2.0f && \
        gimbalData.pitchMotorAngle > gimbalData.pitchAngleStop - 2.0f))\
		|| xTaskGetTickCount() - xLastWakeTime > getConfigData()->backCenterTime * 2 ){
        //保存从初始化模式到跟随模式的陀螺仪角度
		gimbalData.pitchAngleSave = gimbalData.pitchGyroAngle;								
		gimbalData.yawAngleSave = gimbalData.yawGyroAngle;

		if(!gimbalData.motorFlag){
			//期望值清零
			digitalClan(&gimbalData.yawAngleRef);                   						
			digitalClan(&gimbalData.pitchAngleRef);
		}		
	}
}

static void gimbalRelaxHandle(void){
	 digitalLo(&gimbalData.initFinishFlag);
	 digitalLo(&gimbalData.motorFlag);
	 pidZeroState(gimbalData.pitchAnglePID);
	 pidZeroState(gimbalData.pitchSpeedPID);
	 pidZeroState(gimbalData.yawAnglePID);
	 pidZeroState(gimbalData.yawSpeedPID);

	 gimbalData.ctrlMode = GIMBAL_INIT;
}


//斜坡函数初始化
void gimbalRampInit(void){ 													  				
    RampInit(&pitchRamp, getConfigData()->backCenterTime/2);
    RampInit(&yawRamp, getConfigData()->backCenterTime/2);
    RampInit(&pitchVisionRamp, 100);
    RampInit(&yawVisionRamp, 100);
}


void gimbalInit(void){
    
    gimbalData.ctrlMode  = GIMBAL_RELAX;
	gimbalRampInit();
    
	gimbalData.pitchAnglePID = pidInit(&getConfigData()->pitchAnglePID);    
	gimbalData.yawAnglePID = pidInit(&getConfigData()->yawAnglePID);    
	gimbalData.pitchSpeedPID = pidInit( &getConfigData()->pitchRatePID);        
    gimbalData.yawSpeedPID = pidInit(&getConfigData()->yawRatePID);    
	digitalLo(&getGimbalData()->initFinishFlag);    
    
}








