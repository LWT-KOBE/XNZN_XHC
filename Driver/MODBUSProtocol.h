/****************************************************************************************
* Copyright (C), 2020-2030,MyAntenna Tech. Co.,Ltd.
* 文件名： MODBUSProtocol.h
* 描   述：L1模组的MODBUS协议命令定义
* 作   者：pamala           
* 版   本：V1.0
* 日   期: 2020.2.14
*****************************************************************************************/
#ifndef __X_MODBUSPROTOCOL_H_X__
#define __X_MODBUSPROTOCOL_H_X__

void Get_Meas_Dis(unsigned char addr);  //读取测量距离
unsigned short CRC16(unsigned char *Start_Byte,  unsigned short Num_Bytes);
#endif

