#ifndef __PID_H
#define __PID_H

#include "util.h"
#include "config.h"


extern float MyPidKp;
extern float MyPidKi;
extern float MyPidKd;

/* 串级PIDA轮结构体 */
typedef struct  
{
	float Position_KP;			/* 位置式PID系数 */
	float Position_KI;
	float Position_KD;
	
	float Incremental_KP;		/* 增量式PID系数 */
	float Incremental_KI;
	float Incremental_KD;
	
	float Target_Velocity;		/*    目标速度脉冲值	 */
	float Reality_Velocity;		/*    实际速度脉冲值	 */
	
	float Target_Position;		/*    目标位置脉冲值	 */
	float Reality_Position;     /*    实际位置脉冲值	 */
	
	float PWM;					/*	  PWM输出值	 */
	
	float dt;					/*		频率	 */
	float encoder_counts;		/*		编码圈值(多少个脉冲一圈) */
	
	float Pos;					/*目标圈数*/
	float Pos_current;			/*当前圈数*/			
	float Vel;					/*目标速度,单位RPS*/
	float Vel_current;			/*目标速度,单位RPS*/
	
	float error_margin;			/*	允许误差		*/
	
}Cascade_PIDA;




typedef struct{
	f32_t *yCoefficient;
	f32_t *xCoefficient;
}coefficientStruct_t;

typedef struct{
	uint8_t differentialLength;
	f32_t 	*inputData;
	f32_t 	*outputData;
	uint8_t	initFlag;
	coefficientStruct_t *coefficient;
}differentialDataStruct_t;

typedef struct {
								// Last setpoint 											最后的设置值
	f32_t 						setPoint;		
								// Last position input										最后的状态输入
	f32_t 						dState;		
								// Integrator state   										积分状态
	f32_t 						iState;		
								// integral gain											积分增益
	f32_t 						*iGain;		
								// proportional gain										比例增益
	f32_t 						*pGain;		
								// derivative gain											微分增益
	f32_t 						*dGain;		
								// low pass filter factor (1 - pole) for derivative gain	用于微分增益的低通滤波器因子
	f32_t 						*fGain;
	f32_t 						*pMax, *iMax, *dMax, *oMax;
								// pointers to radio trim channels (or NULL)
	int16_t 					*pTrim, *iTrim, *dTrim, *fTrim;	
	f32_t 						pv_1, pv_2;
	f32_t 						co_1;
	f32_t 						pTerm_1;
	f32_t 						iTerm_1;
	f32_t 						dTerm_1;
	f32_t 						sp_1;
	
	differentialDataStruct_t 	*differential;
} pidStruct_t;




//定义PID结构体用于存放一个PID的数据
typedef struct
{
   	float kp, ki, kd; //三个系数
    float error, lastError; //误差、上次误差
    float integral, maxIntegral; //积分、积分限幅
    float output, maxOutput; //输出、输出限幅
	float pout;//比例计算输出
	float alpha;//低通滤波器系数
	float last_dev;//上一次低通滤波结果
	float this_dev;//当前低通滤波结果
}PID;

//串级PID的结构体，包含两个单级PID
typedef struct
{
    PID inner; //内环
    PID outer; //外环
    float output; //串级输出，等于inner.output
}CascadePID;



typedef struct {
	f32_t dataFbd;
    f32_t dataRef;
    f32_t dataOut;
} pidData_t;


extern Cascade_PIDA  PIDA;             	/* 串级PIDA轮参数结构体 */

pidStruct_t *pidInit(systemConfigPID_t *PIDConfigData);
f32_t differentialCal(differentialDataStruct_t *differential,f32_t currentData);

float pidUpdate(pidStruct_t *pid, float setpoint, float position,float Dt);
void pidZeroIntegral(pidStruct_t *pid, float pv, float iState);
void pidZeroState(pidStruct_t *pid);
float PID_angel(float angle_target, float angle_current,float limit);
float Position_based_PID(float target,float current);
float Incremental_PID(float reality,float target);
float Position_PID(float reality,float target);
float Position_PID_G(float reality,float target);
float Position_PID_P(float reality,float target);
float Position_PID_N(float reality,float target);
float Position_PID_N1(float reality,float target);
float Position_PIDA(Cascade_PIDA *PID,float target);
void PID_Init(PID *pid, float p, float i, float d, float maxI, float maxOut);
void PID_Calc(PID *pid, float reference, float feedback);
void PID_CascadeCalc(CascadePID *pid, float outerRef, float outerFdb, float innerFdb);

float Incremental_PID1(float reality,float target);
#endif


