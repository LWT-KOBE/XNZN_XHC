//////////////////////////////////////////////////////////////////////////////////	 
//
//  文 件 名   : main.c
//  功能描述   : OLED 接口演示例程
//              说明: 
//              ----------------------------------------------------------------
//              GND    电源地
//              VCC  接5V或3.3v电源
//              D0   接PB13（SCL）
//              D1   接PB15（SDA）
//              RES  接系统复位
//              DC   接PB1
//              CS   接PB12            
//              ----------------------------------------------------------------
//******************************************************************************/

#ifndef __OLED_H
#define __OLED_H			  	 
#include "bsp.h"
#include "FreeRTOS_board.h" 

//OLED模式设置
//0:4线串行模式

#define OLED_MODE 0
#define SIZE 16
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	    						  
//-----------------OLED端口定义----------------  					   
#define OLED_CS_Clr()  //GPIO_ResetBits(GPIOA,GPIO_Pin_8)//CS
#define OLED_CS_Set()  //GPIO_SetBits(GPIOA,GPIO_Pin_8)

#define OLED_RST_Clr() GPIO_ResetBits(GPIOE,GPIO_Pin_5)//RES
#define OLED_RST_Set() GPIO_SetBits(GPIOE,GPIO_Pin_5)

#define OLED_DC_Clr() GPIO_ResetBits(GPIOE,GPIO_Pin_3)//DC
#define OLED_DC_Set() GPIO_SetBits(GPIOE,GPIO_Pin_3)

#define OLED_SCLK_Clr() GPIO_ResetBits(GPIOE,GPIO_Pin_2)//SCL  D0
#define OLED_SCLK_Set() GPIO_SetBits(GPIOE,GPIO_Pin_2)

#define OLED_SDIN_Clr() GPIO_ResetBits(GPIOE,GPIO_Pin_4)//SDA   D1
#define OLED_SDIN_Set() GPIO_SetBits(GPIOE,GPIO_Pin_4)

 
//PC0~7,作为数据线
#define DATAOUT(x) GPIO_Write(GPIOC,x);//输出  
//使用4线串行接口时使用 


#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

 extern int Menu;
 extern int X,Y;
 extern float length;
 extern float Voltage;
 extern s16 BST_s32RightMotorPulseSigma;
 extern s16 BST_s32LeftMotorPulseSigma;
 extern uint8_t CurMode, Radius,Line, X_point,Y_point, slope;


//OLED控制用函数
void oled_first_show(void);
void OLED_Num_write(unsigned char x,unsigned char y,unsigned char asc);
 void OLED_Num2(unsigned char x,unsigned char y, int number);
void OLED_Float(unsigned char Y,unsigned char X,double real,unsigned char N) ;
void OLED_fuhao_write(unsigned char x,unsigned char y,unsigned char asc) ;
void OLED_Num3(unsigned char x,unsigned char y,int number);



void OLED_Num4(unsigned char x,unsigned char y, int number);
void oledUpdate(void);
void OLED_WR_Byte(u8 dat,u8 cmd);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);	   							   		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(u8 x,u8 y,u8 chr);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);
void OLED_ShowString(u8 x,u8 y, u8 *p);	 
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_ShowCHinese(u8 x,u8 y,u8 no);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);
#endif  
	 



