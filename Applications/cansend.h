#ifndef __CANDSEND_H
#define __CANDSEND_H
#include "Driver_RMMotor.h"
#include "driver_Odrive.h"

#include "bsp.h"
#include "util.h"
#include "stdbool.h"	




void canSendUpdate(void);
void canSendInit(void);
void driver_can1_init(CAN_TypeDef* rm_canx,BSP_GPIOSource_TypeDef *rm_canx_rx,BSP_GPIOSource_TypeDef *rm_canx_tx,u8 Preemption,u8 Sub);
void driver_can2_init(CAN_TypeDef* rm_canx,BSP_GPIOSource_TypeDef *rm_canx_rx,BSP_GPIOSource_TypeDef *rm_canx_tx,u8 Preemption,u8 Sub);
void canSendRelax(void);


#endif
