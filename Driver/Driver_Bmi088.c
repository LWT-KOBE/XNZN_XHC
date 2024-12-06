#include "Driver_Bmi088.h"
#include "util.h"
#include "processing.h"
#include "imu.h"
#include "nav_para.h"
#include "supervisor.h"
#include "BMI088reg.h"

f32_t BMI088_ACCEL_SEN = BMI088_ACCEL_12G_SEN;
f32_t BMI088_GYRO_SEN = BMI088_GYRO_2000_SEN;

BSP_SPI_TypeDef BMI088_SPI_Base = BMI088_SPI_DEFAULT;
BSP_SPI_TypeDef *BMI088_SPI = &BMI088_SPI_Base;

BMI088Struct_t BMI088Data ;

void BMI088_delay_us(uint16_t us)
{

    uint32_t ticks = 0;
    uint32_t told = 0;
    uint32_t tnow = 0;
    uint32_t tcnt = 0;
    uint32_t reload = 0;
    reload = SysTick->LOAD;
    ticks = us * 168;
    told = SysTick->VAL;
    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            if (tnow < told)
            {
                tcnt += told - tnow;
            }
            else
            {
                tcnt += reload - tnow + told;
            }
            told = tnow;
            if (tcnt >= ticks)
            {
                break;
            }
        }
    }


}


void BMI088_delay_ms(uint16_t ms)
{
    while(ms--)
    {
        BMI088_delay_us(1000);
    }
}




float *getBMI088GyroScale(void) {
    return &BMI088Data.gyroScale;
}

float *getBMI088AccelScale(void) {
    return &BMI088Data.accelScale;
}

int16_t getBMI088Gyroscope(uint8_t index) {
    return BMI088Data.gyroscope[index].s16_temp;
}

int16_t getBMI088Acceleration(uint8_t index) {
    return BMI088Data.acceleration[index].s16_temp;
}

int16_t getBMI088Temperature(void) {
    return BMI088Data.temperature.s16_temp;
}


uint16_t Bmi088DataCNTR_Check(void) {
	static uint16_t last_CNTR;
	uint16_t value = abs((int16_t)BMI088Data.dataCNTR.s16_temp - (int16_t)last_CNTR);
	last_CNTR = BMI088Data.dataCNTR.s16_temp ;
	return value;
}

static void calculateBMI088GyroScale(void) {
    float actualFSR = 0.0f;
    switch (*BMI088Data.gyroFSR) {
        case BMI088_FSR_2000DPS:
            actualFSR = BMI088_GYRO_2000_SEN;
            break;
        case BMI088_FSR_1000DPS:
            actualFSR = BMI088_GYRO_1000_SEN;
            break;
        case BMI088_FSR_500DPS:
            actualFSR = BMI088_GYRO_500_SEN;
            break;
        case BMI088_FSR_250DPS:
            actualFSR = BMI088_GYRO_250_SEN;
            break;
        case BMI088_FSR_125DPS:
            actualFSR = BMI088_GYRO_125_SEN;
            break;

    }
    BMI088Data.gyroScale = actualFSR * 2.0f;   
}

static void calculateBMI088AccelScale(void) {
    float actualFSR = 0.0f;
    switch (*BMI088Data.accelFSR) {
        case BMI088_FSR_3G:
            actualFSR = BMI088_ACCEL_3G_SEN;
            break; 
        case BMI088_FSR_6G:
            actualFSR = BMI088_ACCEL_6G_SEN;
            break;
        case BMI088_FSR_12G:
            actualFSR = BMI088_ACCEL_12G_SEN;
            break;
        case BMI088_FSR_24G:
            actualFSR = BMI088_ACCEL_24G_SEN;
            break;
    }
    BMI088Data.accelScale = actualFSR ; 
}





/*
***************************************************
函数名：		changeBMI088GyroFSR
功能：			更改BMI088的加速度和陀螺仪FSR
入口参数：	无
返回值：		无
应用范围：	内部调用
备注：
***************************************************
*/
static void changeBMI088GyroFSR(void) {
 
            //重新计算分辨率
            calculateBMI088AccelScale();
            //set accel fsr
            vTaskDelay(1); 
            //重新计算分辨率
            calculateBMI088GyroScale();
            //set gyro fsr
            vTaskDelay(1);
}

static void BMI088_write_single_reg(uint8_t reg, uint8_t data)
{
    BSP_SPI_ReadWriteByte(BMI088_SPI,reg);
    BSP_SPI_ReadWriteByte(BMI088_SPI,data);
}

static void BMI088_read_muli_reg(uint8_t regAddr,uint8_t *dat, uint8_t len )
{
    BSP_SPI_ReadWriteByte(BMI088_SPI,regAddr | 0x80);

    while (len != 0)
    {

        *dat = BSP_SPI_ReadWriteByte(BMI088_SPI,0x55);
        dat++;
        len--;
    }
}

static void BMI088_read_single_reg(uint8_t regAddr, uint8_t *return_data){

    BSP_SPI_ReadWriteByte(BMI088_SPI,regAddr | 0x80);
    *return_data = BSP_SPI_ReadWriteByte(BMI088_SPI,0x55);
}	


#define BMI088_accel_write_single_reg(reg, data) \
    {                                            \
        BMI088_ACC=0;                     \
        BMI088_write_single_reg((reg), (data));  \
        BMI088_ACC=1;                     \
    }

	
#define BMI088_accel_read_single_reg(reg, data) \
    {                                           \
        BMI088_ACC=0;                    \
        BSP_SPI_ReadWriteByte(BMI088_SPI,(reg) | 0x80);   \
        BSP_SPI_ReadWriteByte(BMI088_SPI,0x55);           \
        (data) = BSP_SPI_ReadWriteByte(BMI088_SPI,0x55);  \
        BMI088_ACC=1;                    \
    }

	
#define BMI088_accel_read_muli_reg(reg, data, len) \
    {                                              \
        BMI088_ACC=0;                       \
        BSP_SPI_ReadWriteByte(BMI088_SPI, (reg) | 0x80);      \
        BMI088_read_muli_reg(reg, data, len);      \
        BMI088_ACC=1;                       \
    }


#define BMI088_gyro_write_single_reg(reg, data) \
    {                                           \
        BMI088_GYO=0;                      \
        BMI088_write_single_reg((reg), (data)); \
         BMI088_GYO=1;                    \
    }
	

#define BMI088_gyro_read_single_reg(reg, data)  \
    {                                           \
        BMI088_GYO=0;                     \
        BMI088_read_single_reg((reg), &(data)); \
        BMI088_GYO=1;                     \
    }


	
#define BMI088_gyro_read_muli_reg(reg, data, len)   \
    {                                               \
        BMI088_GYO=0;                          \
        BMI088_read_muli_reg((reg), (data), (len)); \
        BMI088_GYO=1;                          \
    }

	
//static void BMI088_write_single_reg(uint8_t reg, uint8_t data);
//static void BMI088_read_single_reg(uint8_t reg, uint8_t *return_data);
////static void BMI088_write_muli_reg(uint8_t reg, uint8_t* buf, uint8_t len );
//static void BMI088_read_muli_reg(uint8_t reg, uint8_t *buf, uint8_t len);
	
	
static uint8_t write_BMI088_accel_reg_data_error[BMI088_WRITE_ACCEL_REG_NUM][3] =
    {
        {BMI088_ACC_PWR_CTRL, BMI088_ACC_ENABLE_ACC_ON, BMI088_ACC_PWR_CTRL_ERROR},
        {BMI088_ACC_PWR_CONF, BMI088_ACC_PWR_ACTIVE_MODE, BMI088_ACC_PWR_CONF_ERROR},
        {BMI088_ACC_CONF,  BMI088_ACC_NORMAL| BMI088_ACC_800_HZ | BMI088_ACC_CONF_MUST_Set, BMI088_ACC_CONF_ERROR},
        {BMI088_ACC_RANGE, BMI088_ACC_RANGE_3G, BMI088_ACC_RANGE_ERROR},
        {BMI088_INT1_IO_CTRL, BMI088_ACC_INT1_IO_ENABLE | BMI088_ACC_INT1_GPIO_PP | BMI088_ACC_INT1_GPIO_LOW, BMI088_INT1_IO_CTRL_ERROR},
        {BMI088_INT_MAP_DATA, BMI088_ACC_INT1_DRDY_INTERRUPT, BMI088_INT_MAP_DATA_ERROR}

};

static uint8_t write_BMI088_gyro_reg_data_error[BMI088_WRITE_GYRO_REG_NUM][3] =
    {
        {BMI088_GYRO_RANGE, BMI088_GYRO_2000, BMI088_GYRO_RANGE_ERROR},
        {BMI088_GYRO_BANDWIDTH, BMI088_GYRO_1000_116_HZ | BMI088_GYRO_BANDWIDTH_MUST_Set, BMI088_GYRO_BANDWIDTH_ERROR},
        {BMI088_GYRO_LPM1, BMI088_GYRO_NORMAL_MODE, BMI088_GYRO_LPM1_ERROR},
        {BMI088_GYRO_CTRL, BMI088_DRDY_ON, BMI088_GYRO_CTRL_ERROR},
        {BMI088_GYRO_INT3_INT4_IO_CONF, BMI088_GYRO_INT3_GPIO_PP | BMI088_GYRO_INT3_GPIO_LOW, BMI088_GYRO_INT3_INT4_IO_CONF_ERROR},
        {BMI088_GYRO_INT3_INT4_IO_MAP, BMI088_GYRO_DRDY_IO_INT3, BMI088_GYRO_INT3_INT4_IO_MAP_ERROR}

};
	
uint8_t BMI088_init(uint8_t *gyroFSR, uint8_t *accelFSR)
{
    uint8_t error = BMI088_NO_ERROR;
    //给指针赋值
    BMI088Data.gyroFSR = gyroFSR;
    BMI088Data.accelFSR = accelFSR;	
    // GPIO and SPI  Init .
	BSP_SPI_Init(BMI088_SPI);
	//GYO
	BSP_GPIO_Init(BSP_GPIOD11,GPIO_Mode_Out_PP);
	//ACC
	BSP_GPIO_Init(BSP_GPIOD10,GPIO_Mode_Out_PP);
	
    changeBMI088GyroFSR();
	
	
    error |= bmi088_gyro_init();	
    error |= bmi088_accel_init();


    return error;
}

bool_t bmi088_accel_init(void)
{
    uint8_t res = 0;
    uint8_t write_reg_num = 0;

    //check commiunication
    BMI088_accel_read_single_reg(BMI088_ACC_CHIP_ID, res);
    BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);
    BMI088_accel_read_single_reg(BMI088_ACC_CHIP_ID, res);
    BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);

    //accel software reset
    BMI088_accel_write_single_reg(BMI088_ACC_SOFTRESET, BMI088_ACC_SOFTRESET_VALUE);
    BMI088_delay_ms(BMI088_LONG_DELAY_TIME);

    //check commiunication is normal after reset
    BMI088_accel_read_single_reg(BMI088_ACC_CHIP_ID, res);
    BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);
    BMI088_accel_read_single_reg(BMI088_ACC_CHIP_ID, res);
    BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);

    // check the "who am I"
    if (res != BMI088_ACC_CHIP_ID_VALUE)
    {
        return BMI088_NO_SENSOR;
    }

    //set accel sonsor config and check
    for (write_reg_num = 0; write_reg_num < BMI088_WRITE_ACCEL_REG_NUM; write_reg_num++)
    {

        BMI088_accel_write_single_reg(write_BMI088_accel_reg_data_error[write_reg_num][0], write_BMI088_accel_reg_data_error[write_reg_num][1]);
        BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);

        BMI088_accel_read_single_reg(write_BMI088_accel_reg_data_error[write_reg_num][0], res);
        BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);

        if (res != write_BMI088_accel_reg_data_error[write_reg_num][1])
        {
            return write_BMI088_accel_reg_data_error[write_reg_num][2];
        }
    }
    return BMI088_NO_ERROR;
}

bool_t bmi088_gyro_init(void)
{
    uint8_t write_reg_num = 0;
    uint8_t res = 0;

    //check commiunication
    BMI088_gyro_read_single_reg(BMI088_GYRO_CHIP_ID, res);
    BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);
    BMI088_gyro_read_single_reg(BMI088_GYRO_CHIP_ID, res);
    BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);

    //reset the gyro sensor
    BMI088_gyro_write_single_reg(BMI088_GYRO_SOFTRESET, BMI088_GYRO_SOFTRESET_VALUE);
    BMI088_delay_ms(BMI088_LONG_DELAY_TIME);
    //check commiunication is normal after reset
    BMI088_gyro_read_single_reg(BMI088_GYRO_CHIP_ID, res);
    BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);
    BMI088_gyro_read_single_reg(BMI088_GYRO_CHIP_ID, res);
    BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);

    // check the "who am I"
    if (res != BMI088_GYRO_CHIP_ID_VALUE)
    {
        return BMI088_NO_SENSOR;
    }

    //set gyro sonsor config and check
    for (write_reg_num = 0; write_reg_num < BMI088_WRITE_GYRO_REG_NUM; write_reg_num++)
    {

        BMI088_gyro_write_single_reg(write_BMI088_gyro_reg_data_error[write_reg_num][0], write_BMI088_gyro_reg_data_error[write_reg_num][1]);
        BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);

        BMI088_gyro_read_single_reg(write_BMI088_gyro_reg_data_error[write_reg_num][0], res);
        BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);

        if (res != write_BMI088_gyro_reg_data_error[write_reg_num][1])
        {
            return write_BMI088_gyro_reg_data_error[write_reg_num][2];
        }
    }

    return BMI088_NO_ERROR;
}


uint16_t BMI088_read(void)
{
    uint8_t buf[8] = {0, 0, 0, 0, 0, 0};
    int16_t bmi088_raw_temp;
    uint16_t dataSum = 0;



    BMI088_accel_read_muli_reg(BMI088_ACCEL_XOUT_L, buf, 6);

    bmi088_raw_temp = (int16_t)((buf[1]) << 8) | buf[0];	
    BMI088Data.acceleration[0].s16_temp	= bmi088_raw_temp;
	
    bmi088_raw_temp = (int16_t)((buf[3]) << 8) | buf[2];
    BMI088Data.acceleration[1].s16_temp	= bmi088_raw_temp;
		
    bmi088_raw_temp = (int16_t)((buf[5]) << 8) | buf[4];
    BMI088Data.acceleration[2].s16_temp	= bmi088_raw_temp;
	
    BMI088_gyro_read_muli_reg(BMI088_GYRO_CHIP_ID, buf, 8);
    if(buf[0] == BMI088_GYRO_CHIP_ID_VALUE)
    {
        bmi088_raw_temp = (int16_t)((buf[3]) << 8) | buf[2];
		BMI088Data.gyroscope[0].s16_temp = bmi088_raw_temp;
		
        bmi088_raw_temp = (int16_t)((buf[5]) << 8) | buf[4];
		BMI088Data.gyroscope[1].s16_temp = bmi088_raw_temp;
		
        bmi088_raw_temp = (int16_t)((buf[7]) << 8) | buf[6];
		BMI088Data.gyroscope[2].s16_temp = bmi088_raw_temp;		
    }
	
    BMI088_accel_read_muli_reg(BMI088_TEMP_M, buf, 2);

    bmi088_raw_temp = (int16_t)((buf[0] << 3) | (buf[1] >> 5));

    if (bmi088_raw_temp > 1023)
    {
        bmi088_raw_temp -= 2048;
    }	
	
	
    BMI088Data.temperature.s16_temp = bmi088_raw_temp ;
	
    dataSum = BMI088Data.acceleration[0].s16_temp + BMI088Data.acceleration[1].s16_temp \
                    + BMI088Data.acceleration[2].s16_temp + BMI088Data.temperature.s16_temp \
                    + BMI088Data.gyroscope[0].s16_temp + BMI088Data.gyroscope[1].s16_temp \
                    + BMI088Data.gyroscope[2].s16_temp;
    if(dataSum != 0) {
        digitalIncreasing(&BMI088Data.dataCNTR.u16_temp);
		digitalIncreasing(&BMI088Data.ImuError.errorCount);
	
    }	
	
    return Bmi088DataCNTR_Check();
	
}



