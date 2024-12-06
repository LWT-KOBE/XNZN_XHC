#include "warning.h"
#include "config.h"
#include "supervisor.h"
#include "shoot.h"

warningStruct_t warningData;

void lightBarsReportErrors(void){																			//用于报错
	uint8_t index = 0;
	uint8_t nowFaultBit = 0; 
	uint16_t remainFaultSate; 
	if((WARNING_STATE & 0x3f) != 0){
		remainFaultSate = (WARNING_STATE & 0x3f);
		while(index < SK6812_LED_STRIP_LENGTH && !warningData.highestFaultFlag){		//用于检测最高一级的错误的
			nowFaultBit = remainFaultSate % 2; 
			if(nowFaultBit){
				warningData.highestFault = index;
				digitalHi(&warningData.highestFaultFlag);
			}
			remainFaultSate = remainFaultSate >> 1;
			index ++;
		}
		digitalLo(&warningData.highestFaultFlag);
	}
	index = 0;
	if(warningData.highestFault != 0){																	//在第一位和最高错误位间来一个渐变效果，用于帮助计数
		while(index <= warningData.highestFault){
			if(index >= warningData.highestFaultLoop)
				setOneLedHsv(index,&SK6812_DARK);
			else
				setOneLedHsv(index,&SK6812_GREEN);
			index ++;
		}
	}
	index = warningData.highestFault;
	remainFaultSate = (WARNING_STATE & 0x3f) >> warningData.highestFault;
	while(index < SK6812_LED_STRIP_LENGTH){
		nowFaultBit = remainFaultSate % 2;
		if(nowFaultBit){
			setOneLedHsv(index,&SK6812_RED);
		}
		else{
			setOneLedHsv(index,&SK6812_GREEN);
		}
		remainFaultSate = remainFaultSate >> 1;
		index ++;
	}
	if(!(warningData.loops % 5)){					//500ms
		digitalIncreasing(&warningData.highestFaultLoop);
		if(warningData.highestFaultLoop > warningData.highestFault)
			digitalClan(&warningData.highestFaultLoop);
	}
	else{
		warningData.highestFault = 0;
	}
}

void lightBarsOfContrl(uint8_t contrlMode,uint8_t safeMode){					//用于控制状态
	uint8_t index = 0;
	uint8_t invertFreq = 0;
	static uint8_t lightBarsSwitch = ENABLE;
	switch(contrlMode){																									//控制模式
		case MANUAL_SINGLE:
			warningData.displayNumber = 1;
			break;
		case MANUAL_CONTINUOUS:
			warningData.displayNumber = 2;
			break;
		case AUTO_CONTINUOUS:
			warningData.displayNumber = 3;
			break;
	}
	switch(safeMode){																										//识别安保等级
		case SAFE:
			warningData.blinkFrequency = 1;
			warningData.displayColor = SK6812_GREEN;
			break;
		/**********无警告模式屏蔽此处*****************
		case WARNING:
			warningData.blinkFrequency = 2;
			warningData.displayColor = SK6812_YELLOW;
			break;
		********************************************/
		case DANGEROUS:
			warningData.blinkFrequency = 5;
			warningData.displayColor = SK6812_RED;
			break;
	}
	invertFreq = (1000 / WARNING_STACK_PERIOD) / warningData.blinkFrequency;
	if(!(warningData.loops % invertFreq)){
		lightBarsSwitch = !lightBarsSwitch;
		if(invertFreq == 10)//安全模式长亮
			lightBarsSwitch = ENABLE;
	}
	if(!lightBarsSwitch){
		warningData.displayColor = SK6812_DARK;
	}
	
	//从左到右显示射频单发，三连发，连发
	while(index < SK6812_LED_STRIP_LENGTH){
		if(index < warningData.displayNumber){
			setOneLedHsv(index,&warningData.displayColor);
		}
		else{
			setOneLedHsv(index,&SK6812_DARK);
		}
		index ++;
	}
	//右侧第三个灯显示扭腰
	if(warningData.aviodFlag)
		warningData.displayColor = SK6812_RED;
	else if(warningData.rotateFlag)
		warningData.displayColor = SK6812_GREEN;
	else
		warningData.displayColor = SK6812_DARK;
	setOneLedHsv(SK6812_LED_STRIP_LENGTH - 3,&warningData.displayColor);
	//右侧第二个灯珠用于显示任务状态
	switch(warningData.currentTask){																	//最左侧和最右侧的灯珠用于显示任务状态
		case R_TASK:
			warningData.displayColor = SK6812_RED;													
			break;	
		case V_TASK:
			warningData.displayColor = SK6812_GREEN;
			break;
		case Z_TASK:
			warningData.displayColor = SK6812_YELLOW;
			break;
		default:
			warningData.displayColor = SK6812_DARK;
			break;
	}
	setOneLedHsv(SK6812_LED_STRIP_LENGTH - 2,&warningData.displayColor);
	//最右边的灯显示电容状态
	//电容充电
	bool CapLight = false;
	if(warningData.chargeFlag){
		if(warningData.capSoc == 0x01){
			//满电长亮
			warningData.displayColor = SK6812_GREEN;
		}
		else if(warningData.capSoc == 0x00){
			static uint8_t index = 0;
			if(!(warningData.loops % 2)){
				if(index){
					warningData.displayColor = SK6812_GREEN;
					index = 0;
				}
				else{
					warningData.displayColor = SK6812_DARK;
					index = 1;
				}	
			}
		}
		CapLight = true;
	}
	//电容放电
	else{
		if(warningData.capSoc == 0x02){
			//电容即将没电，警告
			warningData.displayColor = SK6812_RED;
		}
		else if(warningData.capSoc == 0x03){
			//安全电量
			warningData.displayColor = SK6812_GREEN;
		}
		CapLight = true;
	}
	//连底盘时灯不亮
	if(!warningData.linkCapFlag){
		warningData.displayColor = SK6812_DARK;
		CapLight = true;
	}
	/*if(warningData.judgeDataMask == 0x07){  //记得改回来
		warningData.displayColor = SK6812_GREEN;
		CapLight = true;
	}else{
		CapLight = false;
	}*/
	if(CapLight)
		setOneLedHsv(SK6812_LED_STRIP_LENGTH - 1,&warningData.displayColor);
	if(wiredControlData.cmd.robotMode == MODE_KM && !shootData.FricONflag){
		setAllLedColors(&SK6812_RED);
	}
}

void lightBarsStateUpdata(void){
	if(WARNING_STATE & 0x0080)
		shootData.shootStatusMode = SAFE;
	else if(WARNING_STATE & 0x0100)
		shootData.shootStatusMode = DANGEROUS;
	if(WARNING_STATE & 0x0200)
		warningData.aviodFlag = true;
	else
		warningData.aviodFlag = false;
	
	if(WARNING_STATE & 0x0400)
		warningData.rotateFlag = true;
	else
		warningData.rotateFlag = false;
	
	if(WARNING_STATE & 0x0800)
		warningData.chargeFlag = true;
	else
		warningData.chargeFlag = false;
	
	if(WARNING_STATE & 0x1000)
		warningData.linkCapFlag = true;
	else
		warningData.linkCapFlag = false;
	
	if(WARNING_STATE & 0x2000)
		warningData.currentTask = R_TASK;
	else if(WARNING_STATE & 0x4000)
		warningData.currentTask = V_TASK;
	else if(WARNING_STATE & 0x8000)
		warningData.currentTask = Z_TASK;
	else
		warningData.currentTask = NO_TASK;
}

void lightBarsUpdate(void){																					//灯带状态更新
	lightBarsStateUpdata();
	if(!warningData.reportError){										
		lightBarsReportErrors();																				//没有解锁或按下ctrl显示车身状态
	}
	else{																															//如果在拥有控制权的情况下
		switch(warningData.typeOfRobot){
			case INFANTRY_ID:
				lightBarsOfContrl(shootData.shootMode,shootData.shootStatusMode); 
				break;
			case TANK_ID:
				lightBarsOfContrl(shootData.shootMode,shootData.shootStatusMode);
				break;
			case AUXILIARY_ID:																						//工程车的人机界面还包括登岛界面		
				break;
			case SENTRY_ID:																								//哨兵不用灯带
				break;
			case UAV_ID:
				lightBarsOfContrl(shootData.shootMode,shootData.shootStatusMode);
				break;
		}
	}
	SK6812UpdateStrip();
}

void warningUpdate(void){
  lightBarsUpdate();																								//sk6812 更新
	digitalIncreasing(&warningData.loops);
}
