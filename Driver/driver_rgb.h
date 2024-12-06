#ifndef __DRIVER_RGB_H
#define __DRIVER_RGB_H

#include "bsp.h"
#include "control.h"


#define LED_R	PDout(5)
#define LED_G PDout(4)
#define LED_B PDout(6)

#define LED_LIST 22
#define LED_SLOW 20
#define LED_NORMAL 4
#define LED_FAST 1


#define BEEP_LENGTH 50
#define BEEP_MAX_NOISE 50
#define BEEP_TIM TIM1
#define BEEP_PORT BEEP_TIM->CCR3


#define BEEP_PRESCALER 180
#define BEEP_MIN_NOISE 0
#define BEEP_NOTE_MIN 10
#define BEEP_NOTE_MAX 32768 
#define BEEP_PSC BEEP_TIM->PSC
#define MUSIC_MAX_LENGHT 48

#define	ID_0x0101 7
#define ID_0x0102 8
#define ID_0x0103 9
#define ID_0x0104 10
#define ID_0x0105 11

/*声音种类*/
enum{			
	QUIET=0,
	//解锁提示音
	MUSIC_ARMED,
	//上锁提示音
	MUSIC_DISARMED,	
	//IMU校准提示音
	MUSIC_IMUCALI,
	//参数保存提示音
	MUSIC_PARAMCALI,
	//MAG校准
	MUSIC_MAGCALI,	
	//失控
	MUSIC_RADIO_LOSS,
	//步兵
	MUSIC_TYPE_INFANTRY,
	//英雄
	MUSIC_TYPE_TANK,			
	//工程车
	MUSIC_TYPE_AUXILIARY,	
	//哨兵
	MUSIC_TYPE_SENTRY,		
	//无人机
	MUSIC_TYPE_UAV,				
	//小云台
	MUSIC_TYPE_SMALLGIMBAL,
	//无ID
	MUSIC_NO_ID,					
	//低电压
	MUSIC_LOWPOWER,				
	//电机高温
	MUSIC_HIGHTEMPERATURE,	
	MUSIC_LIST
};
/*音阶*/
enum{     
	BASS_DO=0,
	BASS_RE,
	BASS_MI,
	BASS_FA,
	BASS_SOL,
	BASS_LA,
	BASS_SI,
	ALTO_DO,
	ALTO_RE,
	ALTO_MI,
	ALTO_FA,
	ALTO_SOL,
	ALTO_LA,
	ALTO_SI,
	HIGH_DO,
	HIGH_RE,
	HIGH_MI,
	HIGH_FA,
	HIGH_SOL,
	HIGH_LA,
	HIGH_SI,
};

/*音量*/
#define	N_NOISE 0
#define L_NOISE 50
#define	H_NOISE 100
#pragma pack(1)
typedef struct {
	uint16_t note[MUSIC_MAX_LENGHT];
	uint16_t volume[MUSIC_MAX_LENGHT];
	uint16_t lenght;
} BeepSound_t;


typedef struct{
	void (*beep) (uint16_t commandState);
	void (*led)  (uint16_t commandState);
}appSightFuncClass;

extern appSightFuncClass appSightClass;




enum{
	LED_DISWORK=0,
	LED_WORK_RED,
	LED_WORK_GREEN,
	LED_WORK_YELLOW,
	LED_WORK_BLUE,
	LED_WORK_PINK,
	LED_WORK_CYAN,
	LED_WORK_WHITE
};

enum{
	LED_ENABLE=0,
	LED_DISABLE	
};

typedef struct {
	uint16_t colour;
	uint16_t frequency;
} LedStruct_t;

void ledUpdateTask(uint16_t commandState);
void rgbLedConfig(void);

#endif
