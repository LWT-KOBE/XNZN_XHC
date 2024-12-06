#include "constant_temp.h"

constantTempSturct_t constantTempData;

float constantTempPidConfig[]={
	150,19,0,0.25f,1000,1000,1000,1000
};

float lowPassFilter(float input, struct LowPassFilterData *filterParameters){
    float output;

    output = filterParameters->gx1 * input +
             filterParameters->gx2 * filterParameters->previousInput -
             filterParameters->gx3 * filterParameters->previousOutput;

    filterParameters->previousInput  = input;
    filterParameters->previousOutput = output;

    return output;
}

static void initTempLowPassFilter(void){
    float a;
    a = 2.0f * 0.03f * 500.0f;
    constantTempData.lowPassFilterData.gx1 = 1.0f / (1.0f + a);
    constantTempData.lowPassFilterData.gx2 = 1.0f / (1.0f + a);
    constantTempData.lowPassFilterData.gx3 = (1.0f - a) / (1.0f + a);
    constantTempData.lowPassFilterData.previousInput  = 0.0f;
    constantTempData.lowPassFilterData.previousOutput = 0.0f;
}

void constanTempUpdate(s16 temp){
	constantTempData.tempreature = lowPassFilter((float)temp/10,&constantTempData.lowPassFilterData);
	if(!(constantTempData.loops % 2)){											//2ms∫„Œ¬“ª¥Œ
		constantTempData.tempOut = pidUpdate(constantTempData.tempPID,CONSTANT_TEMP_VAULE,constantTempData.tempreature,0.002f);
		constantTempData.tempOut = constrainFloat(constantTempData.tempOut,0,1000);	
		CONSTANT_TEMP_CCR = constantTempData.tempOut;
	}
	digitalIncreasing(&constantTempData.loops);
}

void constantTempInit(void){
	initTempLowPassFilter();
	BSP_TIM_PWM_Init(CONSTANT_TEMP_TIM, CONSTANT_TEMP_LENGTH, CONSTANT_TEMP_PRES, NULL, NULL, NULL, CONSTANT_TEMP_PIN);
	constantTempData.tempPID = pidInit(&constantTempPidConfig[0],&constantTempPidConfig[1],&constantTempPidConfig[2],&constantTempPidConfig[3],
														&constantTempPidConfig[4],&constantTempPidConfig[5],&constantTempPidConfig[6],&constantTempPidConfig[7],
														NULL, NULL, NULL, NULL);
}
