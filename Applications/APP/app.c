#include "app.h"
#include "stdlib.h"
#include <string.h>
#include "application.h"
#include "MyADC.h"  
#include "pid.h"  
CanRxMsg RxMessage;
CanTxMsg TxMessage;

u8 gShowNumberData[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6d, 0x7d, 0x07, 0x7F, 0x6F};
// 显示空   E   P  A  b  C
u8 gShowAlphabetData[7] = {0x00, 0x79, 0x73, 0x77, 0x7c, 0x39,0x3e};

u16 gTim4UpdateCounter = 0;
u32 gNewGetTim4Counter = 0;
u32 gOldGetTim4Counter = 0;

u32 gErrorVaule = 0;
u32 GetSpeedVaule[SPEED_VAULE] = {0};  
u16 gGetSpeedVauleCounter = 0;
u32 gSum = 0;
u32 PulseCntA = 0;
u32 PulseCntB = 0;
//u16 gCheckHeartLiveCount = 0;
//u16 gCheckTrailLiveCount = 0;
//u16 gCheckFroCarLiveCount = 0;

u16 KeyUpCount = 0;
u8 KeyUpFlag = 0;
u8 KeyValue,KeyValueold,KeyValuenew = 0;
u16 LedNumDisplay = 0;
u8 KeyMode = 0;

u16 KeyUpCount2 = 0;
u8 KeyUpFlag2 = 0;
u8 KeyValue2,KeyValue2old,KeyValue2new = 0;

u32 CAN_ID = 0;
u8 RxRAM0[8];
u8 Rxflag1 = 0;
u32 g_Lock_Code = 0x34;
int PocketCount,PocketCountold = 0;//U型挡片计数

int PocketCount1,PocketCount2 = 0; 

int Pocket_A_Count,OldPocket_A_Count,NewPocket_A_Count = 0;//金属挡片

int Pocket_A_Count1,Pocket_A_Count2 = 0;  

u16 LongLDCount = 0;
u16 Timer40msCount = 0;
u16 Timer10msCount = 0;
u16 Timer100msCount = 0; 
u16 gCheckHeartLiveCount[21];//车厢在线检测

u16 gCheckHeartLiveDelay = 0;

u16 gCheckHeartOdriveCount = 0;//Odrive在线检测
u16 gCheckTrailLiveCount = 0;
u16 gCheckFroCarLiveCount = 0; 
u16 SwitchCount1 = 0;
u16 SwitchCount1_B = 0;

u8 StationDirGD = 0;//1--前进，2--后退
u8 StationDirGD_B = 0;//1--前进，2--后退

u8 StationDirLD = 0;//1--前进，2--后退
u8 StationDirLD_B = 0;//1--前进，2--后退

u32 SwitchCount2 = 0; 
u32 SwitchCount2_B = 0; 
u8 SwitchCount = 0;
u8 SwitchCount_B = 0; 
u16 LedCnt = 0; 
u16 CarGoDelay = 0; 

float gSpeedR = 0;
float gSpeedRA = 0;
float gSpeedRB = 0;
float gSpeedRC = 0;
float gSpeedRD = 0;
u8 TrainStop = 0; //bit0--慢反停车  bit1--驱动失联      bit2--遥控急停        bit3--进站未识别 
									//bit4--申请出站  bit5--按键急停  bit6--车厢失联   bit7--休息

u8 ReadSpeedFlag = 0;//电机读速异常1
u16 ReadSpeedTimerCount = 0;
u8 MotorDir = 0; //
u8 MotorGoFlag = 0;
u32 APPBasketNum = 0x00000000;//车号B3  车厢号B2  投递格口号B0\1


u32 APPSendADDRFlag = 0;//APP下发地址标志位  21个位代表21个车厢下发的地址
u16 APPSendADDRDelay = 0;
u8 APPSendADDRACKFlag = 0;//APP下发地址标志位-自动模式
int AppSendAddr[21];//下发地址  车号B3  车厢号B2  投递格口号B0\1
u32 AppAddrACK = 0;//应答回复

unsigned short int AddrAckState[21];//下发地址应答状态1--表示车厢收到地址  2--表示车厢故障未收到地址  0--未下发地址

int LastDeliverResultNum[21];//车号B1  车厢号B2  投递格口号B3\4 
int LGTimerCntBuff[21] ;//投递到落格时间
unsigned short int LastPoketNum[21];//上一圈挡片数量

u32 BasketApplicationFlag = 0;//21个位代表21个车厢对应申请地址状态    1--申请地址标志
u32 DeliverResultFlag = 0;//21个位代表21个车厢对应上报结果状态  1--上报

u32 DeliverResul2APPtBuff = 0;

u16 DeliverResultTimer[21];//计时清零

//u8 DeliverResultLive = 0;
//u16 DeliverResultTimer = 0;

u32 DeliverResultACKFlag = 0;//21个位代表21个车厢对应上报结果应答状态  1--应答

u8 BasketError[24];//车厢故障缓存

u32 NFCNUM,NFCNUMold,NFCNUMnew = 0;//车头位置
u32 TailstockNFC = 0;

u16 CarTailNFCNum = 0;//车尾位置
u8 MoveOneFlag = 0;

u8 HeadSendBasketFlag = 0; // 0x01--下发地址  0x02--停止下发  0x04--下发回复投递结果应答信号  0x08--停止投递结果应答下发
u32 BasketPocketNum,BasketPocketNumold,BasketPocketNumnew,LastDeliverResult = 0;

u32 BasketReciveAddrFlag = 0;//车厢收到地址标志位 21bit缓存21个车厢--自动模式

u16 BasketNumACKADDRToAPP = 0;

u16 PositionTail = 0;//挡片车尾位置
u16 FrontCarPositionNFC = 0;//前车位置，指的是前车最后一节车厢的位置

u16 testflag = 0;

u16 FrontCarPositionPokec = 0;//前车车尾位置
u16 FrontCarPositionPokec1 = 0;//前车车尾位置
u16 FrontCarPositionPokec2 = 0;//前车车尾位置

u8 FrontCarPositionFlag = 0;//前车位置获取成功标志位
u8 FrontCarInStationCount = 0;//前车进站次数
//u8 TrailPositionFlag = 0;//车尾位置在线标志

u16 FrontCarPositionHead = 0;//前车车头位置位置
u16 CarPositionPocket = 0;//挡片数+接近挡片
u16 CarDistance = 0;  //与前车距离格口数量
u8 SendDataFlag = 0;//收到前车的广播允许发送数据标志
u8 ReciveRightFlag = 0;//正常接收标志位

u8 CarSignalNum = 0;//超时等待发送时间计算
u16 TrainBasketPosition = 0;//车尾位置格口号

float gPowerValueFloat = 0;
u16 powervol = 0;
unsigned  int gCarBatPower = 0; //电源电压
u16 carpower = 0;

u8 ST1ACKFlag = 0;//自检指令标志位

u8 TrainApplyForExitFlag = 0;//出站申请标志位
u8 BreakCargo = 0;
u8 CarGo = 1;//1--前进
u8 CarBack = 1;//后退
u8 ScranTrain = 0;//跟随标志--1
u8 StionStop  = 0;   		//进站停车挡片--手动模式

u8 CarDirection = 0;//0-stop 1前进  2后退
u16 IDMaxNum = 0;//最大格口号
u8 CarScranFlag = 0;//急停标志位
u8 TrainRestFlag = 0;//火车临停标志位
u8 CarDriveFlag = 0;

u8 NowCarGoACK = 0;
u8 ChargeACK = 0;
u8 StopAddrFlag = 0;

u8 TrainRestACK = 0; 
u8 BoxesDieStop = 0;//车厢失联停车

u8 ApplicationtTavel=9;	//申请发车挡片
u8 ForceTravel1 =10;
u8 ForceTravel2 =11;
u8 ForceTravel3 =29;

u8 ConfigrationFlag = 0;//配置标志位 1--配置
u8 InStationFlag = 0;//进站标志--进站上报
u8 InStationCount = 0;//进站次数，用于自检

u8 CarInStationFlag = 0;
u8 CarInStationDieStop = 0;

u8 LDDelayFlag = 0;

u8 LD_Step = 0;

u8 OdriveLifeFlag = 0;//新驱动在线标志


u32 BasketLifeFlag = 0;//车厢在线标志

u8 ReadDataFlag = 0;
u8 CanSendCoount = 0;

u16 ChargePosition = 0x45;//指定位置充电
u8 ChargeFlag = 0;//指定位置充电标志位

u8 WhyStoping = 0;

u8 SensorLD = 0;

//u8 DriveMode = 1;//电机驱动类型 0--老驱动板 1--ODRIVE

////////////////////////////设置参数///////////////////////////

u16 CaseNum = 77;//格口数   ADDR 0-1
u8 Pocket4=11;						//装载挡片 ADDR 2
u8 TrainBasketMaxNum = 7;//车厢数量，即车尾编号 ADDR 3
u8 TrainMode = 1;//扫码模式  //0--手动扫码    1--自动扫码  ADDR 4
u8 HigSpeed = 200;//ADDR 5
u8 MidSpeed = 150;//ADDR 6
u8 LowSpeed = 80;//ADDR 7

u8 OutStationQuicken = 4;//出站加速格口 ADDR 8

u8 FollowHigSpeed = 20;//跟随间隔高速 ADDR 9
u8 FollowMidSpeed = 10;//跟随间隔中速 ADDR 10
u8 FollowLowSpeed = 5;//跟随间隔低速 ADDR 11

u8 InStationHigtoMidSpeed = 10;//进入装载区前高速减中速 ADDR 12
u8 InStationMidtoLowSpeed = 4;//进入装载区后中速减低速 ADDR 13
u8 TrainMaxNum = 3;//火车数量    				ADDR 14
u16 RecycleCaseNum = 77;//回收格口  			ADDR 15-16

u8 MFContralZS = 1;//跟随区间格口号正数  ADDR 17
u8 MFContralDS = 2;//跟随区间格口号倒数  ADDR 18

////////////////////////////设置参数///////////////////////////

////////////////////////////保存参数///////////////////////////
u8 TrainHeadNum = 0;   //车号           ADDR 19

u8 WorkOffFlag = 0;    //正常下电标志		ADDR 20   0x55正常下班   0xaa未知断电
////////////////////////////保存参数///////////////////////////
void IAP_APP_Init(void)
{
	SCB->VTOR = FLASH_BASE | IAP_Bootloat_SIZE; //IAP_Bootloat_SIZE:0x2000=8K 字节
//	GetLockCode(&g_Lock_Code);						//获取芯片唯一ID
}

void systeam_ReStart(void)
{
	u32 time = 65535;
	while(time--)
	{
		__NOP();
	}
	__set_FAULTMASK(1);
	NVIC_SystemReset();
}

//can发送一组数据(固定格式:ID为0X12,标准帧,数据帧)	
//len:数据长度(最大为8)				     
//msg:数据指针,最大为8个字节.
//返回值:0,成功;
//		 其他,失败;
static u8 IAP_Can_Send_Msg(u8 ID,u8* msg,u8 len)
{	
	u8 mbox;
	u16 i=0;
	CanTxMsg TxMessage;
	TxMessage.StdId = ID;			// 标准标识符  
	TxMessage.ExtId = ID;			// 设置扩展标示符 
	TxMessage.IDE=CAN_Id_Standard; 	// 标准帧
	TxMessage.RTR=CAN_RTR_Data;		// 数据帧
	TxMessage.DLC=len;				// 要发送的数据长度
	for(i=0;i<len;i++)
	TxMessage.Data[i]=msg[i];			          
	mbox= CAN_Transmit(CAN1, &TxMessage);   
	i=0; 
	while((CAN_TransmitStatus(CAN1, mbox)!=CAN_TxStatus_Ok)&&(i<0XFFF))
		i++;	//等待发送结束
	if(i>=0XFFF)
		return 1;
	return 0;	 
}


u8 Send_response(u8 Can_ID,u8 Byte1,u8 Byte6,u8 Byte7,u8 Byte8)	//发送响应
{
	u8 temp[8]={0};
	
//	GetLockCode(&g_Lock_Code);			//获取芯片唯一ID
	

		
	temp[0] = Byte1;					//命令字
	temp[1] = g_Lock_Code>>24&0xff;		//唯一ID
	temp[2] = g_Lock_Code>>16&0xff;	
	temp[3] = g_Lock_Code>>8&0xff;
	temp[4] = g_Lock_Code&0xff;
	temp[5] = Byte6;					//CAN_ID	//刷固件模式:接收数据长度3个字节
	temp[6] = Byte7;					//设备类型
	temp[7] = Byte8;					//运行模式 01:BootLoader  02:APP

	return IAP_Can_Send_Msg(Can_ID,temp,8);
}

//向上位机发送设备信息
u8 IAP_Send_Device_ino(void)
{
	return Send_response(0x7B,0x81,TrainVersion,0x07,2); 
}




//APP程序CAN中断内,刷固件跳转程序
void IAP_APP_CAN_ReStart(CanRxMsg temp_CAN_Msg)
{
	
	if(temp_CAN_Msg.StdId == 0x7D)	//PC读设备唯一ID
	{
		if(temp_CAN_Msg.Data[0]==1&&temp_CAN_Msg.Data[1]==0&&temp_CAN_Msg.Data[2]==0&&temp_CAN_Msg.Data[3]==0&&
		   temp_CAN_Msg.Data[4]==0&&temp_CAN_Msg.Data[5]==0&&temp_CAN_Msg.Data[6]==0&&temp_CAN_Msg.Data[7]==0)
		{	
	//		UpdataFlag = 2;
			IAP_Send_Device_ino();					//向上位机发送设备信息
		}
	
	}
	if(temp_CAN_Msg.StdId == 0x7F)
	{	
		if(	temp_CAN_Msg.Data[0]==0x01&&	
			temp_CAN_Msg.Data[1]==(g_Lock_Code>>24&0xff)&& 
			temp_CAN_Msg.Data[2]==(g_Lock_Code>>16&0xff)&&
			temp_CAN_Msg.Data[3]==(g_Lock_Code>>8&0xff)&&	
			temp_CAN_Msg.Data[4]==(g_Lock_Code&0xff) )
		{	
			systeam_ReStart();	
		}			
		
	}
}


u32 count = 0;
u8 Icount[21];
u8 Icount1[21];
u8 Icount2[21];
u8 i = 0;
void SaveData(CanRxMsg temp_CAN_Msg)
{ 
  u8 i; 
	
	CAN_ID = temp_CAN_Msg.StdId;
	
	if(temp_CAN_Msg.IDE == CAN_Id_Standard)
	{
		for(i = 0; i < 8; i++)
		{
			RxRAM0[i] = temp_CAN_Msg.Data[i];
		}
	}	
	Rxflag1 = SUCCESS;
	gCheckHeartLiveDelay = 0;

}

void CheckCarCanCmd(void)
{	
	if(Rxflag1 == SUCCESS)
	{		
		for(i=0;i<TrainBasketMaxNum;i++)
		{
			if(RxRAM0[7] == 1)
			{
				if(CAN_ID == i+1)	
				{			
					BasketError[i] = RxRAM0[3];//缓存各车厢故障
					BasketLifeFlag |= 1 << i;
					gCheckHeartLiveCount[i] = 0;//车厢在线检测时间清零
				}
				if(TrainMode == 0)//手动扫码
				{
					if((RxRAM0[0] & 0x0f)== 0x01)//申请地址
					{
						if(CAN_ID == i+1)	
						{
							Icount1[i]++;
							if(Icount1[i] == 1)
							{
								MoveOneFlag = 0x01;
								BasketApplicationFlag |= 1 << i;//缓存各车厢申请状态
								IntermediateVariable2 = 0;
							}
							if(Icount1[i] >= 0xee)
								Icount1[i] = 0xee;
							
							Icount[i] = 0; 
						}
					}	
					else if((RxRAM0[0]& 0x0f) == 0x02)//车厢收到地址停止下发
					{
						if(CAN_ID == i+1)	
						{					
							Icount[i]++;
							APPSendADDRFlag &= ~(1<<i);//0808
							if(Icount[i] == 1)
							{
								APPSendADDRFlag &= ~(1<<i);//
								HeadSendBasketFlag &= ~0x03;
								HeadSendBasketFlag |= 0x02;
								BasketApplicationFlag &= ~(0x00000001 << i);//清除申请状态
							}
							if(Icount[i] >= 0xee)
								Icount[i] = 0xee;
							Icount1[i] = 0;
						}
					}
				}
				else//自动扫码
				{
					if(CAN_ID == i+1)		
					{
						if((RxRAM0[0]& 0x0f) == 0x02)//车厢收到地址停止下发
						{					
							Icount[i]++;
							APPSendADDRFlag &= ~(1<<i);
							BasketReciveAddrFlag |= (1<<i);//车厢收到地址
							if(Icount[i] == 1)
							{
								HeadSendBasketFlag &= ~0x03;
								HeadSendBasketFlag |= 0x02;									
							}
							if(Icount[i] >= 0xee)
								Icount[i] = 0xee;
						}
						else if((RxRAM0[0]& 0x0f) == 0x03)
						{
							Icount[i] = 0;
//							BasketReciveAddrFlag &= ~(1<<i);
						}					
					}
				}
		
				
//				if((RxRAM0[0] & 0xf0)== 0x10)//收到车厢上报结果
//				{
//					if(CAN_ID == i+1)	
//					{	
//						Icount2[i]++;
//						if(Icount2[i] == 1)
//						{					
//							LastDeliverResultNum[i] = RxRAM0[1] <<8 | RxRAM0[2];  //缓存各车厢投递结果
//							LastPoketNum[i] = RxRAM0[4] <<8 | RxRAM0[5];
//							DeliverResultFlag |= 1 << i;//缓存各车厢上报状态
//							HeadSendBasketFlag &= ~0x0c;
//							HeadSendBasketFlag |= 0x04;							
//						}
//						if(Icount2[i] >= 0xee)
//							Icount2[i] = 0xee;					
//						
//					}	
//				}
//				else if((RxRAM0[0]& 0xf0) == 0x20)//车厢收到投递结果确认应答信号
//				{
//					if(CAN_ID == i+1)	
//					{					
//						HeadSendBasketFlag &= ~0x0c;
//						HeadSendBasketFlag |= 0x08;			
//				//		DeliverResultFlag &= ~(1 << i);//清除上报状态//241017注释
//						Icount2[i] = 0;
//					}
//				}
			}
			else if(RxRAM0[7] == 2)//解析上报结果
			{
				if(CAN_ID == i+1)		
				{	
					if(TrainState == 3)
						LastPoketNum[i] = RxRAM0[0] <<8 | RxRAM0[6];
					LastDeliverResultNum[i] = RxRAM0[1] <<8 | RxRAM0[2];  //缓存各车厢投递结果
					LGTimerCntBuff[i] = RxRAM0[4] <<8 | RxRAM0[5];  //缓存各车厢落格时间
					DeliverResultFlag |= 1 << i;//缓存各车厢上报状态		 在长遮光清除状态
					DeliverResul2APPtBuff |= 1 << i;//
					DeliverResultTimer[i] = 0;
				}					
			}

		}		
	}
}




void PowerValueLedShow(void)//
{

//	float dat=0;
	gPowerValueFloat = ((1.0 * gCarBatPower) * 3.3 * (18.0 + 1.0) / 4096.0 / 1.0);
	powervol = gPowerValueFloat *10;	
//	dat = (powervol - CARPOWER_VUALE_MIN_FLOAT)*100.0/(CARPOWER_VUALE_MAX_FLOAT-CARPOWER_VUALE_MIN_FLOAT);

//	if(dat>99.0)
//	{
//		dat = 99; 
//	}
//	
//	if(dat < 1)
//	{
//		dat = 1;
//	}
//	carpower=(u8)(dat);
	
}


void Read_vol_contral(void)
{
	uint16_t  cnt = 0;
	u8 TransmitMailbox=0;	
	{
		TxMessage.ExtId = 0x0F585944;
		TxMessage.RTR = CAN_RTR_DATA;
		TxMessage.IDE = CAN_ID_EXT;
		TxMessage.DLC = 8;
			
		TxMessage.Data[0] = 0x02;
		TxMessage.Data[1] = 2;
		TxMessage.Data[2] = 0xa7;//vol
		TxMessage.Data[3] = 0xa8;//cur
		TxMessage.Data[4] = 0;
		TxMessage.Data[5] = 0;
		TxMessage.Data[6] = 0;
		TxMessage.Data[7] = 0;

		//send to meter
		TransmitMailbox=CAN_Transmit(CAN1,&TxMessage);
		cnt = 0;
		while((CAN_TransmitStatus(CAN1,TransmitMailbox) != CANTXOK) && (cnt != 0xffff))
		{
			cnt++;
		}
	}	
	
}

void time2_init(u16 arr,u16 psc)//
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStrct;
	NVIC_InitTypeDef NVIC_InitStrct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	TIM_TimeBaseInitStrct.TIM_Period=arr;
	TIM_TimeBaseInitStrct.TIM_Prescaler=psc;
	TIM_TimeBaseInitStrct.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStrct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStrct.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStrct);
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	
	TIM_Cmd(TIM2,ENABLE);//失能
	
	NVIC_InitStrct.NVIC_IRQChannel=TIM2_IRQn;
	NVIC_InitStrct.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStrct.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStrct.NVIC_IRQChannelCmd=ENABLE;
	
	NVIC_Init(&NVIC_InitStrct);	
}


void TIM2_IRQHandler(void)
{
	if(TIM_GetFlagStatus(TIM2,TIM_IT_Update)==SET)//
	{	
		if(gSpeedR >= 10 && (TrainState == ST6 || TrainState == ST1))
			LongLDCount++;
		
		Timer40msCount++;
		Timer10msCount++;
		Timer100msCount++;
		CarGoDelay++;
		if(APPSendADDRFlag != 0 && CarGoGoFlag ==1)
			APPSendADDRDelay++;

		gCheckHeartLiveDelay++;
		if(gCheckHeartLiveDelay >=800)//0.8S车厢在线检测和车尾位置检测
		{
			Rxflag1 = ERROR;	
		}

		
		gCheckHeartOdriveCount++;
		if(gCheckHeartOdriveCount >=800)//0.8S车厢在线检测
		{
			OdriveLifeFlag = 0;
		}			

		gCheckFroCarLiveCount++;
		if(gCheckFroCarLiveCount >= 1000)
		{
			FrontCarPositionFlag = 0;
		}		
		



		
///////////////////////////////挡片计数//////////////////////////////	
	//	
		if(Q_GDSW == 0 && H_GDSW == 1 && StationDirGD == 0)
		{
			StationDirGD =1;//前进
		}
		else if(Q_GDSW == 1 && H_GDSW == 0 && StationDirGD == 0)
		{
			StationDirGD =2;//后退
		}
		if(Q_GDSW == 0 && H_GDSW == 0)		
		{
			SwitchCount1++;
			if(SwitchCount1 == 1)
			{
				if(StationDirGD == 1)
					PocketCount1++;		
				else if(StationDirGD == 2)
				{
					PocketCount1--;
				}
				if(PocketCount1<=0)
					PocketCount1 = 0;
			}
			else if(SwitchCount1 >= 20)
			{
				SwitchCount1 = 20;
			}
		}
		if(Q_GDSW == 1 && H_GDSW == 1)	
		{
			SwitchCount1 = 0;
			StationDirGD =0;
		}	
		


		if(Q_GDSW_B == 0 && H_GDSW_B == 1 && StationDirGD_B == 0)
		{
			StationDirGD_B =1;//前进
		}
		else if(Q_GDSW_B == 1 && H_GDSW_B == 0 && StationDirGD_B == 0)
		{
			StationDirGD_B =2;//后退
		}		
		if(Q_GDSW_B == 0 && H_GDSW_B == 0)		
		{
			SwitchCount1_B++;
			if(SwitchCount1_B == 1)
			{
				if(StationDirGD_B == 1)
					PocketCount2++;	
				else if(StationDirGD_B == 2)
				{
					PocketCount2--;	
				}
				if(PocketCount2<=0)
					PocketCount2 = 0;				
			}
			else if(SwitchCount1_B >= 20)
			{
				SwitchCount1_B = 20;
			}
		}
	  if(Q_GDSW_B == 1 && H_GDSW_B == 1)
		{
			SwitchCount1_B = 0;
			StationDirGD_B =0;
		}

		if(PocketCount1 >= PocketCount2)
			PocketCount = PocketCount1;
		else
			PocketCount = PocketCount2;
		CageNumber = PocketCount/2+PocketCount%2;//格口号		
	
///////////////////////////////挡片计数//////////////////////////////			
		
		LoadingStation();//装载台计数

	
	
		#if !DriveMode //老驱动板
	
	
//	GetCarSpeed();//速度获取   输入捕获起步不准确		
				//	PWM控速  
			if(Timer100msCount >=100)
			{
				Timer100msCount = 0;
				if(ReadSpeedFlag == 0)
				{
						gSpeedRA = PulseCntA*3.866f;
						PulseCntA = 0;
					
						gSpeedRB = PulseCntB*3.866f;//3.666f
						PulseCntB = 0;
					
						if(gSpeedRA > gSpeedRB)
							gSpeedR = gSpeedRA;
						else
							gSpeedR = gSpeedRB;
						PIDA.PWM = Position_PIDA(&PIDA,MBSpeed);//PID调速需要重新调参数 
						Motor_Contral(CarGo,PIDA.PWM,MOTOR_ENA_B);//		
				}	
			
			}
			 

			if(MBSpeed == 0)
			{
				MOTOR_ENA_B = 0;
				CarDriveFlag = 0;
			}	
			else
			{
				MOTOR_ENA_B = 1;
				CarDriveFlag = 1;
			}
			
/////////////////////防止速度读不到导致失控//////////////////////////			
//			if(MOTOR_ENA_B==1)
//			{
//				if(PulseCounter <2)
//				{
//					ReadSpeedTimerCount++;
//					if(ReadSpeedTimerCount >=3000)//3S
//					{
//						MBSpeed = 0;
//						MOTOR_ENA_B = 0;
//						ReadSpeedFlag = 1;
//					}
//					else if(ReadSpeedTimerCount >3000)
//						ReadSpeedTimerCount = 3000;
//				}
//				else 
//					ReadSpeedTimerCount = 0;				
//			}

/////////////////////防止速度读不到导致失控//////////////////////////					
	
    #endif		
		
/* 车头灯指示  */	
		if(!MF_SW)
		{
			HeadLed = 1;
		}
		else
		{
			if(TrainState == 1)
			{
				LedCnt++;
				if(LedCnt <=1000)
					HeadLed = 1;
				else if(LedCnt >1000 && LedCnt <=2000)
				{
					HeadLed = 0;
				}
				else
					LedCnt = 0;
			}
			else
				HeadLed = 0;
		}
/* 车头灯指示  */	
		
	}
	
	TIM_ClearFlag(TIM2,TIM_IT_Update);
}


u8 LoadingStation(void)
{
	SensorLD = LDB;
///////////////////////////////金属挡片计数//////////////////////////////	
	if((SensorWarning & 0x03) == 0)
	{	
		if(H_LD == 0 && Q_LD ==0)
		{
			SwitchCount++;
			if(gSpeedR >= 30)
				SwitchCount2++;				
			if(SwitchCount == 1)
			{
				Pocket_A_Count1++;
				if(Pocket_A_Count1 >= Pocket_A_Count2)
					Pocket_A_Count = Pocket_A_Count1;
				else
					Pocket_A_Count = Pocket_A_Count2;
				SwitchCount2 = 0;
				if(Pocket_A_Count == Pocket4)//0802
				{
					PocketCountold = 0;
				}			
			}
			else if(SwitchCount >= 20 )
			{
				SwitchCount = 20;
			}
			
			//241105
//			if(TrainState == ST6 && Pocket_A_Count == 1 && LD_Step == 0)//防止长遮光识别不准
//					LD_Step = 1;			
			//241105
			
	/*  长遮光  */		
	
			if(SwitchCount2 >= 200)//触发进站
			{
				if(LD_Step == 0)
					LD_Step = 1;
	//			InStationCount1_A++;
	//			if(InStationCount1_A >2)	
	//				InStationCount1_A =2;	
			}
		}
		else if(H_LD == 1 && Q_LD ==1)
		{
			SwitchCount = 0;
		}	
	}
	
	if((SensorWarning & 0x0c) == 0)
	{
		if(H_LD_B == 0 && Q_LD_B ==0)
		{
			SwitchCount_B++;
			if(gSpeedR >= 30)
				SwitchCount2_B++;
			
			if(SwitchCount_B == 1)
			{
				Pocket_A_Count2++;
				if(Pocket_A_Count1 >= Pocket_A_Count2)
					Pocket_A_Count = Pocket_A_Count1;
				else
					Pocket_A_Count = Pocket_A_Count2;			
				SwitchCount2_B = 0;
						
				if(Pocket_A_Count == Pocket4)//0802
				{
					PocketCountold = 0;
					if(CarInStationFlag ==1 && InStationCount >0)//241204
					{
						TrainStop &= ~0x08;
						CarInStationDieStop = 0;
					}
				  if(CarInStationFlag ==0 && InStationCount >0 && TrainState != ST10)//长遮光未识别停车
					{
						TrainStop |= 0x08;
						CarInStationDieStop = 1;
					}						
					
				}			
			}
			else if(SwitchCount_B >= 20 )
			{
				SwitchCount_B = 20;
			}
			
			if(SwitchCount2_B >= 200)//触发进站
			{ 
				if(LD_Step == 0)
					LD_Step = 1;			
	//			InStationCount1_B++;
	//			if(InStationCount1_B >2)	
	//				InStationCount1_B =2;				
			}

		}
		else if(H_LD_B == 1 && Q_LD_B ==1)
		{
			SwitchCount_B = 0;
		}	
	
	}
	
	
//	if(InStationCount1_A >= InStationCount1_B)
//		InStationCount1 = InStationCount1_A;
//	else 
//		InStationCount1 = InStationCount1_B;
	
	if(LD_Step == 1)
	{
		LD_Step = 2;
		
		InStationCount++;

		{
			BasketReciveAddrFlag = 0;
			Pocket_A_Count = 0;
			Pocket_A_Count1 = 0;
			Pocket_A_Count2 = 0;		
			PocketCountold = PocketCount;
			CageNumber = 0;PocketCount = 0;//清零格口计数
			PocketCount1 = 0;
			PocketCount2 = 0;		
			
			InStationFlag = 1;
			LDDelayFlag = 1;
			StionStop = 1;
			PocketStep = 0;
			APPSendADDRFlag = 0;
			CarInStationFlag = 1;
			memset(AppSendAddr, 0, 21*sizeof(int));//清除下发地址	

			if(TrainState == ST6)
			{
				TrainState = ST3;//进站	
				StepState = 2;
			}
			if(TrainState == ST1 || TrainState == ST10)		
			{	
				if(InStationCount ==2)//挡片计数正确退出自检	
				{
					TrainState = ST2;	
					StepState = 2;				
				}
			}			
		}
	
	 if(InStationCount >2)	
		InStationCount =2;	
	}
	
	

///////////////////////////////金属挡片计数//////////////////////////////	


}

void TIM3_IRQHandler(void)
{
	u8 i = 0;
	if(TIM_GetITStatus(TIM3, TIM_IT_CC1) == SET)
	{
			gOldGetTim4Counter = gNewGetTim4Counter;
			gNewGetTim4Counter =  TIM_GetCounter(TIM3);
		if(gTim4UpdateCounter < 5)
		{		
			gErrorVaule = gNewGetTim4Counter + 65535 * gTim4UpdateCounter - gOldGetTim4Counter;
			GetSpeedVaule[gGetSpeedVauleCounter] = gErrorVaule;
			gGetSpeedVauleCounter++;
			if(gGetSpeedVauleCounter >= SPEED_VAULE)
			{
				gGetSpeedVauleCounter = 0;
			}
		}
		gTim4UpdateCounter = 0;
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
	}
	else if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
	{
		gTim4UpdateCounter++;
		if(gTim4UpdateCounter >= 5) //HALL状态超时捕抓
		{
			gTim4UpdateCounter = 0;
			for(i = 0; i < SPEED_VAULE; i++)
			{
				GetSpeedVaule[i] = 0;
			}
		}
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}



/*	老师傅驱动函数初始化	*/

void Motor_Contral(u8 Dir,u16 Speed,u8 MotorEnable)  
{

	BreakCargo = Dir;
	MOTOR_ENA_B = MotorEnable; 
	
	//DIR 0 前进
	MOTOR_DIRA = Dir;	
	MOTOR_DIRB = ~Dir;
	TIM_SetCompare1(TIM14,Speed);		
	
}


//TIM3 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM14_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);	//使能定时器14时钟
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_TIM14);	//TIM14
		
	//输出TIM14 CH1的PWM脉冲波形	GPIOA.7
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;//TIM14 CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;            //输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO		
	
 
   //初始化TIM14
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM14 Channel1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	
	
	TIM_OC1Init(TIM14, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM14 OC1
	TIM_OC1PreloadConfig(TIM14, TIM_OCPreload_Enable);  //使能TIM14在CCR2上的预装载寄存器	
	
	TIM_Cmd(TIM14, ENABLE);  //使能TIM3
	

}

void TIM3_Int_Init(u16 arr, u16 psc)
{

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_TIM3);	//TIM3
		
	//输出TIM3 CH2的PWM脉冲波形	GPIOA.6
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//TIM3 CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;            //输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO			
	
	
	
	TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_ARRPreloadConfig(TIM3, DISABLE);

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0x0;
	TIM_ICInit(TIM3, &TIM_ICInitStructure);



	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级4级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_ClearFlag(TIM3, TIM_IT_CC1 | TIM_IT_Update);
	TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);
	TIM_SetCounter(TIM3,0);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设

//  TIM_ClearFlag(TIM4, TIM_FLAG_Update);
//  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
//	TIM_SetCounter(TIM4,0);
//  TIM_Cmd(TIM4, ENABLE); 

}

void GetCarSpeed(void)
{
	u8 i = 0;
	u32 buf = 0;
	//计算小车速度
	for(i = 0; i < SPEED_VAULE; i++)
	{
		buf += GetSpeedVaule[i];
	}
	gSum = buf;
	gSpeedRA = (u32)(22.0 / (1.0 /1000000 * gSum));

	
	buf = 0;
}


/*	老师傅驱动函数初始化	*/


void TM1620_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIOD时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOE时钟
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//SDA
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//下拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO		
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;//SSCL
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//下拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO			
	
	
}

void ryydelay(void)
{ 
//	delay_us(50);
}

void Start(void)
{
//    PA_ODR |=0x02;   //CLK=1;
	SCLH;//CLK=1
	ryydelay();
//    PA_ODR |=0x04;    //DAT=1;
	SDAH;//DAT=1

	ryydelay();
//    PA_ODR &=~0x04;     //DAT=0;
	SDAL;//DAT=0
	ryydelay();

//    PA_ODR &=~0x02;     //CLK=0;
	SCLL;//CLK=0
	ryydelay();
//	SCLK=0;
//	_asm("nop");//NOP();
}
void Stop(void)
{
//    PA_ODR &=~0x04;     //DAT=0;
	SDAL;//DAT=0
	ryydelay();
//    PA_ODR |=0x02;   //CLK=1;
	SCLH;//CLK=1
	ryydelay();
//    PA_ODR |=0x04;    //DAT=1;
	SDAH;//DAT=1
	ryydelay();
//	_asm("nop"); //NOP();
}

void TM1620_Write(unsigned char	DATA)
{
	unsigned char i;

	for(i = 0; i < 8; i++)
	{
//    PA_ODR &=~0x02;     //CLK=0;
		SCLL;//CLK=0
		ryydelay();
//		_asm("nop"); //NOP();
		if(DATA & 0x80)
//    	PA_ODR |=0x04;    //DAT=1;
			SDAH;//DAT=1
		else
//    	PA_ODR &=~0x04;     //DAT=0;
			SDAL;//DAT=0
		ryydelay();
//		_asm("nop"); //NOP();
//		_asm("nop"); //NOP();
//    PA_ODR |=0x02;   //CLK=1;
		SCLH;//CLK=1
		ryydelay();
//    PA_ODR &=~0x02;     //CLK=0;
		SCLL;//CLK=0
		ryydelay();
		DATA <<= 1;
//    PA_ODR &=~0x02;     //CLK=0;
		SCLL;//CLK=0
		ryydelay();
//		_asm("nop"); //NOP();
	}
//	 	PA_ODR &=~0x04;     //DAT=0;
	SDAL;//DAT=0
	ryydelay();
//	  PA_ODR |=0x02;   //CLK=1;
	SCLH;//CLK=1
	ryydelay();
//    PA_ODR &=~0x02;     //CLK=0;
	SCLL;//CLK=0

}

void Write_DATA(unsigned char add, unsigned char DATA)
{
//	Write_COM(0x44);
	Start();
	TM1620_Write(add);
	TM1620_Write(DATA);
	Stop();
}

void TM1620_init(void)
{

	SCLH;
	SDAH;
	Start();
	TM1620_Write(0x44);
	Stop();
	Start();
	TM1620_Write(0x8f);
	Stop();
	Write_DATA(0x48, 0x31);
	
	Write_DATA(0x48, 0x31);
	Write_DATA(0X68, 0x00);
	Write_DATA(0X6A, 0x00);
	Write_DATA(0X6C, 0x00);			
}

void LED_Tube_Choose_DisPlay1(u8 num1,u8 num2,u8 num3)
{
	Write_DATA(0X68,num1);
	Write_DATA(0X6A,num2);
	Write_DATA(0X6C,num3);
	
}


void Display (void)
{
	u16 buf,buf1,buf2 = 0;
	
//	KeyValueold = KeyValuenew;
//	KeyValuenew = KeyValue;
	if(KeyValue ==1 && KeyMode == 1)
	{	
		WPS = 0;//写保护关闭
		delay_ms(5);
		TrainHeadNum++;
		
		if(TrainHeadNum>21 || TrainHeadNum == 0)TrainHeadNum = 1;
			
		EEPROM_Write_Byte(19, TrainHeadNum);
		delay_ms(5);
		WPS = 1;
		TrainHeadNum = EEPROM_Read_Byte(19);
		KeyValue = 0;
	}

	else if(KeyValue ==3)
	{
		KeyMode = !KeyMode;
//		KeyMode++;
//		if(KeyMode > 2)
//			KeyMode = 1;
		KeyValue = 0;
	}
	
	
	
	buf = LedNumDisplay/10%10;
	buf1 = LedNumDisplay%10;	
	buf2 = LedNumDisplay/100;
	if(KeyMode == 0)
	{
		LedNumDisplay = TrainHeadNum;
		LED_Tube_Choose_DisPlay1(gShowAlphabetData[3], gShowNumberData[buf], gShowNumberData[buf1]);	
	}
	else if(KeyMode == 1)
	{
		LedNumDisplay = TrainHeadNum;
		LED_Tube_Choose_DisPlay1(gShowAlphabetData[5], gShowNumberData[buf], gShowNumberData[buf1]);	
	}	
	else if(KeyMode == 2)
	{
		LedNumDisplay = Pocket_A_Count;
		LED_Tube_Choose_DisPlay1(gShowAlphabetData[4], gShowNumberData[buf], gShowNumberData[buf1]);		
	}
	else if(KeyMode == 3)
	{
		LedNumDisplay = CageNumber;
		LED_Tube_Choose_DisPlay1(gShowNumberData[buf2], gShowNumberData[buf], gShowNumberData[buf1]);		
	}		
}

void KeyScan(void)
{
	if(SW2 == 0)
	{
		KeyUpCount++;	
		KeyUpFlag = 1;
	}
	else
	{
		if(KeyUpFlag ==1)
		{
			if(KeyUpCount >2 && KeyUpCount <=500)
			{
				KeyValue = 1;
			}
			else if(KeyUpCount >500 && KeyUpCount <=1500) 
			{
				KeyValue = 2;
			}
			else if(KeyUpCount >=2000) 
			{
				KeyValue = 3;
				KeyUpCount = 2000;
			}		
			
			KeyUpCount = 0;
			KeyUpFlag = 0;
		}
	}
	
	
	
	
//	if(SW1 == 0)
//	{
//		KeyUpCount2++;	
//		KeyUpFlag2 = 1;
//	}
//	else
//	{
//		if(KeyUpFlag2 ==1)
//		{
//			if(KeyUpCount2 >2 && KeyUpCount2 <=500)
//			{
//				KeyValue2 = 1;
//			}
//			else if(KeyUpCount2 >500 && KeyUpCount2 <=1500) 
//			{
//				KeyValue2 = 2;
//			}
//			else if(KeyUpCount2 >=2000) 
//			{
//				KeyValue2 = 3;
//				KeyUpCount2 = 2000;
//			}		
//			
//			KeyUpCount2 = 0;
//			KeyUpFlag2 = 0;
//		}
//	}	
	
	
}


void ChargeStop (void)
{
//	if(ChargePosition > CageNumber)
	{
		if(ChargePosition - CageNumber >= 20)
		{
			MBSpeed = 150;
			BreakStopFlag = 0;
			MOTOR_ENA_B = 1;			
		}
		else if((ChargePosition - CageNumber < 20) && (ChargePosition - CageNumber >= 15))
		{
			MBSpeed = 100;
			BreakStopFlag = 0;
			MOTOR_ENA_B = 1;	
		}
		else if((ChargePosition - CageNumber < 15) && (ChargePosition - CageNumber >= 10))
		{
			MBSpeed = 60;
			BreakStopFlag = 0;
			MOTOR_ENA_B = 1;
		}	
		else if((ChargePosition - CageNumber < 10) && (ChargePosition - CageNumber >= 5))
		{
			MBSpeed = 40;
			BreakStopFlag = 0;
			MOTOR_ENA_B = 1;
		}	
		else if((ChargePosition - CageNumber < 5) && (ChargePosition - CageNumber >= 1))
		{
			MBSpeed = 20;
			BreakStopFlag = 0;
			MOTOR_ENA_B = 1;
		}	
		else if(ChargePosition - CageNumber <1)
		{
			MoterBrakeFlag = 1;//刹车
			MBSpeed = 0;	
			MOTOR_ENA_B = 0;
			ChargeFlag = 0;
			StopAddrFlag = 0x01;
			ChargeACK = 1;
			TrainState = ST0;
		}		
	}
	
}





