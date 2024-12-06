//#include "application.h"
//#include "supervisor.h"
//#include "lcdfont.h"
//#include "pic.h"


//lcdStruct_t lcdData;



///******************************************************************************
//      函数说明：在指定区域填充颜色
//      入口数据：xsta,ysta   起始坐标
//                xend,yend   终止坐标
//								color       要填充的颜色
//      返回值：  无
//******************************************************************************/
//void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color)
//{          
//	u16 i,j; 
//	LCD_Address_Set(xsta,ysta,xend-1,yend-1);//设置显示范围
//	for(i=ysta;i<yend;i++)
//	{													   	 	
//		for(j=xsta;j<xend;j++)
//		{
//			LCD_WR_DATA(color);
//		}
//	} 					  	    
//}


///******************************************************************************
//      函数说明：在指定位置画点
//      入口数据：x,y 画点坐标
//                color 点的颜色
//      返回值：  无
//******************************************************************************/
//void LCD_DrawPoint(u16 x,u16 y,u16 color)
//{
//	LCD_Address_Set(x,y,x,y);//设置光标位置 
//	LCD_WR_DATA(color);
//} 


///******************************************************************************
//      函数说明：画线
//      入口数据：x1,y1   起始坐标
//                x2,y2   终止坐标
//                color   线的颜色
//      返回值：  无
//******************************************************************************/
//void LCD_DrawLine(u16 x1,u16 y1,u16 x2,u16 y2,u16 color)
//{
//	u16 t; 
//	int xerr=0,yerr=0,delta_x,delta_y,distance;
//	int incx,incy,uRow,uCol;
//	delta_x=x2-x1; //计算坐标增量 
//	delta_y=y2-y1;
//	uRow=x1;//画线起点坐标
//	uCol=y1;
//	if(delta_x>0)incx=1; //设置单步方向 
//	else if (delta_x==0)incx=0;//垂直线 
//	else {incx=-1;delta_x=-delta_x;}
//	if(delta_y>0)incy=1;
//	else if (delta_y==0)incy=0;//水平线 
//	else {incy=-1;delta_y=-delta_y;}
//	if(delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
//	else distance=delta_y;
//	for(t=0;t<distance+1;t++)
//	{
//		LCD_DrawPoint(uRow,uCol,color);//画点
//		xerr+=delta_x;
//		yerr+=delta_y;
//		if(xerr>distance)
//		{
//			xerr-=distance;
//			uRow+=incx;
//		}
//		if(yerr>distance)
//		{
//			yerr-=distance;
//			uCol+=incy;
//		}
//	}
//}


///******************************************************************************
//      函数说明：画矩形
//      入口数据：x1,y1   起始坐标
//                x2,y2   终止坐标
//                color   矩形的颜色
//      返回值：  无
//******************************************************************************/
//void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
//{
//	LCD_DrawLine(x1,y1,x2,y1,color);
//	LCD_DrawLine(x1,y1,x1,y2,color);
//	LCD_DrawLine(x1,y2,x2,y2,color);
//	LCD_DrawLine(x2,y1,x2,y2,color);
//}


///******************************************************************************
//      函数说明：画圆
//      入口数据：x0,y0   圆心坐标
//                r       半径
//                color   圆的颜色
//      返回值：  无
//******************************************************************************/
//void Draw_Circle(u16 x0,u16 y0,u8 r,u16 color)
//{
//	int a,b;
//	a=0;b=r;	  
//	while(a<=b)
//	{
//		LCD_DrawPoint(x0-b,y0-a,color);             //3           
//		LCD_DrawPoint(x0+b,y0-a,color);             //0           
//		LCD_DrawPoint(x0-a,y0+b,color);             //1                
//		LCD_DrawPoint(x0-a,y0-b,color);             //2             
//		LCD_DrawPoint(x0+b,y0+a,color);             //4               
//		LCD_DrawPoint(x0+a,y0-b,color);             //5
//		LCD_DrawPoint(x0+a,y0+b,color);             //6 
//		LCD_DrawPoint(x0-b,y0+a,color);             //7
//		a++;
//		if((a*a+b*b)>(r*r))//判断要画的点是否过远
//		{
//			b--;
//		}
//	}
//}

///******************************************************************************
//      函数说明：显示汉字串
//      入口数据：x,y显示坐标
//                *s 要显示的汉字串
//                fc 字的颜色
//                bc 字的背景色
//                sizey 字号 可选 16 24 32
//                mode:  0非叠加模式  1叠加模式
//      返回值：  无
//******************************************************************************/
//void LCD_ShowChinese(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
//{
//	while(*s!=0)
//	{
//		if(sizey==12) LCD_ShowChinese12x12(x,y,s,fc,bc,sizey,mode);
//		else if(sizey==16) LCD_ShowChinese16x16(x,y,s,fc,bc,sizey,mode);
//		else if(sizey==24) LCD_ShowChinese24x24(x,y,s,fc,bc,sizey,mode);
//		else if(sizey==32) LCD_ShowChinese32x32(x,y,s,fc,bc,sizey,mode);
//		else return;
//		s+=2;
//		x+=sizey;
//	}
//}

///******************************************************************************
//      函数说明：显示单个12x12汉字
//      入口数据：x,y显示坐标
//                *s 要显示的汉字
//                fc 字的颜色
//                bc 字的背景色
//                sizey 字号
//                mode:  0非叠加模式  1叠加模式
//      返回值：  无
//******************************************************************************/
//void LCD_ShowChinese12x12(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
//{
//	u8 i,j,m=0;
//	u16 k;
//	u16 HZnum;//汉字数目
//	u16 TypefaceNum;//一个字符所占字节大小
//	u16 x0=x;
//	TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
//	                         
//	HZnum=sizeof(tfont12)/sizeof(typFNT_GB12);	//统计汉字数目
//	for(k=0;k<HZnum;k++) 
//	{
//		if((tfont12[k].Index[0]==*(s))&&(tfont12[k].Index[1]==*(s+1)))
//		{ 	
//			LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
//			for(i=0;i<TypefaceNum;i++)
//			{
//				for(j=0;j<8;j++)
//				{	
//					if(!mode)//非叠加方式
//					{
//						if(tfont12[k].Msk[i]&(0x01<<j))LCD_WR_DATA(fc);
//						else LCD_WR_DATA(bc);
//						m++;
//						if(m%sizey==0)
//						{
//							m=0;
//							break;
//						}
//					}
//					else//叠加方式
//					{
//						if(tfont12[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);//画一个点
//						x++;
//						if((x-x0)==sizey)
//						{
//							x=x0;
//							y++;
//							break;
//						}
//					}
//				}
//			}
//		}				  	
//		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
//	}
//} 

///******************************************************************************
//      函数说明：显示单个16x16汉字
//      入口数据：x,y显示坐标
//                *s 要显示的汉字
//                fc 字的颜色
//                bc 字的背景色
//                sizey 字号
//                mode:  0非叠加模式  1叠加模式
//      返回值：  无
//******************************************************************************/
//void LCD_ShowChinese16x16(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
//{
//	u8 i,j,m=0;
//	u16 k;
//	u16 HZnum;//汉字数目
//	u16 TypefaceNum;//一个字符所占字节大小
//	u16 x0=x;
//  TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
//	HZnum=sizeof(tfont16)/sizeof(typFNT_GB16);	//统计汉字数目
//	for(k=0;k<HZnum;k++) 
//	{
//		if ((tfont16[k].Index[0]==*(s))&&(tfont16[k].Index[1]==*(s+1)))
//		{ 	
//			LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
//			for(i=0;i<TypefaceNum;i++)
//			{
//				for(j=0;j<8;j++)
//				{	
//					if(!mode)//非叠加方式
//					{
//						if(tfont16[k].Msk[i]&(0x01<<j))LCD_WR_DATA(fc);
//						else LCD_WR_DATA(bc);
//						m++;
//						if(m%sizey==0)
//						{
//							m=0;
//							break;
//						}
//					}
//					else//叠加方式
//					{
//						if(tfont16[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);//画一个点
//						x++;
//						if((x-x0)==sizey)
//						{
//							x=x0;
//							y++;
//							break;
//						}
//					}
//				}
//			}
//		}				  	
//		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
//	}
//} 


///******************************************************************************
//      函数说明：显示单个24x24汉字
//      入口数据：x,y显示坐标
//                *s 要显示的汉字
//                fc 字的颜色
//                bc 字的背景色
//                sizey 字号
//                mode:  0非叠加模式  1叠加模式
//      返回值：  无
//******************************************************************************/
//void LCD_ShowChinese24x24(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
//{
//	u8 i,j,m=0;
//	u16 k;
//	u16 HZnum;//汉字数目
//	u16 TypefaceNum;//一个字符所占字节大小
//	u16 x0=x;
//	TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
//	HZnum=sizeof(tfont24)/sizeof(typFNT_GB24);	//统计汉字数目
//	for(k=0;k<HZnum;k++) 
//	{
//		if ((tfont24[k].Index[0]==*(s))&&(tfont24[k].Index[1]==*(s+1)))
//		{ 	
//			LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
//			for(i=0;i<TypefaceNum;i++)
//			{
//				for(j=0;j<8;j++)
//				{	
//					if(!mode)//非叠加方式
//					{
//						if(tfont24[k].Msk[i]&(0x01<<j))LCD_WR_DATA(fc);
//						else LCD_WR_DATA(bc);
//						m++;
//						if(m%sizey==0)
//						{
//							m=0;
//							break;
//						}
//					}
//					else//叠加方式
//					{
//						if(tfont24[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);//画一个点
//						x++;
//						if((x-x0)==sizey)
//						{
//							x=x0;
//							y++;
//							break;
//						}
//					}
//				}
//			}
//		}				  	
//		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
//	}
//} 

///******************************************************************************
//      函数说明：显示单个32x32汉字
//      入口数据：x,y显示坐标
//                *s 要显示的汉字
//                fc 字的颜色
//                bc 字的背景色
//                sizey 字号
//                mode:  0非叠加模式  1叠加模式
//      返回值：  无
//******************************************************************************/
//void LCD_ShowChinese32x32(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
//{
//	u8 i,j,m=0;
//	u16 k;
//	u16 HZnum;//汉字数目
//	u16 TypefaceNum;//一个字符所占字节大小
//	u16 x0=x;
//	TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
//	HZnum=sizeof(tfont32)/sizeof(typFNT_GB32);	//统计汉字数目
//	for(k=0;k<HZnum;k++) 
//	{
//		if ((tfont32[k].Index[0]==*(s))&&(tfont32[k].Index[1]==*(s+1)))
//		{ 	
//			LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
//			for(i=0;i<TypefaceNum;i++)
//			{
//				for(j=0;j<8;j++)
//				{	
//					if(!mode)//非叠加方式
//					{
//						if(tfont32[k].Msk[i]&(0x01<<j))LCD_WR_DATA(fc);
//						else LCD_WR_DATA(bc);
//						m++;
//						if(m%sizey==0)
//						{
//							m=0;
//							break;
//						}
//					}
//					else//叠加方式
//					{
//						if(tfont32[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);//画一个点
//						x++;
//						if((x-x0)==sizey)
//						{
//							x=x0;
//							y++;
//							break;
//						}
//					}
//				}
//			}
//		}				  	
//		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
//	}
//}


///******************************************************************************
//      函数说明：显示单个字符
//      入口数据：x,y显示坐标
//                num 要显示的字符
//                fc 字的颜色
//                bc 字的背景色
//                sizey 字号
//                mode:  0非叠加模式  1叠加模式
//      返回值：  无
//******************************************************************************/
//void LCD_ShowChar(u16 x,u16 y,u8 num,u16 fc,u16 bc,u8 sizey,u8 mode)
//{
//	u8 temp,sizex,t,m=0;
//	u16 i,TypefaceNum;//一个字符所占字节大小
//	u16 x0=x;
//	sizex=sizey/2;
//	TypefaceNum=(sizex/8+((sizex%8)?1:0))*sizey;
//	num=num-' ';    //得到偏移后的值
//	LCD_Address_Set(x,y,x+sizex-1,y+sizey-1);  //设置光标位置 
//	for(i=0;i<TypefaceNum;i++)
//	{ 
//		if(sizey==12)temp=ascii_1206[num][i];		       //调用6x12字体
//		else if(sizey==16)temp=ascii_1608[num][i];		 //调用8x16字体
//		else if(sizey==24)temp=ascii_2412[num][i];		 //调用12x24字体
//		else if(sizey==32)temp=ascii_3216[num][i];		 //调用16x32字体
//		else return;
//		for(t=0;t<8;t++)
//		{
//			if(!mode)//非叠加模式
//			{
//				if(temp&(0x01<<t))LCD_WR_DATA(fc);
//				else LCD_WR_DATA(bc);
//				m++;
//				if(m%sizex==0)
//				{
//					m=0;
//					break;
//				}
//			}
//			else//叠加模式
//			{
//				if(temp&(0x01<<t))LCD_DrawPoint(x,y,fc);//画一个点
//				x++;
//				if((x-x0)==sizex)
//				{
//					x=x0;
//					y++;
//					break;
//				}
//			}
//		}
//	}   	 	  
//}


///******************************************************************************
//      函数说明：显示字符串
//      入口数据：x,y显示坐标
//                *p 要显示的字符串
//                fc 字的颜色
//                bc 字的背景色
//                sizey 字号
//                mode:  0非叠加模式  1叠加模式
//      返回值：  无
//******************************************************************************/
//void LCD_ShowString(u16 x,u16 y,const u8 *p,u16 fc,u16 bc,u8 sizey,u8 mode)
//{         
//	while(*p!='\0')
//	{       
//		LCD_ShowChar(x,y,*p,fc,bc,sizey,mode);
//		x+=sizey/2;
//		p++;
//	}  
//}


///******************************************************************************
//      函数说明：显示数字
//      入口数据：m底数，n指数
//      返回值：  无
//******************************************************************************/
//u32 mypow(u8 m,u8 n)
//{
//	u32 result=1;	 
//	while(n--)result*=m;
//	return result;
//}


///******************************************************************************
//      函数说明：显示整数变量
//      入口数据：x,y显示坐标
//                num 要显示整数变量
//                len 要显示的位数
//                fc 字的颜色
//                bc 字的背景色
//                sizey 字号
//      返回值：  无
//******************************************************************************/
//void LCD_ShowIntNum(u16 x,u16 y,u16 num,u8 len,u16 fc,u16 bc,u8 sizey)
//{         	
//	u8 t,temp;
//	u8 enshow=0;
//	u8 sizex=sizey/2;
//	for(t=0;t<len;t++)
//	{
//		temp=(num/mypow(10,len-t-1))%10;
//		if(enshow==0&&t<(len-1))
//		{
//			if(temp==0)
//			{
//				LCD_ShowChar(x+t*sizex,y,' ',fc,bc,sizey,0);
//				continue;
//			}else enshow=1; 
//		 	 
//		}
//	 	LCD_ShowChar(x+t*sizex,y,temp+48,fc,bc,sizey,0);
//	}
//} 


///******************************************************************************
//      函数说明：显示两位小数变量
//      入口数据：x,y显示坐标
//                num 要显示小数变量
//                len 要显示的位数
//                fc 字的颜色
//                bc 字的背景色
//                sizey 字号
//      返回值：  无
//******************************************************************************/
//void LCD_ShowFloatNum1(u16 x,u16 y,float num,u8 len,u16 fc,u16 bc,u8 sizey)
//{         	
//	u8 t,temp,sizex;
//	u16 num1;
//	sizex=sizey/2;
//	num1=num*100;
//	for(t=0;t<len;t++)
//	{
//		temp=(num1/mypow(10,len-t-1))%10;
//		if(t==(len-2))
//		{
//			LCD_ShowChar(x+(len-2)*sizex,y,'.',fc,bc,sizey,0);
//			t++;
//			len+=1;
//		}
//	 	LCD_ShowChar(x+t*sizex,y,temp+48,fc,bc,sizey,0);
//	}
//}


///******************************************************************************
//      函数说明：显示图片
//      入口数据：x,y起点坐标
//                length 图片长度
//                width  图片宽度
//                pic[]  图片数组    
//      返回值：  无
//******************************************************************************/
//void LCD_ShowPicture(u16 x,u16 y,u16 length,u16 width,const u8 pic[])
//{
//	u16 i,j;
//	u32 k=0;
//	LCD_Address_Set(x,y,x+length-1,y+width-1);
//	for(i=0;i<length;i++)
//	{
//		for(j=0;j<width;j++)
//		{
//			LCD_WR_DATA8(pic[k*2]);
//			LCD_WR_DATA8(pic[k*2+1]);
//			k++;
//		}
//	}			
//}





//void ShowString(uint16_t x, uint16_t y, const uint8_t *p, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode) {
//    while (*p != '\0') {
//        LCD_ShowChar(x, y, *p, fc, bc, sizey, mode);
//        x += sizey / 2;
//        p++;
//    }
//}

///********************************************************************************
//    @FunctionName:    ShowText
//    @Description:     显示字符串
//    @FunctionAuthor:  矛盾聚合体
//    @parameters:
//                    NAME      TYPE       DESCRIBE
//                    @x:       uint16_t   显示坐标
//                    @y:       uint16_t   显示坐标
//                    @fc:      uint16_t   字的颜色
//                    @bc:      uint16_t   字的背景色
//                    @sizey:   uint8_t    字号
//                    @mode:    uint8_t    0非叠加模式  1叠加模式
//                    @str:     uint8_t*   要显示的字符串
//    @Return:          void
//    @Other:
//********************************************************************************/
//void ShowText(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode, const char *str, ...) {
//    char strFormatted[256];
//    va_list args;         //存放可变参数的数据结构
//    va_start(args, str);  //初始化可变参数,需要传一个va_list类型变量,和可变参数之前的参数,这里是str
//    vsprintf(strFormatted, str, args);
//    ShowString(x, y, (const uint8_t *)strFormatted, fc, bc, sizey, mode);  //此函数再头文件 stdio中
//    va_end(args);
//}






//void DrawTab(uint16_t x0, uint16_t y0, uint16_t width, uint16_t height, uint16_t row, uint16_t col, uint16_t color) {
//    int i, j;
//    for (i = 0; i <= row; i++) {
//        LCD_DrawLine(
//            x0,
//            y0 + height * i / row,
//            x0 + width,
//            y0 + height * i / row,
//            color);
//    }
//    for (j = 0; j <= col; j++) {
//        LCD_DrawLine(
//            x0 + width * j / col,
//            y0,
//            x0 + width * j / col,
//            y0 + height,
//            color);
//    }
//}


//const char tab_name[7][2][32] =
//    {

//        {"By:Mr.Pan", "Distance"},      // odrv0.axis0.fet_thermistor.temperature		
//        {"Dcbus", "Height"},        // odrv0.axis0.controller.vel_setpoint, odrv0.axis0.encoder.vel_estimate
//        {"Pitch", "Roll"},  // odrv0.axis0.controller.torque_setpoint
//        {"Yaw", "Temp"},        // odrv0.axis0.encoder.error  odrv0.axis1.encoder.shadow_count
//        {"  Robot_Mode ", "  "},      // odrv0.axis0.motor.gate_driver.drv_fault
//        {"  Task", " "},        // odrv0.axis0.motor.current_control.Ibus
//        {"  X_point", "  Y_point"},  // odrv0.axis0.motor_thermistor.temperature
//    };

//const char tab_val[8][2][32] =
//    {
//        {"", ""},
//        {"", ""},
//        {"", ""},
//        {"", ""},
//        {"", ""},
//        {"", ""},
//        {"", ""},
//        {"", ""},
//    };

//int i=0;
//void lcdGlobalInit(void){

//	LCD_Init();//LCD初始化 
//	LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);

//	ShowText(LCD_W * 2 / 4 -1, (LCD_H - 1) * 0 / NUM_ROW + 2, WHITE, BLACK, FontSize_12, 0, "Vbus:");	
//    ShowText(LCD_W * 2 / 4 +70, (LCD_H - 1) * 0 / NUM_ROW + 2, WHITE, BLACK, FontSize_16, 0, "V");

//	ShowText(LCD_W * 2 / 4 -1, (LCD_H - 1) * 0 / NUM_ROW + 15, WHITE, BLACK, FontSize_12, 0, "Temp:");		
//    ShowText(LCD_W * 2 / 4 +70, (LCD_H - 1) *0 / NUM_ROW + 15, WHITE, BLACK, FontSize_12, 0,"C");
//	
//	
//	
//    ShowText(LCD_W * 0 / 4 + 2, (LCD_H - 1) *0 / NUM_ROW + 2, WHITE, BLACK, FontSize_12, 0,"Con:");
//    ShowText(LCD_W * 0 / 4 + 2, (LCD_H - 1) *0 / NUM_ROW + 15, WHITE, BLACK, FontSize_12, 0,"Mode:");


//	for(i=0;i<4;i++){ShowText(LCD_W * 2 / 4 + 6, (LCD_H - 1) *(i+2.5) / NUM_ROW + 2, YELLOW, BLACK, FontSize_12, 0, "->");}
//	

//		
//    ShowText(LCD_W * 0 / 4 + 2, (LCD_H - 1) *2.5 / NUM_ROW + 2, WHITE, BLACK, FontSize_16, 0, "Cur");
//	

//    ShowText(LCD_W * 0 / 4 + 2, (LCD_H - 1) *3.5 / NUM_ROW + 2, WHITE, BLACK, FontSize_16, 0, "Pos");
//		
//		
//    ShowText(LCD_W * 0 / 4 + 2, (LCD_H - 1) *4.5/ NUM_ROW + 2, WHITE, BLACK, FontSize_16, 0, "Vel");

//    //ShowText(LCD_W * 0 / 4 + 55, (LCD_H - 1) *5.5/ NUM_ROW + 2, GREEN, BLACK, FontSize_12, 0, "No_Error!");


//}




//void MenuUpdate(void)
//{
//	
//    //显示当前电压值
//	LCD_ShowFloatNum1(LCD_W * 2 / 4 +30, (LCD_H - 1) * 0 / NUM_ROW + 2,OdReceivedData.vbus_voltage.float_temp,4,YELLOW, BLACK,FontSize_16);

//    //显示当前温度
//	LCD_ShowFloatNum1(LCD_W * 2 / 4 +30, (LCD_H - 1) * 0 / NUM_ROW + 15,OdReceivedData.temperature[0].float_temp,4,BROWN, BLACK,FontSize_16);	

////    //显示当前使用编码器类型
////    ShowText(LCD_W * 0 / 4 + 30, (LCD_H - 1) *0 / NUM_ROW + 2, MAGENTA, BLACK, FontSize_12, 0,"OUT_SPI");

//	//显示当前使用的控制模式
//	switch(OdriveData.ControlMode[0]){
//		
//		case CONTROL_MODE_POSITION_TRAP:
// 
//			ShowText(LCD_W * 0 / 4 + 30, (LCD_H - 1) *0 / NUM_ROW + 15, GREEN, BLACK, FontSize_12, 0, "Trap_Pos");
//            break;  
//		
//		case CONTROL_MODE_VELOCITY_RAMP:
// 
//			ShowText(LCD_W * 0 / 4 + 30, (LCD_H - 1) *0 / NUM_ROW + 15, GREEN, BLACK, FontSize_12, 0, "Ramp_Vel");
//            break;  
//		
//		case CONTROL_MODE_CURRENT_RAMP:
// 
//			ShowText(LCD_W * 0 / 4 + 30, (LCD_H - 1) *0 / NUM_ROW + 15, GREEN, BLACK, FontSize_12, 0, "Ramp_Cur");
//            break;
// 		
////		case CONTROL_MODE_CURRENT:
//// 
////			ShowText(LCD_W * 0 / 4 + 30, (LCD_H - 1) *0 / NUM_ROW + 15, GREEN, BLACK, FontSize_12, 0, "Current");
////            break; 

////		case CONTROL_MODE_POSITION:
//// 
////			ShowText(LCD_W * 0 / 4 + 30, (LCD_H - 1) *0 / NUM_ROW + 15, GREEN, BLACK, FontSize_12, 0, "Position");
////            break; 		

////		case CONTROL_MODE_VELOCITY:
//// 
////			ShowText(LCD_W * 0 / 4 + 30, (LCD_H - 1) *0 / NUM_ROW + 15, GREEN, BLACK, FontSize_12, 0, "Velocity");
////            break; 		
//		default:break;
//		} 	

//		
//    //显示设置电流值
//	//LCD_ShowFloatNum1(LCD_W * 2 / 4 +35, (LCD_H - 1) * 0 / NUM_ROW + 27,OdriveData.SetCur[0].float_temp,5,BROWN, BLACK,FontSize_12);	
//    //显示设置位置值
//	LCD_ShowFloatNum1(LCD_W * 2 / 4 +35, (LCD_H - 1) * 0 / NUM_ROW + 40,OdriveData.SetPos[0].float_temp,5,BROWN, BLACK,FontSize_12);	
//    //显示设置速度值
//	LCD_ShowFloatNum1(LCD_W * 2 / 4 +35, (LCD_H - 1) * 0 / NUM_ROW + 53,OdriveData.SetVel[0].float_temp,5,BROWN, BLACK,FontSize_12);	

//    //显示设置电流值
//	if(OdriveData.SetCur[0].float_temp>0.0f)	{
//		
//		LCD_ShowFloatNum1(LCD_W * 2 / 4 +35, (LCD_H - 1) * 0 / NUM_ROW + 27,OdriveData.SetCur[0].float_temp,5,BROWN, BLACK,FontSize_12);	
//		ShowText(LCD_W * 2 / 4 + 22, (LCD_H - 1) *(0+2.5) / NUM_ROW + 2, BROWN, BLACK, FontSize_16, 0, "+");		
//		
//	}
//	else{
//		LCD_ShowFloatNum1(LCD_W * 2 / 4 +35, (LCD_H - 1) * 0 / NUM_ROW + 27,-OdriveData.SetCur[0].float_temp,5,BROWN, BLACK,FontSize_12);	
//		ShowText(LCD_W * 2 / 4 + 22, (LCD_H - 1) *(0+2.5) / NUM_ROW + 2, BROWN, BLACK, FontSize_16, 0, "-");		
//		
//	}		
//		
//    //显示设置位置值
//	if(OdriveData.SetPos[0].float_temp>0.0f)	{
//		
//		LCD_ShowFloatNum1(LCD_W * 2 / 4 +35, (LCD_H - 1) * 0 / NUM_ROW + 40,OdriveData.SetPos[0].float_temp,5,BROWN, BLACK,FontSize_12);	
//		ShowText(LCD_W * 2 / 4 + 22, (LCD_H - 1) *(1+2.5) / NUM_ROW + 2, BROWN, BLACK, FontSize_16, 0, "+");		
//		
//	}
//	else{
//		LCD_ShowFloatNum1(LCD_W * 2 / 4 +35, (LCD_H - 1) * 0 / NUM_ROW + 40,-OdriveData.SetPos[0].float_temp,5,BROWN, BLACK,FontSize_12);	
//		ShowText(LCD_W * 2 / 4 + 22, (LCD_H - 1) *(1+2.5) / NUM_ROW + 2, BROWN, BLACK, FontSize_16, 0, "-");		
//		
//	}	

//    //显示设置速度值
//	if(OdriveData.SetVel[0].float_temp>0.0f)	{
//		
//		LCD_ShowFloatNum1(LCD_W * 2 / 4 +35, (LCD_H - 1) * 0 / NUM_ROW + 53,OdriveData.SetVel[0].float_temp,5,BROWN, BLACK,FontSize_12);	
//		ShowText(LCD_W * 2 / 4 + 22, (LCD_H - 1) *(2+2.5) / NUM_ROW + 2, BROWN, BLACK, FontSize_16, 0, "+");		
//		
//	}
//	else{
//		LCD_ShowFloatNum1(LCD_W * 2 / 4 +35, (LCD_H - 1) * 0 / NUM_ROW + 53,-OdriveData.SetVel[0].float_temp,5,BROWN, BLACK,FontSize_12);	
//		ShowText(LCD_W * 2 / 4 + 22, (LCD_H - 1) *(2+2.5) / NUM_ROW + 2, BROWN, BLACK, FontSize_16, 0, "-");		
//		
//	}

//	
//		
//    //显示当前电流值
//	if(OdReceivedData.heartbeat_Cur[0]>0.0f)	{
//		
//		LCD_ShowFloatNum1(LCD_W * 2 / 4 +-38, (LCD_H - 1) * 0 / NUM_ROW + 27,OdReceivedData.heartbeat_Cur[0],5,BROWN, BLACK,FontSize_12);	
//		ShowText(LCD_W * 0 / 4 + 27, (LCD_H - 1) *2.5/ NUM_ROW + 2, BROWN, BLACK, FontSize_16, 0, "+");		
//		
//	}
//	else{
//		LCD_ShowFloatNum1(LCD_W * 2 / 4 +-38, (LCD_H - 1) * 0 / NUM_ROW + 27,-OdReceivedData.heartbeat_Cur[0],5,BROWN, BLACK,FontSize_12);	
//		ShowText(LCD_W * 0 / 4 + 27, (LCD_H - 1) *2.5/ NUM_ROW + 2,BROWN, BLACK, FontSize_16, 0, "-");		
//		
//	}

//		

//    //显示当前位置值
//	if(OdReceivedData.heartbeat_Pos[0]>0.0f)	{
//		
//		LCD_ShowFloatNum1(LCD_W * 2 / 4 +-38, (LCD_H - 1) * 0 / NUM_ROW + 40,OdReceivedData.heartbeat_Pos[0],5,BROWN, BLACK,FontSize_12);	
//		ShowText(LCD_W * 0 / 4 + 27, (LCD_H - 1) *3.5/ NUM_ROW + 2, BROWN, BLACK, FontSize_16, 0, "+");		
//		
//	}
//	else{
//		LCD_ShowFloatNum1(LCD_W * 2 / 4 +-38, (LCD_H - 1) * 0 / NUM_ROW + 40,-OdReceivedData.heartbeat_Pos[0],5,BROWN, BLACK,FontSize_12);	
//		ShowText(LCD_W * 0 / 4 + 27, (LCD_H - 1) *3.5/ NUM_ROW + 2, BROWN, BLACK, FontSize_16, 0, "-");		
//		
//	}


//		
//    //显示当前速度值
//	if(OdReceivedData.heartbeat_Vel[0]>0.0f)	{
//		LCD_ShowFloatNum1(LCD_W * 2 / 4 +-38, (LCD_H - 1) * 0 / NUM_ROW + 53,OdReceivedData.heartbeat_Vel[0],5,BROWN, BLACK,FontSize_12);	
//		ShowText(LCD_W * 0 / 4 + 27, (LCD_H - 1) *4.5/ NUM_ROW + 2, BROWN, BLACK, FontSize_16, 0, "+");		
//		
//	}
//	else{
//		LCD_ShowFloatNum1(LCD_W * 2 / 4 +-38, (LCD_H - 1) * 0 / NUM_ROW + 53,-OdReceivedData.heartbeat_Vel[0],5,BROWN, BLACK,FontSize_12);		
//		ShowText(LCD_W * 0 / 4 + 27, (LCD_H - 1) *4.5/ NUM_ROW + 2, BROWN, BLACK, FontSize_16, 0, "-");				
//	}
//		


//	//显示当前的错误信息
////	switch(OdReceivedData.heartbeat_AxisError[0]){
////		case 0:
////			
////				LCD_ShowString(LCD_W * 0 / 4 + 2, (LCD_H - 1) *5.5/ NUM_ROW + 2,"    No_Error!             ",RED,BLACK,FontSize_12,0);
//// 
////            break;  
////		
////		case 10:
////			
////				LCD_ShowString(LCD_W * 0 / 4 + 2, (LCD_H - 1) *5.5/ NUM_ROW + 2,"ERR_OVER_TEMP!",RED,BLACK,FontSize_12,0);
//// 
////            break;  
////		
////		case 2:
////			
////				LCD_ShowString(LCD_W * 0 / 4 + 2, (LCD_H - 1) *5.5/ NUM_ROW + 2,"ERR_UNDER_VOLTAGE!",RED,BLACK,FontSize_12,0);
////		
////            break;  
////		
////		case 1:
////			
////				LCD_ShowString(LCD_W * 0 / 4 + 2, (LCD_H - 1) *5.5/ NUM_ROW + 2,"ERR_OVER_VOLTAGE!",RED,BLACK,FontSize_12,0);		
//// 
////            break;
////		case 4:
////			
////				LCD_ShowString(LCD_W * 0 / 4 + 2, (LCD_H - 1) *5.5/ NUM_ROW + 2,"ERR_OVER_SPEED!",RED,BLACK,FontSize_12,0);
//// 
////            break; 	
////			
////		
////		default:break;
////		} 	
//	
//	//显示当前的错误信息
//	switch(OdriveData.AxisState[axis0]){
//		case CMD_MENU:
//			
//				ShowText(LCD_W * 0 / 4 + 30, (LCD_H - 1) *0 / NUM_ROW + 2, MAGENTA, BLACK, FontSize_12, 0,"Relax");
// 
//            break;  
//		case CMD_MOTOR:
//			
//				ShowText(LCD_W * 0 / 4 + 30, (LCD_H - 1) *0 / NUM_ROW + 2, MAGENTA, BLACK, FontSize_12, 0,"Enable");
// 
//            break;  		
//		case CMD_CALIBRATION:
//			
//				ShowText(LCD_W * 0 / 4 + 30, (LCD_H - 1) *0 / NUM_ROW + 2, MAGENTA, BLACK, FontSize_12, 0,"Cali");
// 
//            break;	
//		default:break;
//		} 	
//	
//}



//void lcdUpdateTask(void *Parameters){
//	TickType_t xLastWakeTime = xTaskGetTickCount();
//	digitalLo(&lcdData.dataInitFlag);
//	while(true){
//		vTaskDelayUntil(&xLastWakeTime,LCD_NORMAL_PERIOD);
//        //防止重复初始化
//		if(!lcdData.dataInitFlag){	
//            //所有控制全部初始化            
//			lcdGlobalInit();																																							
//			digitalHi(&lcdData.dataInitFlag);
//            
//		}  
//		
//		MenuUpdate();
//		
//		digitalHi(&OdriveData.readpairsFlag);         

//		digitalIncreasing(&lcdData.loops);        
//		
//		
//	}
//	
//	
//	
//}

//void LcdTaskInit(void){
//	/* uxPriority
//	在调用任务创建函数xTaskCreate()时就为任务指定了优先级，在启动了任务调度函数之后，
//	可以通过调用xTaskPrioritySet()函数来修改任务的优先级。系统中理论上没有对优先级做出上限要求，
//	只要内存足够大就可以创建多个任务，设置不同的优先级，不同的任务可以赋予相同的优先级，
//	在FreeRTOSConfig.h中configMAX_PRIORITIES 的大小决定最大优先级个数，
//	优先级是从0开始到configMAX_PRIORITIES - 1为止，优先级为0的任务的优先级最高。
//     系统中的调度器总是先让优先级最高的任务先运行，如果多个任务拥有相同的优先级，
//	那么调度器将会使得任务轮流执行一个时间片。这里的时间片等于1/心跳时钟频率，心跳时钟即滴答时钟。
//	可以通过设置FreeRTOSConfig.h中的configTICK_RATE_HZ来设置心跳中断时钟，
//	当configTICK_RATE_HZ赋值为100HZ时，每100模式发生一次心跳时钟中断，系统会执行相应的中断函数。	
//	*/
//				
//	getsupervisorData()->taskEvent[LCD_TASK] = xTaskCreate(lcdUpdateTask,"LCD",LCD_STACK_SIZE,NULL,LCD_PRIORITY,&lcdData.xHandleTask);
//    
//}


