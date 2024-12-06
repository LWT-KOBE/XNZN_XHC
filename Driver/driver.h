#ifndef __DRIVER_H
#define __DRIVER_H

#include "driver_clockCount.h"
#include "driver_usbvcp.h"
#include "driver_flash.h"
#include "driver_rgb.h"
#include "driver_clockCount.h"
#include "driver_crc.h"
#include "driver_key.h"



#include "driver_lcd.h"




/*初始化类结构体*/
typedef struct{
	void (*Init) 	(void);
    
}deviceInitClass;
/*时钟类*/
extern deviceInitClass clockClass;
/*ADC类*/
extern deviceInitClass adcClass;
/*外饰类*/
extern deviceInitClass sightClass;
/*数传类*/
extern deviceInitClass DTUClass;
/*陀螺仪类*/
extern deviceInitClass IMUIintClass;
/*遥控器类*/
extern deviceInitClass DT7IintClass;
/*电机类*/
extern deviceInitClass motorSeverInitClass;
/*舵机类*/
extern deviceInitClass LobotServoInitClass;



#endif
