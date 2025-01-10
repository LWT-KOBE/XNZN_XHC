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
		if(TrainSetingFlag == 1 && ReadDataFlag != 1)//设置
		{
			TrainSetingFlag = 0;
			WPS = 0;//写保护关闭
			delay_ms(2);
			
			if(CaseNum != EEPROM_Read_u16(0))
			{
				EEPROM_Write_u16(0,CaseNum);//2
				delay_ms(5);
			}
			if(Pocket4 != EEPROM_Read_Byte(2))
			{
				EEPROM_Write_Byte(2, Pocket4);// 
				delay_ms(5);
			}
			if(TrainBasketMaxNum != EEPROM_Read_Byte(3))
			{
				EEPROM_Write_Byte(3,TrainBasketMaxNum);
				delay_ms(5);
			}
			if(TrainMode != EEPROM_Read_Byte(4))
			{
				EEPROM_Write_Byte(4,TrainMode);
				delay_ms(5);
			}
			if(HigSpeed != EEPROM_Read_Byte(5))
			{
				EEPROM_Write_Byte(5,HigSpeed);
				delay_ms(5);
			}
			if(MidSpeed != EEPROM_Read_Byte(6))
			{
				EEPROM_Write_Byte(6,MidSpeed);
				delay_ms(5);
			}
			if(LowSpeed != EEPROM_Read_Byte(7))
			{
				EEPROM_Write_Byte(7,LowSpeed);
				delay_ms(5);
			}
			if(OutStationQuicken != EEPROM_Read_Byte(8))
			{
				EEPROM_Write_Byte(8,OutStationQuicken);
				delay_ms(5);
			}
			if(FollowHigSpeed != EEPROM_Read_Byte(9))
			{
				EEPROM_Write_Byte(9,FollowHigSpeed);
				delay_ms(5);
			}
			if(FollowMidSpeed != EEPROM_Read_Byte(10))
			{
				EEPROM_Write_Byte(10,FollowMidSpeed);
				delay_ms(5);
			}
			if(FollowLowSpeed != EEPROM_Read_Byte(11))
			{
				EEPROM_Write_Byte(11,FollowLowSpeed);
				delay_ms(5);
			}
			if(InStationHigtoMidSpeed != EEPROM_Read_Byte(12))
			{
				EEPROM_Write_Byte(12,InStationHigtoMidSpeed);
				delay_ms(5);	
			}
			if(InStationMidtoLowSpeed != EEPROM_Read_Byte(13))
			{
				EEPROM_Write_Byte(13,InStationMidtoLowSpeed);
				delay_ms(5);
			}
			if(TrainMaxNum != EEPROM_Read_Byte(14))
			{
				EEPROM_Write_Byte(14,TrainMaxNum);
				delay_ms(5);
			}
			if(RecycleCaseNum != EEPROM_Read_u16(15))
			{
				EEPROM_Write_u16(15,RecycleCaseNum);//15-16	
				delay_ms(5);
			}
			if(MFContralZS != EEPROM_Read_Byte(17))
			{
				EEPROM_Write_Byte(17,MFContralZS);
				delay_ms(5);
			}
			if(MFContralDS != EEPROM_Read_Byte(18))
			{
				EEPROM_Write_Byte(18,MFContralDS);
				delay_ms(5);
			}
			WPS = 1;
			
			ConfigrationFlag = 1; //设置标志位
			SendUDPDataDelay100ms = 3;
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
