#include "driver.h"
#include "util.h"

LedStruct_t ledFuntionData[LED_LIST]={
	{LED_DISWORK,LED_SLOW},
	{LED_WORK_RED,LED_SLOW},{LED_WORK_GREEN,LED_SLOW},{LED_WORK_YELLOW,LED_SLOW},{LED_WORK_BLUE,LED_SLOW},{LED_WORK_PINK,LED_SLOW},{LED_WORK_CYAN,LED_SLOW},{LED_WORK_WHITE,LED_SLOW},
	{LED_WORK_RED,LED_NORMAL},{LED_WORK_GREEN,LED_NORMAL},{LED_WORK_YELLOW,LED_NORMAL},{LED_WORK_BLUE,LED_NORMAL},{LED_WORK_PINK,LED_NORMAL},{LED_WORK_CYAN,LED_NORMAL},{LED_WORK_WHITE,LED_NORMAL},
	{LED_WORK_RED,LED_FAST},{LED_WORK_GREEN,LED_FAST},{LED_WORK_YELLOW,LED_FAST},{LED_WORK_BLUE,LED_FAST},{LED_WORK_PINK,LED_FAST},{LED_WORK_CYAN,LED_FAST},{LED_WORK_WHITE,LED_FAST}
};

/**************************************************/
static void sightInit(void);
static void beepUpdateTask(uint16_t commandState);
static void ledUpdateTask(uint16_t commandState);
/**************************************************/

deviceInitClass sightClass = {
	sightInit,
};

appSightFuncClass appSightClass = {
	beepUpdateTask,
	ledUpdateTask,
};


static void ledUpdateTask(uint16_t commandState){
	static uint16_t ledLoops=0;
	static uint8_t workFlag = 0;
	static LedStruct_t *ledWork;
	//对命令进行限制，防止误填导致指针越界
	commandState = constrainInt(commandState,0,LED_LIST-1);
	ledWork = &ledFuntionData[commandState];
	if(ledWork->frequency==0){
		LED_R = LED_G = LED_B = LED_DISABLE;
	}
	else if(!(ledLoops % ledWork->frequency)){
		workFlag =~ workFlag;
	}
	if(workFlag){
		if(ledWork->colour & LED_WORK_RED){
			LED_R = LED_ENABLE;
		}
		else{
			LED_R = LED_DISABLE;
		}
		if(ledWork->colour & LED_WORK_GREEN){
			LED_G = LED_ENABLE;
		}
		else{
			LED_G = LED_DISABLE;
		}
		if(ledWork->colour & LED_WORK_BLUE){
			LED_B = LED_ENABLE;
		}
		else{
			LED_B = LED_DISABLE;
		}
	}
	else{
		LED_R = LED_G = LED_B = LED_DISABLE;
	}
	ledLoops++;
}

/*C调*/
int32_t BeepCode[]={
	//低音
	6412,5714,5090,4813,4285,3818,3400,
	//中音
	3212,2906,2549,2406,2142,1909,1700,
	//高音
	1606,1429,1274,1202,1071,954,850		
};

static void beepUpdate(uint16_t beepSound,uint16_t note){
	BEEP_PORT=constrainInt(beepSound,BEEP_MIN_NOISE,BEEP_MAX_NOISE);
	BEEP_PSC=constrainInt(note,BEEP_NOTE_MIN,BEEP_NOTE_MAX);
}

BeepSound_t beepNewSound[MUSIC_LIST]={
	//ARMED
	{	
		{ALTO_DO,ALTO_DO,ALTO_DO,ALTO_DO,ALTO_DO,ALTO_DO,ALTO_DO,ALTO_DO,ALTO_DO,ALTO_RE,ALTO_RE,ALTO_RE,ALTO_MI,ALTO_MI,ALTO_MI,ALTO_MI},    
		{H_NOISE,H_NOISE,H_NOISE,H_NOISE,N_NOISE,N_NOISE,H_NOISE,H_NOISE,H_NOISE,H_NOISE,H_NOISE,H_NOISE,H_NOISE,H_NOISE,H_NOISE,N_NOISE},16
	},
	//DISARMED
	{
		{ALTO_MI,ALTO_MI,ALTO_MI,ALTO_MI,ALTO_MI,ALTO_MI,ALTO_MI,ALTO_MI,ALTO_MI,ALTO_RE,ALTO_RE,ALTO_RE,ALTO_DO,ALTO_DO,ALTO_DO,ALTO_DO},		
		{H_NOISE,H_NOISE,H_NOISE,H_NOISE,N_NOISE,N_NOISE,H_NOISE,H_NOISE,H_NOISE,H_NOISE,H_NOISE,H_NOISE,H_NOISE,H_NOISE,H_NOISE,N_NOISE},16
	},
	//IMU_CALI
	{
		{HIGH_SOL,HIGH_SOL,ALTO_DO,ALTO_DO,HIGH_SOL,HIGH_SOL,ALTO_DO,ALTO_DO,HIGH_SOL},																												
		{H_NOISE,H_NOISE,H_NOISE,H_NOISE,H_NOISE,H_NOISE,H_NOISE,H_NOISE,N_NOISE},9
	},
	//FLASH_SAVE	
	{
		{HIGH_SOL,HIGH_SOL,HIGH_SOL,HIGH_SOL,HIGH_SOL,HIGH_SOL,HIGH_SOL,HIGH_SOL,HIGH_SOL},																										
		{H_NOISE,H_NOISE,H_NOISE,N_NOISE,N_NOISE,H_NOISE,H_NOISE,H_NOISE,N_NOISE},9	
	},
	//MAG_CALI
	{
		{HIGH_SOL,HIGH_SOL,HIGH_SOL,HIGH_SOL,HIGH_SOL,HIGH_SOL,HIGH_SOL,HIGH_SOL,HIGH_SOL,HIGH_SOL},																					
		{H_NOISE,N_NOISE,H_NOISE,N_NOISE,H_NOISE,N_NOISE,H_NOISE,N_NOISE,H_NOISE,N_NOISE},10
	},
	//RADIO_LOSS
	{
		{ALTO_DO,ALTO_DO,ALTO_DO,ALTO_DO,ALTO_DO,ALTO_DO,ALTO_DO,ALTO_DO,ALTO_DO,ALTO_DO},																										
		{H_NOISE,H_NOISE,N_NOISE,N_NOISE,N_NOISE,H_NOISE,H_NOISE,N_NOISE,N_NOISE,N_NOISE},10
	},
	//TYPE_INFANTRY
	{
		{ALTO_MI,ALTO_MI},																																																										
		{H_NOISE,N_NOISE},2
	},
	//TYPE_TANK
	{
		{ALTO_MI,ALTO_MI,ALTO_MI,ALTO_MI},																																																		
		{H_NOISE,N_NOISE,H_NOISE,N_NOISE},4		
	},
	//TYPE_AUXILIARY
	{
		{ALTO_MI,ALTO_MI,ALTO_MI,ALTO_MI,ALTO_MI,ALTO_MI},																																										
		{H_NOISE,N_NOISE,H_NOISE,N_NOISE,H_NOISE,N_NOISE},6		
	},
	//TYPE_SENTRY
	{
		{ALTO_MI,ALTO_MI,ALTO_MI,ALTO_MI,ALTO_MI,ALTO_MI,ALTO_MI,ALTO_MI},																																		
		{H_NOISE,N_NOISE,H_NOISE,N_NOISE,H_NOISE,N_NOISE,H_NOISE,N_NOISE},8			
	},
	//TYPE_UAV
	{
		{ALTO_MI,ALTO_MI,ALTO_MI,ALTO_MI,ALTO_MI,ALTO_MI,ALTO_MI,ALTO_MI,ALTO_MI,ALTO_MI},																										
		{H_NOISE,N_NOISE,H_NOISE,N_NOISE,H_NOISE,N_NOISE,H_NOISE,N_NOISE,H_NOISE,N_NOISE},10		
	},
	//SMALLGIMBAL_ID
	{
		{ALTO_MI,ALTO_MI,ALTO_MI,ALTO_MI,ALTO_MI,ALTO_MI,ALTO_MI,ALTO_MI,ALTO_MI,ALTO_MI,ALTO_MI,ALTO_MI},																		
		{H_NOISE,N_NOISE,H_NOISE,N_NOISE,H_NOISE,N_NOISE,H_NOISE,N_NOISE,H_NOISE,N_NOISE,H_NOISE,N_NOISE},12		
	},
	//TYPE_NO_ID
	{
		{BASS_RE,BASS_RE,BASS_RE,BASS_RE,BASS_RE,BASS_RE,BASS_RE,BASS_RE,BASS_RE,BASS_RE},																										
		{H_NOISE,N_NOISE,H_NOISE,N_NOISE,N_NOISE,N_NOISE,H_NOISE,H_NOISE,H_NOISE,N_NOISE},10
	},
	{
		0,0,0
	},
	//BRAKETEMPERATURE
	{
		{HIGH_FA,HIGH_RE,HIGH_FA,HIGH_RE,HIGH_FA,HIGH_RE,HIGH_FA,HIGH_RE,HIGH_FA,HIGH_RE,HIGH_FA},
		{H_NOISE,L_NOISE,H_NOISE,L_NOISE,H_NOISE,L_NOISE,H_NOISE,L_NOISE,H_NOISE,L_NOISE,H_NOISE},11
	},
};

BeepSound_t frequencyCode;
static void prepareBeepPacket(BeepSound_t *sound){
	uint16_t lenght=sound->lenght;
	for(uint16_t i=0;i<lenght;i++){
		frequencyCode.note[i]=BeepCode[sound->note[i]];
		frequencyCode.volume[i]=sound->volume[i];
	}
}

static void beepUpdateTask(uint16_t commandState){
	static uint16_t musicNum=0;
	BeepSound_t *musicSound;
	musicSound = (BeepSound_t*)aqCalloc(MUSIC_LIST,sizeof(BeepSound_t));
	if(musicNum==0){
		if(commandState==QUIET){
			aqFree(musicSound,MUSIC_LIST,sizeof(BeepSound_t));
			return;
		}
		else{
			*musicSound = beepNewSound[commandState-1];
			prepareBeepPacket(musicSound);
			musicNum++;
		}
	}
	else{
		beepUpdate(frequencyCode.volume[musicNum-1],frequencyCode.note[musicNum-1]);
		musicNum++;
		if(musicNum > musicSound->lenght){
			musicNum=0;
		}
	}
	aqFree(musicSound,MUSIC_LIST,sizeof(BeepSound_t));
}







void rgbLedConfig(void){
	//R
	BSP_GPIO_Init(BSP_GPIOD5,GPIO_Mode_Out_PP);
	//G
	BSP_GPIO_Init(BSP_GPIOD4,GPIO_Mode_Out_PP);
	//B
	BSP_GPIO_Init(BSP_GPIOD6,GPIO_Mode_Out_PP);       
	LED_R = LED_G = LED_B = LED_DISABLE;
}

static void beepConfig(void){
    
	//BSP_TIM_PWM_Init(TIM1,BEEP_LENGTH,BEEP_PRESCALER,NULL,NULL,BSP_GPIOE13,NULL);

	BSP_TIM_PWM_Init(TIM4,1000,83,NULL,NULL,NULL,BSP_GPIOD15);//84M/84=1Mhz的计数频率,重装载值1000，所以PWM频率为 1M/1000=1Khz. 
    
    
	beepUpdate(0,ALTO_DO);
}

static void sightInit(void){
	beepConfig();
	rgbLedConfig();
}


