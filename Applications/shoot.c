#include "application.h"
#include "bsp.h"

shootStruct_t shootData;

#ifdef USE_SNAIL
void shootUpdate(uint8_t FricONflag){
	static uint16_t shootPwm = SHOOT_OFF;
	static uint16_t maxShootPwm;
	if(shootData.shootMode == MANUAL_SINGLE){
		maxShootPwm = HIGH_SPEED;
	}
	else{
		maxShootPwm = LOW_SPEED;
	}
	if(FricONflag){																									//直接把shootPWM赋予MAXshootpwm 打开摩擦轮就打开激光
		LASER = LASER_ON;
		
#ifdef USE_2019_B
		CHANGE = CHANGE_ON;
#endif
		
		if(shootPwm > maxShootPwm){
			shootPwm -= SHOOT_PWM_INCREMENT;
			if(shootPwm < maxShootPwm)
				shootPwm = maxShootPwm;
		}
		if(shootPwm < maxShootPwm){
			shootPwm += SHOOT_PWM_INCREMENT;
			if( shootPwm > maxShootPwm ) 
				shootPwm = maxShootPwm;																		//单发连发模式大概26m/s射速
		}
		FricMotor_L = FricMotor_R = shootPwm;
	}
	else{
#ifdef USE_2019_B
		CHANGE = CHANGE_OFF;
#endif
		
		LASER = LASER_OFF;																						//摩擦轮不开激光也不开
		FricMotor_L = FricMotor_R = SHOOT_OFF;
	}
}

void shootInit(void){																							//发射机构初始化
	digitalLo(&shootData.shootState);	
	BSP_TIM_PWM_Init(SHOOT_TIM,SHOOT_PERIOD,SHOOT_PRES,SHOOT_L_PIN,SHOOT_R_PIN,NULL,NULL);	//两个无刷电机,频率400Hz																								
	FricMotor_L = FricMotor_R = SHOOT_OFF;
	shootData.FricONflag = DISABLE;
	digitalHi(&shootData.shootState);	
}
#endif

void bulletMonitorUpdata(void){
	if(BULLET_MONITOR_FLAG)
		shootData.bulletMonitorFlag = BULLET_READY;
	else
		shootData.bulletMonitorFlag = NO_BULLET;
}

void bulletMonitorInit(){
#ifdef USE_2019_A
	BSP_GPIO_Init(BSP_GPIOC0,GPIO_Mode_IPU);										  //17mm小子弹识别
#endif
#ifdef USE_2019_B
	BSP_GPIO_Init(BSP_GPIOC5,GPIO_Mode_IPU);										  //17mm小子弹识别
	BSP_GPIO_Init(BSP_GPIOB9,GPIO_Mode_Out_PP);										//充能装置开关
#endif
		BSP_GPIO_Init(BSP_GPIOA9,GPIO_Mode_Out_PP);										//激光初始化
}
