#include "application.h"


canSendStruct_t canSendData;
ODCANSendStruct_t ODSendData;

/****************************本文件使用的静态变量******************************/
static BSP_CAN_TypeDef can1;
static BSP_CAN_TypeDef can2;
/*****************************************************************************/

void ODRequestedState(void){		
    //如果想要对状态参数进行操作，直接将OdriveData.flashSave拉高一次即可    
	if(OdriveData.RequestedState){                                              
       //检测到需要改变状态参数		参数保存提示音
		//supervisorData.beepState = MUSIC_PARAMCALI;		        
		//set_axis_requested_state(CAN1,AXIS0_ID,MSG_SET_AXIS_REQUESTED_STATE,1,&OdriveData,axis0,&ODSendData);
		set_axis_requested_state(CAN1,AXIS1_ID,MSG_SET_AXIS_REQUESTED_STATE,1,&OdriveData,axis0,&ODSendData);
		
		//ODSendControllerModes(CAN1,AXIS0_ID,MSG_SET_CONTROLLER_MODES,8,&OdriveData,axis0,&ODSendData);
		//ODSendControllerModes(CAN1,AXIS1_ID,MSG_SET_CONTROLLER_MODES,8,&OdriveData,axis0,&ODSendData);
		
		
//		vTaskDelay(1000);	
//		digitalLo(&OdriveData.RequestedState);         
        
	} 

}

void ODFlashSave(void){		
    //如果想要对状态参数进行操作，直接将OdriveData.flashSave拉高一次即可    
	if(OdriveData.flashSave){                                              
       //检测到需要改变状态参数		参数保存提示音
		//supervisorData.beepState = MUSIC_PARAMCALI;	

		//ODSendControllerModes(CAN1,AXIS0_ID,MSG_SET_CONTROLLER_MODES,8,&OdriveData,axis0,&ODSendData);		

		
		//清楚错误
		OdriveSend_RemoteCmd(CAN1,AXIS0_ID,MSG_CLEAR_ERRORS);						

		//config
		OdriveSend_RemoteCmd(CAN1,AXIS0_ID,MSG_SAVE_CONFIG);						
		
		//digitalLo(&OdriveData.flashSave);         
        
	} 	
}


void canSendGimbalUpdate(void){
 

	//1KHZ
	//发送位置命令
	ODSendInputPosData(CAN1,AXIS1_ID,MSG_SET_INPUT_POS,8,&OdriveData,axis1,&ODSendData);	
	
	//发送速度命令
	//ODSendInputVelData(CAN1,AXIS0_ID,MSG_SET_INPUT_VEL,8,&OdriveData,axis0,&ODSendData);		

	//发送力矩命令
//	ODSendInputCurData(CAN1,AXIS0_ID,MSG_SET_INPUT_TORQUE,4,&OdriveData,axis0,&ODSendData);	
//	ODSendInputCurData(CAN1,AXIS1_ID,MSG_SET_INPUT_TORQUE,4,&OdriveData,axis0,&ODSendData);	
	
	
	//读取心跳信号
	OdriveSend_RemoteCmd(CAN1,AXIS0_ID,MSG_ODRIVE_HEARTBEAT);
	OdriveSend_RemoteCmd(CAN1,AXIS1_ID,MSG_ODRIVE_HEARTBEAT);	
	
	
	//250Hz  轮循发送　
	if(!((canSendData.loops + 1) % 2)){																										
			
	
		
		//读取总线电压、电流
		OdriveSend_RemoteCmd(CAN1,AXIS0_ID,MSG_GET_VBUS_VOLTAGE);						
		//读取温度
		OdriveSend_RemoteCmd(CAN1,AXIS0_ID,MSG_GET_TEMP);
		OdriveSend_RemoteCmd(CAN1,AXIS1_ID,MSG_GET_TEMP);

		//读取电机错误值
		OdriveSend_RemoteCmd(CAN1,AXIS0_ID,MSG_GET_MOTOR_ERROR);
		OdriveSend_RemoteCmd(CAN1,AXIS1_ID,MSG_GET_MOTOR_ERROR);

		//读取编码器错误值
		OdriveSend_RemoteCmd(CAN1,AXIS0_ID,MSG_GET_ENCODER_ERROR);
		OdriveSend_RemoteCmd(CAN1,AXIS1_ID,MSG_GET_ENCODER_ERROR);

		
		//读取编码器CPR值
		OdriveSend_RemoteCmd(CAN1,AXIS0_ID,MSG_GET_ENCODER_COUNT);
		OdriveSend_RemoteCmd(CAN1,AXIS1_ID,MSG_GET_ENCODER_COUNT);
		
		//读取位置/速度值
		OdriveSend_RemoteCmd(CAN1,AXIS0_ID,MSG_GET_ENCODER_ESTIMATES);
		OdriveSend_RemoteCmd(CAN1,AXIS1_ID,MSG_GET_ENCODER_ESTIMATES);

		
    }
	
	//125Hz  轮循发送　	
	if(!((canSendData.loops + 1) % 4)){		

		//设置电机状态
		ODRequestedState();
		
		//flash
		ODFlashSave();	


	}		

	
	

}


void canSendUpdate(void){
	canSendGimbalUpdate();

	digitalIncreasing(&canSendData.loops);
}

void canSendInit(void){								//CAN发送初始化
  
    driver_can1_init(CAN1,BSP_GPIOD0,BSP_GPIOD1,4,0);
	
	
    driver_can2_init(CAN2,BSP_GPIOB5,BSP_GPIOB6,3,0);    
}



void canSendRelax(void){

	memset((void *)&canSendData.can1_0x141, 0, sizeof(canSendData.can1_0x141));
	memset((void *)&canSendData.can1_0x142, 0, sizeof(canSendData.can1_0x142));
}


/*
***************************************************
函数名：driver_can1_init
功能：RM电机CAN1初始化
入口参数：	rm_canx：使用的CAN通道
					rm_canx_rx：RM CAN接收引脚
					rm_canx_tx：RM CAN发送引脚
					Preemption：CAN中断抢占优先级
					Sub：CAN中断次优先级
返回值：无
应用范围：外部调用
备注：
***************************************************
*/
void driver_can1_init(CAN_TypeDef* rm_canx,BSP_GPIOSource_TypeDef *rm_canx_rx,BSP_GPIOSource_TypeDef *rm_canx_tx,u8 Preemption,u8 Sub){
	can1.CANx = rm_canx;
	can1.CANx_RX = rm_canx_rx;
	can1.CANx_TX = rm_canx_tx;
	if(rm_canx == CAN1){
		can1.CAN_FilterInitStructure = CAN1_FilterInitStructure;
	}
	else if(rm_canx == CAN2){
		can1.CAN_FilterInitStructure = CAN2_FilterInitStructure;
	}
	BSP_CAN_Mode_Init(&can1,CAN_SJW_1tq,CAN_BS2_5tq,CAN_BS1_9tq,3,CAN_Mode_Normal,Preemption,Sub);
	
	//BSP_CAN_Mode_Init(&can1,CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_Normal,Preemption,Sub);
	
} 
/*
***************************************************
函数名：driver_can1_init
功能：RM电机CAN2初始化
入口参数：	rm_canx：使用的CAN通道
					rm_canx_rx：RM CAN接收引脚
					rm_canx_tx：RM CAN发送引脚
					Preemption：CAN中断抢占优先级
					Sub：CAN中断次优先级
返回值：无
应用范围：外部调用
备注：
***************************************************
*/
void driver_can2_init(CAN_TypeDef* rm_canx,BSP_GPIOSource_TypeDef *rm_canx_rx,BSP_GPIOSource_TypeDef *rm_canx_tx,u8 Preemption,u8 Sub){
	can2.CANx = rm_canx;
	can2.CANx_RX = rm_canx_rx;
	can2.CANx_TX = rm_canx_tx;
	if(rm_canx == CAN1){
		can2.CAN_FilterInitStructure = CAN1_FilterInitStructure;
	}
	else if(rm_canx == CAN2){
		can2.CAN_FilterInitStructure = CAN2_FilterInitStructure;
	}
	BSP_CAN_Mode_Init(&can2,CAN_SJW_1tq,CAN_BS2_5tq,CAN_BS1_9tq,3,CAN_Mode_Normal,Preemption,Sub);
	
	//BSP_CAN_Mode_Init(&can2,CAN_SJW_4tq,CAN_BS2_4tq,CAN_BS1_16tq,4,CAN_Mode_Normal,Preemption,Sub);
	
	
}
