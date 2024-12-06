#include "bsp_iic.h"
#include "driver_ist8310.h"

BSP_I2C_TypeDef *IST8310_I2C = &BSP_I2C1;


//the first column:the registers of IST8310. 第一列:IST8310的寄存器
//the second column: the value to be writed to the registers.第二列:需要写入的寄存器值
//the third column: return error value.第三列:返回的错误码
static const uint8_t ist8310_write_reg_data_error[IST8310_WRITE_REG_NUM][3] ={
        {0x0B, 0x08, 0x01},     //enalbe interrupt  and low pin polarity.开启中断，并且设置低电平
        {0x41, 0x09, 0x02},     //average 2 times.平均采样两次
        {0x42, 0xC0, 0x03},     //must be 0xC0. 必须是0xC0
        {0x0A, 0x0B, 0x04}};    //200Hz output rate.200Hz输出频率

		

uint8_t IST8310_Init(void)
{
    uint8_t res = 0;
    uint8_t writeNum = 0;
	
	
	BSP_I2C_Init(IST8310_I2C);	
	
	BSP_GPIO_Init(BSP_GPIOB7, GPIO_Mode_Out_PP);	
	IST8310_RESET=0;
	vTaskDelay(20);
	IST8310_RESET=1;
	vTaskDelay(20);

		
	
    res = BSP_I2C_ReadOneByte(IST8310_I2C,IST8310_IIC_ADDRESS,IST8310_WHO_AM_I);
    if (res != IST8310_WHO_AM_I_VALUE)
    {
        return IST8310_NO_SENSOR;
    }

    //set mpu6500 sonsor config and check
    for (writeNum = 0; writeNum < IST8310_WRITE_REG_NUM; writeNum++)
    {
        BSP_I2C_WriteOneByte(IST8310_I2C,IST8310_IIC_ADDRESS,ist8310_write_reg_data_error[writeNum][0], ist8310_write_reg_data_error[writeNum][1]);
		vTaskDelay(1);
        res = BSP_I2C_ReadOneByte(IST8310_I2C,IST8310_IIC_ADDRESS,ist8310_write_reg_data_error[writeNum][0]);
		vTaskDelay(1);
        if (res != ist8310_write_reg_data_error[writeNum][1])
        {
            return ist8310_write_reg_data_error[writeNum][2];
        }
    }

    return IST8310_NO_ERROR;

	
}




//磁力机航向角计算与补偿：https://blog.csdn.net/u013636775/article/details/72675148
void IST8310_Updata(imusensorStruct_t *MagData)
{
	int16_t temp_ist8310_data = 0;
	uint8_t buf[6];
	static uint16_t IST8310_Sample_Cnt=0;
	IST8310_Sample_Cnt++;
	if(IST8310_Sample_Cnt==6){//至少间隔6ms
		buf[0]=BSP_I2C_ReadOneByte(IST8310_I2C,IST8310_IIC_ADDRESS,0x03);//OUT_X_L_A
		buf[1]=BSP_I2C_ReadOneByte(IST8310_I2C,IST8310_IIC_ADDRESS,0x04);//OUT_X_H_A
		buf[2]=BSP_I2C_ReadOneByte(IST8310_I2C,IST8310_IIC_ADDRESS,0x05);//OUT_Y_L_A
		buf[3]=BSP_I2C_ReadOneByte(IST8310_I2C,IST8310_IIC_ADDRESS,0x06);//OUT_Y_H_A
		buf[4]=BSP_I2C_ReadOneByte(IST8310_I2C,IST8310_IIC_ADDRESS,0x07);//OUT_Z_L_A
		buf[5]=BSP_I2C_ReadOneByte(IST8310_I2C,IST8310_IIC_ADDRESS,0x08);//OUT_Z_H_A
				
		/*****************合成三轴磁力计数据******************/			
		temp_ist8310_data = (int16_t)((buf[1] << 8) | buf[0]);
		MagData->rawMag.x = -MAG_SEN * temp_ist8310_data;
		temp_ist8310_data = (int16_t)((buf[3] << 8) | buf[2]);
		MagData->rawMag.y = MAG_SEN * temp_ist8310_data;
		temp_ist8310_data = (int16_t)((buf[5] << 8) | buf[4]);
		MagData->rawMag.z = MAG_SEN * temp_ist8310_data;

			  
		IST8310_Sample_Cnt=0;
		//digitalIncreasing(&imuSensorData.MagError.errorCount);
	  }


}


