//#include "application.h"
//#include "bsp.h"
//#include "driver.h"

///*
//***************************************************
//函数名：USART1_IRQHandler
//功能：串口1中断服务函数
//备注：本串口中断为空闲中断+DMA中断
//***************************************************
//*/
////DMA2-2
////void USART1_IRQHandler(void){																	
////	//接收数据的长度
////	uint16_t USART1_len;
////	u8 res = 0;
////	//检测是否是空闲中断
////	if(USART_GetITStatus(USART1,USART_IT_IDLE) == SET){
////		//清USART_IT_IDLE标志
////		USART1_len = USART1->SR;
////		USART1_len = USART1->DR; 
////		//关闭DMA
////		DMA_Cmd(DMA2_Stream2,DISABLE); 
////		//读取接收数据长度		
////		USART1_len = Length_USART1_RX_Buff - DMA2_Stream2->NDTR;
////		//清空标志位		
////		DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_TCIF2 | DMA_FLAG_HTIF2);	
////		while(DMA_GetCmdStatus(DMA2_Stream2) != DISABLE);
////		DMA2_Stream2->NDTR = Length_USART1_RX_Buff;
////		DMA_Cmd(DMA2_Stream2, ENABLE);
////		/*********以下是自定义部分**********/           
////		if(USART1_len){
////		}


////	}
////}



////static unsigned int gresult = 0;
////static bool         fstatus = false;
////static unsigned int gdistance = 0;
////static int          gbytes    = 0;
////static unsigned char gcmd[32] = {0};
//////L1模组串口中断服务函数
////void USART2_IRQHandler(void)
////{
////    unsigned char ch = 0;
////	  int length       = 0;
////    
////    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
////	  {
////			  USART_ClearITPendingBit(USART2, USART_IT_RXNE);
////        ch = USART_ReceiveData(USART2);
////			  if(g_L1Mod_node.type == 1)//ASCII连续测量和快速连续测量
////				{
////					  //Usart_Write_Bytes(DEBUG_USART, &ch, 1);
////				}
////				else if(g_L1Mod_node.type == 2)//HEX连续测量和快速连续测量
////				{
////					  if(!fstatus)
////						{
////							  gresult = gresult|ch;
////							  if((gresult&0xffffff) == 0XB46903) //帧头是B4 69 03
////						    {
////									 fstatus = true;//找到帧头了
////									 gresult = 0;
////						    }
////								else
////								{
////									  gresult = gresult<<8;
////								}
////						}
////						else
////						{
////							 gbytes++;
////							 gdistance = gdistance | ch;
////							 if(gbytes != 4)
////							 {
////								  gdistance = gdistance<<8;
////							 }
////							 else
////							 {
////								   sprintf((char*)gcmd, "D=%f", gdistance/10000.0);
////								   //Usart_Write_Bytes(DEBUG_USART, gcmd, strlen((char*)gcmd));
////								   gbytes    = 0;
////								   gdistance = 0;
////								   fstatus = false;
////							 }
////							 
////						}
////				
////				}
////				else//其他
////				{
////					  length                     = g_L1Mod_node.length;
////			      g_L1Mod_node.buf[length++] = ch;
////			      g_L1Mod_node.length        = length % 256;
////				}	  
////	  }
////		if(USART_GetFlagStatus(USART2,USART_FLAG_ORE) == SET) 
////    {
////        USART_ClearFlag(USART2,USART_FLAG_ORE);
////        USART_ReceiveData(USART2);
////    }
////}









///*
//***************************************************
//函数名：USART2_IRQHandler
//功能：串口2中断服务函数
//备注：本串口中断为空闲中断+DMA中断
//***************************************************
//*/	
////DMA1-5
////void USART2_IRQHandler(void){
////	//接收数据的长度	
////	uint16_t USART2_len;	
////	if(USART_GetITStatus(USART2,USART_IT_IDLE) == SET){
////		//清USART_IT_IDLE标志
////		USART2_len = USART2->SR;
////		USART2_len = USART2->DR; 
////		//关闭DMA
////		DMA_Cmd(DMA1_Stream5,DISABLE);   
////		//清空标志位		
////		DMA_ClearFlag(DMA1_Stream5, DMA_FLAG_TCIF5 | DMA_FLAG_HTIF5);	
////		while(DMA_GetCmdStatus(DMA1_Stream5) != DISABLE);
////		//读取接收数据长度
////		USART2_len = Length_USART2_RX_Buff - DMA1_Stream5->NDTR;	
////		DMA1_Stream5->NDTR = Length_USART2_RX_Buff;
////		DMA_Cmd(DMA1_Stream5, ENABLE);
////		
////		/*********以下是自定义部分**********/	
////		if(USART2_len){

////			TFmini_Recive(Array_USART2_RX);
////			
////		}
////	}
////}

///*
//***************************************************
//函数名：USART3_IRQHandler
//功能：串口3中断服务函数
//备注：本串口中断为空闲中断+DMA中断
//***************************************************
//*/
////DMA1-1
//void USART3_IRQHandler(void){	
//	//接收数据的长度
//	uint16_t USART3_len;	
//	if(USART_GetITStatus(USART3,USART_IT_IDLE) == SET){
//		//清USART_IT_IDLE标志
//		USART3_len = USART3->SR;
//		USART3_len = USART3->DR; 
//		//关闭DMA
//		DMA_Cmd(DMA1_Stream1,DISABLE);    
//		DMA_ClearFlag(DMA1_Stream1, DMA_FLAG_TCIF1 | DMA_FLAG_HTIF1);
//		//清空标志位		
//		while(DMA_GetCmdStatus(DMA1_Stream1) != DISABLE);
//		//读取接收数据长度
//		USART3_len = Length_USART3_RX_Buff - DMA1_Stream1->NDTR;		
//		DMA1_Stream1->NDTR = Length_USART3_RX_Buff;
//		DMA_Cmd(DMA1_Stream1, ENABLE);
//		
//		/*********以下是自定义部分**********/	
//		if(USART3_len){
////			Driver_LobotReadDMA((uint8_t*)DMA1_Stream1->M0AR);
////            receiveHandle(); //接收处理                                
////			digitalIncreasing(&getsupervisorData()->Lobot_ErrorCount);
//			
//			//for(uint16_t i = 0;i < USART3_len;i++)
//				//ANO_DT_Data_Receive_Prepare(Array_USART3_RX[i]);			
//			
//			
//		}
//	}
//}

///*
//***************************************************
//函数名：UART4_IRQHandler
//功能：串口4中断服务函数
//备注：本串口中断为空闲中断+DMA中断
//***************************************************
//*/
////DMA1-2
//void UART4_IRQHandler(void){																			
//	//接收数据的长度
//	uint16_t UART4_len;	
//	if(USART_GetITStatus(UART4,USART_IT_IDLE) == SET){
//		//清USART_IT_IDLE标志
//		UART4_len = UART4->SR;
//		UART4_len = UART4->DR; 
//		//关闭DMA
//		DMA_Cmd(DMA1_Stream2,DISABLE); 
//		//清空标志位		
//		DMA_ClearFlag(DMA1_Stream2, DMA_FLAG_TCIF2 | DMA_FLAG_HTIF2);	
//		while(DMA_GetCmdStatus(DMA1_Stream2) != DISABLE);
//		//读取接收数据长度
//		UART4_len = Length_UART4_RX_Buff - DMA1_Stream2->NDTR;				
//		DMA1_Stream2->NDTR = Length_UART4_RX_Buff;
//		DMA_Cmd(DMA1_Stream2, ENABLE);
//		
//		/*********以下是自定义部分**********/	
//		if(UART4_len){
//		};	//这里只是为了减少warning提示，使用时可以删掉
//	}
//}

///*
//***************************************************
//函数名：UART5_IRQHandler
//功能：串口5中断服务函数
//备注：本串口中断为空闲中断+DMA中断
//***************************************************
//*/
////DMA1-0
//void UART5_IRQHandler(void){																			
//	//接收数据的长度
//	uint16_t UART5_len;	
//	if(USART_GetITStatus(UART5,USART_IT_IDLE) == SET){
//		//清USART_IT_IDLE标志
//		UART5_len = UART5->SR;
//		UART5_len = UART5->DR; 
//		//关闭DMA
//		DMA_Cmd(DMA1_Stream0,DISABLE); 
//		//清空标志位
//		DMA_ClearFlag(DMA1_Stream0, DMA_FLAG_TCIF0 | DMA_FLAG_HTIF0);	
//		while(DMA_GetCmdStatus(DMA1_Stream0) != DISABLE);
//		//读取接收数据长度
//		UART5_len = Length_UART5_RX_Buff - DMA1_Stream0->NDTR;	
//		DMA1_Stream0->NDTR = Length_UART5_RX_Buff;
//		DMA_Cmd(DMA1_Stream0, ENABLE);
//		
//		/*********以下是自定义部分**********/	
//		if(UART5_len){
//		}
//	}
//}

///*
//***************************************************
//函数名：USART6_IRQHandler
//功能：串口6中断服务函数
//备注：本串口中断为空闲中断+DMA中断
//***************************************************
//*/
////DMA2-6
//void USART6_IRQHandler(void){	
//	//接收数据的长度
//	uint16_t USART6_len;	
//	if(USART_GetITStatus(USART6,USART_IT_IDLE) == SET){
//		//清USART_IT_IDLE标志
//		USART6_len = USART6->SR;
//		USART6_len = USART6->DR; 
//		//关闭DMA
//		DMA_Cmd(DMA2_Stream1,DISABLE); 
//		//清空标志位		
//		DMA_ClearFlag(DMA2_Stream1, DMA_FLAG_TCIF1 | DMA_FLAG_HTIF1);	
//		while(DMA_GetCmdStatus(DMA2_Stream1) != DISABLE);
//		//读取接收数据长度
//		USART6_len = Length_USART6_RX_Buff - DMA2_Stream1->NDTR;			
//		DMA2_Stream1->NDTR = Length_USART6_RX_Buff;
//		DMA_Cmd(DMA2_Stream1, ENABLE);
//		
//		/*********以下是自定义部分**********/	
//		if(USART6_len){
//           // Driver_WT931ReadDMA(Array_USART6_RX);  //读取主控上的陀螺仪数据
//           // digitalIncreasing(&getimuData()->IMU_ErrorCount);         //主控陀螺仪状态机                       
//            
//            
//		}	
//	}
//}


///*
//***************************************************
//函数名：UART7_IRQHandler
//功能：串口7中断服务函数
//备注：本串口中断为空闲中断+DMA中断
//***************************************************
//*/
////*
////***************************************************
////函数名：UART7_IRQHandler
////功能：串口7中断服务函数
////备注：本串口中断为空闲中断+DMA中断
////***************************************************
////DMA1-3
//	void UART7_IRQHandler(void){	
//		//接收数据的长度
//		uint16_t UART7_len;	
//		if(USART_GetITStatus(UART7,USART_IT_IDLE) == SET){
//			//清USART_IT_IDLE标志
//			UART7_len = UART7->SR;
//			UART7_len = UART7->DR; 
//			//关闭DMA
//			DMA_Cmd(DMA1_Stream3,DISABLE);
//			//清空标志位
//			DMA_ClearFlag(DMA1_Stream3, DMA_FLAG_TCIF3 | DMA_FLAG_HTIF3);	
//			while(DMA_GetCmdStatus(DMA1_Stream3) != DISABLE);
//			//读取接收数据长度
//			UART7_len = Length_UART7_RX_Buff - DMA1_Stream3->NDTR;				
//			DMA1_Stream3->NDTR = Length_UART7_RX_Buff;
//			DMA_Cmd(DMA1_Stream3, ENABLE);
//			
//			/*********以下是自定义部分**********/	
//			if(UART7_len){                
//              
//				//Openmv_Recive(Array_UART7_RX);
//				
//			}
//		}
//	}


