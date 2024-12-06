
#ifndef __IMU_H
#define __IMU_H

#include "nav_ukf.h"
#include "d_imu.h"
#include "parameter.h"
#include "nav_para.h"
//使用WT931陀螺仪
#define  IMU_WT931

//使用外接陀螺仪
//#define  IMU_SLAVE


//使用板载陀螺仪
//#define IMU_BZ		

typedef struct {
   float sinRot, cosRot;
   uint32_t fullUpdates;
   uint32_t halfUpdates;
   bool initFlag;        
   uint32_t intervalNum;	    	
   uint32_t IMU_ErrorCount;
   uint32_t IMU_LastErrorCount;
   uint32_t loops;    
        
} imuStruct_t;


#ifdef IMU_SLAVE		

#define AQ_YAW			slaveSensorData.yaw.float_temp
#define AQ_PITCH		slaveSensorData.pitch.float_temp
#define AQ_ROLL			slaveSensorData.roll.float_temp



#define IMU_RAW_RATEX		slaveSensorData.normalizedGyo[0].float_temp
#define IMU_RAW_RATEY		slaveSensorData.normalizedGyo[1].float_temp
#define IMU_RAW_RATEZ		slaveSensorData.normalizedGyo[2].float_temp

//#define IMU_CHASE_RATEZ		imuSensorOfChassisData.gyo[2]
#define IMU_RAW_ACCX		slaveSensorData.normalizedAcc[0].float_temp
#define IMU_RAW_ACCY		slaveSensorData.normalizedAcc[1].float_temp
#define IMU_RAW_ACCZ		slaveSensorData.normalizedAcc[2].float_temp

#define IMU_RAW_MAGX		slaveSensorData.normalizedMag[0].float_temp
#define IMU_RAW_MAGY		slaveSensorData.normalizedMag[1].float_temp
#define IMU_RAW_MAGZ		slaveSensorData.normalizedMag[2].float_temp


#define IMU_TEMP		slaveSensorData.temp
#define IMU_LASTUPD	dImuData.lastUpdate




#endif

#ifdef IMU_BZ		

#define AQ_YAW			1//navUkfData.yaw.float_temp
#define AQ_PITCH		1//navUkfData.pitch.float_temp
#define AQ_ROLL			1//navUkfData.roll.float_temp

#define IMU_ACCX		imuSensorData.acc[0]
#define IMU_ACCY		imuSensorData.acc[1]
#define IMU_ACCZ		imuSensorData.acc[2]

#define IMU_RATEX		imuSensorData.gyo[0]
#define IMU_RATEY		imuSensorData.gyo[1]
#define IMU_RATEZ		imuSensorData.gyo[2]

#endif


#define IMU_RAW_RATEX		imuSensorData.normalizedGyo[0].float_temp
#define IMU_RAW_RATEY		imuSensorData.normalizedGyo[1].float_temp
#define IMU_RAW_RATEZ		imuSensorData.normalizedGyo[2].float_temp

#define IMU_CHASE_RATEZ		imuSensorOfChassisData.gyo[2]
#define IMU_RAW_ACCX		imuSensorData.normalizedAcc[0].float_temp
#define IMU_RAW_ACCY		imuSensorData.normalizedAcc[1].float_temp
#define IMU_RAW_ACCZ		imuSensorData.normalizedAcc[2].float_temp

#define IMU_RAW_MAGX		imuSensorData.normalizedMag[0].float_temp
#define IMU_RAW_MAGY		imuSensorData.normalizedMag[1].float_temp
#define IMU_RAW_MAGZ		imuSensorData.normalizedMag[2].float_temp
#define IMU_MAGX		imuSensorData.mag[0]
#define IMU_MAGY		imuSensorData.mag[1]
#define IMU_MAGZ		imuSensorData.mag[2]

#define IMU_TEMP		imuSensorData.temp
#define IMU_LASTUPD	dImuData.lastUpdate



#ifdef  IMU_WT931   

#define AQ_YAW			wt931Data.stcAngle.z
#define AQ_PITCH		wt931Data.stcAngle.x
#define AQ_ROLL			wt931Data.stcAngle.y 

#define IMU_RATEX		wt931Data.stcGyro.x
#define IMU_RATEY		wt931Data.stcGyro.y
#define IMU_RATEZ		wt931Data.stcGyro.z

#define IMU_ACCX		wt931Data.stcAcc.x
#define IMU_ACCY		wt931Data.stcAcc.y
#define IMU_ACCZ		wt931Data.stcAcc.z
#endif








imuStruct_t* getimuData(void);

void imuInit(void);

#endif




