#include "nav_ukf.h"
#include "imu.h"
#include "run.h"
#include "processing.h"
#include "clockcount.h"
#include "supervisor.h"

//过程激励噪声协方差，是状态转移矩阵与实际过程之间的误差
#define UKF_ACC_BIAS_Q          +1.3317e-03	     					
#define UKF_GYO_BIAS_Q          +4.5256e-02    
#define UKF_QUAT_Q              +5.4005e-04   
#define UKF_ACC_BIAS_V          +7.8673e-07    
#define UKF_GYO_BIAS_V          +4.0297e-09    
#define UKF_RATE_V              +1.7538e-05    
#define UKF_ACC_N               +6.3287e-05   
#define UKF_DIST_N              +9.7373e-03  
#define UKF_MAG_N               +5.2355e-01    

#ifndef __CC_ARM
#include <intrinsics.h>
#endif

navUkfStruct_t navUkfData;

void navUkfNormalizeVec3(float *vr, float *v) {
    float norm;

    norm = __sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);

    vr[0] = v[0] / norm;
    vr[1] = v[1] / norm;
    vr[2] = v[2] / norm;
}

void navUkfNormalizeQuat(float *qr, float *q) {
    float norm;

    norm = 1.0f / __sqrtf(q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]);

    qr[0] *= norm;
    qr[1] *= norm;
    qr[2] *= norm;
    qr[3] *= norm;
}

void crossVector3(float *vr, float *va, float *vb) {
    vr[0] = va[1] * vb[2] - vb[1] * va[2];
    vr[1] = va[2] * vb[0] - vb[2] * va[0];
    vr[2] = va[0] * vb[1] - vb[0] * va[1];
}

float dotVector3(float *va, float *vb) {
    return va[0]*vb[0] + va[1]*vb[1] + va[2]*vb[2];
}

void navUkfRotateVectorByQuat(float *vr, float *v, float *q) {
    float w, x, y, z;

    w = q[0];
    x = q[1];
    y = q[2];
    z = q[3];

    vr[0] = w*w*v[0] + 2.0f*y*w*v[2] - 2.0f*z*w*v[1] + x*x*v[0] + 2.0f*y*x*v[1] + 2.0f*z*x*v[2] - z*z*v[0] - y*y*v[0];
    vr[1] = 2.0f*x*y*v[0] + y*y*v[1] + 2.0f*z*y*v[2] + 2.0f*w*z*v[0] - z*z*v[1] + w*w*v[1] - 2.0f*x*w*v[2] - x*x*v[1];
    vr[2] = 2.0f*x*z*v[0] + 2.0f*y*z*v[1] + z*z*v[2] - 2.0f*w*y*v[0] - y*y*v[2] + 2.0f*w*x*v[1] - x*x*v[2] + w*w*v[2];
}

void navUkfRotateVectorByRevQuat(float *vr, float *v, float *q) {
    float qc[4];

    qc[0] = q[0];
    qc[1] = -q[1];
    qc[2] = -q[2];
    qc[3] = -q[3];

    navUkfRotateVectorByQuat(vr, v, qc);			//将四元数旋转量取负后进行叉乘
}
//将加速度转到世界坐标系
void navUkfRotateVecByMatrix(float *vr, float *v, float *m) {
    vr[0] = m[0*3 + 0]*v[0] + m[0*3 + 1]*v[1] + m[0*3 + 2]*v[2];
    vr[1] = m[1*3 + 0]*v[0] + m[1*3 + 1]*v[1] + m[1*3 + 2]*v[2];
    vr[2] = m[2*3 + 0]*v[0] + m[2*3 + 1]*v[1] + m[2*3 + 2]*v[2];
}

static void navUkfRotateVecByRevMatrix(float *vr, float *v, float *m) {
    vr[0] = m[0*3 + 0]*v[0] + m[1*3 + 0]*v[1] + m[2*3 + 0]*v[2];
    vr[1] = m[0*3 + 1]*v[0] + m[1*3 + 1]*v[1] + m[2*3 + 1]*v[2];
    vr[2] = m[0*3 + 2]*v[0] + m[1*3 + 2]*v[1] + m[2*3 + 2]*v[2];
}
/*--------------- 由四元数计算出重力向量 ---------------*/
static void navUkfQuatToMatrix(float *m, float *q, int normalize) {
    float sqw = q[0]*q[0];
    float sqx = q[1]*q[1];
    float sqy = q[2]*q[2];
    float sqz = q[3]*q[3];
    float tmp1, tmp2;
    float invs;

    // get the invert square length					获取反演平方长度
    if (normalize)												//如果已经归一化则也对四元数进行归一化
			invs = 1.0f / (sqx + sqy + sqz + sqw);
    else
			invs = 1.0f;

    // rotation matrix is scaled by inverse square length		用逆平方长度缩放旋转矩阵？？？
    m[0*3 + 0] = ( sqx - sqy - sqz + sqw) * invs;
    m[1*3 + 1] = (-sqx + sqy - sqz + sqw) * invs;
    m[2*3 + 2] = (-sqx - sqy + sqz + sqw) * invs;

    tmp1 = q[1]*q[2];
    tmp2 = q[3]*q[0];
    m[1*3 + 0] = 2.0f * (tmp1 + tmp2) * invs;
    m[0*3 + 1] = 2.0f * (tmp1 - tmp2) * invs;

    tmp1 = q[1]*q[3];
    tmp2 = q[2]*q[0];
    m[2*3 + 0] = 2.0f * (tmp1 - tmp2) * invs;
    m[0*3 + 2] = 2.0f * (tmp1 + tmp2) * invs;

    tmp1 = q[2]*q[3];
    tmp2 = q[1]*q[0];
    m[2*3 + 1] = 2.0f * (tmp1 + tmp2) * invs;
    m[1*3 + 2] = 2.0f * (tmp1 - tmp2) * invs;
}

void navUkfQuatExtractEuler(float *q, float *yaw, float *pitch, float *roll) {
    float q0, q1, q2, q3;

    q0 = q[0];
    q1 = q[1];
    q2 = q[2];
    q3 = q[3];

	  *yaw = -atan2f((2.0f * (q1 * q2 + q0 * q3)), (1.0f - 2.0f * q2 * q2 - 2.0f * q3 *q3));
    *pitch = (0.5f * M_PI) - acosf(2.0f *(q1 * q3 - q0 *q2));
    *roll = atan2f((2.0f * (q0 * q1 + q3 * q2)), (1.0f - 2.0f * q1 * q1 - 2.0f * q2 * q2));
}

// result and source can be the same				//结果和输入可以一样
//计算余弦矩阵
static void navUkfRotateQuat(float *qOut, float *qIn, float *rate) {
    float q[4];
    float r[3];

    r[0] = rate[0] * -0.5f;
    r[1] = rate[1] * -0.5f;
    r[2] = rate[2] * -0.5f;

    q[0] = qIn[0];
    q[1] = qIn[1];
    q[2] = qIn[2];
    q[3] = qIn[3];

    // rotate
    qOut[0] =       q[0] + r[0]*q[1] + r[1]*q[2] + r[2]*q[3];
    qOut[1] = -r[0]*q[0] +      q[1] - r[2]*q[2] + r[1]*q[3];
    qOut[2] = -r[1]*q[0] + r[2]*q[1] +      q[2] - r[0]*q[3];
    qOut[3] = -r[2]*q[0] - r[1]*q[1] + r[0]*q[2] +      q[3];
}
/*------------------- 这个函数相当于状态推导公式 ------------------*/
void navUkfTimeUpdate(float *in, float *noise, float *out, float *u, float dt, int n) {				
    float tmp[3], acc[3];
    float rate[3];
    float mat3x3[3*3];
    float q[4];
    int i;

    // assume out == in									//假定输出等于输入
    out = in;

		for (i = 0; i < n; i++){

			// create rot matrix from current quat			//四元数计算余弦矩阵
			q[0] = in[UKF_STATE_Q1*n + i];
			q[1] = in[UKF_STATE_Q2*n + i];
			q[2] = in[UKF_STATE_Q3*n + i];
			q[3] = in[UKF_STATE_Q4*n + i];
			navUkfQuatToMatrix(mat3x3, q, 1);						//算出重力向量

			// acc											//中间量加速度 = 输入加速度(原始量) +输入偏差加速度
			tmp[0] = u[0] + in[UKF_STATE_ACC_BIAS_X*n + i];
			tmp[1] = u[1] + in[UKF_STATE_ACC_BIAS_Y*n + i];
			tmp[2] = u[2] + in[UKF_STATE_ACC_BIAS_Z*n + i];

			// rotate acc to world frame					//将加速度转到世界坐标系
			navUkfRotateVecByMatrix(acc, tmp, mat3x3);
			acc[2] += GRAVITY;								//Z轴加上重力

			// acc bias										//输出加速度偏差 = 输入加速度偏差+ 噪声*时间 
			out[UKF_STATE_ACC_BIAS_X*n + i] = in[UKF_STATE_ACC_BIAS_X*n + i] + noise[UKF_V_NOISE_ACC_BIAS_X*n + i] * dt;
			out[UKF_STATE_ACC_BIAS_Y*n + i] = in[UKF_STATE_ACC_BIAS_Y*n + i] + noise[UKF_V_NOISE_ACC_BIAS_Y*n + i] * dt;
			out[UKF_STATE_ACC_BIAS_Z*n + i] = in[UKF_STATE_ACC_BIAS_Z*n + i] + noise[UKF_V_NOISE_ACC_BIAS_Z*n + i] * dt;

			// rate = rate + bias + noise					//输出角速度 = (输入角速度(原始量) + 输入偏差角速度 + 噪声)*时间 
			rate[0] = (u[3] + in[UKF_STATE_GYO_BIAS_X*n + i] + noise[UKF_V_NOISE_RATE_X*n + i]) * dt;
			rate[1] = (u[4] + in[UKF_STATE_GYO_BIAS_Y*n + i] + noise[UKF_V_NOISE_RATE_Y*n + i]) * dt;
			rate[2] = (u[5] + in[UKF_STATE_GYO_BIAS_Z*n + i] + noise[UKF_V_NOISE_RATE_Z*n + i]) * dt;

			// rotate quat												//旋转四元数
			navUkfRotateQuat(q, q, rate);					//这一步完成角速度对四元数的更新
			out[UKF_STATE_Q1*n + i] = q[0];
			out[UKF_STATE_Q2*n + i] = q[1];
			out[UKF_STATE_Q3*n + i] = q[2];
			out[UKF_STATE_Q4*n + i] = q[3];

			// gbias															//输出角速度偏差 = 输入角速度偏差 + 角速度噪声*时间 
			out[UKF_STATE_GYO_BIAS_X*n + i] = in[UKF_STATE_GYO_BIAS_X*n + i] + noise[UKF_V_NOISE_GYO_BIAS_X*n + i] * dt;
			out[UKF_STATE_GYO_BIAS_Y*n + i] = in[UKF_STATE_GYO_BIAS_Y*n + i] + noise[UKF_V_NOISE_GYO_BIAS_Y*n + i] * dt;
			out[UKF_STATE_GYO_BIAS_Z*n + i] = in[UKF_STATE_GYO_BIAS_Z*n + i] + noise[UKF_V_NOISE_GYO_BIAS_Z*n + i] * dt;
    }
}

void navUkfRateUpdate(float *u, float *x, float *noise, float *y) {
	y[0] = -x[UKF_STATE_GYO_BIAS_X+(int)u[0]] + noise[0];			//这里的u用来自动选择状态变量的下标
}

void navUkfAccUpdate(float *u, float *x, float *noise, float *y) {
	navUkfRotateVectorByRevQuat(y, navUkfData.v0a, &x[UKF_STATE_Q1]);			//与四元数进行叉乘
	y[0] += noise[0];
	y[1] += noise[1];
	y[2] += noise[2];
}

void navUkfMagUpdate(float *u, float *x, float *noise, float *y) {
	navUkfRotateVectorByRevQuat(y, navUkfData.v0m, &x[UKF_STATE_Q1]);			//与四元数进行叉乘
	y[0] += noise[0];
	y[1] += noise[1];
	y[2] += noise[2];
}
/*-------------------- 结束navUKF运算，并给四元数赋值，提取欧拉角 ----------------------*/
void navUkfFinish(void) {
	navUkfNormalizeQuat(&UKF_Q1, &UKF_Q1);														//标准化四元数
	navUkfQuatExtractEuler(&UKF_Q1, &navUkfData.yaw.float_temp, &navUkfData.pitch.float_temp, &navUkfData.roll.float_temp);		//四元数提取欧拉角
	navUkfData.yaw.float_temp = compassNormalize(navUkfData.yaw.float_temp * RAD_TO_DEG);
	navUkfData.pitch.float_temp *= RAD_TO_DEG;
	navUkfData.roll.float_temp *= RAD_TO_DEG;
	//    x' = x cos f - y sin f
	//    y' = y cos f + x sin f
	navUkfData.yawCos = cosf(navUkfData.yaw.float_temp * DEG_TO_RAD);
	navUkfData.yawSin = sinf(navUkfData.yaw.float_temp * DEG_TO_RAD);
	
}
/*----------------- 此函数为导航UKF惯性更新 -----------------*/
void navUkfInertialUpdate(void) {
	float u[6];

	u[0] = IMU_ACCX;																			//获取六轴数据
	u[1] = IMU_ACCY;
	u[2] = IMU_ACCZ;

	u[3] = IMU_RATEX;
	u[4] = IMU_RATEY;
	u[5] = IMU_RATEZ;
	
	navUkfData.time[0] = getClockCount();
	navUkfData.intervalTime = (float)(navUkfData.time[0] - navUkfData.time[1]);
	navUkfData.time[1] = navUkfData.time[0];

	srcdkfTimeUpdate(navUkfData.kf, u, navUkfData.intervalTime);//UKF时序更新，第二个输入为状态变量，第三个输入为运行周期
}

void navUkfZeroRate(float rate, int axis) {
    float noise[1];        // measurement variance		测量方差
    float y[1];            // measurment(s)				测量量
    float u[1];		   // user data						用户数据

    noise[0] = 0.00001f;
    y[0] = rate;
    u[0] = (float)axis;

    srcdkfMeasurementUpdate(navUkfData.kf, u, y, 1, 1, noise, navUkfRateUpdate);
}

void simDoAccUpdate(float accX, float accY, float accZ) {
    float noise[3];        // measurement variance
    float y[3];            // measurement(s)
    float norm;

    // normalize vector						//归一化向量
    norm =  __sqrtf(accX*accX + accY*accY + accZ*accZ);
		navUkfData.accNorm = __sqrtf(accX*accX + accY*accY + accZ*accZ);
    y[0] = accX / norm;
    y[1] = accY / norm;
    y[2] = accZ / norm;

    noise[0] = UKF_ACC_N + fabsf(GRAVITY - norm) * UKF_DIST_N;
    noise[1] = noise[0];
    noise[2] = noise[0];

    srcdkfMeasurementUpdate(navUkfData.kf, 0, y, 3, 3, noise, navUkfAccUpdate);
}

void simDoMagUpdate(float magX, float magY, float magZ) {
    float noise[3];        // measurement variance
    float y[3];            // measurement(s)
    float norm;

    noise[0] = UKF_MAG_N;
    noise[1] = noise[0];
    noise[2] = noise[0];

    // normalize vector
    norm = 1.0f / __sqrtf(magX*magX + magY*magY + magZ*magZ);
    y[0] = magX * norm;
    y[1] = magY * norm;
    y[2] = magZ * norm;

    srcdkfMeasurementUpdate(navUkfData.kf, 0, y, 3, 3, noise, navUkfMagUpdate);
}

void navUkfResetBias(void) {
    // acc bias
    UKF_ACC_BIAS_X = 0.0f;
    UKF_ACC_BIAS_Y = 0.0f;
    UKF_ACC_BIAS_Z = 0.0f;

    // gyo bias
    UKF_GYO_BIAS_X = 0.0f;
    UKF_GYO_BIAS_Y = 0.0f;
    UKF_GYO_BIAS_Z = 0.0f;
}

void imuQuasiStatic(int n) {
	float stdX, stdY, stdZ;
	float vX[n];
	float vY[n];
	float vZ[n];
	int i, j;
	i = 0;
	j = 0;
	do{
		xEventGroupWaitBits(taskInitData.eventGroups,UKF_INIT_FLAG,pdTRUE,pdTRUE,portMAX_DELAY);
		vX[j] = IMU_ACCX;
		vY[j] = IMU_ACCY;
		vZ[j] = IMU_ACCZ;
		j = (j + 1) % n;
		//最少计算n组数据的标准差
		if (i >= n){
			arm_std_f32(vX, n, &stdX);
			arm_std_f32(vY, n, &stdY);
			arm_std_f32(vZ, n, &stdZ);
		}
		i++;
	}while (i < (int)(1.0f / DIMU_OUTER_DT) * IMU_STATIC_TIMEOUT && (i <= n || (stdX + stdY + stdZ) > IMU_STATIC_STD));		
	//循环直到次数不小于1000或标准差总和小于IMU_STATIC_STD,最少循环n次
}

/*--------------- 初始化状态变量 ---------------*/
static void navUkfInitState(void *Parameters) {
	float acc[3];
	float estAcc[3];
#ifdef USE_MAG
	float mag[3];
	float estMag[3];
#endif
	float m[3*3];
	//第一次更新则等待赋值
	if(!navUkfData.initLoops){
		// acc bias
		UKF_ACC_BIAS_X = 0.0f;
		UKF_ACC_BIAS_Y = 0.0f;
		UKF_ACC_BIAS_Z = 0.0f;

		// gyo bias
		UKF_GYO_BIAS_X = 0.0f;
		UKF_GYO_BIAS_Y = 0.0f;
		UKF_GYO_BIAS_Z = 0.0f;

		// quat
		UKF_Q1 =  1.0f;
		UKF_Q2 =  0.0f;
		UKF_Q3 =  0.0f;
		UKF_Q4 =  0.0f;

		// wait for lack of movement		等待加速度计出数据
		imuQuasiStatic(UKF_GYO_AVG_NUM);
		// estimate initial orientation		估计初始方向
	}
	while(navUkfData.initLoops < 250){
		//等待传感器准备完毕
		xEventGroupWaitBits(taskInitData.eventGroups,UKF_INIT_FLAG,pdTRUE,pdTRUE,portMAX_DELAY);
		float rotError[3];
		
		vTaskDelay(4*(configTICK_RATE_HZ / 1000));
#ifdef USE_MAG
		mag[0] = IMU_MAGX;
		mag[1] = IMU_MAGY;
		mag[2] = IMU_MAGZ;
#endif
		acc[0] = IMU_ACCX;
		acc[1] = IMU_ACCY;
		acc[2] = IMU_ACCZ;

		navUkfNormalizeVec3(acc, acc);								//加速度归一化
#ifdef USE_MAG
		navUkfNormalizeVec3(mag, mag);								//磁力计归一化
#endif		
		navUkfQuatToMatrix(m, &UKF_Q1, 1);
		// rotate gravity to body frame of reference	//旋转重力到身体参照系
		navUkfRotateVecByRevMatrix(estAcc, navUkfData.v0a, m);
#ifdef USE_MAG
		// rotate mags to body frame of reference			//旋转磁力到身体参照系
		navUkfRotateVecByRevMatrix(estMag, navUkfData.v0m, m);	
#endif
		// measured error, starting with ACC vector		//测量误差，从加速度矢量开始
		rotError[0] = -(acc[2] * estAcc[1] - estAcc[2] * acc[1]) * 1.0f;
		rotError[1] = -(acc[0] * estAcc[2] - estAcc[0] * acc[2]) * 1.0f;
		rotError[2] = -(acc[1] * estAcc[0] - estAcc[1] * acc[0]) * 1.0f;
#ifdef USE_MAG
		// add in MAG vector													//测量误差，添加磁力矢量
		rotError[0] += -(mag[2] * estMag[1] - estMag[2] * mag[1]) * 1.0f;
		rotError[1] += -(mag[0] * estMag[2] - estMag[0] * mag[2]) * 1.0f;
		rotError[2] += -(mag[1] * estMag[0] - estMag[1] * mag[0]) * 1.0f;
#endif
		navUkfRotateQuat(&UKF_Q1, &UKF_Q1, rotError);	//计算UKF四元数
		navUkfNormalizeQuat(&UKF_Q1, &UKF_Q1);				//UKF的四元数归一化
		digitalIncreasing(&navUkfData.initLoops);
	}
	//初始化结束
	navUkfData.initState = true;
	digitalClan(&navUkfData.initLoops);
	//删除当前任务
	vTaskDelete(navUkfData.xHandleTask);
}

void navUkfTaskInit(void){
	navUkfData.initState = false;
	digitalClan(&navUkfData.initLoops);
	getsupervisorData()->taskEvent[UKF_INIT_TASK] = xTaskCreate(navUkfInitState,"UKF_INIT",UKF_INIT_STACK_SIZE,NULL,UKF_INIT_PRIORITY,&navUkfData.xHandleTask);
}

/*------------- UKF初始化 ------------*/
void navUkfInit(void) {
	float Q[SIM_S];		// state variance		状态方差
	float V[SIM_V];		// process variance		过程噪声方差
#ifdef USE_MAG
	float mag[3];
#endif
  memset((void *)&navUkfData, 0, sizeof(navUkfData));

  navUkfData.v0a[0] = 0.0f;
  navUkfData.v0a[1] = 0.0f;
  navUkfData.v0a[2] = -1.0f;
#ifdef USE_MAG
	// calculate mag vector based on inclination			基于倾角的MAG矢量计算
	mag[0] = cosf(-65.0 * DEG_TO_RAD);
	mag[1] = 0.0f;
	mag[2] = -sinf(-65.0 * DEG_TO_RAD);
	// rotate local mag vector to align with true north		旋转本地MAG矢量与真北对齐
	navUkfData.v0m[0] = mag[0] * cosf(-65.0 * DEG_TO_RAD) - mag[1] * sinf(-65.0  * DEG_TO_RAD);
	navUkfData.v0m[1] = mag[1] * cosf(-65.0 * DEG_TO_RAD) + mag[0] * sinf(-65.0  * DEG_TO_RAD);
	navUkfData.v0m[2] = mag[2];
#endif
	//navUkfData.kf具有17个状态变量，最大测量数为3，过程噪声数为12，最大观测噪声数为3
	navUkfData.kf = srcdkfInit(SIM_S, SIM_M, SIM_V, SIM_N, navUkfTimeUpdate);					//Srcdkf中的主参数为navUkfData.kf

	navUkfData.x = srcdkfGetState(navUkfData.kf);
	//给各类状态变量方差赋初值
	Q[0] = UKF_ACC_BIAS_Q;
	Q[1] = UKF_ACC_BIAS_Q;
	Q[2] = UKF_ACC_BIAS_Q;
	Q[3] = UKF_GYO_BIAS_Q;
	Q[4] = UKF_GYO_BIAS_Q;
	Q[5] = UKF_GYO_BIAS_Q;
	Q[6] = UKF_QUAT_Q;
	Q[7] = UKF_QUAT_Q;
	Q[8] = UKF_QUAT_Q;
	Q[9] = UKF_QUAT_Q;
	//给各类过程噪声方差赋初值
	V[UKF_V_NOISE_ACC_BIAS_X] = UKF_ACC_BIAS_V;
	V[UKF_V_NOISE_ACC_BIAS_Y] = UKF_ACC_BIAS_V;
	V[UKF_V_NOISE_ACC_BIAS_Z] = UKF_ACC_BIAS_V;
	V[UKF_V_NOISE_GYO_BIAS_X] = UKF_GYO_BIAS_V;
	V[UKF_V_NOISE_GYO_BIAS_Y] = UKF_GYO_BIAS_V;
	V[UKF_V_NOISE_GYO_BIAS_Z] = UKF_GYO_BIAS_V;
	V[UKF_V_NOISE_RATE_X] = UKF_RATE_V;
	V[UKF_V_NOISE_RATE_Y] = UKF_RATE_V;
	V[UKF_V_NOISE_RATE_Z] = UKF_RATE_V;
	//这里没有给观测噪声赋初值
	srcdkfSetVariance(navUkfData.kf, Q, V, 0, 0);												//设置过程噪声和观测噪声的协方差
}
