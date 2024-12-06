#include "application.h"
#include "driver.h"
#include "nav_para.h"
#include "math.h"




float R = 10; 					 //半径设置(cm)
float angle = 40.0;					 //摆动角度设置(°)
uint8_t RoundDir = 0; 				 //正反转控制
float priod = 512.0;  //单摆周期(毫秒)
////相位补偿 0, 10   20   30   40   50   60   70   80   90   100  110  120  130  140  150  160  170 180
float Phase[19]= {0,-0.1,-0.05,0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,0.05,0.05,0.05,0.07,0};
float Ax = 0.0;
float Ay = 0.0;
float Pendulum_angle = 0.0;
uint32_t pOffset = 0;
float Normalization = 0.0;
float Omega = 0.0;  
float phase = 0.0;

/**************************************画线********************************************/
//入口参数：
//R：摆动的距离
//angle：摆动的角度
void Draw_Line(float R,float angle)			//画线 线长1-60cm步进1cm 角度0-180 步进10度
{

   static float MoveTimeCnt;      
    if(getcontrolData()->loops%2){    
    MoveTimeCnt += 6;							 //每4ms运算1次
    }
	Normalization = (float)MoveTimeCnt / priod;	 //对单摆周期归一化 计算出现在对应时间与单摆周期的关系
	Omega = 2.0f*M_PI*Normalization;			 //归一化处理	将现在对应的时间映射到0-2pi区间  	 					
	Pendulum_angle = atan(R/15.00f)*57.2958f; 	//根据摆幅求出角度,94.8为摆杆距离地面长度(cm)57.2958是把弧度转角度
	
	//利用李萨如图形将线分解到X,Y方向
	Ax = Pendulum_angle*cos(angle*0.017453f);	 //计算出X方向摆幅分量，0.017453为弧度转换
	Ay = Pendulum_angle*sin(angle*0.017453f);	 //计算出Y方向摆幅分量
									
	getGimbalData()->pitchAngleRef = Ax*sin(Omega); //计算出X方向动态摆角
	getGimbalData()->yawAngleRef  = Ay*sin(Omega); //计算出Y方向动态摆角
	
//	getGimbalData()->yawAngleOut = constrainFloat(getGimbalData()->yawAngleOut,getConfigData()->yawMinRange,getConfigData()->yawMaxRange); 
//	getGimbalData()->pitchAngleOut = constrainFloat(getGimbalData()->pitchAngleOut,getConfigData()->pitchMinRange,getConfigData()->pitchMaxRange);     
    
    
}

/**************************************画圆********************************************/
void Round(float R)
{
	
   static float MoveTimeCnt;      
    if(getcontrolData()->loops%2){    
    MoveTimeCnt += 4;							 //每4ms运算1次
    }
	Normalization = (float)MoveTimeCnt / priod;	 //对单摆周期归一化 计算出现在对应时间与单摆周期的关系
	Omega = 2.0f*M_PI*Normalization;			 //归一化处理	将现在对应的时间映射到0-2pi区间  	 					
	Pendulum_angle = atan(R/15.00f)*57.2958f; 	//根据摆幅求出角度,94.8为摆杆距离地面长度(cm)57.2958是把弧度转角度
 
	//利用李萨如图形将线分解到X,Y方向	
	getGimbalData()->pitchAngleRef = Pendulum_angle*sin(Omega); 	 		  //计算出X方向当前摆角
	getGimbalData()->yawAngleRef  = Pendulum_angle*sin(Omega+3.141592/2)-1; //计算出Y方向当前摆角	，-1位Y方向电机补偿
   
}    
    
void Mode_1(void){
	OLED_ShowCHinese(48,2,6);//基
	OLED_ShowCHinese(66,2,7);//本
	OLED_ShowCHinese(84,2,8);//一

}

void Mode_2(void){
    //Draw_Line(R*0.80f,angle);
	OLED_ShowCHinese(48,2,6);//基
	OLED_ShowCHinese(66,2,7);//本
	OLED_ShowCHinese(84,2,9);//二      
}

void Mode_3(void){
    
	OLED_ShowCHinese(48,2,10);//发
	OLED_ShowCHinese(66,2,11);//挥
	OLED_ShowCHinese(84,2,8);//一    
    
}

void Mode_4(void){
    
    
    //Round(R);
	OLED_ShowCHinese(48,2,10);//发
	OLED_ShowCHinese(66,2,11);//挥
	OLED_ShowCHinese(84,2,9);//二    
   
}



