#ifndef __NAV_UKF_H
#define __NAV_UKF_H

#include "srcdkf.h"
#include "stdbool.h"
#include "util.h"
#include "FreeRTOS_board.h"

#define SIM_S                   10	// states
#define SIM_M                   3		// max measurements
#define SIM_V                   9		// process noise
#define SIM_N                   3		// max observation noise

#define UKF_GYO_AVG_NUM		40

#define UKF_STATE_ACC_BIAS_X	0
#define UKF_STATE_ACC_BIAS_Y	1
#define UKF_STATE_ACC_BIAS_Z	2
#define UKF_STATE_GYO_BIAS_X	3
#define UKF_STATE_GYO_BIAS_Y	4
#define UKF_STATE_GYO_BIAS_Z	5
#define UKF_STATE_Q1		6
#define UKF_STATE_Q2		7
#define UKF_STATE_Q3		8
#define UKF_STATE_Q4		9

#define UKF_V_NOISE_ACC_BIAS_X	0
#define UKF_V_NOISE_ACC_BIAS_Y	1
#define UKF_V_NOISE_ACC_BIAS_Z	2
#define UKF_V_NOISE_GYO_BIAS_X	3
#define UKF_V_NOISE_GYO_BIAS_Y	4
#define UKF_V_NOISE_GYO_BIAS_Z	5
#define UKF_V_NOISE_RATE_X	6
#define UKF_V_NOISE_RATE_Y	7
#define UKF_V_NOISE_RATE_Z	8

#define UKF_ACC_BIAS_X		navUkfData.x[UKF_STATE_ACC_BIAS_X]		//六轴姿态偏差数据
#define UKF_ACC_BIAS_Y		navUkfData.x[UKF_STATE_ACC_BIAS_Y]
#define UKF_ACC_BIAS_Z		navUkfData.x[UKF_STATE_ACC_BIAS_Z]
#define UKF_GYO_BIAS_X		navUkfData.x[UKF_STATE_GYO_BIAS_X]
#define UKF_GYO_BIAS_Y		navUkfData.x[UKF_STATE_GYO_BIAS_Y]
#define UKF_GYO_BIAS_Z		navUkfData.x[UKF_STATE_GYO_BIAS_Z]
#define UKF_Q1			navUkfData.x[UKF_STATE_Q1]					//四元数
#define UKF_Q2			navUkfData.x[UKF_STATE_Q2]
#define UKF_Q3			navUkfData.x[UKF_STATE_Q3]
#define UKF_Q4			navUkfData.x[UKF_STATE_Q4]

#define UKF_INIT_PRIORITY	    	7
#define UKF_INIT_STACK_SIZE	  	256

#ifdef USE_PRES_ALT
#define UKF_ALTITUDE	UKF_PRES_ALT
#else
#define UKF_ALTITUDE	UKF_POSD
#endif

typedef struct {
	TaskHandle_t xHandleTask;
	srcdkf_t *kf;
	float v0a[3];
	float v0m[3];
	double r1, r2;
	int navHistIndex;
	formatTrans32Struct_t yaw, pitch, roll;
	int32_t roundCnt;
	float yawCos, yawSin;
	float *x;			// states				//状态
	float accNorm;
	double time[2];
	float intervalTime;
	bool initState;
	uint16_t initLoops;
} navUkfStruct_t;

extern navUkfStruct_t navUkfData;

void navUkfOfPosUpdate(float *u, float *x, float *noise, float *y);
void navUkfTaskInit(void);
void navUkfInit(void);
void navUkfInertialUpdate(void);
void simDoAccUpdate(float accX, float accY, float accZ);
void simDoMagUpdate(float magX, float magY, float magZ);
void navUkfQuatExtractEuler(float *q, float *yaw, float *pitch, float *roll);
void navUkfZeroRate(float zRate, int axis);
void navUkfFinish(void);
void navUkfRotateVectorByRevQuat(float *vr, float *v, float *q);
void navUkfResetBias(void);
void navUkfRotateVectorByQuat(float *vr, float *v, float *q);

#endif



