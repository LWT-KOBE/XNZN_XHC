#include "processing.h"
#include "imu.h"
#include "Driver_bMI088.h"
#include "driver_spl06.h"

sensorStruct_t sensorRotate;
void sensorCalibProcess(formatTrans32Struct_t *in,volatile float *out,float *bias,uint8_t t,int16_t *rotate,float *p){
	float a, b, c;
	float *r = p; 
	
	a = rotate[0] * (in[0].float_temp + bias[0]);
	b = rotate[1] * (in[1].float_temp + bias[1]);
	c = rotate[2] * (in[2].float_temp + bias[2]);

	out[0] = a + b*r[0] + c*r[1];
	out[1] = a*r[2] + b + c*r[3];
	out[2] = a*r[4] + b*r[5] + c;
}

void sensorScaleProcess(float *in,formatTrans32Struct_t *out,uint8_t t,float coefficient,int16_t *orient){
	uint8_t i =0;
	for(i=0; i<t ;i++)
		out[i].float_temp = in[i] * coefficient * orient[i];
}

void sensorTempScaleProcess(float *in,formatTrans32Struct_t *out,float coefficient){
	if(coefficient != NULL)
		out[0].float_temp =((float)in[0])* BMI088_TEMP_FACTOR + BMI088_TEMP_OFFSET;
	else
		out[0].float_temp=0.0f;
}

void sensorProcessInit(void){
	sensorRotate.accRotate[0] = -1;sensorRotate.gyoRotate[0] = 1;sensorRotate.magRotate[0] = 1;
	sensorRotate.accRotate[1] = 1;sensorRotate.gyoRotate[1] = -1;sensorRotate.magRotate[1] = -1;
	sensorRotate.accRotate[2] = -1;sensorRotate.gyoRotate[2] = 1;sensorRotate.magRotate[2] = -1;
	
	sensorRotate.accOrient[0] = 1;sensorRotate.gyoOrient[0] = 1;sensorRotate.magOrient[0] = 1;
	sensorRotate.accOrient[1] = 1;sensorRotate.gyoOrient[1] = 1;sensorRotate.magOrient[1] = 1;
	sensorRotate.accOrient[2] = 1;sensorRotate.gyoOrient[2] = 1;sensorRotate.magOrient[2] = -1;
}

void sensorProcessUpdate(imusensorStruct_t *imuSensor,coordinateFloat_t *accData,\
						 coordinateFloat_t *gyoData,coordinateFloat_t *magData,formatTrans16Struct_t *tempreature){
	static float temp[1],acc[3],gyo[3],mag[3];
	if(accData != NULL){
		acc[0] = accData->y;
		acc[1] = accData->x;
		acc[2] = accData->z;
		sensorScaleProcess(acc,imuSensor->normalizedAcc,3,BMI088Data.accelScale,sensorRotate.accOrient);
		sensorCalibProcess(imuSensor->normalizedAcc,imuSensor->acc,imuSensor->accBIAS,3,sensorRotate.accRotate,NULL);
	}
	if(gyoData != NULL){
		gyo[0] = gyoData->y;
		gyo[1] = gyoData->x;
		gyo[2] = gyoData->z;		
		sensorScaleProcess(gyo,imuSensor->normalizedGyo,3,BMI088Data.gyroScale,sensorRotate.gyoOrient);
		sensorCalibProcess(imuSensor->normalizedGyo,imuSensor->gyo,imuSensor->gyoBIAS,3,sensorRotate.gyoRotate,NULL);	
//		imuSensorData.gyoUkf[0].s16_temp = (IMU_RATEX - UKF_GYO_BIAS_X) * 1000;
//		imuSensorData.gyoUkf[1].s16_temp = (IMU_RATEY - UKF_GYO_BIAS_Y) * 1000;
//		imuSensorData.gyoUkf[2].s16_temp = (IMU_RATEZ - UKF_GYO_BIAS_Z) * 1000;		
	}
	if(magData != NULL){
		mag[0] = magData->y;
		mag[1] = magData->x;
		mag[2] = magData->z;				
		sensorScaleProcess(mag,imuSensor->normalizedMag,3,SLAVE_MAG_SCALE,sensorRotate.magOrient);
		sensorCalibProcess(imuSensor->normalizedMag,imuSensor->mag,imuSensor->magBIAS,3,sensorRotate.magRotate,NULL);
	}	
	if(tempreature != NULL){
		temp[0] = tempreature->s16_temp;
		sensorTempScaleProcess(temp,&imuSensor->temp,SLAVE_TEM_SCALE);
	}
}

float meanRecursiveFilter(s16 newVuale,u8 axis){
	static float gyoSum[6];
	static float gyoVaule[6][10];
	float outVaule;
	static uint8_t index[6] = {0,0,0,0,0,0};
	gyoSum[axis] -= gyoVaule[axis][index[axis]];
	gyoVaule[axis][index[axis]] = newVuale;
	gyoSum[axis] += gyoVaule[axis][index[axis]];
	index[axis] = (index[axis] + 1) % 10;
	outVaule = (float)gyoSum[axis] / 10;
	return outVaule;
}	

void initAccLowPassFilter(void){
    float a;

    ///////////////////////////////////

    a = 2.0f * 0.03f * 1000.0f;

    sensorRotate.lowPassFilterData[ACCEL_X_1000HZ_LOWPASS].gx1 = 1.0f / (1.0f + a);
    sensorRotate.lowPassFilterData[ACCEL_X_1000HZ_LOWPASS].gx2 = 1.0f / (1.0f + a);
    sensorRotate.lowPassFilterData[ACCEL_X_1000HZ_LOWPASS].gx3 = (1.0f - a) / (1.0f + a);
    sensorRotate.lowPassFilterData[ACCEL_X_1000HZ_LOWPASS].previousInput  = 0.0f;
    sensorRotate.lowPassFilterData[ACCEL_X_1000HZ_LOWPASS].previousOutput = 0.0f;

    ///////////////////////////////////

    a = 2.0f * 0.03f * 1000.0f;

    sensorRotate.lowPassFilterData[ACCEL_Y_1000HZ_LOWPASS].gx1 = 1.0f / (1.0f + a);
    sensorRotate.lowPassFilterData[ACCEL_Y_1000HZ_LOWPASS].gx2 = 1.0f / (1.0f + a);
    sensorRotate.lowPassFilterData[ACCEL_Y_1000HZ_LOWPASS].gx3 = (1.0f - a) / (1.0f + a);
    sensorRotate.lowPassFilterData[ACCEL_Y_1000HZ_LOWPASS].previousInput  = 0.0f;
    sensorRotate.lowPassFilterData[ACCEL_Y_1000HZ_LOWPASS].previousOutput = 0.0f;

    ///////////////////////////////////

    a = 2.0f * 0.03f * 1000.0f;

    sensorRotate.lowPassFilterData[ACCEL_Z_1000HZ_LOWPASS].gx1 = 1.0f / (1.0f + a);
    sensorRotate.lowPassFilterData[ACCEL_Z_1000HZ_LOWPASS].gx2 = 1.0f / (1.0f + a);
    sensorRotate.lowPassFilterData[ACCEL_Z_1000HZ_LOWPASS].gx3 = (1.0f - a) / (1.0f + a);
    sensorRotate.lowPassFilterData[ACCEL_Z_1000HZ_LOWPASS].previousInput  = -9.8065f;
    sensorRotate.lowPassFilterData[ACCEL_Z_1000HZ_LOWPASS].previousOutput = -9.8065f;
}


float LowPass_Filter(float input, struct LowPassFilterData *filterParameters){
    float output;

    output = filterParameters->gx1 * input +
             filterParameters->gx2 * filterParameters->previousInput -
             filterParameters->gx3 * filterParameters->previousOutput;

    filterParameters->previousInput  = input;
    filterParameters->previousOutput = output;

    return output;
}

