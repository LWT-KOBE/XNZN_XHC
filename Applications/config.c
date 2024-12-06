#include "driver_flash.h"
#include "config.h"
#include "stdio.h"
#include "string.h"
#include "math.h"
#include "board.h"
systemConfig_t  systemConfigData __attribute__((at(0x10006000)));
systemConfig_t *getConfigData(void){
    return &systemConfigData;
}



/*----------------------------
== 此处的命名不可超过16个字符 ==
----------------------------*/
const char *configParameterStrings[] = {
	"CONFIG_VERSION",									//版本号
	"YAW_ANGLE_P",
	"YAW_ANGLE_I",
	"YAW_ANGLE_D",
	"YAW_ANGLE_F",
	"YAW_ANGLE_PM",
	"YAW_ANGLE_IM",
	"YAW_ANGLE_DM",
	"YAW_ANGLE_OM",    
  
    
	"PITCH_ANGLE_P",
	"PITCH_ANGLE_I",
	"PITCH_ANGLE_D",
	"PITCH_ANGLE_F",
	"PITCH_ANGLE_PM",
	"PITCH_ANGLE_IM",
	"PITCH_ANGLE_DM",
	"PITCH_ANGLE_OM", 
	"PITCH_ANGLE_MIX",
	"PITCH_ANGLE_MIN",         
    
	"PITCH_RATE_P",
	"PITCH_RATE_I",
	"PITCH_RATE_D",
	"PITCH_RATE_F",
	"PITCH_RATE_PM",
	"PITCH_RATE_IM",
	"PITCH_RATE_DM",
	"PITCH_RATE_OM",
	"YAW_RATE_P",
	"YAW_RATE_I",
	"YAW_RATE_D",
	"YAW_RATE_F",
	"YAW_RATE_PM",
	"YAW_RATE_IM",
	"YAW_RATE_DM",
	"YAW_RATE_OM",   


    "PITCH_CENTER",
	"YAW_CENTER",	   
    "BACK_CENTER_TIME",    
	"YAW_CONFIG",    
	
	"YAW_FIX",	
	"YAW_TURN",	
	"YAW_TYPE",	
	"YAW_INSTALL",	

	"PITCH_FIX",	
	"PITCH_TURN",	
	"PITCH_TYPE",	
	"PITCH_INSTALL",

	"IMU_ACC_BIAS_X",
	"IMU_ACC_BIAS_Y",
	"IMU_ACC_BIAS_Z",
	"IMU_GYO_BIAS_X",
	"IMU_GYO_BIAS_Y",
	"IMU_GYO_BIAS_Z",
	"IMU_MAG_BIAS_X",
	"IMU_MAG_BIAS_Y",
	"IMU_MAG_BIAS_Z",	
	
	
};
void configLoadDefault(void){	
	systemConfigData.configVersion = DEFAULT_CONFIG_VERSION;
    
	systemConfigData.yawAnglePID.PID_P = DEFAULT_YAW_ANGLE_P;
	systemConfigData.yawAnglePID.PID_I = DEFAULT_YAW_ANGLE_I;
	systemConfigData.yawAnglePID.PID_D = DEFAULT_YAW_ANGLE_D;
	systemConfigData.yawAnglePID.PID_F = DEFAULT_YAW_ANGLE_F;
	systemConfigData.yawAnglePID.PID_PM = DEFAULT_YAW_ANGLE_PM;
	systemConfigData.yawAnglePID.PID_IM = DEFAULT_YAW_ANGLE_IM;
	systemConfigData.yawAnglePID.PID_DM = DEFAULT_YAW_ANGLE_DM;
	systemConfigData.yawAnglePID.PID_OM = DEFAULT_YAW_ANGLE_OM;

  
	systemConfigData.pitchAnglePID.PID_P = DEFAULT_PITCH_ANGLE_P;
	systemConfigData.pitchAnglePID.PID_I = DEFAULT_PITCH_ANGLE_I;
	systemConfigData.pitchAnglePID.PID_D = DEFAULT_PITCH_ANGLE_D;
	systemConfigData.pitchAnglePID.PID_F = DEFAULT_PITCH_ANGLE_F;
	systemConfigData.pitchAnglePID.PID_PM = DEFAULT_PITCH_ANGLE_PM;
	systemConfigData.pitchAnglePID.PID_IM = DEFAULT_PITCH_ANGLE_IM;
	systemConfigData.pitchAnglePID.PID_DM = DEFAULT_PITCH_ANGLE_DM;
	systemConfigData.pitchAnglePID.PID_OM = DEFAULT_PITCH_ANGLE_OM;


	systemConfigData.pitchRatePID.PID_P = DEFAULT_PITCH_RATE_P;
	systemConfigData.pitchRatePID.PID_I = DEFAULT_PITCH_RATE_I;
	systemConfigData.pitchRatePID.PID_D = DEFAULT_PITCH_RATE_D;
	systemConfigData.pitchRatePID.PID_F = DEFAULT_PITCH_RATE_F;
	systemConfigData.pitchRatePID.PID_PM = DEFAULT_PITCH_RATE_PM;
	systemConfigData.pitchRatePID.PID_IM = DEFAULT_PITCH_RATE_IM;
	systemConfigData.pitchRatePID.PID_DM = DEFAULT_PITCH_RATE_DM;
	systemConfigData.pitchRatePID.PID_OM = DEFAULT_PITCH_RATE_OM;
    
   
	systemConfigData.yawRatePID.PID_P = DEFAULT_YAW_RATE_P;
	systemConfigData.yawRatePID.PID_I = DEFAULT_YAW_RATE_I;
	systemConfigData.yawRatePID.PID_D = DEFAULT_YAW_RATE_D;
	systemConfigData.yawRatePID.PID_F = DEFAULT_YAW_RATE_F;
	systemConfigData.yawRatePID.PID_PM = DEFAULT_YAW_RATE_PM;
	systemConfigData.yawRatePID.PID_IM = DEFAULT_YAW_RATE_IM;
	systemConfigData.yawRatePID.PID_DM = DEFAULT_YAW_RATE_DM;
	systemConfigData.yawRatePID.PID_OM = DEFAULT_YAW_RATE_OM;
 
       

    
	systemConfigData.yawCenter= DEFAULT_YAW_CENTER;								
	systemConfigData.pitchCenter = DEFAULT_PITCH_CENTER;	
	systemConfigData.backCenterTime = DEFAULT_BACK_CENTER_TIME;		


    systemConfigData.yawSetting.u8_temp[0] = DEFAULT_YAW_FIX;
	systemConfigData.yawSetting.u8_temp[1] = DEFAULT_YAW_TURN;
	systemConfigData.yawSetting.u8_temp[2] = DEFAULT_YAW_TYPE;
    systemConfigData.yawSetting.u8_temp[3] = DEFAULT_YAW_INSTALL;		

    systemConfigData.pitchSetting.u8_temp[0] = DEFAULT_PITCH_FIX;
	systemConfigData.pitchSetting.u8_temp[1] = DEFAULT_PITCH_TURN;
	systemConfigData.pitchSetting.u8_temp[2] = DEFAULT_PITCH_TYPE;
    systemConfigData.pitchSetting.u8_temp[3] = DEFAULT_PITCH_INSTALL;



	systemConfigData.imu_acc_bias_x = DEFAULT_IMU_ACC_BIAS_X;
	systemConfigData.imu_acc_bias_y = DEFAULT_IMU_ACC_BIAS_Y;
	systemConfigData.imu_acc_bias_z = DEFAULT_IMU_ACC_BIAS_Z;

	systemConfigData.imu_gyo_bias_x = DEFAULT_IMU_GYO_BIAS_X;
	systemConfigData.imu_gyo_bias_y = DEFAULT_IMU_GYO_BIAS_Y;
	systemConfigData.imu_gyo_bias_z = DEFAULT_IMU_GYO_BIAS_Z;

	systemConfigData.imu_mag_bias_x = DEFAULT_IMU_MAG_BIAS_X;
	systemConfigData.imu_mag_bias_y = DEFAULT_IMU_MAG_BIAS_Y;
	systemConfigData.imu_mag_bias_z = DEFAULT_IMU_MAG_BIAS_Z;

}


configToken_t *configTokenFindEmpty(void) {
	configToken_t *p = (configToken_t *)(FLASH_END_ADDR + 1);
	do {
		p--;
	} while (p->key != 0xffffffff);
	return p;
}

void configTokenStore(configToken_t *token) {
  flashAddress((uint32_t)configTokenFindEmpty(), (uint32_t *)token, sizeof(configToken_t)/sizeof(uint32_t));
}

configToken_t *configTokenGet(uint32_t key) {
	configToken_t *p, *t;
	p = (configToken_t *)(FLASH_END_ADDR + 1);
	t = 0;
	do {
		p--;
		if (p->key == key)
			t = p;
	} while (p->key != 0xffffffff);
	return t;
}
/*------------------- 配置FLASH读取 ---------------------*/
void configFlashRead(void) {
    float *recs;
    int i;
    //从扇区首地址开始读取
    recs = (void *)flashStartAddr();
    for (i = 0; i < sizeof(systemConfig_t); i++) {
        *((&systemConfigData.configVersion) + i) = recs[i];
    }    
    
    
}

configToken_t *configTokenIterate(configToken_t *t) {
	if (t == 0)
		t = (configToken_t *)(FLASH_END_ADDR + 1);
	t--;
	if (t->key != 0xffffffff)
		return t;
	else
		return 0;
}
/*------------------- 配置FLASH写入 ---------------------*/
uint8_t configFlashWrite(void) {

    float *recs;
	uint8_t ret = 0;
	int i;
    
    //申请一段动态内存用于储存临时信息
	recs = (void *)aqCalloc(sizeof(systemConfig_t), sizeof(float));
    //如果成功申请到内存地址就执行写入操作
	if (recs) {
        //从扇区初始地址开始擦除flash
		ret = flashErase(flashStartAddr(), sizeof(systemConfig_t) * sizeof(float) / sizeof(uint32_t));
        //使闪存数据缓存无效	        
		FLASH_DataCacheCmd(DISABLE);
        //重置数据缓存
		FLASH_DataCacheReset();
        //启用数据缓存功能
		FLASH_DataCacheCmd(ENABLE);
        //如果擦除成功，执行写入操作
		if (ret) {
            //在内存中创建参数列表
			for (i = 0; i < sizeof(systemConfig_t); i++) {
                recs[i] = *((&systemConfigData.configVersion) + i);
			}
            //存flash数据，从扇区起始地址往前存
			ret = flashAddress(flashStartAddr(), (uint32_t *)recs, \
                                sizeof(systemConfig_t) * sizeof(float) / sizeof(uint32_t));
		}
        else {
            usbVCP_Printf("Flash erase failed! \r\n");
        }
        //释放内存
		aqFree(recs, sizeof(systemConfig_t), sizeof(float));
	}
	else {
        usbVCP_Printf("Flash failed to apply for memory! \r\n");
    }
	return ret;




}

void configInit(void) {

	float ver;	
    //从Flash中开始
	configFlashRead();																							
    //读取当前flash版本
	ver = *(float *)(flashStartAddr());																
	if (isnan(ver))
		ver = 0.0f;
    //如果编译的默认值大于flash版本和加载版本																											
	if (DEFAULT_CONFIG_VERSION > ver || DEFAULT_CONFIG_VERSION > systemConfigData.configVersion){
        //加载默认值
		configLoadDefault();																					
		digitalHi(&getsupervisorData()->flashSave);	
	}
    //如果flash版本大于当前或等于当前版本
	else if (ver >= systemConfigData.configVersion){                                                              
        //读取flash   
        configFlashRead();																								
	}																			
    //如果加载的版本大于flash版本
	else if (systemConfigData.configVersion > ver){                                                                          
        //写入flash,这个情况只存在于有SD卡时，且SD卡中的版本高于flash中的版本才会发生        
        configFlashWrite();	
	}																											
    usbVCP_Printf("ConfigInit Successfully \r\n");    
    
    
    
    
}


