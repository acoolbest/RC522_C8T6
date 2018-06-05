#include "delay.h"
#include "usart3.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"
#include "my_global.h"


//注意,读取USARTx->SR能避免莫名其妙的错误

u8 USART3_RX_BUF[USART3_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART3_RX_STA=0;       //接收状态标记	  

//初始化IO 串口3 
//bound:波特率
void USART3_Init(u32 bound){
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);// GPIOB时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);

	USART_DeInit(USART3);  //复位串口3
	//USART3_TX   PB.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PB.10

	//USART3_RX	  PB11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PB11

	//RS485 控制使能端使用的是 PB1 管脚
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;  //CS-485
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  //推挽输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	//USART3 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//响应优先级为1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	//USART 初始化设置
	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_Init(USART3, &USART_InitStructure); //初始化串口
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断
	USART_Cmd(USART3, ENABLE);                    //使能串口 
}


#if EN_USART3_RX   //如果使能了接收
void USART3_IRQHandler(void)                	//串口3中断服务程序
{
	u8 Res;
	//while((time_sys-time_uart1)<2);//等待总线空闲
	USART_ClearFlag(USART3,USART_FLAG_TC);
	if(USART_GetITStatus(USART3,USART_IT_RXNE)!=RESET)//检查指定的USART 中断发生与否 
	{
		LED0 = 0;
		Res=USART_ReceiveData(USART3);
		delay_ms(1);
		USART_SendData(USART3,Res);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);
		delay_ms(2);
	}

	#if 0
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		Res=USART_ReceiveData(USART3);
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
	if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
	{
		USART_ClearITPendingBit(USART3, USART_IT_TXE);           /* Clear the USART transmit interrupt */
	}
	#endif
} 

#else
void USART3_IRQHandler(void)                	//串口3中断服务程序
{
	u8 Res;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		LED0 = 0;
		Res =USART_ReceiveData(USART3);//(USART3->DR);	//读取接收到的数据

		if((USART3_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART3_RX_STA&0x4000)//接收到了0x0d
			{
				if(Res!=0x0a)USART3_RX_STA=0;//接收错误,重新开始
				else USART3_RX_STA|=0x8000;	//接收完成了 
			}
			else //还没收到0X0D
			{	
				if(Res==0x0d)USART3_RX_STA|=0x4000;
				else
				{
					USART3_RX_BUF[USART3_RX_STA&0X3FFF]=Res ;
					USART3_RX_STA++;
					if(USART3_RX_STA>(USART3_REC_LEN-1))USART3_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}   		 
	}
} 
#endif	
void USART3SendByte(unsigned char SendData)
{	   
	USART_SendData(USART3,SendData);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);	    
} 
void USART3SendString(u8 *cmd,u16 len)
{
	u16 i=0;	
	while((*(cmd+i)!=0)&&(i<len))
	{
		USART3SendByte(*(cmd+i));i++;
	}
}

