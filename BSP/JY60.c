#include "JY60.h"
#include <string.h>
#include <stdio.h>
//#include "delay.h"
#include "usartx.h"

char YAWCMD[3] = {0XFF,0XAA,0X52};				//Z轴角度归零的指令
char ACCCMD[3] = {0XFF,0XAA,0X67};              //加速度计校准的指令
char SLEEPCMD[3] = {0XFF,0XAA,0X60};            //休眠与解休眠的指令
char UARTMODECMD[3] = {0XFF,0XAA,0X61};         //串口模式的指令
char IICMODECMD[3] = {0XFF,0XAA,0X62};          //IIC模式的指令

SAcc     stcAcc;	//定义加速度计结构体变量
SGyro 	stcGyro;	//定义陀螺仪结构体变量
SAngle	stcAngle;	//定义角度结构体变量

int32_t CNT = 0;

int FLAG = 0,FLAG1 = 0;
float angle_now = 0,angle_last = 0,angle_err = 0,angle_frist = 0;

//用串口2给JY模块发送指令
void sendcmd(char cmd[])
{
	char i;
	for(i=0;i<3;i++)
		UART2_Put_Char(cmd[i]);
}

//CopeSerialData为串口2中断调用函数，串口每收到一个数据，调用一次这个函数。
void CopeSerial2Data(unsigned char ucData)
{
	static unsigned char ucRxBuffer[250];
	static unsigned char ucRxCnt = 0;	
					//接收到数据，LED灯闪烁一下
	ucRxBuffer[ucRxCnt++]=ucData;	//将收到的数据存入缓冲区中
	if (ucRxBuffer[0]!=0x55) //数据头不对，则重新开始寻找0x55数据头
	{
		ucRxCnt=0;
		return;
	}
	if (ucRxCnt<11) {return;}//数据不满11个，则返回
	else
	{
		switch(ucRxBuffer[1])//判断数据是哪种数据，然后将其拷贝到对应的结构体中，有些数据包需要通过上位机打开对应的输出后，才能接收到这个数据包的数据
		{
			//memcpy为编译器自带的内存拷贝函数，需引用"string.h"，将接收缓冲区的字符拷贝到数据结构体里面，从而实现数据的解析。
			case 0x51:	memcpy(&stcAcc,&ucRxBuffer[2],8);break;
			case 0x52:	memcpy(&stcGyro,&ucRxBuffer[2],8);break;
			case 0x53:	memcpy(&stcAngle,&ucRxBuffer[2],8);break;

		}
		ucRxCnt=0;//清空缓存区
	}
}

void CopeSerial1Data(unsigned char ucData)
{	
	UART2_Put_Char(ucData);//转发串口1收到的数据给串口2（JY模块）
}

//发送JY60的数据
void JY60_send(void){
		printf("Acc:%.3f %.3f %.3f\r\n",(float)stcAcc.a[0]/32768*16,(float)stcAcc.a[1]/32768*16,(float)stcAcc.a[2]/32768*16);
		//delay_ms(10);
		//输出角速度
		printf("Gyro:%.3f %.3f %.3f\r\n",(float)stcGyro.w[0]/32768*2000,(float)stcGyro.w[1]/32768*2000,(float)stcGyro.w[2]/32768*2000);
		//delay_ms(10);
		//输出角度
		printf("Angle:%.3f %.3f %.3f\r\n",(float)stcAngle.Angle[0]/32768*180,(float)stcAngle.Angle[1]/32768*180,(float)stcAngle.Angle[2]/32768*180);
		//delay_ms(10);//等待传输完成
}

//校准JY60传感器数据
void JY60_Calibration(void){
	//校准加速度计数据
	sendcmd(ACCCMD);//等待模块内部自动校准好，模块内部会自动计算需要一定的时间
	//校准Z轴角度为0
	sendcmd(YAWCMD);//等待模块内部自动校准好，模块内部会自动计算需要一定的时间
	//printf("校准成功!\r\n");
}
float JY_GET_FIRST(float* bbb){
	FLAG ++;
	if(FLAG == 20){
		if((float)stcAngle.Angle[2]>=0.0f && stcAngle.Angle[2] < 32767){
		bbb[0] = (float)stcAngle.Angle[2]/32768*180.0f;	//z轴
		}
	
		if((float)stcAngle.Angle[2]< 0.0f && stcAngle.Angle[2] > -32768){
			bbb[0] = (float)stcAngle.Angle[2]/32768*180.0f + 360.0f;	//z轴
		}
		FLAG = 0;
	}
	if(fabs(bbb[0]>0)){
		FLAG1 = 1;
	}
	if(FLAG1 == 1){
		FLAG = 0;
	}
	return bbb[0];
}



//获取JY60传感器数据
void JY60_Get(float * pbuf){
	
//	static float A = 0;
	//获取加速度计数据
	pbuf[0] = (float)stcAcc.a[0]/32768*16;			//Acc_x
	pbuf[1] = (float)stcAcc.a[1]/32768*16;			//Acc_y
	pbuf[2] = (float)stcAcc.a[2]/32768*16;			//Acc_z
	
	//获取陀螺仪数据	
	pbuf[3] = (float)stcGyro.w[0]/32768*2000;		//Gyro_x
	pbuf[4] = (float)stcGyro.w[1]/32768*2000;		//Gyro_y
	pbuf[5] = (float)stcGyro.w[2]/32768*2000;		//Gyro_z
	
	//姿态解算之后的数据
	pbuf[6] = (float)stcAngle.Angle[0]/32768*180;	//x轴
	pbuf[7] = (float)stcAngle.Angle[1]/32768*180;	//y轴
	
	if((float)stcAngle.Angle[2]>=0.0f && stcAngle.Angle[2] < 32767){
		pbuf[8] = (float)stcAngle.Angle[2]/32768*180.0f;	//z轴
	}
	
	if((float)stcAngle.Angle[2]< 0.0f && stcAngle.Angle[2] > -32768){
		pbuf[8] = (float)stcAngle.Angle[2]/32768*180.0f + 360.0f;	//z轴
	}
	
	
	
	//获取当前角度
	//angle_now = pbuf[8];
	//后续读取到的角度都减去初始角度,确保准确性(JY60六轴系列会自动调零，JY90系列是绝对角度所以得手动调零)
	//pbuf[8] = pbuf[8] - A;
	angle_now = pbuf[8];
	//计算角度误差
	angle_err = angle_now - angle_last;
	
	
	
	//过零检测算法
	//如果误差大于180.0度
	if(angle_err >  180.0f){
		CNT--;
	}
	if(angle_err < -180.0){
		CNT++;
	}
	
	//计算累积角度
	pbuf[9] = CNT * 360 + pbuf[8];
	//获取上一次的角度
	angle_last = angle_now;
	
}


