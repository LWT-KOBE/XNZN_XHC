#ifndef __SHOOT_H
#define __SHOOT_H

#include "bsp.h"
#include "util.h"

#define USE_SNAIL

#define SHOOT_TIM TIM1
#define SHOOT_PERIOD	2499
#define SHOOT_PRES	167
#define SHOOT_L_PIN	BSP_GPIOA8
#define SHOOT_R_PIN	BSP_GPIOA9

#define SHOOT_PWM_INCREMENT	2
#define FricMotor_L TIM1->CCR1
#define FricMotor_R TIM1->CCR2
#define SHOOT_OFF	1000

#ifdef USE_2019_A
#define BULLET_MONITOR_FLAG  PCin(0)
#endif
#ifdef USE_2019_B
#define BULLET_MONITOR_FLAG  PCin(5)
#define CHANGE							PBout(9)
#define CHANGE_ON						1
#define CHANGE_OFF					0
#endif

#define LASER								PAout(9)


#define HIGH_SPEED	 1195
#define LOW_SPEED	   1080
#define LASER_ON      1
#define LASER_OFF 		0

typedef enum{
	MANUAL_SINGLE = 0,
	MANUAL_CONTINUOUS,
	AUTO_CONTINUOUS,
} shootMode_e;      						//Éä»÷Ä£Ê½

typedef enum{
	SAFE = 0,
//	WARNING,
	DANGEROUS,
}shootStatus_e;								//Éä»÷×´Ì¬

typedef enum{
	BULLET_READY = 0,
	NO_BULLET,	
} shootBulletMonitorMode_e;

typedef struct{
	bool FricONflag;
	shootMode_e shootMode;
	shootStatus_e shootStatus;    //Éä»÷×´Ì¬ 
	shootStatus_e shootStatusMode;//Éä»÷°²È«Ä£Ê½
	shootBulletMonitorMode_e  bulletMonitorFlag;
	uint32_t shootHighPwm;
	uint32_t shootLowPwm;
	uint8_t	 shootState;	
} shootStruct_t;

extern shootStruct_t shootData;

void shootUpdate(uint8_t FricONflag );
void shootInit(void);
void bulletMonitorInit(void);
void bulletMonitorUpdata(void);

#endif
