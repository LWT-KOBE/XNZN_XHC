#include "State.h"
#include <stdlib.h>
#include <string.h>
#include "application.h"
#include "BSP_eeprom.h"
u8 Uart2_Rx[UART2_RX_LEN]={0};
u8 Uart2_Tx[UART2_TX_LEN]={0};
u16 Uart2_Rx_length = 0;
u16 Uart2_Tx_length = 0;


u8 Uart1_Rx[UART1_RX_LEN]={0};
u8 Uart1_Tx[UART1_TX_LEN]={0};
u16 Uart1_Rx_length = 0;
u16 Uart1_Tx_length = 0;

u8 TrainWarning = 0;//bit0--前车位置丢失  bit1--电机1异常 bit2--电机2异常  bit3--电机3异常  
										//bit4--电机4异常     bit5--参数错误  bit6--   bit7--

u8 SensorWarning = 0;//bit0--LD1异常  bit1--LD2异常 bit2--LD1B异常  bit3--LD2B异常  bit4--ST1异常
										//bit5--ST2异常  bit6--ST1B异常   bit7--ST2B异常

u16 SensorWarningDelay1,SensorWarningDelay2,SensorWarningDelay3,SensorWarningDelay4,\
	 SensorWarningDelay5,SensorWarningDelay6,SensorWarningDelay7,SensorWarningDelay8;

u8 TrainDircition = 0;//0--前进   1--后退
u8 TrainState = 0;//火车状态 
u8 TrainStateOld = 0;//
u8 StepState = 0;   //1跟随  2进站减速 3申请出站 4出站速度  5手动模式装载速度  7自检  8进站高速减中速  9车厢失联
u8 StepSt0 = 0;
u8 CarGoGoFlag = 0;
u8 PocketStep = 0;
u8 OldTrainState,NewTrainState;
u8 zeroCount = 0;
u8 zeroFlag = 0;
u32 TrainMileage = 0;
u32 OldTrainMileage = 0;
u8 vofa_sendFlag = 0;
u8 InStationLSFlag = 0;//进站提前减速

u8 BasketNumber,DeliverResulBasketNumber = 0;

u8 MoterBrakeFlag,MoterBrakeFlagH = 0;//1--brake
u8 BreakStopFlag = 0;

u8 GoTrainflag = 0;//测试前进按键标志位

u8 TravelCommand = 0;//上位机发车命令  0--无申请  1申请发车
u8 ApplicationAddrFlag = 0;// 0申请地址    1--车厢收到地址
//u8 StopAPPToHeadAddr = 0;//1--停止接收上位机下发的地址

u8 Reply_Result_Flag = 0;
u8 SendDataBuf1[7];		        //发送数据的数组
u8 SendDataBuf2[180];
u8 SendDataBuf3[90];
u8 SendDataBuf4[6];
u8 SendDataBuf5[6];
u8 SendDataBuf6[27];
u8 SendDataBuf7[6];
u8 SendDataBuf8[8];
u8 SendDataBuf9[30];
u8 SendDataBuf10[50];
u8 SendDataBuf11[10];
u8 SendDataBuf12[10];

u8 SendDataBuf13[10];
u8 SendDataBuf14[10];
u8 SendDataBuf15[10];
u8 SendDataBuf16[10];

u8 TimerSendDataCount = 0;//超时发送计时
u8 SendCount = 0;
u8 DelaySendFlag = 0;
u16 CageNumber = 0;//实时格口号

u8 WIFISendDataBuf[150];
u8 WIFIdatalen = 0;

u8 IntermediateVariable1 = 0;
u8 IntermediateVariable2 = 0;

u16 StopLG = 0;
u8 StopLGFlag = 0;
u8 ShieldBasket = 0;
u8 RecycleBasket = 0;
u8 RecycleBasketFlag = 0;
u8 CanSendOutStationFlag = 0;//BIT0-1停止落格 2停止落格解除 3屏蔽车厢  4屏蔽车厢解除  5去回收框
u16 CanSendOutStationDelay = 0;
float MBSpeed = 0;//期望速度
float MBSpeedOld = 0;
u16 MBSpeeddelay = 0;
u8 SendUDPDataDelay200ms = 0;
u8 SendUDPDataDelay100ms = 0;
u8 SendUDPDataDelay50ms,SendUDPDataDelay30ms =0;
u8 HeartDelayCount = 0;

u8 SendCanDataDelayCnt = 0;

u8 WIFISendDataDelayCount = 0;
u8 AlltrainST1Flag = 0;//自检完成标志bit0-7对应1-8车自检状态
u8 trainST1Flag = 0;
u32 CheckSumAPP = 0;
u32 CheckSumAPP1 = 0;
u32 CheckSumAPP2 = 0;
u8 wer = 0;

u8 test121 = 0;
u8 test123 = 0;
u8 test124 = 0;

//#define BEEP_ON  GPIO_SetBits(GPIOA,GPIO_Pin_11)
//#define BEEP_OFF GPIO_ResetBits(GPIOA,GPIO_Pin_11)

void IO_Init(void)
{    	 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIOD时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOE时钟
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);	
	
	//GPIOD2初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//ST2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化GPIO
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//ST1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;//LD1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIO	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;//LD2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO		
	



	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;//ST1B
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIO	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;//ST2B
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIO		
	
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//LD1B
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIO	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;//LD2B
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIO		

	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//MF
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//上拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIO	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//MFB
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//上拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIO	


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;//SW2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO	
	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//SW1
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//上拉
//	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO		



	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;//WIFI连接指示
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO		


//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;//车头编号
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
//	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;//车头编号
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
//	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIO	


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//车头灯
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//下拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO	


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//A_B_MOTOREN
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//下拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;//DIRA
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//下拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIO	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//DIRB
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//下拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIO	

	MOTOR_ENA_B = 0;
	MBSpeed = 0;

	
}


void TrainBusinessLogic (void)
{	
	if(Uart1_Rx[0] == 0xfd)
	{
		if(Uart1_Rx[1] == 0xA1 && TrainState == ST0)//设置  
		{
			CheckSumAPP2 = 0;
			for(u8 c=0;c<21;c++)
			{
				CheckSumAPP2 ^= Uart1_Rx[3+c];
				CheckSumAPP2 = CheckSumAPP2 & 0xff;
			}
			if(CheckSumAPP2 == Uart1_Rx[Uart1_Rx_length-2])
			{					
			//	if(TrainState == ST0)
				WPS = 0;//写保护关闭
				delay_ms(1);
				{
					CaseNum = Uart1_Rx[4]<<8 |Uart1_Rx[5];
					EEPROM_Write_u16(0,CaseNum);//2
					delay_ms(5);
					
					Pocket4 = Uart1_Rx[9];
					EEPROM_Write_Byte(2, Pocket4);// 
					delay_ms(5);
					
//					if(Uart1_Rx[11] == TrainHeadNum || Uart1_Rx[11] ==0xff)//
					{
						TrainBasketMaxNum = Uart1_Rx[8];//
						EEPROM_Write_Byte(3,TrainBasketMaxNum);
						delay_ms(5);
					}
					
					TrainMode = Uart1_Rx[10];
					EEPROM_Write_Byte(4,TrainMode);
					delay_ms(5);
					
					HigSpeed = Uart1_Rx[12];
					EEPROM_Write_Byte(5,HigSpeed);
					delay_ms(5);
						
					MidSpeed = Uart1_Rx[13];
					EEPROM_Write_Byte(6,MidSpeed);
					delay_ms(5);
					
					LowSpeed = Uart1_Rx[14];
					EEPROM_Write_Byte(7,LowSpeed);
					delay_ms(5);	

					OutStationQuicken = Uart1_Rx[11];
					EEPROM_Write_Byte(8,OutStationQuicken);
					delay_ms(5);	
					
					FollowHigSpeed = Uart1_Rx[15];
					EEPROM_Write_Byte(9,FollowHigSpeed);
					delay_ms(5);		

					FollowMidSpeed = Uart1_Rx[16];
					EEPROM_Write_Byte(10,FollowMidSpeed);
					delay_ms(5);	

					FollowLowSpeed = Uart1_Rx[17];
					EEPROM_Write_Byte(11,FollowLowSpeed);
					delay_ms(5);	

					InStationHigtoMidSpeed = Uart1_Rx[19];
					EEPROM_Write_Byte(12,InStationHigtoMidSpeed);
					delay_ms(5);	
					
					InStationMidtoLowSpeed = Uart1_Rx[20];
					EEPROM_Write_Byte(13,InStationMidtoLowSpeed);
					delay_ms(5);	
					
					TrainMaxNum = Uart1_Rx[21];
					EEPROM_Write_Byte(14,TrainMaxNum);
					delay_ms(5);	

					RecycleCaseNum = Uart1_Rx[6]<<8 |Uart1_Rx[7];
					EEPROM_Write_u16(15,RecycleCaseNum);//15-16	
					delay_ms(5);

					MFContralZS = Uart1_Rx[22];
					EEPROM_Write_Byte(17,MFContralZS);
					delay_ms(5);	

					MFContralDS = Uart1_Rx[23];
					EEPROM_Write_Byte(18,MFContralDS);
					delay_ms(5);	

					ConfigrationFlag = 1; //设置标志位
					WPS = 1;
				}
				
			}
		}

	
		
	
		if(Uart1_Rx[3] == TrainHeadNum && TrainMode ==1 && TrainState != 1)//自动扫码模式APP下发地址 
		{	
			if(Uart1_Rx[1] == 0x05 && Uart1_Rx[Uart1_Rx_length-1] == 0xdf)
			{
				CheckSumAPP = 0;
				//for(wer=0;wer<43;wer++)//21
				for(wer=0;wer<TrainBasketMaxNum*2+1;wer++)
        {
					CheckSumAPP ^= Uart1_Rx[3+wer];	
        }
				CheckSumAPP = CheckSumAPP & 0xff;
				if(CheckSumAPP == Uart1_Rx[Uart1_Rx_length-2])
				{
					for(u8 b=0;b<TrainBasketMaxNum;b++)
					{
						if(BasketError[b] ==0)//车厢有故障不接收下发地址
						{
							if((Uart1_Rx[4+2*b]<<8 | Uart1_Rx[5+2*b]) >0 && (Uart1_Rx[4+2*b]<<8 | Uart1_Rx[5+2*b]) < CaseNum)
							{
								APPSendADDRACKFlag =1;
								HeadSendBasketFlag &= ~0x03;
								HeadSendBasketFlag |= 0x01;						
								APPSendADDRFlag |= 1 << b;	
								AppSendAddr[b] = ((b+1)<<16) | (Uart1_Rx[4+2*b]<<8) | (Uart1_Rx[5+2*b]);
								AddrAckState[b] = 1;
							}	
							else if((Uart1_Rx[4+2*b]<<8 | Uart1_Rx[5+2*b]) > CaseNum)
							{
								APPSendADDRACKFlag =1;
								HeadSendBasketFlag &= ~0x03;
								HeadSendBasketFlag |= 0x01;						
								APPSendADDRFlag |= 1 << b;								
								AppSendAddr[b] = ((b+1)<<16) | RecycleCaseNum;
								AddrAckState[b] = 2;
							}
						}
						else
						{
							AddrAckState[b] = 3;
						}
					}
				}					
			}
		}			
	}	
	else if(Uart1_Rx[0] == 0xfc)//获取前车车尾位置
	{
		FrontCarPositionPokec = (u16)((Uart1_Rx[3]<<8) | Uart1_Rx[4]); 
		FrontCarInStationCount = Uart1_Rx[5];
		FrontCarPositionFlag = 1;
		gCheckFroCarLiveCount = 0;
		
	}		
	
	if(Uart1_Rx[0] == 0xfd)
	{
		if(Uart1_Rx[3] == TrainHeadNum)
		{
			if(Uart1_Rx[1] == 0x01 && TrainMode ==0)//人工模式APP下发地址
			{					
				BasketNumber = 0;//收到地址清除申请车厢号  本车厢申请-填充发送数组--收到清除 其实不用清除也可以 下个车厢申请时会覆盖掉  //0620未验证
				if(BasketError[Uart1_Rx[4] -1] ==0)//车厢有故障不接收下发地址
				{		
					CheckSumAPP1 = 0;
					for(u8 b=0;b<4;b++)
					{
						CheckSumAPP1 ^= Uart1_Rx[3+b];
						CheckSumAPP1 = CheckSumAPP1 & 0xff;
					}
					if(CheckSumAPP1 == Uart1_Rx[Uart1_Rx_length-2])
					{						
						AppSendAddr[Uart1_Rx[4] -1] = Uart1_Rx[4] << 16 | Uart1_Rx[5] << 8 | Uart1_Rx[6];//上位机投递信息
						if((AppSendAddr[Uart1_Rx[4] -1] & 0xffff) > 0 && (AppSendAddr[Uart1_Rx[4] -1] & 0xffff) < CaseNum)
						{
							APPSendADDRFlag |= 1 << (Uart1_Rx[4]-1);
							HeadSendBasketFlag &= ~0x03;
							HeadSendBasketFlag |= 0x01;	
						}
						else if((AppSendAddr[Uart1_Rx[4] -1] & 0xffff) == 0 || (AppSendAddr[Uart1_Rx[4] -1] & 0xffff)>CaseNum)
						{
							AppSendAddr[Uart1_Rx[4] -1] = Uart1_Rx[4] << 16 | (CaseNum-1);
							APPSendADDRFlag |= 1 << (Uart1_Rx[4]-1);
							HeadSendBasketFlag &= ~0x03;
							HeadSendBasketFlag |= 0x01;							
						}
					}
					
				}
					
					
//					APPBasketNum = 0;
//					APPBasketNum = Uart1_Rx[11] << 16 | Uart1_Rx[12] << 8 | Uart1_Rx[13];//上位机投递信息	
//					HeadSendBasketFlag &= ~0x03;
//					HeadSendBasketFlag |= 0x01;		
							
			}
//			else if(Uart1_Rx[8] == 0x05)//自动扫码模式APP下发地址
//			{	
//				if(TrainMode ==1)
//				{											
////						AppAddrACK = 0;
////						AppAddrACK = Uart1_Rx[11] << 16 | Uart1_Rx[12] << 8 | Uart1_Rx[13];
//					for(u8 b=0;b<21;b++)
//					{
//						if(BasketError[b] ==0 && (Uart1_Rx[11+2*b] !=0 || Uart1_Rx[12+2*b] !=0))//车厢有故障不接收下发地址
//						{
//							AppSendAddr[b] = (b+1)<<16 | Uart1_Rx[11+2*b]<<8 | Uart1_Rx[12+2*b];
//							APPSendADDRFlag |= 1 << b;
//							
//							APPSendADDRACKFlag =1;
//							HeadSendBasketFlag &= ~0x03;
//							HeadSendBasketFlag |= 0x01;								
//						}
//					}					
//				}
//			}				
			else if(Uart1_Rx[1] == 0x02)//发车指令
			{	
				if(TrainState == ST5)
				{
					APPSendADDRDelay = 0;
					TrainApplyForExitFlag= 0;//收到发车指令清除申请标志位
					CarGoGoFlag = 1;
					memset(&LastDeliverResultNum, 0, 21*sizeof(int));//清除投递结果	
					memset(&LGTimerCntBuff, 0, 21*sizeof(int)); 
					memset(&LastPoketNum, 0, 21*sizeof(unsigned short int));
					memset(&AddrAckState, 0, 21*sizeof(unsigned short int));
					TrainStop &= ~0x10;//
				}	
				
//				if(TrainMode == 1)////241017注释
//				{
//					HeadSendBasketFlag &= ~0x0c;
//					HeadSendBasketFlag |= 0x04;					
//				}
			}
			else if(Uart1_Rx[1] == 0x04)//APP上报结果应答
			{	
				
				DeliverResul2APPtBuff &= ~(0x00000001 << (Uart1_Rx[4]-1));
				
//				HeadSendBasketFlag &= ~0x0c;//241017注释
//				HeadSendBasketFlag |= 0x04;			
			}	
			
			else if(Uart1_Rx[1] == 0x0A)//停止落格指令  全车指令
			{
				StopLGFlag =1;
				CanSendOutStationDelay = 0;
				StopLG = Uart1_Rx[4]<<8 | Uart1_Rx[5];//停止落格位置	
				if(Uart1_Rx[6] == 1)
					CanSendOutStationFlag = 1;
				else
					CanSendOutStationFlag = 2;
			}	
			else if(Uart1_Rx[1] == 0x0B)//APP下发屏蔽指令  不回复从心跳查找是否下发成功
			{
				CanSendOutStationDelay = 0;
				if(Uart1_Rx[5] == 0x01)
				{
					ShieldBasket = Uart1_Rx[4];//屏蔽车厢号
					CanSendOutStationFlag = 3;//1停止落格 2停止落格解除 3屏蔽车厢  4屏蔽车厢解除  5去回收框
				}
				else if(Uart1_Rx[5] == 0x00)
				{
					ShieldBasket = Uart1_Rx[4];//屏蔽车厢号
					CanSendOutStationFlag = 4;//1停止落格 2停止落格解除 3屏蔽车厢  4屏蔽车厢解除  5去回收框					
				}
			}
			else if(Uart1_Rx[1] == 0x0C)//APP下发回收指令
			{
				CanSendOutStationDelay = 0;
				RecycleBasket = Uart1_Rx[4];//回收车厢号
				RecycleBasketFlag = 1;
				CanSendOutStationFlag = 5;//1停止落格 2停止落格解除 3屏蔽车厢  4屏蔽车厢解除  5去回收框
			}	

			else if(Uart1_Rx[1] == 0x0D)//APP下发指定位置停车指令
			{
				ChargePosition = Uart1_Rx[4]<<8 | Uart1_Rx[5];//停车位置
				ChargeFlag = 1;
				ChargeACK = 1;
				StopAddrFlag = 0xff; 
			}	
			
			else if(Uart1_Rx[1] == 0x0E)//立即发车
			{
				if(ChargeFlag == 1)
					TrainState = ST9;
				else
					TrainState = ST6;
				StepState = 4;	
				NowCarGoACK= 1;
			}	
			else if(Uart1_Rx[1] == 0x0F)//暂停
			{
				if(Uart1_Rx[4] == 1)
				{
					TrainRestFlag = 1;
					TrainStop |= 0x80;
				}
				else
				{
					TrainRestFlag = 0;
					TrainStop &= ~0x80;
				}
				TrainRestACK = 1;
			}			
			
			
		}
		
		if(Uart1_Rx[1] == 0x03)//收到自检指令
		{
			AlltrainST1Flag =0;
			trainST1Flag = 0;
			ST1ACKFlag = 1;
			TrainState = ST1;//自检
			CarScranFlag = 0;//恢复
			StionStop =0;
			CageNumber = 0;
			Pocket_A_Count = 0;
			Pocket_A_Count1 = 0;
			Pocket_A_Count2 = 0;			
			InStationCount = 0;
			
			TrainApplyForExitFlag= 0;InStationFlag = 0;
			StepSt0 = 0;
			
		}
		
		else if(Uart1_Rx[1] == 0x00)//进站指令应答
		{
			if(Uart1_Rx[3] == TrainHeadNum)
			{	
				InStationFlag = 0;
				test123 = 1;
			}
		}
		
		else if(Uart1_Rx[1] == 0x06)//急停
		{
//			if(Uart1_Rx[3] == 0x01)
			{
				CarScranFlag = 1;//停车
				AlltrainST1Flag =0;
				trainST1Flag = 0;
				InStationCount = 0;
				TrainRestFlag = 0;
				StepSt0 = 0;
				
				ChargeFlag = 0;
				
				//241010
				WPS = 0;//写保护关闭
				delay_ms(1);
				WorkOffFlag = 0x55;//正常下班标志
				EEPROM_Write_Byte(20,WorkOffFlag);
				WPS = 1;
					
				TrainStop |= 0x20;
				LD_Step = 0;
			}
		}
	


		else if(Uart1_Rx[1] == 0xA2)//读取参数
		{
		//	if(Uart1_Rx[3] == TrainHeadNum)
			{
				ReadDataFlag = 1;
			}	
		}
	
	}
		
	


	
	

	/* 遥控器指令 */

	if(Uart1_Rx[0] == 0xfe && Uart1_Rx[1] == 0xb1)
	{
		if(Uart1_Rx[4] == 1 && Uart1_Rx[5] == 1 && TrainHeadNum >= 1 && TrainHeadNum <= 10 )//急停1
		{
			
			CarScranFlag = 1;//停车
			AlltrainST1Flag =0;
			trainST1Flag = 0;
			InStationCount = 0;
			TrainRestFlag = 0;
			StepSt0 = 0;
			
			ChargeFlag = 0;
			
			//241010
			WPS = 0;//写保护关闭
			delay_ms(1);
			WorkOffFlag = 0x55;//正常下班标志
			EEPROM_Write_Byte(20,WorkOffFlag);
			WPS = 1;
			TrainStop |= 0x04;
			LD_Step = 0;
		}
		else if(Uart1_Rx[4] == 2 && Uart1_Rx[5] == 1  && TrainHeadNum >= 11 && TrainHeadNum <= 20 )//急停2
		{
			CarScranFlag = 1;//停车
			AlltrainST1Flag =0;
			trainST1Flag = 0;
			InStationCount = 0;
			TrainRestFlag = 0;
			StepSt0 = 0;
			
			ChargeFlag = 0;
			
			//241010
			WPS = 0;//写保护关闭
			delay_ms(1);
			WorkOffFlag = 0x55;//正常下班标志
			EEPROM_Write_Byte(20,WorkOffFlag);
			WPS = 1;
			TrainStop |= 0x04;
			LD_Step = 0;
		}		
		
	 
		if(Uart1_Rx[4] == 1 && Uart1_Rx[5] == 2  && TrainHeadNum >= 1 && TrainHeadNum <= 10 && TrainState < ST2)//自检1
		{
			if(Uart1_Rx[3] == TrainHeadNum)
			{
				AlltrainST1Flag =0;
				trainST1Flag = 0;
				ST1ACKFlag = 1;
				TrainState = ST1;//自检
				CarScranFlag = 0;//恢复
				StionStop =0;
				CageNumber = 0;
				Pocket_A_Count = 0;
				Pocket_A_Count1 = 0;
				Pocket_A_Count2 = 0;			
				InStationCount = 0;
				StepSt0 = 0;
				TrainApplyForExitFlag= 0;InStationFlag = 0;	
			}
			else if(Uart1_Rx[3] == 21)
			{
				AlltrainST1Flag =0;
				trainST1Flag = 0;
				ST1ACKFlag = 1;
				TrainState = ST1;//自检
				CarScranFlag = 0;//恢复
				StionStop =0;
				CageNumber = 0;
				Pocket_A_Count = 0;
				Pocket_A_Count1 = 0;
				Pocket_A_Count2 = 0;			
				InStationCount = 0;
				StepSt0 = 0;
				TrainApplyForExitFlag= 0;InStationFlag = 0;	
			}
		}
	  else if(Uart1_Rx[4] == 2 && Uart1_Rx[5] == 2  && TrainHeadNum >= 11 && TrainHeadNum <= 20 && TrainState < ST2)//自检2
		{
			if(Uart1_Rx[3] == TrainHeadNum)
			{
				AlltrainST1Flag =0;
				trainST1Flag = 0;
				ST1ACKFlag = 1;
				TrainState = ST1;//自检
				CarScranFlag = 0;//恢复
				StionStop =0;
				CageNumber = 0;
				Pocket_A_Count = 0;
				Pocket_A_Count1 = 0;
				Pocket_A_Count2 = 0;			
				InStationCount = 0;
				StepSt0 = 0;
				TrainApplyForExitFlag= 0;InStationFlag = 0;	
			}
			else if(Uart1_Rx[3] == 21)
			{
				AlltrainST1Flag =0;
				trainST1Flag = 0;
				ST1ACKFlag = 1;
				TrainState = ST1;//自检
				CarScranFlag = 0;//恢复
				StionStop =0;
				CageNumber = 0;
				Pocket_A_Count = 0;
				Pocket_A_Count1 = 0;
				Pocket_A_Count2 = 0;			
				InStationCount = 0;
				StepSt0 = 0;
				TrainApplyForExitFlag= 0;InStationFlag = 0;	
			}
		}		
		
		
	
		if(TrainState == ST0 && Uart1_Rx[4] == 1 && TrainHeadNum >= 1 && TrainHeadNum <= 10)
		{
			if(Uart1_Rx[5] == 3)//停车高
			{
				StepSt0 = 1;
				CarScranFlag = 0;//恢复
			}		
			else if(Uart1_Rx[5] == 4)//停车中
			{
				StepSt0 = 2;
				CarScranFlag = 0;//恢复
			}		
			else if(Uart1_Rx[5] == 5)//停车低
			{
				StepSt0 = 3;
				CarScranFlag = 0;//恢复
			}			
		}

		if(TrainState == ST0 && Uart1_Rx[4] == 2  && TrainHeadNum >= 11 && TrainHeadNum <= 20 )
		{
			if(Uart1_Rx[5] == 3)//停车高
			{
				StepSt0 = 1;
				CarScranFlag = 0;//恢复
			}		
			else if(Uart1_Rx[5] == 4)//停车中
			{
				StepSt0 = 2;
				CarScranFlag = 0;//恢复
			}		
			else if(Uart1_Rx[5] == 5)//停车低
			{
				StepSt0 = 3;
				CarScranFlag = 0;//恢复
			}			
		}			
		
		
	}
	
	else if(Uart1_Rx[0] == 0xfe && Uart1_Rx[1] == 0xb2)
	{
		if(Uart1_Rx[4] == 1 && TrainHeadNum >= 1 && TrainHeadNum <= 10 )//按键急停1
		{
			CarScranFlag = 1;//停车
			AlltrainST1Flag =0;
			trainST1Flag = 0;
			InStationCount = 0;
			TrainRestFlag = 0;
			StepSt0 = 0;
			
			ChargeFlag = 0;
			
			//241010
			WPS = 0;//写保护关闭
			delay_ms(1);
			WorkOffFlag = 0x55;//正常下班标志
			EEPROM_Write_Byte(20,WorkOffFlag);
			WPS = 1;
			TrainStop |= 0x20;
			LD_Step = 0;
		}
		else if(Uart1_Rx[4] == 2 && TrainHeadNum >= 11 && TrainHeadNum <= 20 )//按键急停2
		{
			CarScranFlag = 1;//停车
			AlltrainST1Flag =0;
			trainST1Flag = 0;
			InStationCount = 0;
			TrainRestFlag = 0;
			StepSt0 = 0;
			
			ChargeFlag = 0;
			
			//241010
			WPS = 0;//写保护关闭
			delay_ms(1);
			WorkOffFlag = 0x55;//正常下班标志
			EEPROM_Write_Byte(20,WorkOffFlag);
			WPS = 1;
			TrainStop |= 0x20;
			LD_Step = 0;
		}		
			
	}
	
//	//////////////////////////测试指令//////////////////////////
//    if(Uart1_Rx[15] == 0xA1)//测试指令       
//    {	
//			
//			if(Uart1_Rx[16] == 0x00)//0停止
//			{
//				
//				MOTOR_ENA_B = 0;
//				MBSpeed = 0;
//				MoterBrakeFlag = 1;//刹车
//			}
//			else if(Uart1_Rx[16] == 0x01)//1前进
//			{
////				CarGo = 0;//0--前进
////				MBSpeed = 150;BreakStopFlag = 0;
//				
////				MOTOR_ENA_B = 1;							
//				
//				TrainState = ST1;
//				CarScranFlag = 0;
//				
//			}
//			
//			else if(Uart1_Rx[16] == 0x02)//2后退
//			{
////				CarGo = 1;//1--后退
////				MBSpeed = 100;BreakStopFlag = 0;
////				MOTOR_ENA_B = 1;				
//			}		
//		
//		}	
////////////////////////////测试指令//////////////////////////		
//		
}





u8 JT808DataPackageSend(u8 *Databuff,u8 type,size_t DataLen) 
{
  u8 i = 0;	
//  u32 CheckSum=0;	
	WIFIdatalen = DataLen;
  if(DataLen > 150)
  {
    return 0;
  }	
  for(i=0;i<DataLen;i++)
  {
    WIFISendDataBuf[i]= Databuff[i];		
  }	
	
	WIFIdataSend(&WIFISendDataBuf[0],WIFIdatalen);
////////////////转义处理///////////////////////	
//  for(y = 0; y < DataLen; y++)
//	{
//		u8 ii,yy = 0;
//		if(y > 1 && y < DataLen-1)
//		{		
//			if(Databuff[y] == FLAG_HEAD)
//			{
//				for(ii=0;ii<DataLen-y-1;ii++)
//				{
//					Databuff[DataLen -ii] = Databuff[DataLen-ii-1];
//				}
//				Databuff[y] = 0x7D;
//				Databuff[y+1] = 0x02; 
//				DataLen++;
//			} 
//			else if(Databuff[y] == 0x7D)
//			{
//				for(yy=0;yy<DataLen-y-1;yy++)
//				{
//					Databuff[DataLen -yy] = Databuff[DataLen-yy-1];
//				}			
//				Databuff[y] = 0x7D;
//				Databuff[y+1] = 0x01;
//				DataLen++;
//			}
//		}
//  }	
	
////////////////转义处理///////////////////////

	//	printf("AT+CIPSEND=%d\r\n",15+DataLen);	
//	if(WIFISendDataFlag  == 0 )
//	{
//		WIFISendDataDelayCount = 0;
//		WIFISendDataFlag = 1;
//		
//		
//	}
	
//	while(WIFISendDataFlag ==1)
//	{
//		WIFISendDataDelayCount++;
////		if(WIFISendDataDelayCount >= 100)
////		{
////			WIFISendDataFlag =2;
////		}
//			
//	}
	
//	if(WIFISendDataFlag ==2)
//	{
	
//		WIFISendDataFlag = 0;
//	}	
	
  return 1;	
	
}

u8 WIFIdataSend(u8 *Databuff,u8 DataLen) 
{
	
		for(u8 i=0;i<DataLen;i++)
		{
			while((USART2->SR&0X40)==0);//循环发送,直到发送完毕
			USART_SendData(USART2,Databuff[i]);  
	//		if(HeartDelayCount >= 5)
			{
				while((USART3->SR&0X40)==0);//循环发送,直到发送完毕
				USART_SendData(USART3,Databuff[i]);
			}				
		}	
	return 1;
}


u8 DataPackageSend1(u8 *Databuff,u8 type,u8 DataLen) 
{
  u8 datbuf[60]={0};
  u8 i = 0;	
  u32 CheckSum=0;		
  if(DataLen > 60)
  {
    return 0;
  }	
  datbuf[0] = 0xFE;              //数据第一个字节 0xFe
	datbuf[1] = type;
	datbuf[2] = DataLen;
  for(i=0;i<DataLen;i++)
  {
    datbuf[i+3]= Databuff[i];		
    CheckSum += Databuff[i]; 
  }	
  //防止数据个数不同都能通信成功
 // CheckSum = DataLen+2+CheckSum;
	
  datbuf[3+DataLen] = CheckSum ^ 0xFF;
  datbuf[4+DataLen] = 0xEF;

  for(i=0;i<DataLen+5;i++)
  {
      while((USART1->SR&0X40)==0);//循环发送,直到发送完毕
      USART_SendData(USART1,datbuf[i]);          
  }
  return 1;	
	
}





void ApplicationCommandUart1 (u8 Type,u8 DataLen)//申请地址
{	

////////////////HEAD/////////////////	
	SendDataBuf1[0] = 0xfe;
	SendDataBuf1[1] = Type;
	SendDataBuf1[2] = DataLen;

////////////////V////////////////////	
	SendDataBuf1[0+3] = TrainHeadNum;//车号
	SendDataBuf1[1+3] = BasketNumber;// 车厢号
	
////////////////TAIL/////////////////	
	SendDataBuf1[3+DataLen] = 0x11;
	SendDataBuf1[4+DataLen] = 0xef;	
	
  JT808DataPackageSend(&SendDataBuf1[0],Type,DataLen+5);	 
}



void ApplyForExit(u8 Type,u8 DataLen)//出站申请上报
{
	u8 i = 0;	
////////////////HEAD/////////////////	
	SendDataBuf2[0] = 0xfe;
	SendDataBuf2[1] = Type;
	SendDataBuf2[2] = DataLen;

////////////////V////////////////////	
	SendDataBuf2[0+3] = TrainHeadNum;
	for(i=0;i<TrainBasketMaxNum;i++)
  {
		SendDataBuf2[1+3+i*7] = ((LastDeliverResultNum[i]&0x0f00)>>8);
		SendDataBuf2[2+3+i*7] = LastDeliverResultNum[i] & 0xff;
		SendDataBuf2[3+3+i*7] = BasketError[i];
		SendDataBuf2[4+3+i*7] =	(LastPoketNum[i]&0xff00)>>8;
		SendDataBuf2[5+3+i*7] =	LastPoketNum[i] & 0xff;
		SendDataBuf2[6+3+i*7] =	(LGTimerCntBuff[i]&0xff00)>>8;
		SendDataBuf2[7+3+i*7] =	LGTimerCntBuff[i] & 0xff;			
	}		
	
	 
////////////////TAIL/////////////////	
	SendDataBuf2[3+DataLen] = 0x11;
	SendDataBuf2[4+DataLen] = 0xef;		
	
  JT808DataPackageSend(&SendDataBuf2[0],Type,DataLen+5);	
	
}


void HeartToApp (u8 Type,u8 DataLen)//心跳包
{
	u8 CheckSum = 0;
////////////////HEAD/////////////////	
	SendDataBuf3[0] = 0xfe;
	SendDataBuf3[1] = Type;
	SendDataBuf3[2] = DataLen;	
	
////////////////V////////////////////		
	SendDataBuf3[0+3] = TrainHeadNum;
	SendDataBuf3[1+3] = PocketCount <<8;
	SendDataBuf3[2+3] = PocketCount;
//	SendDataBuf3[1+3] = CarPositionPocket <<8;
//	SendDataBuf3[2+3] = CarPositionPocket;	
	
	SendDataBuf3[3+3] = gPowerValueFloat*2;
	SendDataBuf3[4+3] = gSpeedR;
	SendDataBuf3[5+3] = TrainWarning;  //
	SendDataBuf3[6+3] = TrainState;
	SendDataBuf3[7+3] = ((TrainVersion&0xf00)>>4) | (NFCMode << 2) | (DriveMode<<1) | (TrainMode);//版本号+模式
	SendDataBuf3[8+3] = TrainVersion;//版本号		
	SendDataBuf3[9+3] = TrainBasketMaxNum;
	
	
////////////////////////////////////
	SendDataBuf3[10+3] = gSpeedRA;
	SendDataBuf3[11+3] = gSpeedRB;
	SendDataBuf3[12+3] = Pocket_A_Count;
	SendDataBuf3[13+3] = PIDA.PWM*0.03;
	SendDataBuf3[14+3] = CaseNum>>8;
	SendDataBuf3[15+3] = CaseNum;
	SendDataBuf3[16+3] = TrainMaxNum;
	SendDataBuf3[17+3] = HigSpeed;
	SendDataBuf3[18+3] = MidSpeed;
	SendDataBuf3[19+3] = LowSpeed;
	
	SendDataBuf3[20+3] = MFContralZS ;
	SendDataBuf3[21+3] = MFContralDS ;
	SendDataBuf3[22+3] = InStationHigtoMidSpeed ;
	SendDataBuf3[23+3] = InStationMidtoLowSpeed ;
	
	SendDataBuf3[24+3] = Pocket4 ;
	SendDataBuf3[25+3] = FollowHigSpeed ;
	SendDataBuf3[26+3] = FollowMidSpeed;
	SendDataBuf3[27+3] = FollowLowSpeed ;
	SendDataBuf3[28+3] = OutStationQuicken;

	SendDataBuf3[29+3] = SensorWarning;
	SendDataBuf3[30+3] = StepState;

	SendDataBuf3[31+3] = gSpeedRC;
	SendDataBuf3[32+3] = gSpeedRD;
	SendDataBuf3[33+3] = TrainStop;
	SendDataBuf3[34+3] = 0;	
////////////////////////////////////	
	
	
	
	for(u8 i=0;i<TrainBasketMaxNum;i++)
  {
		SendDataBuf3[35+3+i] = (BasketError[i]);
  }	

	
	
  for(u8 i=0;i<DataLen;i++)
  {		
    CheckSum ^= SendDataBuf3[i+3]; 
  }		
	
////////////////TAIL/////////////////	
	SendDataBuf3[3+DataLen] = CheckSum;
	SendDataBuf3[4+DataLen] = 0xef;		
	
	JT808DataPackageSend(&SendDataBuf3[0],Type,DataLen+5);	
}

void ST1ACKToApp (u8 Type,u8 DataLen)//自检指令应答 3
{
////////////////HEAD/////////////////	
	SendDataBuf4[0] = 0xfe;
	SendDataBuf4[1] = Type;
	SendDataBuf4[2] = DataLen;

////////////////V////////////////////		
	SendDataBuf4[0+3] = TrainHeadNum;

////////////////TAIL/////////////////	
	SendDataBuf4[3+DataLen] = 0x11;
	SendDataBuf4[4+DataLen] = 0xef;		
	
	JT808DataPackageSend(&SendDataBuf4[0],Type,DataLen+5);	
}

void  ApplyForExitToAPP(u8 Type,u8 DataLen)//申请出站指令
{
	
////////////////HEAD/////////////////	
	SendDataBuf5[0] = 0xfe;
	SendDataBuf5[1] = Type;
	SendDataBuf5[2] = DataLen;

////////////////V////////////////////		
	SendDataBuf5[0+3] = TrainHeadNum;

////////////////TAIL/////////////////	
	SendDataBuf5[3+DataLen] = 0x11;
	SendDataBuf5[4+DataLen] = 0xef;		

	
	JT808DataPackageSend(&SendDataBuf5[0],Type,DataLen+5);	
}




void ReadDataACK(u8 Type,u8 DataLen)
{

	ReadE2promData();//上电读取参数	

////////////////HEAD/////////////////	
	SendDataBuf6[0] = 0xfe;
	SendDataBuf6[1] = Type;
	SendDataBuf6[2] = DataLen;	

////////////////V////////////////////		
	SendDataBuf6[0+3] = TrainHeadNum;
	SendDataBuf6[1+3] = CaseNum>>8;
	SendDataBuf6[2+3] = CaseNum;
	SendDataBuf6[3+3] = RecycleCaseNum>>8;
	SendDataBuf6[4+3] = RecycleCaseNum;
	SendDataBuf6[5+3] = TrainBasketMaxNum;
	SendDataBuf6[6+3] = Pocket4;
	SendDataBuf6[7+3] = TrainMode;
	SendDataBuf6[8+3] = OutStationQuicken;
	SendDataBuf6[9+3] = HigSpeed;
	SendDataBuf6[10+3] = MidSpeed;
	SendDataBuf6[11+3] = LowSpeed;
	SendDataBuf6[12+3] = FollowHigSpeed;
	SendDataBuf6[13+3] = FollowMidSpeed;
	SendDataBuf6[14+3] = FollowLowSpeed;
	SendDataBuf6[15+3] = 1;
	SendDataBuf6[16+3] = InStationHigtoMidSpeed;
	SendDataBuf6[17+3] = InStationMidtoLowSpeed;
	SendDataBuf6[18+3] = TrainMaxNum;
	SendDataBuf6[19+3] = MFContralZS;
	SendDataBuf6[20+3] = MFContralDS;
//	
////////////////TAIL/////////////////	
	SendDataBuf6[3+DataLen] = 0x11;
	SendDataBuf6[4+DataLen] = 0xef;	
	
	JT808DataPackageSend(&SendDataBuf6[0],Type,DataLen+5);
}

void  RecycleACKToAPP(u8 Type,u8 DataLen)//回收指令应答
{
////////////////HEAD/////////////////	
	SendDataBuf7[0] = 0xfe;
	SendDataBuf7[1] = Type;
	SendDataBuf7[2] = DataLen;	

////////////////V////////////////////		
	SendDataBuf7[0+3] = TrainHeadNum;
	
////////////////TAIL/////////////////	
	SendDataBuf7[3+DataLen] = 0x11;
	SendDataBuf7[4+DataLen] = 0xef;		
	JT808DataPackageSend(&SendDataBuf7[0],Type,DataLen+5);	
}


void  InStationToAPP(u8 Type,u8 DataLen)//进站上报
{
////////////////HEAD/////////////////	
	SendDataBuf8[0] = 0xfe;
	SendDataBuf8[1] = Type;
	SendDataBuf8[2] = DataLen;	

////////////////V////////////////////		
	SendDataBuf8[0+3] = TrainHeadNum;
	SendDataBuf8[1+3] = PocketCount>>8;
	SendDataBuf8[2+3] = PocketCount;
	
////////////////TAIL/////////////////	
	SendDataBuf8[3+DataLen] = 0x11;
	SendDataBuf8[4+DataLen] = 0xef;		
	JT808DataPackageSend(&SendDataBuf8[0],Type,DataLen+5);
	test121++;
	if(test123 ==1)
	{
		test124 = 1;
	}
}


void DeliverResultToApp (u8 Type,u8 DataLen,u8 Number)//上报投递结果
{
////////////////HEAD/////////////////	
	SendDataBuf9[0] = 0xfe;
	SendDataBuf9[1] = Type;
	SendDataBuf9[2] = DataLen;

////////////////V////////////////////	
	SendDataBuf9[0+3] = TrainHeadNum;
	SendDataBuf9[1+3] = DeliverResulBasketNumber;	
	
	SendDataBuf9[2+3] = ((LastDeliverResultNum[Number]&0x0f00)>>4);
	SendDataBuf9[3+3] = LastDeliverResultNum[Number] & 0xff;
	SendDataBuf9[4+3] = BasketError[Number];	
	
	SendDataBuf9[5+3] =	(LastPoketNum[Number]&0xff00)>>8;
	SendDataBuf9[6+3] =	LastPoketNum[Number] & 0xff;
	SendDataBuf9[7+3] =	(LGTimerCntBuff[Number]&0xff00)>>8;
	SendDataBuf9[8+3] =	LGTimerCntBuff[Number] & 0xff;		
	

	
	
	
	
////////////////TAIL/////////////////	
	SendDataBuf9[3+DataLen] = 0x11;
	SendDataBuf9[4+DataLen] = 0xef;		
	
	JT808DataPackageSend(&SendDataBuf9[0],Type,DataLen+5);
}


void ADDRACKToApp (u8 Type,u8 DataLen)//下发地址应答 3
{
////////////////HEAD/////////////////	
	SendDataBuf10[0] = 0xfe;
	SendDataBuf10[1] = Type;
	SendDataBuf10[2] = DataLen;

////////////////V////////////////////		
	SendDataBuf10[0+3] = TrainHeadNum;
	SendDataBuf10[1+3] = BasketReciveAddrFlag>>16;
	SendDataBuf10[2+3] = BasketReciveAddrFlag>>8;
	SendDataBuf10[3+3] = BasketReciveAddrFlag;
	
	for(u8 i = 0;i<TrainBasketMaxNum;i++)
	{
		SendDataBuf10[4+3+i] = AddrAckState[i];
	}
	
////////////////TAIL/////////////////	
	SendDataBuf10[3+DataLen] = 0x11;
	SendDataBuf10[4+DataLen] = 0xef;		
	
	JT808DataPackageSend(&SendDataBuf10[0],Type,DataLen+5);	
}

void ConfigrationAck(u8 Type,u8 DataLen)
{
////////////////HEAD/////////////////	
	SendDataBuf11[0] = 0xfe;
	SendDataBuf11[1] = Type;
	SendDataBuf11[2] = DataLen;
	
////////////////V////////////////////		
	SendDataBuf11[0+3] = TrainHeadNum;
	
////////////////TAIL/////////////////	
	SendDataBuf11[3+DataLen] = 0x11;
	SendDataBuf11[4+DataLen] = 0xef;	

	JT808DataPackageSend(&SendDataBuf11[0],Type,DataLen+5);
}


void RecyAck(u8 Type,u8 DataLen)
{
////////////////HEAD/////////////////	
	SendDataBuf12[0] = 0xfe;
	SendDataBuf12[1] = Type;
	SendDataBuf12[2] = DataLen;
	
////////////////V////////////////////		
	SendDataBuf12[0+3] = TrainHeadNum;//车号
	SendDataBuf12[1+3] = RecycleBasket;//车厢号
	
////////////////TAIL/////////////////	
	SendDataBuf12[3+DataLen] = 0x11;
	SendDataBuf12[4+DataLen] = 0xef;	

	JT808DataPackageSend(&SendDataBuf12[0],Type,DataLen+5);
}

void BanStationAck(u8 Type,u8 DataLen)
{
////////////////HEAD/////////////////	
	SendDataBuf16[0] = 0xfe;
	SendDataBuf16[1] = Type;
	SendDataBuf16[2] = DataLen;
	
////////////////V////////////////////		
	SendDataBuf16[0+3] = TrainHeadNum;//车号	
////////////////TAIL/////////////////	
	SendDataBuf16[3+DataLen] = 0x11;
	SendDataBuf16[4+DataLen] = 0xef;	

	JT808DataPackageSend(&SendDataBuf16[0],Type,DataLen+5);
}

//新增指令

void ChargeACKData(u8 Type,u8 DataLen)//指定位置停车应答
{
////////////////HEAD/////////////////	
	SendDataBuf13[0] = 0xfe;
	SendDataBuf13[1] = Type;
	SendDataBuf13[2] = DataLen;
	
////////////////V////////////////////		
	SendDataBuf13[0+3] = TrainHeadNum;//车号
	SendDataBuf13[1+3] = StopAddrFlag;//
	
////////////////TAIL/////////////////	
	SendDataBuf13[3+DataLen] = 0x11;
	SendDataBuf13[4+DataLen] = 0xef;	

	JT808DataPackageSend(&SendDataBuf13[0],Type,DataLen+5);
}

void TrainGoACK(u8 Type,u8 DataLen)//立即发车应答
{
////////////////HEAD/////////////////	
	SendDataBuf14[0] = 0xfe;
	SendDataBuf14[1] = Type;
	SendDataBuf14[2] = DataLen;
	
////////////////V////////////////////		
	SendDataBuf14[0+3] = TrainHeadNum;//车号
	
////////////////TAIL/////////////////	
	SendDataBuf14[3+DataLen] = 0x11;
	SendDataBuf14[4+DataLen] = 0xef;	

	JT808DataPackageSend(&SendDataBuf14[0],Type,DataLen+5);
}



void CarRestACK(u8 Type,u8 DataLen)//临停指令应答
{
////////////////HEAD/////////////////	
	SendDataBuf15[0] = 0xfe;
	SendDataBuf15[1] = Type;
	SendDataBuf15[2] = DataLen;
	
////////////////V////////////////////		
	SendDataBuf15[0+3] = TrainHeadNum;//车号
	SendDataBuf15[1+3] = TrainRestFlag;//
	
////////////////TAIL/////////////////	
	SendDataBuf15[3+DataLen] = 0x11;
	SendDataBuf15[4+DataLen] = 0xef;	

	JT808DataPackageSend(&SendDataBuf15[0],Type,DataLen+5);
}

//新增指令


void TrainFollowTrain(void)
{
	if(FrontCarPositionPokec >= CageNumber)	//在本车前面
	{
		CarDistance = FrontCarPositionPokec - CageNumber;
	}
	else if(CageNumber > FrontCarPositionPokec)//在本车后面
	{
		CarDistance = FrontCarPositionPokec + CaseNum - CageNumber;//
	}

////////////////////////////////////前后车距离////////////////////////////////////////////////////		
		
////////////////////////////////////跟随//////////////////////////////////////////////////////	
//	if(CageNumber >=OutStationQuicken)
//	if(FrontCarPositionFlag == 1 && TrailPositionFlag == 1)	
	if(FrontCarPositionFlag == 1)	
	{
		if(MF_SW == 0)//漫反射触发立即刹车
		{
			MOTOR_ENA_B = 0;
			MoterBrakeFlag = 1;//刹车
			MBSpeed = 0;
		}
		else//跟随		
		{
			if(CarDistance>0 && CarDistance<=3)//距离太近刹车 ---后续是否考虑不刹车
			{
				MoterBrakeFlag = 1;//刹车
				MBSpeed = 0;	
				MOTOR_ENA_B = 0;
			}		
			else if(CarDistance>3 && CarDistance<=FollowLowSpeed)//  滑行
			{
				MBSpeed = 60;
				BreakStopFlag = 0;
				MOTOR_ENA_B = 1;				
			}
			else if(CarDistance>FollowLowSpeed && CarDistance<=FollowMidSpeed)//  低速
			{
				MBSpeed = LowSpeed;
				BreakStopFlag = 0;
				MOTOR_ENA_B = 1;				
			}		
			else if(CarDistance>FollowMidSpeed && CarDistance<=FollowHigSpeed)//  中速
			{
				MBSpeed = MidSpeed;
				BreakStopFlag = 0;
				MOTOR_ENA_B = 1;				
			}				
			else if(CarDistance>FollowHigSpeed)//
			{
				MBSpeed = HigSpeed;
				BreakStopFlag = 0;
				MOTOR_ENA_B = 1;				
			}	
		}			
	}
	else
	{
		if(CarScranFlag == 0x01 || TrainRestFlag ==1 || BoxesDieStop == 1 || CarInStationDieStop == 1)//网关紧急停车
		{
			MOTOR_ENA_B = 0;
			MBSpeed = 0;
			MoterBrakeFlag = 1;//急刹
		}	
		else
		{
			if(MF_SW == 0)//漫反射触发立即刹车
			{
				MOTOR_ENA_B = 0;
				MoterBrakeFlag = 1;//刹车
				MBSpeed = 0;
			}
			else//跟随
			{
				MBSpeed = 120;
				BreakStopFlag = 0;
				MOTOR_ENA_B = 1;
			}	
		}			
	}



////////////////////////////////////跟随//////////////////////////////////////////////////////			
	
//		IDMaxNum = CaseNum + ApplicationtTavel;
//		CarPositionPocket = Pocket_A_Count + CageNumber; //挡片数+接近挡片	
	
///////////根据前车车头位置计算车尾位置//////////////////////////////////////////
//		if(TrainBasketMaxNum < FrontCarPositionHead)
//		{
//			FrontCarPositionPokec = FrontCarPositionHead - TrainBasketMaxNum;				
//		}
//		else
//		{
//			FrontCarPositionPokec = IDMaxNum - (TrainBasketMaxNum - FrontCarPositionHead);
//		}
///////////根据前车车头位置计算车尾位置//////////////////////////////////////////		
	
////////////////////////////////////前后车距离////////////////////////////////////////////////////

//		if(FrontCarPositionNFC >= NFCNUM) //在本车前面，前车未超过最大格口号，超过则认为在本车后面
//		{
//			CarDistance = FrontCarPositionNFC - NFCNUM;		
//		}
//		else//在本车后面
//		{
//			CarDistance = IDMaxNum - NFCNUM + FrontCarPositionNFC;	
//		}		
		
}


void ReadE2promData(void)//读取设置参数
{	
	
	CaseNum = EEPROM_Read_u16(0);
	Pocket4 = EEPROM_Read_Byte(2);
	TrainBasketMaxNum = EEPROM_Read_Byte(3);
	if(TrainBasketMaxNum >21)TrainBasketMaxNum = 1;
	TrainMode = EEPROM_Read_Byte(4);
	HigSpeed = EEPROM_Read_Byte(5);
	MidSpeed = EEPROM_Read_Byte(6);
	LowSpeed = EEPROM_Read_Byte(7);
	OutStationQuicken = EEPROM_Read_Byte(8);
	FollowHigSpeed = EEPROM_Read_Byte(9);
	FollowMidSpeed = EEPROM_Read_Byte(10);
	FollowLowSpeed = EEPROM_Read_Byte(11);
	InStationHigtoMidSpeed = EEPROM_Read_Byte(12);
	InStationMidtoLowSpeed = EEPROM_Read_Byte(13);
	TrainMaxNum = EEPROM_Read_Byte(14);
	RecycleCaseNum = EEPROM_Read_u16(15);
	MFContralZS = EEPROM_Read_Byte(17);
	MFContralDS = EEPROM_Read_Byte(18);
	TrainHeadNum = EEPROM_Read_Byte(19);
	
	
	

	
	if(EEPROM_Read_u16(4) == 0xff)
		TrainWarning |= 0x20;//参数错误
	else
		TrainWarning &= ~0x20;//清除参数错误
	
}


u8 Can_Send_Distribute(u8 len)  
{
	u8 mbox;
	u16 i=0;
	CanTxMsg TxMessage;
	TxMessage.StdId=0x52;			// 标准标识符  广播所有的车厢
	TxMessage.IDE=CAN_Id_Standard; 	// 标准帧
	TxMessage.RTR=CAN_RTR_Data;		// 数据帧
	TxMessage.DLC=len;				// 要发送的数据长度	
	
	TxMessage.Data[0] = InStationCount;
	TxMessage.Data[1] = TrainState;
	
//	else if(TrainMode == 1)
	{
		if(APPSendADDRFlag)//只要还有一个车厢没收到应答就一直是下发地址状态
		{
			HeadSendBasketFlag &= ~0x03;
			HeadSendBasketFlag |= 0x01;		
		}
		else
		{
			HeadSendBasketFlag &= ~0x03;			
		}
		if(((APPSendADDRFlag >> CanSendCoount)&0x01) == 0x01)
		{		
			TxMessage.Data[2] = HeadSendBasketFlag;
			TxMessage.Data[3] = (AppSendAddr[CanSendCoount]>>16) & 0xff;			
			TxMessage.Data[4] = (AppSendAddr[CanSendCoount]>>8) & 0xff;
			TxMessage.Data[5] = AppSendAddr[CanSendCoount] & 0xff;
		}
		else if(((APPSendADDRFlag >> CanSendCoount)&0x01) == 0x00)
		{
			TxMessage.Data[2] = 0;//0806
			TxMessage.Data[3] = 0;			
			TxMessage.Data[4] = 0;
			TxMessage.Data[5] = 0;			
		}
	}
	
	
	TxMessage.Data[6] = gSpeedR;
	TxMessage.Data[7] = Pocket_A_Count;
	
	mbox= CAN_Transmit(CAN1, &TxMessage);   
	i=0; 
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//等待发送结束
	if(i>=0XFFF)return 1;
	return 0;	 
}

u8 Can_Send_Config(u8 len)  
{
	u8 mbox;
	u16 i=0;
	CanTxMsg TxMessage;
	TxMessage.StdId=0x53;			// 标准标识符 
	TxMessage.IDE=CAN_Id_Standard; 	// 标准帧
	TxMessage.RTR=CAN_RTR_Data;		// 数据帧
	TxMessage.DLC=len;				// 要发送的数据长度
	
	TxMessage.Data[0] = TrainMode;
	TxMessage.Data[1] = CaseNum << 8;//
	TxMessage.Data[2] = CaseNum;
	TxMessage.Data[3] = RecycleCaseNum<<8;
	TxMessage.Data[4] = RecycleCaseNum;
	TxMessage.Data[5] = TrainBasketMaxNum;
	TxMessage.Data[6] = Pocket4;
	TxMessage.Data[7] = 0;//0--设置1
	
	mbox= CAN_Transmit(CAN1, &TxMessage);   
	i=0; 
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//等待发送结束
	if(i>=0XFFF)return 1;
	return 0;	 
}

u8 Can_Send_OutStation(u8 len)  
{
	u8 mbox;
	u16 i=0;
	CanTxMsg TxMessage;
	TxMessage.StdId=0x53;			// 标准标识符 
	TxMessage.IDE=CAN_Id_Standard; 	// 标准帧
	TxMessage.RTR=CAN_RTR_Data;		// 数据帧
	TxMessage.DLC=len;				// 要发送的数据长度
	
	TxMessage.Data[0] = CanSendOutStationFlag;//1停止落格 2停止落格解除 3屏蔽车厢  4屏蔽车厢解除  5去回收框
	if(CanSendOutStationFlag == 3)
	{
		CanSendOutStationFlag = 0;
		TxMessage.Data[1] = ShieldBasket;//车厢号
	}
	else if(CanSendOutStationFlag == 5)
	{
		CanSendOutStationFlag = 0;
		TxMessage.Data[1] = RecycleBasket;//车厢号	
	}

	TxMessage.Data[2] = StopLG >>8;//停止落格格口号高位
	TxMessage.Data[3] = StopLG;//停止落格格口号低位	
	TxMessage.Data[4] = DeliverResultACKFlag>>16;
	TxMessage.Data[5] = DeliverResultACKFlag>>8;//DeliverResultFlag
	TxMessage.Data[6] = DeliverResultACKFlag;
	TxMessage.Data[7] = 1;//1--命令
	
	mbox= CAN_Transmit(CAN1, &TxMessage);   
	i=0; 
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//等待发送结束
	if(i>=0XFFF)return 1;
	return 0;	 
}


u8 Can_Send_ReciveResult(u8 len)  
{
	u8 mbox;
	u16 i=0;
	CanTxMsg TxMessage;
	TxMessage.StdId=0x53;			// 标准标识符 
	TxMessage.IDE=CAN_Id_Standard; 	// 标准帧
	TxMessage.RTR=CAN_RTR_Data;		// 数据帧
	TxMessage.DLC=len;				// 要发送的数据长度
	
	TxMessage.Data[0] = 1;//
	TxMessage.Data[1] = 2;
	TxMessage.Data[2] = 3;//
	TxMessage.Data[3] = 4;//	
	TxMessage.Data[4] = DeliverResultFlag>>16;
	TxMessage.Data[5] = DeliverResultFlag>>8;//DeliverResultFlag
	TxMessage.Data[6] = DeliverResultFlag;
	TxMessage.Data[7] = 2;//2--回复上报结果应答
	
	mbox= CAN_Transmit(CAN1, &TxMessage);   
	i=0; 
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//等待发送结束
	if(i>=0XFFF)return 1;
	return 0;	 
}

void CanSendDataTask (void)
{
////////////////////////////////////////
	if(SendCanDataDelayCnt 	== 0)	
	{
		Can_Send_Config(8);//53--0  //设置	
	}	
	SendUDPDataDelay30ms++;
	SendCanDataDelayCnt++;
	if(SendUDPDataDelay30ms >=2)
	{
		CanSendCoount++;
		if(CanSendCoount >21)
			CanSendCoount = 0;
		
	//	if(TrainState != ST0)//241204注释
		
		Can_Send_Distribute(8);//52
		
			
		SendUDPDataDelay30ms = 0;
	}	
	
	if(SendCanDataDelayCnt 	== 1)	
	{
		Can_Send_OutStation(8);//53--1	
	}		

	else if(SendCanDataDelayCnt == 3)
	{
		Can_Send_ReciveResult(8);//53--2
		SendCanDataDelayCnt =0;
	}	
////////////////////////////////////////	
	
}
void USART_Data_Send_Task (void)
{
	u8 i = 0;
	{
		SendUDPDataDelay200ms++;
		SendUDPDataDelay100ms++;

		if(SendUDPDataDelay100ms ==1)
		{			
			if(TrainMode == 0)//手动扫码
			{
				if(BasketApplicationFlag)//只要有一个车厢还在申请状态就持续向下申请
				{
					for(i=0;i<TrainBasketMaxNum;i++)//TrainBasketMaxNum
					{
						if((BasketApplicationFlag >> i) == 0x00000001)
						{
							BasketNumber = i+1;			//车厢号
							ApplicationCommandUart1(1,2);	// 申请地址	
						}	
					}
				}
			}					
			else
			{
				if(APPSendADDRACKFlag ==1)
				{	//下发地址应答
					ADDRACKToApp(5,4+TrainBasketMaxNum);
					APPSendADDRACKFlag =0;
				}					
//				if(TrainState != ST1)
//				{			
//					if(Pocket_A_Count >= 11 && Pocket_A_Count <=15)
//					{
//						ApplicationCommandUart1(1,2);	// 申请地址										
//					}
//				}					
			}
			
		}
		else if(SendUDPDataDelay100ms ==2)
		{
			if(DeliverResul2APPtBuff)//只要有一个车厢还在上报状态就持续向上上报
			{	
				for(u8 y=0;y<TrainBasketMaxNum;y++)//
				{
					if(((DeliverResul2APPtBuff >> y) & 0x01) == 0x01) 
					{
						DeliverResulBasketNumber = y+1;			//车厢号
						DeliverResultToApp(4,9,y);	//上报结果-单件上报
					}
				}								
			}					
		}
		
		else if(SendUDPDataDelay100ms ==4)
		{
			if(ST1ACKFlag ==1)//自检指令应答615
			{
				ST1ACKToApp(3,1); 
				ST1ACKFlag = 0;
			}
	
			if(ReadDataFlag == 1)//读取参数应答
			{
				ReadDataFlag = 0;
				ReadDataACK(0xA2,21);
			}

//			if(TrainApplyForExitFlag == 1)//申请出站
//			{
//				InStationFlag = 0;
//				ApplyForExit(2,1+TrainBasketMaxNum*7);	//出站上报结果
//			}
			if(InStationFlag ==1) //进站上报
			{
				InStationToAPP(0x00,3);
			}				
			if(ConfigrationFlag == 1)//配置应答
			{
				ConfigrationAck(0xA1,1);
				ConfigrationFlag = 0;			
			}			
		}		
		else if(SendUDPDataDelay100ms == 3)
		{
			if(TrainApplyForExitFlag == 1)//申请出站
			{	
				ApplyForExit(2,1+TrainBasketMaxNum*7);	//出站上报结果
			}
			if(StopLGFlag == 1)
			{
				BanStationAck(0x0A,1);
				StopLGFlag = 0;
			}
			if(RecycleBasketFlag ==1) //回收指令应答
			{
				RecyAck(0x0C,2);
				RecycleBasketFlag =0;
			}	
			if(ChargeACK == 1)
			{
				ChargeACKData(0x0D,2);//指定位置应答
				ChargeACK = 0;
			}
			if(NowCarGoACK == 1)
			{
				TrainGoACK(0x0E,1);//立即发车应答
				NowCarGoACK = 0;
			}
			if(TrainRestACK == 1)
			{
				CarRestACK(0x0F,2);//临停应答
				TrainRestACK = 0;
			}
			
		}	
		
		else if(SendUDPDataDelay100ms ==5)
		{
			HeartDelayCount++;
			if(HeartDelayCount >=4)
			{
				HeartDelayCount = 0; 
				HeartToApp(9,35+TrainBasketMaxNum);//心跳	
			}
		}		
		else if(SendUDPDataDelay100ms >5)
		{
			SendUDPDataDelay100ms = 0;
		}	
////////////////////////////////////
	}
}


void TrainContral (void)
{	
		if(APPSendADDRDelay >= 10000)
			APPSendADDRFlag = 0;
		
		if(CarGoGoFlag == 1 && APPSendADDRFlag ==0)
		{
			if(ChargeFlag == 1)
				TrainState = ST9;
			else
				TrainState = ST6;
			StepState = 4;
			CarGoGoFlag = 0;			
		}
//	CarPositionPocket = Pocket_A_Count + CageNumber; //挡片数+接近挡片
		for(u8 a = 0;a<TrainBasketMaxNum;a++)//车厢在线封装在车厢故障
		{
			if(((BasketLifeFlag >> a) & 0x01) == 0x00)
			{
				BasketError[a] |= 0x80;
			}
			else if(((BasketLifeFlag >> a) & 0x01) == 0x01)
			{
				BasketError[a] &= ~0x80;
			}
		}
			
		if(CarScranFlag == 1 && gSpeedR ==0)//急停
			TrainState = ST0;		
		
		if(Rxflag1 == ERROR)
		{
			TrainWarning |= 0x40;//车厢失联
			TrainStop |= 0x40;
			BoxesDieStop = 1; 
		}
		else		
		{
			TrainWarning &= ~0x40;//清除车厢失联
			TrainStop &= ~0x40;//			
			BoxesDieStop = 0;
		}	

		if(CageNumber >= CaseNum - InStationHigtoMidSpeed && CageNumber < CaseNum - MFContralDS)//进站提前减速  0911 75
		{	
			if(TrainState > ST1)
			{
				PocketStep = 0;
				InStationLSFlag = 1;
				StepState = 8;
				LD_Step = 0;
			}
			if((TrainState == ST1 || TrainState == ST10) && FrontCarInStationCount >= 1  && InStationCount >= 1)
			{
				PocketStep = 0;
				InStationLSFlag = 1;
				StepState = 8;
				LD_Step = 0;				
			}
		}
		else if(CageNumber >= CaseNum - MFContralDS)//进站提前减速  0911 75
		{	
			if(TrainState > ST1)
			{
				PocketStep = 0;
				InStationLSFlag = 1;
				StepState = 2;
				LD_Step = 0;
			}
			if((TrainState == ST1 || TrainState == ST10) && FrontCarInStationCount >= 1  && InStationCount >= 1)
			{
				PocketStep = 0;
				InStationLSFlag = 1;
				StepState = 2;
				LD_Step = 0;				
			}
		}		
		
//		else if(CageNumber >= CaseNum - InStationHigtoMidSpeed && CageNumber < CaseNum - MFContralDS)//进站前高速减中速67 75
//		{	
//			if(TrainState > ST1)
//			{
//				PocketStep = 0;
//				InStationLSFlag = 1;
//				StepState = 8;
//				LD_Step = 0;
//			}
//		}	
//		else if(CageNumber >= CaseNum - InStationHigtoMidSpeed)//进站前中速减低速
//		{	
//			if(TrainState > ST1)
//			{
//				PocketStep = 0;
//				InStationLSFlag = 1;
//				StepState = 2;
//			}
//		}			
//		if(TrainState == ST1 && InStationCount == 1 && Pocket_A_Count == 1)
//		{
//			StepState = 6;	
//		}
//////////////////////////////////跟随////////////////////////////////////

		if(CageNumber == MFContralZS)//241105
		{
			Pocket_A_Count = 0;
			Pocket_A_Count1 = 0;
			Pocket_A_Count2 = 0;				
		}	
		
		//if(CageNumber >= MFContralZS && CageNumber < (CaseNum - MFContralDS) && TrainState != ST1)
		
//		if(CageNumber >= MFContralZS && CageNumber < (CaseNum - MFContralDS) && InStationCount >=1) FrontCarInStationCount
//		if(CageNumber >= MFContralZS && CageNumber < (CaseNum - InStationHigtoMidSpeed) && TrainState != ST1 && TrainState != ST9 )
//		if(CageNumber >= MFContralZS && CageNumber < (CaseNum - MFContralDS) && TrainState != ST1 && TrainState != ST9  && TrainState != ST10 )
		
		//if(CageNumber >= MFContralZS && CageNumber < (CaseNum - MFContralDS) && InStationCount >=1 && TrainState != ST9  && TrainState != ST10 )
		if(CageNumber >= MFContralZS && CageNumber < (CaseNum - InStationHigtoMidSpeed) && InStationCount >=1 && TrainState != ST9  && TrainState != ST10 )
		{
			if(TrainState == ST6)	
				StepState =1;
			if(TrainState == ST1 && FrontCarInStationCount >=1)	
			{
				StepState =1;
			}				
		}
		if(TrainState == ST1 && (FrontCarInStationCount < 1  || InStationCount < 1))
		{
			StepState = 7;
		}	
		if(TrainState == ST1 && CageNumber < MFContralZS && FrontCarInStationCount == 1  && InStationCount == 1)
		{
			StepState = 7;
		}
		
		if(TrainState != ST9)
		{
			if(StepState ==1)//跟随速度
			{
				if(CarScranFlag == 0x01 || TrainRestFlag == 1 || BoxesDieStop == 1 || CarInStationDieStop == 1)//网关紧急停车
				{
					MOTOR_ENA_B = 0;
					MBSpeed = 0;
					MoterBrakeFlag = 1;//急刹
				}
				else
				{
					if(MF_SW == 0)//漫反射触发立即刹车
					{
						MOTOR_ENA_B = 0;
						MoterBrakeFlag = 1;//刹车
						MBSpeed = 0;
					}	
					else
					{
						TrainFollowTrain(); //跟随防撞	
					}
				}
			}
			else if(StepState == 2)//进站低速
			{
				if(CarScranFlag == 0x01 || TrainRestFlag ==1 || BoxesDieStop == 1 || CarInStationDieStop == 1)//网关紧急停车
				{
					MOTOR_ENA_B = 0;
					MBSpeed = 0;
					MoterBrakeFlag = 1;//急刹
				}
				else				
				{
					if(MF_SW == 0)//漫反射触发立即刹车
					{
						MOTOR_ENA_B = 0;
						MBSpeed = 0;
						MoterBrakeFlag = 1;//刹车
					}
					else//低速行驶	
					{
						MBSpeed = LowSpeed;
						BreakStopFlag = 0;
						MOTOR_ENA_B = 1;						
					}
				}					
			}
			else if(StepState == 8)//进站高速减中速
			{
				if(CarScranFlag == 0x01 || TrainRestFlag ==1 || BoxesDieStop == 1 || CarInStationDieStop == 1)//网关紧急停车
				{
					MOTOR_ENA_B = 0;
					MBSpeed = 0;
					MoterBrakeFlag = 1;//急刹
				}
				else				
				{
					if(MF_SW == 0)//漫反射触发立即刹车
					{
						MOTOR_ENA_B = 0;
						MBSpeed = 0;
						MoterBrakeFlag = 1;//刹车
					}
					else//低速行驶	
					{
						if(gSpeedR > MidSpeed)
							MBSpeed = MidSpeed;
//						else if(gSpeedR > MidSpeed && gSpeedR <= HigSpeed)
//						{
//							MBSpeed = LowSpeed;
//						}
						else
						{
							MBSpeed = LowSpeed;
						}
						BreakStopFlag = 0;
						MOTOR_ENA_B = 1;						
					}
				}					
			}			
			
			
			else if(StepState == 4 && TrainState != ST5)//出站速度
			{
				if(CarScranFlag == 0x01 || TrainRestFlag ==1 || BoxesDieStop == 1 || CarInStationDieStop == 1)//网关紧急停车
				{
					MOTOR_ENA_B = 0;
					MBSpeed = 0;
					MoterBrakeFlag = 1;//急刹
				}
				else				
				{
					if(MF_SW == 0)//漫反射触发立即刹车
					{
						MOTOR_ENA_B = 0;
						MBSpeed = 0;
						MoterBrakeFlag = 1;//刹车
					}
					else//低速行驶	
					{
						MBSpeed = LowSpeed;
						BreakStopFlag = 0;
						MOTOR_ENA_B = 1;						
					}
				}			
			}	

//			else if(StepState == 6 )//自检进站减速
//			{
//				if(CarScranFlag == 0x01 || TrainRestFlag ==1 || BoxesDieStop == 1 || CarInStationDieStop == 1)//网关紧急停车
//				{
//					MOTOR_ENA_B = 0;
//					MBSpeed = 0;
//					MoterBrakeFlag = 1;//急刹
//				}
//				else				
//				{
//					if(MF_SW == 0)//漫反射触发立即刹车
//					{
//						MOTOR_ENA_B = 0;
//						MBSpeed = 0;
//						MoterBrakeFlag = 1;//刹车
//					}
//					else//低速行驶	
//					{
//						MBSpeed = 150;
//						BreakStopFlag = 0;
//						MOTOR_ENA_B = 1;						
//					}
//				}			
//			}
			else if(StepState == 7)//自检第一圈速度
			{
				if(CarScranFlag == 0x01 || TrainRestFlag ==1 || BoxesDieStop == 1 || CarInStationDieStop == 1)//网关紧急停车
				{
					MBSpeed = 0;
					MoterBrakeFlag = 1;//急刹
					MOTOR_ENA_B = 0;
				}
				else			
				{							
					if(MF_SW == 0)//漫反射触发立即刹车
					{
						MBSpeed = 0;
						MoterBrakeFlag = 1;//刹车
						MOTOR_ENA_B = 0;
					}
					else
					{
						MBSpeed = 120;
						BreakStopFlag = 0;
						MOTOR_ENA_B = 1;	
					}
				}
			}	
//			else if(StepState == 9)//车厢失联  //21119
//			{
//				MBSpeed = 0;
//				MoterBrakeFlag = 1;//刹车
//				MOTOR_ENA_B = 0;
//				
//			}
		}
		
		if(ChargeFlag == 1 && TrainState == ST9)
		{
			StepState = 0;
			if(CarScranFlag == 0x01 || TrainRestFlag ==1 || BoxesDieStop == 1 || CarInStationDieStop == 1)//网关紧急停车
			{
				MBSpeed = 0;
				MoterBrakeFlag = 1;//急刹
				MOTOR_ENA_B = 0;
			}
			else			
			{							
				if(MF_SW == 0)//漫反射触发立即刹车
				{
					MBSpeed = 0;
					MoterBrakeFlag = 1;//刹车
					MOTOR_ENA_B = 0;
				}
				else
				{
					ChargeStop();//指定位置充电	
				}
			}			
		}		
		
///////////////////遥控逻辑///////////////////////////	
	if(TrainState == ST0 )
	{
		if(StepSt0 == 1)
		{
			if(CarScranFlag == 0x01)//网关紧急停车
			{
				MBSpeed = 0;
				MoterBrakeFlag = 1;//急刹
				MOTOR_ENA_B = 0;
			}
			else			
			{							
				if(MF_SW == 0)//漫反射触发立即刹车
				{
					MBSpeed = 0;
					MoterBrakeFlag = 1;//刹车
					MOTOR_ENA_B = 0;
				}
				else
				{
					MBSpeed = HigSpeed;
					BreakStopFlag = 0;
					MOTOR_ENA_B = 1;	
				}
			}			
			
		}
		else if(StepSt0 == 2)
		{
			if(CarScranFlag == 0x01)//网关紧急停车
			{
				MBSpeed = 0;
				MoterBrakeFlag = 1;//急刹
				MOTOR_ENA_B = 0;
			}
			else			
			{							
				if(MF_SW == 0)//漫反射触发立即刹车
				{
					MBSpeed = 0;
					MoterBrakeFlag = 1;//刹车
					MOTOR_ENA_B = 0;
				}
				else
				{
					MBSpeed = MidSpeed;
					BreakStopFlag = 0;
					MOTOR_ENA_B = 1;	
				}
			}			
			
		}		
		else if(StepSt0 == 3)
		{
			if(CarScranFlag == 0x01)//网关紧急停车
			{
				MBSpeed = 0;
				MoterBrakeFlag = 1;//急刹
				MOTOR_ENA_B = 0;
			}
			else			
			{							
				if(MF_SW == 0)//漫反射触发立即刹车
				{
					MBSpeed = 0;
					MoterBrakeFlag = 1;//刹车
					MOTOR_ENA_B = 0;
				}
				else
				{
					MBSpeed = LowSpeed;
					BreakStopFlag = 0;
					MOTOR_ENA_B = 1;	
				}
			}			
		}
		else
		{
			MBSpeed = 0;
			MoterBrakeFlag = 1;//刹车
			MOTOR_ENA_B = 0;			
		}
	}
///////////////////遥控逻辑///////////////////////////	
		
//////////////////////////////////跟随////////////////////////////////////
		
		if(PocketCount == 1)//0807
			TrainApplyForExitFlag= 0;//收到发车指令清除申请标志位			
////////////////////////////		
		ApplicationtTavel = TrainBasketMaxNum + Pocket4 + 1;//申请出站位置计算
		ForceTravel1 = TrainBasketMaxNum + Pocket4 + 2;//强制出站位置计算
		if(TrainMode == 0)//人工手动扫码模式
		{	
	//////////////////挡片模式////////////////////////
	////////运动控制/////////////			
			
	////////运动控制/////////////			
			if(StionStop == 1)//进站
			{			
				if(TrainState >= ST2)
				{	
					OldPocket_A_Count = NewPocket_A_Count; 
					NewPocket_A_Count = Pocket_A_Count;
					if(OldPocket_A_Count != Pocket_A_Count && TrainState != ST8)
					{				
						for(u8 ii = 0;ii < TrainBasketMaxNum;ii++) 
            {
							if(Pocket_A_Count == (Pocket4 + 1 + ii))
							{
								if(BasketError[ii] == 0 && TrainState != ST9)	
								{
									MOTOR_ENA_B = 0;
									StepState = 5;
									MBSpeed = 0;
									MoterBrakeFlag = 1;//刹车
									TrainState = ST4;
									InStationLSFlag = 0;
									
								}
							}	 
            }
					}
					
					///装载交互///
					if(TrainState == ST4)
					{
						if(MoveOneFlag == 0x01)//移动
						{
							IntermediateVariable2++;
							if(IntermediateVariable2 == 1)
							{
								if(CarScranFlag == 0x01)//网关紧急停车  不执行临停指令
								{
									MOTOR_ENA_B = 0;
									MBSpeed = 0;
									MoterBrakeFlag = 1;//急刹
								}
								else
								{
									MBSpeed = 90;
									BreakStopFlag = 0;
									MOTOR_ENA_B = 1;
								}														
							}
							if(IntermediateVariable2 >=10)
								IntermediateVariable2 = 10;	
							MoveOneFlag = 0;
						}
					}
					///装载交互///				
				}
			}//进站
	//////////////////挡片模式////////////////////////				
		}


		if(Pocket_A_Count == ApplicationtTavel && TrainState == ST1)//出站
		{
			InStationFlag = 0;
			StionStop = 0;//清除进站标志
			LD_Step = 0;
			CarInStationFlag = 0;
		}		

		if(TrainState == ST2 || TrainState == ST3 || TrainState == ST4|| TrainState == ST5)
		{	
			if(Pocket_A_Count == ApplicationtTavel)//出站
			{
				if(PocketStep == 0) 
					PocketStep = 1;
			}	
			if(PocketStep == 1)
			{
				MOTOR_ENA_B = 0;
				StepState = 3;
				MBSpeed = 0;
				MoterBrakeFlag = 1;//急刹
				TrainApplyForExitFlag =1;//申请出站
				TrainState = ST5;	
				StionStop = 0;//清除进站标志

				PocketStep = 2;
				LD_Step = 0;
				TrainStop |= 0x10;//
				InStationFlag = 0;
				CarInStationFlag = 0;
			}	

			if(Pocket_A_Count == ForceTravel1)//强制出站
			{
				if(ChargeFlag == 1)
					TrainState = ST9;
				else
					TrainState = ST6;
				StepState = 4;
				//0809
				BasketApplicationFlag = 0;
				TrainApplyForExitFlag = 0;
				InStationFlag = 0;
				//0809
				LD_Step = 0;
				TrainStop &= ~0x10;//
				InStationFlag = 0;
				CarInStationFlag = 0;
			}				
		}
		#if DriveMode //新驱动板
			//读取速度
			//第一个车轮
			gSpeedRA = -OdReceivedData.vel_estimate[0].float_temp *22.0;
		
			//第二个车轮
			gSpeedRB =  OdReceivedData.vel_estimate[1].float_temp *22.0;
		
			//第三个车轮
			gSpeedRC = -OdReceivedData.vel_estimate[2].float_temp *22.0;
		
			//第四个车轮
			gSpeedRD =  OdReceivedData.vel_estimate[3].float_temp *22.0;
		
			gSpeedR = max_of_four(gSpeedRA,gSpeedRB,gSpeedRC,gSpeedRD);
				
		#endif
		
	
//		if(TrainState == ST5 || TrainState == ST6)
//		{
//			if(CageNumber == OutStationQuicken)//出站加速
//			{
//				StepState = 0;
//				if(CarScranFlag == 0x01 || TrainRestFlag ==1 || BoxesDieStop == 1 || CarInStationDieStop == 1)//网关紧急停车
//				{
//					MBSpeed = 0;
//					MoterBrakeFlag = 1;//急刹
//				}
//				else				
//				{
//					if(MF_SW == 0)//漫反射触发立即刹车
//					{
//						MBSpeed = 0;
//						MoterBrakeFlag = 1;//刹车
//					}
//					else
//					{						
//						TrainState = ST6; 
//						MBSpeed = 150;BreakStopFlag = 0;
//						MOTOR_ENA_B = 1;
//					}	
//				}					
//			}	
//		}

		if(TrainState == ST0)
		{
			CageNumber = 0;
			PocketStep = 0;
			APPSendADDRFlag = 0;
			CarGoGoFlag = 0;
			HeadSendBasketFlag = 0;
//0822
			StionStop =0;
			CageNumber = 0;
			Pocket_A_Count = 0;
		Pocket_A_Count1 = 0;
		Pocket_A_Count2 = 0;			
			InStationCount = 0;	
			TrainApplyForExitFlag= 0;InStationFlag = 0;			
//0822			
		}	


}







