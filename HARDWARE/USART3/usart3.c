#include "delay.h"
#include "usart3.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"
#include "my_global.h"


//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���

u8 USART3_RX_BUF[USART3_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART3_RX_STA=0;       //����״̬���	  

//��ʼ��IO ����3 
//bound:������
void USART3_Init(u32 bound){
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);// GPIOBʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);

	USART_DeInit(USART3);  //��λ����3
	//USART3_TX   PB.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PB.10

	//USART3_RX	  PB11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PB11

	//RS485 ����ʹ�ܶ�ʹ�õ��� PB1 �ܽ�
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;  //CS-485
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  //�������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	//USART3 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//��Ӧ���ȼ�Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	//USART ��ʼ������
	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_Init(USART3, &USART_InitStructure); //��ʼ������
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�����ж�
	USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ��� 
}


#if EN_USART3_RX   //���ʹ���˽���
void USART3_IRQHandler(void)                	//����3�жϷ������
{
	u8 Res;
	//while((time_sys-time_uart1)<2);//�ȴ����߿���
	USART_ClearFlag(USART3,USART_FLAG_TC);
	if(USART_GetITStatus(USART3,USART_IT_RXNE)!=RESET)//���ָ����USART �жϷ������ 
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
void USART3_IRQHandler(void)                	//����3�жϷ������
{
	u8 Res;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		LED0 = 0;
		Res =USART_ReceiveData(USART3);//(USART3->DR);	//��ȡ���յ�������

		if((USART3_RX_STA&0x8000)==0)//����δ���
		{
			if(USART3_RX_STA&0x4000)//���յ���0x0d
			{
				if(Res!=0x0a)USART3_RX_STA=0;//���մ���,���¿�ʼ
				else USART3_RX_STA|=0x8000;	//��������� 
			}
			else //��û�յ�0X0D
			{	
				if(Res==0x0d)USART3_RX_STA|=0x4000;
				else
				{
					USART3_RX_BUF[USART3_RX_STA&0X3FFF]=Res ;
					USART3_RX_STA++;
					if(USART3_RX_STA>(USART3_REC_LEN-1))USART3_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
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

