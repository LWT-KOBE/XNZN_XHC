/*******************************************************************************
* 文件名： LobotServoController.c
* 作者： 深圳乐幻索尔科技
* 日期：20160806
* LSC系列舵机控制板二次开发示例
*******************************************************************************/
#include "application.h"
bool isUartRxCompleted = false;
uint8_t LobotTxBuf[128];  //发送缓存
u8 UART_RX_BUF[16];
uint8_t LobotRxBuf[16];

uint16_t batteryVolt;

/**************************************************/
static void LobotServeoInit(void);

/**************************************************/

deviceInitClass LobotServoInitClass = {
	LobotServeoInit,
};

/*
***************************************************
函数名：Driver_Lobot_Init
功能：幻尔总线舵机串口初始化
入口参数：	I can understand it at one glance.
返回值：无
应用范围：外部调用
备注：
***************************************************
*/
static void Driver_Lobot_Init(USART_TypeDef *USARTx,\
							BSP_GPIOSource_TypeDef *USART_RX,\
							BSP_GPIOSource_TypeDef *USART_TX,\
							uint8_t PreemptionPriority,uint8_t SubPriority){
	BSP_USART_TypeDef Lobot_USART;
	Lobot_USART.USARTx = USARTx;
	Lobot_USART.USART_RX = USART_RX;
	Lobot_USART.USART_TX = USART_TX;
	//波特率为500000
	Lobot_USART.USART_InitStructure.USART_BaudRate = 9600;	
	//字长为8位数据格式
	Lobot_USART.USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	//一个停止位
	Lobot_USART.USART_InitStructure.USART_StopBits = USART_StopBits_1;
	//无校验位
	Lobot_USART.USART_InitStructure.USART_Parity = USART_Parity_No;
	//接收/发送模式
	Lobot_USART.USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	//无硬件数据流控制
	Lobot_USART.USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None,		
	
	BSP_USART_Init(&Lobot_USART,PreemptionPriority,SubPriority);
	BSP_USART_RX_DMA_Init(&Lobot_USART);																	
}
                            
/****************************************************
函数名：imuSCInit
功能：幻尔总线舵机串口调用初始化
入口参数：无.
返回值：无
应用范围：外部调用
备注：
****************************************************/
static void LobotServeoInit(void){
	Driver_Lobot_Init(Lobot_USARTX,\
	Lobot_USARTX_RX_PIN,\
	Lobot_USARTX_TX_PIN,\
	Lobot_USART_PreemptionPriority,\
	Lobot_USART_SubPriority);
}






void uartWriteBuf(uint8_t *buf, uint8_t len)
{
	while (len--) {
		while ((USART3->SR & 0x40) == 0);
		USART_SendData(USART3,*buf++);
	}
}




void Driver_LobotReadDMA(uint8_t *arrayLobotReceive){
	
	static bool isGotFrameHeader = false;
	static uint8_t frameHeaderCount = 0;
	static uint8_t dataLength = 2;
	static uint8_t dataCount = 0;    
   
	if (!isGotFrameHeader) {  
        
    //若起始和末尾校验失败，则不进行下一步操作    
	if(arrayLobotReceive[0] != FRAME_HEADER){
        
		isGotFrameHeader = false;
		dataCount = 0;
		frameHeaderCount = 0;            
	}	
	else{
		frameHeaderCount++;
			if (frameHeaderCount == 2) {
				frameHeaderCount = 0;
				isGotFrameHeader = true;
				dataCount = 1;
				}                        
 	}         
}   
	if (isGotFrameHeader) { 
		UART_RX_BUF[dataCount] = arrayLobotReceive[dataCount];
		if (dataCount == 2) {
			dataLength = UART_RX_BUF[dataCount];
			if (dataLength < 2 || dataLength > 8) {
				dataLength = 2;
				isGotFrameHeader = false;
				}
			}
		dataCount++;
		if (dataCount == dataLength + 2) {
			if (isUartRxCompleted == false) {
				isUartRxCompleted = true;
				memcpy(LobotRxBuf, UART_RX_BUF, dataCount);
			}
            
		isGotFrameHeader = false;
		}
	}    

    
}







/*********************************************************************************
 * Function:  moveServo
 * Description： 控制单个舵机转动
 * Parameters:   sevoID:舵机ID，Position:目标位置,Time:转动时间
                    舵机ID取值:0<=舵机ID<=31,Time取值: Time > 0
 * Return:       无返回
 * Others:
 **********************************************************************************/
void moveServo(uint8_t servoID, uint16_t Position, uint16_t Time)
{
	if (servoID > 31 || !(Time > 0)) {  //舵机ID不能打于31,可根据对应控制板修改
		return;
	}
	LobotTxBuf[0] = LobotTxBuf[1] = FRAME_HEADER;    //填充帧头
	LobotTxBuf[2] = 8;
	LobotTxBuf[3] = CMD_SERVO_MOVE;           //数据长度=要控制舵机数*3+5，此处=1*3+5//填充舵机移动指令
	LobotTxBuf[4] = 1;                        //要控制的舵机个数
	LobotTxBuf[5] = GET_LOW_BYTE(Time);       //取得时间的低八位
	LobotTxBuf[6] = GET_HIGH_BYTE(Time);      //取得时间的高八位
	LobotTxBuf[7] = servoID;                  //舵机ID
	LobotTxBuf[8] = GET_LOW_BYTE(Position);   //取得目标位置的低八位
	LobotTxBuf[9] = GET_HIGH_BYTE(Position);  //取得目标位置的高八位

	uartWriteBuf(LobotTxBuf, 10);
}

/*********************************************************************************
 * Function:  runActionGroup
 * Description： 运行指定动作组
 * Parameters:   NumOfAction:动作组序号, Times:执行次数
 * Return:       无返回
 * Others:       Times = 0 时无限循环
 **********************************************************************************/
void runActionGroup(uint8_t numOfAction, uint16_t Times)
{
	LobotTxBuf[0] = LobotTxBuf[1] = FRAME_HEADER;  //填充帧头
	LobotTxBuf[2] = 5;                      //数据长度，数据帧除帧头部分数据字节数，此命令固定为5
	LobotTxBuf[3] = CMD_ACTION_GROUP_RUN;   //填充运行动作组命令
	LobotTxBuf[4] = numOfAction;            //填充要运行的动作组号
	LobotTxBuf[5] = GET_LOW_BYTE(Times);    //取得要运行次数的低八位
	LobotTxBuf[6] = GET_HIGH_BYTE(Times);   //取得要运行次数的高八位

	uartWriteBuf(LobotTxBuf, 7);            //发送
}

/*********************************************************************************
 * Function:  moveServosByArray
 * Description： 控制多个舵机转动
 * Parameters:   servos[]:舵机结体数组，Num:舵机个数,Time:转动时间
                    0 < Num <= 32,Time > 0
 * Return:       无返回
 * Others:
 **********************************************************************************/
void moveServosByArray(LobotServo servos[], uint8_t Num, uint16_t Time)
{
	uint8_t index = 7;
	uint8_t i = 0;

	if (Num < 1 || Num > 32 || !(Time > 0)) {
		return;                                          //舵机数不能为零和大与32，时间不能为零
	}
	LobotTxBuf[0] = LobotTxBuf[1] = FRAME_HEADER;      //填充帧头
	LobotTxBuf[2] = Num * 3 + 5;                       //数据长度 = 要控制舵机数*3+5
	LobotTxBuf[3] = CMD_SERVO_MOVE;                    //填充舵机移动指令
	LobotTxBuf[4] = Num;                               //要控制的舵机个数
	LobotTxBuf[5] = GET_LOW_BYTE(Time);                //取得时间的低八位
	LobotTxBuf[6] = GET_HIGH_BYTE(Time);               //取得时间的高八位

	for (i = 0; i < Num; i++) {                        //循环填充舵机ID和对应目标位置
		LobotTxBuf[index++] = servos[i].ID;              //填充舵机ID
		LobotTxBuf[index++] = GET_LOW_BYTE(servos[i].Position); //填充目标位置低八位
		LobotTxBuf[index++] = GET_HIGH_BYTE(servos[i].Position);//填充目标位置高八位
	}

	uartWriteBuf(LobotTxBuf, LobotTxBuf[2] + 2);             //发送
}

/*********************************************************************************
 * Function:  moveServos
 * Description： 控制多个舵机转动
 * Parameters:   Num:舵机个数,Time:转动时间,...:舵机ID,转动角，舵机ID,转动角度 如此类推
 * Return:       无返回
 * Others:
 **********************************************************************************/
void moveServos(uint8_t Num, uint16_t Time, ...)
{
	uint8_t index = 7;
	uint8_t i = 0;
	uint16_t temp;
	va_list arg_ptr;  //

	va_start(arg_ptr, Time); //取得可变参数首地址
	if (Num < 1 || Num > 32) {
		return;               //舵机数不能为零和大与32，时间不能小于0
	}
	LobotTxBuf[0] = LobotTxBuf[1] = FRAME_HEADER;      //填充帧头
	LobotTxBuf[2] = Num * 3 + 5;                //数据长度 = 要控制舵机数 * 3 + 5
	LobotTxBuf[3] = CMD_SERVO_MOVE;             //舵机移动指令
	LobotTxBuf[4] = Num;                        //要控制舵机数
	LobotTxBuf[5] = GET_LOW_BYTE(Time);         //取得时间的低八位
	LobotTxBuf[6] = GET_HIGH_BYTE(Time);        //取得时间的高八位

	for (i = 0; i < Num; i++) {//从可变参数中取得并循环填充舵机ID和对应目标位置
		temp = va_arg(arg_ptr, int);//可参数中取得舵机ID
		LobotTxBuf[index++] = GET_LOW_BYTE(((uint16_t)temp));
		temp = va_arg(arg_ptr, int);  //可变参数中取得对应目标位置
		LobotTxBuf[index++] = GET_LOW_BYTE(((uint16_t)temp)); //填充目标位置低八位
		LobotTxBuf[index++] = GET_HIGH_BYTE(temp);//填充目标位置高八位
	}

	va_end(arg_ptr);  //置空arg_ptr

	uartWriteBuf(LobotTxBuf, LobotTxBuf[2] + 2);    //发送
}




/*********************************************************************************
 * Function:  stopActiongGroup
 * Description： 停止动作组运行
 * Parameters:   Speed: 目标速度
 * Return:       无返回
 * Others:
 **********************************************************************************/
void stopActionGroup(void)
{
	LobotTxBuf[0] = FRAME_HEADER;     //填充帧头
	LobotTxBuf[1] = FRAME_HEADER;
	LobotTxBuf[2] = 2;                //数据长度，数据帧除帧头部分数据字节数，此命令固定为2
	LobotTxBuf[3] = CMD_ACTION_GROUP_STOP;   //填充停止运行动作组命令

	uartWriteBuf(LobotTxBuf, 4);      //发送
}
/*********************************************************************************
 * Function:  setActionGroupSpeed
 * Description： 设定指定动作组的运行速度
 * Parameters:   NumOfAction: 动作组序号 , Speed:目标速度
 * Return:       无返回
 * Others:
 **********************************************************************************/
void setActionGroupSpeed(uint8_t numOfAction, uint16_t Speed)
{
	LobotTxBuf[0] = LobotTxBuf[1] = FRAME_HEADER;   //填充帧头
	LobotTxBuf[2] = 5;                       //数据长度，数据帧除帧头部分数据字节数，此命令固定为5
	LobotTxBuf[3] = CMD_ACTION_GROUP_SPEED;  //填充设置动作组速度命令
	LobotTxBuf[4] = numOfAction;             //填充要设置的动作组号
	LobotTxBuf[5] = GET_LOW_BYTE(Speed);     //获得目标速度的低八位
	LobotTxBuf[6] = GET_HIGH_BYTE(Speed);    //获得目标熟读的高八位

	uartWriteBuf(LobotTxBuf, 7);             //发送
}

/*********************************************************************************
 * Function:  setAllActionGroupSpeed
 * Description： 设置所有动作组的运行速度
 * Parameters:   Speed: 目标速度
 * Return:       无返回
 * Others:
 **********************************************************************************/
void setAllActionGroupSpeed(uint16_t Speed)
{
	setActionGroupSpeed(0xFF, Speed);  //调用动作组速度设定，组号为0xFF时设置所有组的速度
}

/*********************************************************************************
 * Function:  getBatteryVoltage
 * Description： 发送获取电池电压命令
 * Parameters:   Timeout：重试次数
 * Return:       无返回
 * Others:
 **********************************************************************************/
void getBatteryVoltage(void)
{
//	uint16_t Voltage = 0;
	LobotTxBuf[0] = FRAME_HEADER;  //填充帧头
	LobotTxBuf[1] = FRAME_HEADER;
	LobotTxBuf[2] = 2;             //数据长度，数据帧除帧头部分数据字节数，此命令固定为2
	LobotTxBuf[3] = CMD_GET_BATTERY_VOLTAGE;  //填充获取电池电压命令

	uartWriteBuf(LobotTxBuf, 4);   //发送
}

void receiveHandle()
{
	//可以根据二次开发手册添加其他指令
	if (isUartRxCompleted) {
		isUartRxCompleted = false;
		switch (LobotRxBuf[3]) {
		case CMD_GET_BATTERY_VOLTAGE: //获取电压
			batteryVolt = (((uint16_t)(LobotRxBuf[5])) << 8) | (LobotRxBuf[4]);
			break;
		default:
			break;
		}
	}
}





