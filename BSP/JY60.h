#ifndef __JY60_H
#define __JY60_H

//结构体声明
typedef struct 
{
	short a[3];
	short T;
}SAcc;						//加速度计数据结构体
typedef struct 
{
	short w[3];
	short T;
}SGyro;						//陀螺仪数据结构体
typedef struct 
{
	short Angle[3];
	short T;
}SAngle;					//姿态转换之后的数据结构体
 
extern char YAWCMD[3];			//Z轴角度归零
extern char ACCCMD[3];			//加速度计校准
extern char SLEEPCMD[3];		//休眠与解休眠
extern char UARTMODECMD[3];		//串口模式
extern char IICMODECMD[3];		//IIC模式

//函数声明
void sendcmd(char cmd[]);						//模块指令发送函数
void CopeSerial2Data(unsigned char ucData);		//串口读取函数
void CopeSerial1Data(unsigned char ucData);		//转发串口1收到的数据给串口2（JY模块）
void JY60_send(void);							//发送JY60的数据
void JY60_Calibration(void);					//JY60校准函数
void JY60_Get(float * pbuf);					//获取JY60数据
float JY_GET_FIRST(float* bbb);
#endif

