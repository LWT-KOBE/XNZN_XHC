#include "application.h"
#include "usartx.h"
#include "board.h"
XHC_TaskStruct_t XHCData;



XHC_TaskStruct_t* getXHC_Task(){
    return &XHCData;
}

/* 放置初始化 */
void XHC_TaskGlobalInit(void){

//	TIM3_Int_Init(65535, 83); //捕获HALL速度
}

void XHC_TaskUpdateTask(void *Parameters){
	TickType_t xLastWakeTime = xTaskGetTickCount();
	digitalLo(&getXHC_Task()->dataInitFlag);
	while(true){
		vTaskDelayUntil(&xLastWakeTime,XHC_Task_NORMAL_PERIOD);
        //防止重复初始化
		if(!XHCData.dataInitFlag){	
            //所有控制全部初始化            
			XHC_TaskGlobalInit();																																							
			digitalHi(&getXHC_Task()->dataInitFlag);
			
		}
//////////////		
		USART_Data_Send_Task();//串口发送
//////////////	
		if(TrainState == ST2 && WorkOffFlag == 0x55)
		{
			WPS = 0;//写保护关闭
			delay_ms(1);
			EEPROM_Write_Byte(20,0xaa);			
			WPS = 1;	
			delay_ms(1);
			WorkOffFlag = EEPROM_Read_Byte(20);
		}			
///////////////////读电压//////////////////////////////////////////////	
	//	if(Timer40msCount>=40)//
		{
		//	Timer40msCount = 0;			
			ADC_Value = MyADC_GetValue();
			garry_ch0[gGetAdcCounter] = ADC_Value[0]; //电源电压
			gGetAdcCounter++;
			if(gGetAdcCounter >= 10)
			{
				gGetAdcCounter = 0;
			}		
			GetAdcAverage();//10次ADC平均值	
			PowerValueLedShow();	//电量
		}
///////////////////读电压//////////////////////////////////////////////			
		
		digitalIncreasing(&getXHC_Task()->loops);        

	}
}

void XHCDataInit(void){
	getsupervisorData()->taskEvent[XHC_Task] = xTaskCreate(XHC_TaskUpdateTask,"XHC_Task",XHC_Task_STACK_SIZE,NULL,XHC_Task_PRIORITY,&XHCData.xHandleTask);
    //usbVCP_Printf("ControlInit Successfully \r\n");
    
}
