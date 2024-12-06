#include "driver.h"
#include "oledfont.h"  	 
#include "application.h"

u8 OLED_GRAM[128][8];	
int Menu=1;
uint8_t CurMode = 0,Radius=0,Line=0, X_point=0,Y_point=0, slope=0 ;

void OLED_Refresh_Gram(void)
{
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA); 
	}   
}


//向SSD1306写入一个字节。
//dat:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(u8 dat,u8 cmd)
{	
	u8 i;			  
	if(cmd)
	  OLED_DC_Set();
	else 
	  OLED_DC_Clr();		  
	//OLED_CS_Clr();
	for(i=0;i<8;i++)
	{			  
		OLED_SCLK_Clr();
		if(dat&0x80)
		   OLED_SDIN_Set();
		else 
		   OLED_SDIN_Clr();
		OLED_SCLK_Set();
		dat<<=1;   
	}				 		  
	OLED_CS_Set();
	OLED_DC_Set();   	  
} 
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD); 
}   	  
//开启OLED显示    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//关闭OLED显示     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)  
{  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA); 
	} //更新显示
}


//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void OLED_ShowChar(u8 x,u8 y,u8 chr)
{      	
	unsigned char c=0,i=0;	
		c=chr-' ';//得到偏移后的值			
		if(x>Max_Column-1){x=0;y=y+2;}
		if(SIZE ==16)
			{
			OLED_Set_Pos(x,y);	
			for(i=0;i<8;i++)
			OLED_WR_Byte(F8X16[c*16+i],OLED_DATA);
			OLED_Set_Pos(x,y+1);
			for(i=0;i<8;i++)
			OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);
			}
			else {	
				OLED_Set_Pos(x,y+1);
				for(i=0;i<6;i++)
				OLED_WR_Byte(F6x8[c][i],OLED_DATA);
				
			}
}
//m^n函数
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ');
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0'); 
	}
} 
//显示一个字符号串
void OLED_ShowString(u8 x,u8 y,u8 *chr)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{		OLED_ShowChar(x,y,chr[j]);
			x+=8;
		if(x>120){x=0;y+=2;}
			j++;
	}
}
//显示汉字
void OLED_ShowCHinese(u8 x,u8 y,u8 no)
{      			    
	u8 t,adder=0;
	OLED_Set_Pos(x,y);	
    for(t=0;t<16;t++)
		{
				OLED_WR_Byte(Hzk[2*no][t],OLED_DATA);
				adder+=1;
     }	
		OLED_Set_Pos(x,y+1);	
    for(t=0;t<16;t++)
			{	
				OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA);
				adder+=1;
      }					
}





/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{ 	
 unsigned int j=0;
 unsigned char x,y;
  
  if(y1%8==0) y=y1/8;      
  else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
	    {      
	    	OLED_WR_Byte(BMP[j++],OLED_DATA);	    	
	    }
	}
} 

void OLED_Float(unsigned char Y,unsigned char X,double real,unsigned char N) 
{
   unsigned char   i_Count=1;
   unsigned char   n[12]={0};
   long   j=1;  
   int    real_int=0;
   double decimal=0;
   unsigned int   real_decimal=0;
   if(real<0)
	 {
		 real_int=(int)(-real);
	 }
	 else
	 {
		 real_int=(int)real;
   }
	 decimal=real-real_int;
   real_decimal=decimal*1e4;
   while(real_int/10/j!=0)
   {
      j=j*10;i_Count++;  
   } 
   n[0]=(real_int/10000)%10; 
   n[1]=(real_int/1000)%10;
   n[2]=(real_int/100)%10;
   n[3]=(real_int/10)%10;
   n[4]=(real_int/1)%10; 
   n[5]='.';
   n[6]=(real_decimal/1000)%10;
   n[7]=(real_decimal/100)%10;
   n[8]=(real_decimal/10)%10;
   n[9]=real_decimal%10;
   n[6+N]='\0'; 
   for(j=0;j<10;j++) n[j]=n[j]+16+32;
	 if(real<0) 
	 {		 
		 i_Count+=1;
		 n[5-i_Count]='-';
	 }
   n[5]='.';
   n[6+N]='\0';   
   OLED_ShowString(X,Y,&n[5-i_Count]); 
}


 void OLED_Num2(unsigned char x,unsigned char y, int number)
{
        unsigned char shi,ge;
	      int num =number;
	if(num<0)
	{ 
		num=-num;
		shi=num%100/10;
    ge=num%10;
	  OLED_fuhao_write(x,y,13); 
    OLED_Num_write(x+1,y,shi);
    OLED_Num_write(x+2,y,ge); 
  } 
  else
	{

		shi=num%100/10;
    ge=num%10;
		OLED_fuhao_write(x,y,11);
    OLED_Num_write(x+1,y,shi);
    OLED_Num_write(x+2,y,ge); 
  }
        
}

void OLED_Num3(unsigned char x,unsigned char y,int number)
{
  unsigned char ge,shi,bai;
	int num =number;
	if(num<0)
	{
		    num=-num;
		    OLED_fuhao_write(x,y,13); //显示-号
        ge = num %10;
        shi = num/10%10;
        bai = num/100;
        OLED_Num_write(x+3,y,ge);
        OLED_Num_write(x+2,y,shi);
        OLED_Num_write(x+1,y,bai);
	}
	else
	{
       OLED_fuhao_write(x,y,11);
        ge = num %10;
        shi = num/10 %10;
        bai = num/100;
        OLED_Num_write(x+3,y,ge);
        OLED_Num_write(x+2,y,shi);
        OLED_Num_write(x+1,y,bai);
  }
}


void OLED_Num4(unsigned char x,unsigned char y, int number)
{
	unsigned char qian,bai,shi,ge;
	int num =number;
	if(num<0)
	{
		num=-num;
	}
	qian=num/1000;
	bai=num%1000/100;
	shi=num%100/10;
	ge=num%10;

	OLED_Num_write(x,y,qian);
	OLED_Num_write(x+1,y,bai);
	OLED_Num_write(x+2,y,shi);
	OLED_Num_write(x+3,y,ge);
}



void OLED_Num_write(unsigned char x,unsigned char y,unsigned char asc) 
{
	int i=0;
	OLED_Set_Pos(x*6,y);
	for(i=0;i<6;i++)
	{
		 OLED_WR_Byte(F6x8[asc+16][i],OLED_DATA);         
	}
}	

void OLED_fuhao_write(unsigned char x,unsigned char y,unsigned char asc) 
{

	  int i=0;
    OLED_Set_Pos(x*6,y);
    for(i=0;i<6;i++)
    {
       OLED_WR_Byte(F6x8[asc][i],OLED_DATA);         
    }
 
}	


///开机显示需要显示的，只需要显示一次即可。减少CPU的运算。/////////////////
void oled_first_show(void)
{

    OLED_ShowString(10,0,"YAW");  
	OLED_ShowString(65,0,"Pitch");
//	OLED_ShowString(70,6,"Roll:");

    OLED_ShowCHinese(10,2,4);//模
	OLED_ShowCHinese(28,2,5);//式
 
//	OLED_ShowCHinese(74,4,2);//半
	OLED_ShowCHinese(91,4,2);//径

    OLED_ShowCHinese(10,4,0);//斜
	OLED_ShowCHinese(28,4,1);//率

    OLED_ShowString(10,6,"X");
    OLED_ShowString(91,6,"Y");

}


void oledUpdate(void)
{

	       
    OLED_Num3(27,0,imuSensorData.yaw.float_temp);					
    OLED_Num2(103,0,imuSensorData.pitch.float_temp);					
//    OLED_Num2(29,2,CurMode);					
		
	
    
 if(Menu==1)
	{
		OLED_ShowString(0,2,"*");
		OLED_ShowString(0,4," ");
		OLED_ShowString(80,4," ");
		OLED_ShowString(0,6," ");	
		OLED_ShowString(80,6," ");	
        
	}
	
	if(Menu==2)
	{
		OLED_ShowString(0,2," ");
		OLED_ShowString(0,4,"*");
		OLED_ShowString(80,4," ");
		OLED_ShowString(0,6," ");	
		OLED_ShowString(80,6," ");	
	
	}
		if(Menu==3)
	{
		OLED_ShowString(0,2," ");
		OLED_ShowString(0,4," ");
		OLED_ShowString(80,4," ");
		OLED_ShowString(0,6,"*");	
		OLED_ShowString(80,6," ");	
		
	}
    
		if(Menu==4)
	{
		OLED_ShowString(0,2," ");
		OLED_ShowString(0,4," ");
		OLED_ShowString(80,4," ");
		OLED_ShowString(0,6," ");	
		OLED_ShowString(80,6,"*");			
	}  

		if(Menu==5)
	{
		OLED_ShowString(0,2," ");
		OLED_ShowString(0,4," ");
		OLED_ShowString(80,4,"*");
		OLED_ShowString(0,6," ");	
		OLED_ShowString(80,6," ");		
	}        
    
}


//初始化SSD1306					    
void OLED_Init(void)
{ 	
 
    BSP_GPIO_Init(BSP_GPIOE4,GPIO_Mode_Out_PP);
    BSP_GPIO_Init(BSP_GPIOE2,GPIO_Mode_Out_PP);
    BSP_GPIO_Init(BSP_GPIOE3,GPIO_Mode_Out_PP);
    BSP_GPIO_Init(BSP_GPIOE5,GPIO_Mode_Out_PP);

    
 	GPIO_SetBits(GPIOE,GPIO_Pin_3|GPIO_Pin_2|GPIO_Pin_4|GPIO_Pin_5);	
				  
	OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
	OLED_WR_Byte(0xCF,OLED_CMD); // Set SEG Output Current Brightness
	OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WR_Byte(0x00,OLED_CMD);//-not offset
	OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
	OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
	OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
	OLED_WR_Byte(0x12,OLED_CMD);
	OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
	OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
	OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WR_Byte(0x02,OLED_CMD);//
	OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
	OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
	OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
	OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
	OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel	
	OLED_WR_Byte(0xAF,OLED_CMD); /*display ON*/ 
	OLED_Clear();
	OLED_Set_Pos(0,0); 	
    
    oled_first_show(); 
    
    
}  



