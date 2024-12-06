#include "ahrstask.h"
#include "math.h"


ahrsStruct_t AhrsData;

//四元数
static float q0 = 1.0f;					
static float q1 = 0.0f;
static float q2 = 0.0f;
static float q3 = 0.0f;

static float rMat[3][3]; //旋转矩阵







float sqf(float x) 
{
	return ((x)*(x));
}

// Fast inverse square-root
// See: http://en.wikipedia.org/wiki/Fast_inverse_square_root
float invSqrt(float x)	/*快速开平方求倒*/
{
	float halfx = 0.5f * x;
	float y = x;
	long i = *(long*)&y;
	i = 0x5f3759df - (i>>1);
	y = *(float*)&i;
	y = y * (1.5f - (halfx * y * y));
	return y;
}

float acos_approx(float x)
{
    float xa = fabsf(x);
    float result = sqrtf(1.0f - xa) * (1.5707288f + xa * (-0.2121144f + xa * (0.0742610f + (-0.0187293f * xa))));
    if (x < 0.0f)
        return M_PIf - result;
    else
        return result;
}

float atan2_approx(float y, float x)
{
    #define atanPolyCoef1  3.14551665884836e-07f
    #define atanPolyCoef2  0.99997356613987f
    #define atanPolyCoef3  0.14744007058297684f
    #define atanPolyCoef4  0.3099814292351353f
    #define atanPolyCoef5  0.05030176425872175f
    #define atanPolyCoef6  0.1471039133652469f
    #define atanPolyCoef7  0.6444640676891548f

    float res, absX, absY;
    absX = fabsf(x);
    absY = fabsf(y);
    res  = MAX(absX, absY);
    if (res) res = MIN(absX, absY) / res;
    else res = 0.0f;
    res = -((((atanPolyCoef5 * res - atanPolyCoef4) * res - atanPolyCoef3) * res - atanPolyCoef2) * res - atanPolyCoef1) / ((atanPolyCoef7 * res + atanPolyCoef6) * res + 1.0f);
    if (absY > absX) res = (M_PIf / 2.0f) - res;
    if (x < 0) res = M_PIf - res;
    if (y < 0) res = -res;
    return res;
}


void imuComputeRotationMatrix(void)//四元数表转旋转矩阵
{
	float q1q1,q2q2,q3q3;
	float q0q1,q0q2,q0q3,q1q2,q1q3,q2q3;
	
    q1q1 = sqf(q1 );
    q2q2 = sqf(q2 );
    q3q3 = sqf(q3 );

    q0q1 = q0  * q1 ;
    q0q2 = q0  * q2 ;
    q0q3 = q0  * q3 ;
    q1q2 = q1  * q2 ;
    q1q3 = q1  * q3 ;
    q2q3 = q2  * q3 ;

    rMat[0][0] = 1.0f - 2.0f * q2q2 - 2.0f * q3q3;
    rMat[0][1] = 2.0f * (q1q2 -q0q3);
    rMat[0][2] = 2.0f * (q1q3 +q0q2);

    rMat[1][0] = 2.0f * (q1q2 +q0q3);
    rMat[1][1] = 1.0f - 2.0f * q1q1 - 2.0f * q3q3;
    rMat[1][2] = 2.0f * (q2q3 -q0q1);

    rMat[2][0] = 2.0f * (q1q3 -q0q2);
    rMat[2][1] = 2.0f * (q2q3 +q0q1);
    rMat[2][2] = 1.0f - 2.0f * q1q1 - 2.0f * q2q2;
}




void imuUpdate(imusensorStruct_t *imuSensor, float dt)
{
	static float integralAccX = 0.0f,  integralAccY = 0.0f, integralAccZ = 0.0f;    //加速度积分误差
	static float integralMagX = 0.0f,  integralMagY = 0.0f, integralMagZ = 0.0f;    //磁力计积分误差
	float ex, ey, ez;
	
	imuSensor->normalizedGyo[0].float_temp = imuSensor->normalizedGyo[0].float_temp * DEG2RAD;
	imuSensor->normalizedGyo[1].float_temp = imuSensor->normalizedGyo[1].float_temp * DEG2RAD;
	imuSensor->normalizedGyo[2].float_temp = imuSensor->normalizedGyo[2].float_temp * DEG2RAD;
    //计算旋转速率(rad/s)
    const float spin_rate_sq = sq(imuSensor->normalizedGyo[0].float_temp) + sq(imuSensor->normalizedGyo[1].float_temp) +\
								sq(imuSensor->normalizedGyo[2].float_temp);

    //Step 1: Yaw correction
	const float magMagnitudeSq = imuSensor->normalizedMag[0].float_temp * imuSensor->normalizedMag[0].float_temp +\
								imuSensor->normalizedMag[1].float_temp * imuSensor->normalizedMag[1].float_temp + imuSensor->normalizedMag[2].float_temp * imuSensor->normalizedMag[2].float_temp;
	float kpMag = DCM_KP_MAG * 4.0f;
	
	if (magMagnitudeSq > 0.01f) 
	{
//		//单位化磁力计测量值
		const float magRecipNorm = invSqrt(magMagnitudeSq);
		imuSensor->normalizedMag[0].float_temp *= magRecipNorm;
		imuSensor->normalizedMag[1].float_temp *= magRecipNorm;
		imuSensor->normalizedMag[2].float_temp *= magRecipNorm;
	
		//计算X\Y方向的磁通，磁北方向磁通
		const float hx = rMat[0][0] * imuSensor->normalizedMag[0].float_temp + rMat[0][1] * imuSensor->normalizedMag[1].float_temp+ \
						 rMat[0][2] * imuSensor->normalizedMag[2].float_temp;
		const float hy = rMat[1][0] * imuSensor->normalizedMag[0].float_temp + rMat[1][1] * imuSensor->normalizedMag[1].float_temp + \
						rMat[1][2] * imuSensor->normalizedMag[2].float_temp;
		const float bx = sqrtf(hx * hx + hy * hy);

		//磁力计误差是估计磁北和测量磁北之间的交叉乘积
		const float ez_ef = -(hy * bx);

		//旋转误差到机体坐标系
		ex = rMat[2][0] * ez_ef;
		ey = rMat[2][1] * ez_ef;
		ez = rMat[2][2] * ez_ef;
	}
	else 
	{
		ex = 0;
		ey = 0;
		ez = 0;
	}

	//累计误差补偿
	if (DCM_KI_MAG > 0.0f) 
	{
		//如果旋转速率大于限制值则停止积分
		if (spin_rate_sq < sq(DEGREES_TO_RADIANS(SPIN_RATE_LIMIT))) 
		{
			integralMagX += DCM_KI_MAG * ex * dt;
			integralMagY += DCM_KI_MAG * ey * dt;
			integralMagZ += DCM_KI_MAG * ez * dt;

			imuSensor->normalizedGyo[0].float_temp += integralMagX;
			imuSensor->normalizedGyo[1].float_temp += integralMagY;
			imuSensor->normalizedGyo[2].float_temp += integralMagZ;
		}
	}
	
	//误差补偿
	imuSensor->normalizedGyo[0].float_temp += kpMag * ex;
	imuSensor->normalizedGyo[1].float_temp += kpMag * ey;
	imuSensor->normalizedGyo[2].float_temp += kpMag * ez;

	
    //Step 2: Roll and pitch correction
	if(!((imuSensor->normalizedAcc[0].float_temp == 0.0f) && (imuSensor->normalizedAcc[1].float_temp == 0.0f) && (imuSensor->normalizedAcc[2].float_temp == 0.0f)))
	{
		//单位化加速计测量值
		const float accRecipNorm = invSqrt(imuSensor->normalizedAcc[0].float_temp * imuSensor->normalizedAcc[0].float_temp + imuSensor->normalizedAcc[1].float_temp * imuSensor->normalizedAcc[1].float_temp + imuSensor->normalizedAcc[2].float_temp * imuSensor->normalizedAcc[2].float_temp);
		imuSensor->normalizedAcc[0].float_temp *= accRecipNorm;
		imuSensor->normalizedAcc[1].float_temp *= accRecipNorm;
		imuSensor->normalizedAcc[2].float_temp *= accRecipNorm;

		//加速计读取的方向与重力加速计方向的差值，用向量叉乘计算
		ex = (imuSensor->normalizedAcc[1].float_temp * rMat[2][2] - imuSensor->normalizedAcc[2].float_temp * rMat[2][1]);
		ey = (imuSensor->normalizedAcc[2].float_temp * rMat[2][0] - imuSensor->normalizedAcc[0].float_temp * rMat[2][2]);
		ez = (imuSensor->normalizedAcc[0].float_temp * rMat[2][1] - imuSensor->normalizedAcc[1].float_temp * rMat[2][0]);

		//累计误差补偿
		if (DCM_KI_ACC > 0.0f) 
		{
			//如果旋转速率大于限制值则停止积分
			if (spin_rate_sq < sq(DEGREES_TO_RADIANS(SPIN_RATE_LIMIT)))
			{
				integralAccX += DCM_KI_ACC * ex * dt;
				integralAccY += DCM_KI_ACC * ey * dt;
				integralAccZ += DCM_KI_ACC * ez * dt;

				imuSensor->normalizedGyo[0].float_temp += integralAccX;
				imuSensor->normalizedGyo[1].float_temp += integralAccY;
				imuSensor->normalizedGyo[2].float_temp += integralAccZ;
			}
		}

		//误差补偿
		imuSensor->normalizedGyo[0].float_temp += DCM_KP_ACC * ex;
		imuSensor->normalizedGyo[1].float_temp += DCM_KP_ACC * ey;
		imuSensor->normalizedGyo[2].float_temp += DCM_KP_ACC * ez;
	}
	
	//一阶近似算法，四元数运动学方程的离散化形式和积分
    imuSensor->normalizedGyo[0].float_temp *= (0.5f * dt);
    imuSensor->normalizedGyo[1].float_temp *= (0.5f * dt);
    imuSensor->normalizedGyo[2].float_temp *= (0.5f * dt);

    const float qa = q0;
    const float qb = q1;
    const float qc = q2;
    q0 += (-qb * imuSensor->normalizedGyo[0].float_temp - qc * imuSensor->normalizedGyo[1].float_temp - q3 * imuSensor->normalizedGyo[2].float_temp);
    q1 += ( qa * imuSensor->normalizedGyo[0].float_temp + qc * imuSensor->normalizedGyo[2].float_temp - q3 * imuSensor->normalizedGyo[1].float_temp);
    q2 += ( qa * imuSensor->normalizedGyo[1].float_temp - qb * imuSensor->normalizedGyo[2].float_temp + q3 * imuSensor->normalizedGyo[0].float_temp);
    q3 += ( qa * imuSensor->normalizedGyo[2].float_temp + qb * imuSensor->normalizedGyo[1].float_temp - qc * imuSensor->normalizedGyo[0].float_temp);

	//单位化四元数
    const float quatRecipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
    q0 *= quatRecipNorm;
    q1 *= quatRecipNorm;
    q2 *= quatRecipNorm;
    q3 *= quatRecipNorm;

    //计算四元数的旋转矩阵
    imuComputeRotationMatrix();
	
	imuSensor->roll.float_temp = RADIANS_TO_DEGREES((0.5f * M_PIf) - acos_approx(-rMat[2][0]));//arcsin = 0.5PI - arccos
	imuSensor->pitch.float_temp = RADIANS_TO_DEGREES(atan2_approx(rMat[2][1], rMat[2][2])); 
	imuSensor->yaw.float_temp = RADIANS_TO_DEGREES(atan2_approx(rMat[1][0], rMat[0][0]));
	if (imuSensor->yaw.float_temp < 0.0f )//转换位0~360
		imuSensor->yaw.float_temp += 360.0f;	


}



void ahrsGlobalInit(void){

	BSP_TIM_PWM_Init(TIM1,100,840 - 1,NULL,NULL,NULL,BSP_GPIOE14);


	
}


void ahrsUpdateTask(void *Parameters){

	TickType_t xLastWakeTime = xTaskGetTickCount();
	digitalLo(&AhrsData.dataInitFlag);
	while(true){
		vTaskDelayUntil(&xLastWakeTime,AHRS_NORMAL_PERIOD);
        //防止重复初始化
		if(!AhrsData.dataInitFlag){	
            //所有控制全部初始化            
			ahrsGlobalInit();																																							
			digitalHi(&AhrsData.dataInitFlag);
            
		}  
		imuUpdate(&imuSensorData,0.001f);		
	
		digitalIncreasing(&AhrsData.loops);        
	
}

}

void ahrsInit(void){
	getsupervisorData()->taskEvent[AHRS_TASK] = xTaskCreate(ahrsUpdateTask,"AHRS",AHRS_STACK_SIZE,NULL,AHRS_PRIORITY,&AhrsData.xHandleTask);
    usbVCP_Printf("ControlInit Successfully \r\n");
    
}
