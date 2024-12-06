#include "driver_clockCount.h"


static void clockCountInit(void);
deviceInitClass clockClass = {
	clockCountInit,
};


void clockCountInit(void){
	BSP_TIM_INT_Init(CLOCKCOUNT_TIMER, CLOCKCOUNT_PERIOD, CLOCKCOUNT_PRESCALER, CLOCKCOUNT_PRE, CLOCKCOUNT_SUB);	
}
