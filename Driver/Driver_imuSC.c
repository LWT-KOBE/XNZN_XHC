#include "Driver_imuSC.h"
#include "cansend.h"
#include "bsp.h"
#include <string.h>
#include <math.h>
#include "driver.h"

imuBroadcastStruct_t imuBroadcast;
imuBroadcastStruct_t *getScimuData(){
	return &imuBroadcast;
}

/**************************************************/
static void imuSCInit(void);

/**************************************************/

deviceInitClass IMUIintClass = {
	imuSCInit,
};


float ByteToFloat(uint8_t *src, int index) {
	formatTrans32Struct_t value;
	for (uint8_t i = 0; i < 4; i++)
		value.u8_temp[i] = src[index + i];
	return value.float_temp;
}

int16_t ByteToInt16(uint8_t *src, int index) {
	formatTrans16Struct_t value;
	for (uint8_t i = 0; i < 2; i++)
		value.u8_temp[i] = src[index + i];
	return value.s16_temp;
}

uint16_t ByteToUint16(uint8_t *src, int index) {
	formatTrans16Struct_t value;
	for (uint8_t i = 0; i < 2; i++)
		value.u8_temp[i] = src[index + i];
	return value.u16_temp;
}


//can发送函数
uint8_t canTransferPack(CAN_TypeDef *CANx,uint32_t ID_CAN, uint8_t *array) {
    uint8_t mbox;
    uint16_t i = 0;     
	CanTxMsg txMessage;
	txMessage.StdId = ID_CAN;
	txMessage.IDE = CAN_Id_Standard;
	txMessage.RTR = CAN_RTR_Data;
	txMessage.DLC = 0x08;
    
    for(uint8_t i = 0; i < 8; i++) {
        txMessage.Data[i] = (uint8_t)array[i];
    }
    
    mbox = CAN_Transmit(CANx, &txMessage);   
	
	//等待发送结束
    while(CAN_TransmitStatus(CANx, mbox) == CAN_TxStatus_Failed) {
        i++;	
		if(i >= 0xFFF)
			return 1;
	}
    return 0;
}

//读取特定位的配置
int8_t imuReadConfigRegister(uint16_t reg) {
    uint8_t group = (uint8_t)((reg & 0xF00) >> 8);
    uint8_t regBit = (uint8_t)(reg & 0xFF);
    uint8_t shift = 0;
    //循环找到最小的有效位，并记录位移次数，但cmd必须大于0
    if(regBit > 0) {
        for(; shift < 8 ; shift ++) {
            //右侧没有移位到1则继续移位
            if(regBit & 0x01) {
                break;
            }
            regBit = regBit >> 1;
        }
    }
    else {
        return -1;
    }
    //将cmd值复位
    regBit = (uint8_t)(reg & 0xFF);
    return (imuBroadcast.config.setting[group] & regBit) >> shift;
}

//写入特定位的配置
int8_t imuWirteConfigRegister(imuConfigStruct_t *config, uint16_t reg, uint8_t data) {
    uint8_t group = (uint8_t)((reg & 0xF00) >> 8);
    uint8_t regBit = (uint8_t)(reg & 0xFF);
    uint8_t shift = 0;
    //循环找到最小的有效位，记录位移次数，但cmd必须大于0
    if(regBit > 0) {
        for(; shift < 8 ; shift ++) {
            //右侧没有移位到1则继续移位
            if(regBit & 0x01) {
                break;
            }
            regBit = regBit >> 1;
        }
    }
    else {
        return -1;
    }
    //将cmd值复位
    regBit = (uint8_t)(reg & 0xFF);
    //将指定位清零并赋值
    uint8_t value = config->setting[group] & (~regBit);
    value |= (data << shift);
    config->setting[group] = value;
    return 0;
}

//将多字节的数据转换成字节单位
void BytesToMultipleByte(uint8_t *intput, uint8_t length, void *output, uint8_t *add) {
    formatTrans32Struct_t *data_32bit;
    formatTrans16Struct_t *data_16bit;
    if(length == 4) {
        data_32bit = output;
        for(uint8_t i = 0; i < 4; i++) {
            data_32bit->u8_temp[i] = intput[(*add)++];
        }
    }
    else if(length == 2) {
        data_16bit = output;
        for(uint8_t i = 0; i < 2; i++) {
            data_16bit->u8_temp[i] = intput[(*add)++];
        }
    }
}

//从USART获取一次配置信息
void imuUsartGetOnceParameter(uint8_t *array) {
    uint8_t index_ptr = 0;
    array[index_ptr++] = 0xAD;
	array[index_ptr++] = 0x01;
	array[index_ptr++] = 0x00;
	array[index_ptr++] = 0x00;
    //填装长度
	array[2] = index_ptr + 2;
    //填装校验位
	Append_CRC8_Check_Sum(array, 4);
	Append_CRC16_Check_Sum(array, array[2]);
    //发送数据
	BSP_USART_DMA_SendData(UART7, array, (index_ptr + 2));
}

//USART单次写入1个寄存器的配置信息
void imuUsartSetting(uint8_t *array, uint8_t reg, uint8_t data) {
    uint8_t index_ptr = 0;
    array[index_ptr++] = 0xAD;
	array[index_ptr++] = reg;
	array[index_ptr++] = 0x00;
	array[index_ptr++] = 0x00;
    array[index_ptr++] = data;
    //填装长度
	array[2] = index_ptr + 2;
    //填装校验位
	Append_CRC8_Check_Sum(array, 4);
	Append_CRC16_Check_Sum(array, array[2]);
    //发送数据
	BSP_USART_DMA_SendData(UART7, array, (index_ptr + 2));
}

//USART写入全部寄存器的配置信息
void imuUsartAllSetting(uint8_t *array, uint8_t *data) {
    uint8_t index_ptr = 0;
    array[index_ptr++] = 0xAD;
	array[index_ptr++] = 0xBF;
	array[index_ptr++] = 0x00;
	array[index_ptr++] = 0x00;
    for(uint8_t j = 0; j < 8; j++) {
        array[index_ptr++] = data[j];
    }
    //填装长度
	array[2] = index_ptr + 2;
    //填装校验位
	Append_CRC8_Check_Sum(array, 4);
	Append_CRC16_Check_Sum(array, array[2]);
    //发送数据
	BSP_USART_DMA_SendData(UART7, array, (index_ptr + 2));
}

//USART传感器数据接收函数
static void imuUsartSensorReceive(uint8_t *src) {
    uint8_t ptr = 7;
	if (0x01 == (src[2] & 0x01)) {
		uint8_t cellLength = (uint8_t)powf(2, (src[ptr] & 0x03));
		uint8_t structLength = (src[ptr] & 0x3C) >> 2;
		bool type = (src[ptr++] & 0xC0) >> 6;
		if (cellLength == 4) {
			for (uint8_t i = 0; i < MAX(structLength, 3); i++, ptr += cellLength) {
				imuBroadcast.gyro[i] = ByteToFloat(src, ptr);
			}
		}
		else if (cellLength == 2) {
			for (uint8_t i = 0; i < MAX(structLength, 3); i++, ptr += cellLength) {
				if (type) {
					imuBroadcast.gyro[i] = ByteToInt16(src, ptr) * 0.001f;
				}
				else {
					imuBroadcast.gyro[i] = ByteToInt16(src, ptr) * 0.1f;
				}
			}
		}
	}
	if (0x02 == (src[2] & 0x02)) {
		uint8_t cellLength = (uint8_t)powf(2, (src[ptr] & 0x03));
		uint8_t structLength = (src[ptr] & 0x3C) >> 2;
		bool type = (src[ptr++] & 0xC0) >> 6;
		if (cellLength == 4) {
			for (uint8_t i = 0; i < MAX(structLength, 3); i++, ptr += cellLength) {
				imuBroadcast.accel[i] = ByteToFloat(src, ptr);
			}
		}
		else if (cellLength == 2) {
			for (uint8_t i = 0; i < MAX(structLength, 3); i++, ptr += cellLength) {
				imuBroadcast.accel[i] = ByteToInt16(src, ptr) * 0.01f;
			}
		}
	}
	if (0x04 == (src[2] & 0x02)) {
		uint8_t cellLength = (uint8_t)powf(2, (src[ptr] & 0x03));
		uint8_t structLength = (src[ptr++] & 0x3C) >> 2;
		if (cellLength == 4) {
			for (uint8_t i = 0; i < MAX(structLength, 3); i++, ptr += cellLength) {
				imuBroadcast.mag[i] = ByteToFloat(src, ptr);
			}
		}
		else if (cellLength == 2) {
			for (uint8_t i = 0; i < MAX(structLength, 3); i++, ptr += cellLength) {
				imuBroadcast.mag[i] = ByteToInt16(src, ptr) * 0.01f;
			}
		}
	}
	if (0x08 == (src[2] & 0x08)) {
		uint8_t cellLength = (uint8_t)powf(2, (src[ptr] & 0x03));
		uint8_t structLength = (src[ptr++] & 0x3C) >> 2;
		if (cellLength == 4) {
			imuBroadcast.presure = ByteToFloat(src, ptr);
		}
	}
}

//USART姿态数据接收函数
static void imuUsartAttitudeReceive(uint8_t *src) {
    uint8_t ptr = 7;
	uint8_t cellLength = powf(2, (src[ptr] & 0x03));
	uint8_t structLength = (src[ptr] & 0x3C) >> 2;
	bool type = (src[ptr++] & 0xC0) >> 6;
	if (type) {
		if (cellLength == 4) {
			for (int i = 0; i < MAX(structLength, 4); i++, ptr += cellLength) {
				imuBroadcast.q[i] = ByteToFloat(src, ptr);
			}
		}
		else if (cellLength == 2) {
			for (int i = 0; i < MAX(structLength, 4); i++, ptr += cellLength) {
				imuBroadcast.q[i] = ByteToInt16(src, ptr) * 0.0001f;
			}
		}
	}
	else {
		if (cellLength == 4) {
			for (int i = 0; i < MAX(structLength, 3); i++, ptr += cellLength) {
				imuBroadcast.euler[i] = ByteToFloat(src, ptr);
			}
		}
		else if (cellLength == 2) {
			for (int i = 0; i < MAX(structLength, 3); i++, ptr += cellLength) {
				imuBroadcast.euler[i] = ByteToInt16(src, ptr) * 0.02f;
			}
		}
	}
}

//USART配置数据接收函数
static void imuUsartSettingReceive(uint8_t *src) {
    uint8_t ptr = 7;
    uint8_t cellLength = powf(2, (src[ptr] & 0x03));
    uint8_t structLength = (src[ptr] & 0x3C) >> 2;
    imuBroadcast.config.configGroup = (src[ptr++] & 0xC0) >> 6;
    if (cellLength * structLength == 24) {
        for(uint8_t j = 0; j < cellLength * structLength; j++) {
            *(imuBroadcast.config.setting + j) = src[ptr + j];
        }
    }
}

//USART转发数据缓存函数
static void imuUsartForwardReceive(uint8_t *src) {
    buffefLoopStruct_t *forwardContent;
    uint8_t length = src[3] - 9;
    if((src[2] & 0xF0) == 0x40) {
        //CAN转发到usart
        forwardContent = &imuBroadcast.forwardContent[CAN_TRANS_USART];
    }
    else {
        //VCP转发到usart
        forwardContent = &imuBroadcast.forwardContent[VCP_TRANS_USART];
    }
    if(forwardContent != NULL) {
        for(uint8_t i = 0; i < length; i++) {
            forwardContent->buffer[(uint8_t)(forwardContent->tail + i)] = src[7 + i];
        }
        forwardContent->tail += length;
    }
}

//放在串口中断中的接收函数
void imuUsartIspFunction(uint8_t *array, uint16_t len) {
    for(uint16_t i = 0 ; i < len; i++){
        imuBroadcast.bufferLoop.buffer[(uint8_t)(imuBroadcast.bufferLoop.tail + i)] = array[i];
    }
    imuBroadcast.bufferLoop.tail += len;
}

//USART接收函数
void imuUsartReceive(void) { 
    buffefLoopStruct_t *bufferLoop = &imuBroadcast.bufferLoop;
    while((uint8_t)(bufferLoop->tail - bufferLoop->header) > 7) {
        //如果当前数组头的值不为0x3F则向前寻找到0x3F
		while(((uint8_t)(bufferLoop->tail - bufferLoop->header) > 7) && (bufferLoop->buffer[bufferLoop->header] != 0x3F)) {
			digitalIncreasing(&bufferLoop->header);
		}
        if((uint8_t)(bufferLoop->tail - bufferLoop->header) > 7) {
            //先校验CRC8是否通过
            uint8_t *crc8Check = malloc(7 * sizeof(uint8_t));
            for (uint8_t i = 0; i < 7; i++) {
                crc8Check[i] = bufferLoop->buffer[(uint8_t)(bufferLoop->header + i)];
            }
            //CRC8校验，不通过直接则跳过
            if (Verify_CRC8_Check_Sum(crc8Check, 7)) {
                uint8_t bufferLength = bufferLoop->buffer[(uint8_t)(bufferLoop->header + 3)];
                //剩余长度必须足够取完当前包,不满足则直接结束当前循环
                if ((uint8_t)(bufferLoop->tail - bufferLoop->header) < bufferLength){
                    break;
				}
                //申请临时数组内存
                uint8_t *bufferDecode = malloc(bufferLength * sizeof(uint8_t));
                //拷贝到读取数组中
                for(uint8_t i = 0; i < bufferLength; i++) {
                    bufferDecode[i] = bufferLoop->buffer[(uint8_t)(bufferLoop->header + i)];
                }
                //16位CRC校验
                if(Verify_CRC16_Check_Sum(bufferDecode, bufferLength)) {
                    //校验通过后加上长度
                    bufferLoop->header += bufferLength;
                    switch (bufferDecode[2] & 0xF0) {
                    //传感器数据
                    case 0x00:
                        imuUsartSensorReceive(bufferDecode);
                        break;
                    //配置信息数据
                    case 0x10:
                        imuUsartSettingReceive(bufferDecode);
                        break;
                    //姿态数据
                    case 0x20:
                        imuUsartAttitudeReceive(bufferDecode);
                        break;
                    //转发数据
                    case 0x40:
                    case 0x80:
                        imuUsartForwardReceive(bufferDecode);
                        break;
                    }
                }
                else {
                    digitalIncreasing(&bufferLoop->header);
				}
                free(bufferDecode);
            }
            else {
                digitalIncreasing(&bufferLoop->header);
            }
            free(crc8Check);
        }
        
    }
}
 
//从CAN指定地址获取一次配置信息
void imuCanGetOnceParameter(CAN_TypeDef *CANx) {
    uint8_t array[8] = {0};
    array[0] = 0x01;
    canTransferPack(CANx,SETTING_ID + 1, array);
}

//CAN接收函数
void imuCanReceive(CanRxMsg *can_rx_msg) {
    switch(can_rx_msg->StdId) {
        case BRAODCAST_RECEIVE_ID : {
            formatTrans16Struct_t gyro_16bit[3];
            for(uint8_t i = 0; i < 3; i++) {
                for(uint8_t j = 0; j < 2; j++) {
                    gyro_16bit[i].u8_temp[j] = can_rx_msg->Data[i * 2 + j];
                }
                if(imuReadConfigRegister(IMU_GYRO_TYPE_RW)) {
                    imuBroadcast.gyro[i] = (float)gyro_16bit[i].s16_temp * 0.001f;
                }
                else {
                    imuBroadcast.gyro[i] = (float)gyro_16bit[i].s16_temp * 0.1f;
                }
            }
            break;
        }
        case (BRAODCAST_RECEIVE_ID + 1) : {
            formatTrans16Struct_t accel_16bit[3];
            for(uint8_t i = 0; i < 3; i++) {
                for(uint8_t j = 0; j < 2; j++) {
                    accel_16bit[i].u8_temp[j] = can_rx_msg->Data[i * 2 + j];
                }
                imuBroadcast.accel[i] = (float)accel_16bit[i].s16_temp * 0.01f;
            }
            break;
        }
        case (BRAODCAST_RECEIVE_ID + 2) : {
            formatTrans16Struct_t mag_16bit[3];
            for(uint8_t i = 0; i < 3; i++) {
                for(uint8_t j = 0; j < 2; j++) {
                    mag_16bit[i].u8_temp[j] = can_rx_msg->Data[i * 2 + j];
                }
                imuBroadcast.mag[i] = (float)mag_16bit[i].s16_temp * 0.01f;
            }
            break;
        }
        case (BRAODCAST_RECEIVE_ID + 3) : {
            formatTrans32Struct_t pres_32bit;
            for(uint8_t j = 0; j < 4; j++) {
                pres_32bit.u8_temp[j] = can_rx_msg->Data[j];
            }
            imuBroadcast.presure = pres_32bit.float_temp;
            break;
        }
        case (BRAODCAST_RECEIVE_ID + 4) : {
            if(imuReadConfigRegister(IMU_ATTITUDE_TYPE_RW)) {
                formatTrans16Struct_t q_16bit[4];
                for(uint8_t i = 0; i < 4; i++) {
                    for(uint8_t j = 0; j < 2; j++) {
                        q_16bit[i].u8_temp[j] = can_rx_msg->Data[i * 2 + j];
                    }
                    imuBroadcast.q[i] = (float)q_16bit[i].s16_temp * 0.0001f;
                }
            }
            else {
                formatTrans32Struct_t euler_32bit[2];
                for(uint8_t i = 0; i < 2; i++) {
                    for(uint8_t j = 0; j < 4; j++) {
                        euler_32bit[i].u8_temp[j] = can_rx_msg->Data[i * 4 + j];
                    }
                    imuBroadcast.euler[i] = euler_32bit[i].float_temp;
                }
            }
            break;
        }
        case (BRAODCAST_RECEIVE_ID + 5) : {
            formatTrans32Struct_t euler_32bit;
            formatTrans16Struct_t CNTR_16bit;
            if(imuReadConfigRegister(IMU_ATTITUDE_TYPE_RW)) {
                for(uint8_t j = 0; j < 2; j++) {
                    CNTR_16bit.u8_temp[j] = can_rx_msg->Data[j];
                }
                imuBroadcast.CNTR = CNTR_16bit.u16_temp;
            }
            else {
                for(uint8_t j = 0; j < 4; j++) {
                    euler_32bit.u8_temp[j] = can_rx_msg->Data[j];
                }
                imuBroadcast.euler[2] = euler_32bit.float_temp;
                for(uint8_t j = 0; j < 2; j++) {
                    CNTR_16bit.u8_temp[j] = can_rx_msg->Data[4 + j];
                }
                imuBroadcast.CNTR = CNTR_16bit.u16_temp;
            }
            break;
        }
        case (BRAODCAST_RECEIVE_ID + 0x06) : {
            for(uint8_t j = 0; j < 8; j++) {
                imuBroadcast.config.setting[j] = can_rx_msg->Data[j];
            }
            break;
        }
        case (BRAODCAST_RECEIVE_ID + 0x07) : {
            
            break;
        }
        case (BRAODCAST_RECEIVE_ID + 0x08) :
        case (BRAODCAST_RECEIVE_ID + 0x09) : {
            buffefLoopStruct_t *forwardContent = &imuBroadcast.forwardContent[USART_TRANS_CAN];
            forwardContent->buffer[(uint8_t)(forwardContent->tail++)] = can_rx_msg->StdId - BRAODCAST_RECEIVE_ID;
            for(uint8_t i = 0 ; i < 8; i++) {
                forwardContent->buffer[(uint8_t)(forwardContent->tail + i)] = can_rx_msg->Data[i];
            }
            forwardContent->tail += 8;
            break;
        }
        case (BRAODCAST_RECEIVE_ID + 0x0A) :
        case (BRAODCAST_RECEIVE_ID + 0x0B) : {
            buffefLoopStruct_t *forwardContent = &imuBroadcast.forwardContent[VCP_TRANS_CAN];
            forwardContent->buffer[(uint8_t)(forwardContent->tail++)] = can_rx_msg->StdId - BRAODCAST_RECEIVE_ID;
            for(uint8_t i = 0 ; i < 8; i++) {
                forwardContent->buffer[(uint8_t)(forwardContent->tail + i)] = can_rx_msg->Data[i];
            }
            forwardContent->tail += 8;
            break;
        }
    }
}

//接收报文初始化
void imuBroadcastInit(void) {
    memset(&imuBroadcast, 0, sizeof(imuBroadcastStruct_t));
    imuConfigStruct_t writeConfig;
    memset(&writeConfig, 0, sizeof(imuConfigStruct_t));
    //加速度参与姿态融合
    imuWirteConfigRegister(&writeConfig, IMU_ACCEL_FUSION_RW, ENABLE);
	//YAW轴连续输出
    imuWirteConfigRegister(&writeConfig, IMU_YAW_CONtTINUOUS_RW, ENABLE);
//	//串口波特率115200
	imuWirteConfigRegister(&writeConfig, IMU_USART_BAUDRATE_RW, 3);
//    //开启USART报文
    imuWirteConfigRegister(&writeConfig, IMU_UASRT_BROADCAST_RW, ENABLE);
//    //开启姿态报文输出
    imuWirteConfigRegister(&writeConfig, IMU_GYRO_OUTPUT_RW, ENABLE);
//    //姿态报文输出类型为欧拉角
    imuWirteConfigRegister(&writeConfig, IMU_ATTITUDE_TYPE_RW, 0);
//	//姿态输出频率500Hz
	imuWirteConfigRegister(&writeConfig, IMU_ATTITUDE_ODR_RW, 0);
//    //姿态报文32位输出
    imuWirteConfigRegister(&writeConfig, IMU_ATTITUDE_LENGTH_RW, OUTPUT_32BIT);
//    //传感器为icm42605
    imuWirteConfigRegister(&writeConfig, IMU_IMU_TYPE_RW, IMU_TYPE_ICM42605);
	
	//USART发送配置到模块
	imuUsartAllSetting(Array_UART7_TX,writeConfig.setting);
	imuUsartGetOnceParameter(Array_UART7_TX);
//    //CAN发送配置到模块
//    canTransferPack(CAN1,SETTING_ID, writeConfig.setting);
	
}


//获取转发消息的长度
uint8_t getForwardReceiveLength(FORWARD_ENUM group) {
    buffefLoopStruct_t *forwardContent = &imuBroadcast.forwardContent[group];
    return (uint8_t)(forwardContent->tail - forwardContent->header);
}

//获取转发消息内容
bool getDataFromForward(uint8_t *dst, FORWARD_ENUM group) {
    buffefLoopStruct_t *forwardContent = &imuBroadcast.forwardContent[group];
    if(getForwardReceiveLength(group) > 0) {
        uint8_t length = getForwardReceiveLength(group);
        for(uint8_t i = 0; i < length; i ++) {
            dst[i] = forwardContent->buffer[forwardContent->header++];
        }
        return true;
    }
    else {
        return false;
    }
}

void imuUsartForwardTest(uint8_t *array) {
    uint8_t index_ptr = 0;
    array[index_ptr++] = 0xAD;
	array[index_ptr++] = 0x03;
	array[index_ptr++] = 0x00;
	array[index_ptr++] = 0x00;
    
    for(uint8_t i = 0; i < 16; i++) {
        array[index_ptr++] = i + 1;
    }
    
    //填装长度
	array[2] = index_ptr + 2;
    //填装校验位
	Append_CRC8_Check_Sum(array, 4);
	Append_CRC16_Check_Sum(array, array[2]);
    //发送数据
	BSP_USART_DMA_SendData(USART2, array, (index_ptr + 2));
}


/*
***************************************************
函数名：Driver_DTU_Init
功能：无线数传初始化
入口参数：	I can understand it at one glance.
返回值：无
应用范围：外部调用
备注：
***************************************************
*/
static void Driver_DTU_Init(USART_TypeDef *USARTx,\
							BSP_GPIOSource_TypeDef *USART_RX,\
                            BSP_GPIOSource_TypeDef *USART_TX,\
							uint8_t PreemptionPriority,uint8_t SubPriority){
	BSP_USART_TypeDef WIRELESS_USART;
	WIRELESS_USART.USARTx = USARTx;
	WIRELESS_USART.USART_RX = USART_RX;
	WIRELESS_USART.USART_TX = USART_TX;
	//波特率为500000
	WIRELESS_USART.USART_InitStructure.USART_BaudRate = 460800;	
	//字长为8位数据格式
	WIRELESS_USART.USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	//一个停止位
	WIRELESS_USART.USART_InitStructure.USART_StopBits = USART_StopBits_1;
	//无校验位
	WIRELESS_USART.USART_InitStructure.USART_Parity = USART_Parity_No;
	//接收/发送模式
	WIRELESS_USART.USART_InitStructure.USART_Mode =USART_Mode_Tx | USART_Mode_Rx;
	//无硬件数据流控制
	WIRELESS_USART.USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None,		
	
	BSP_USART_Init(&WIRELESS_USART,PreemptionPriority,SubPriority);
	BSP_USART_RX_DMA_Init(&WIRELESS_USART);																				
}


/****************************************************
函数名：imuSCInit
功能：何煜陀螺仪串口调用初始化
入口参数：无.
返回值：无
应用范围：外部调用
备注：
****************************************************/
static void imuSCInit(void){
	Driver_DTU_Init(IMUSC_USARTX,\
	IMUSC_USARTX_RX_PIN,\
    IMUSC_USARTX_TX_PIN,\
	IMUSC_USART_PreemptionPriority,\
	IMUSC_USART_SubPriority);
}



