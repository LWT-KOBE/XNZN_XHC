#ifndef __DRIVER_DATA_LINK_H
#define __DRIVER_DATA_LINK_H

#include "bsp.h"
#include "driver_crc.h"
#include "util.h"

void driver_dataLinkInit(USART_TypeDef* USARTx,BSP_GPIOSource_TypeDef *USART_RX,BSP_GPIOSource_TypeDef *USART_TX,
												u32 baudRate,u8 PreemptionPriority,u8 SubPriority);

#endif
