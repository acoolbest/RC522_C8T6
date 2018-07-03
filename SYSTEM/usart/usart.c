/*
 *֧����Ӧ��ͬƵ���µĴ��ڲ���������.
 *�����˶�printf��֧��
 *�����˴��ڽ��������.
 *������printf��һ���ַ���ʧ��bug
 *V1.4�޸�˵��
 *1,�޸Ĵ��ڳ�ʼ��IO��bug
 *2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
 *3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
 *4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
 */

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "relay.h"
#include "my_global.h"


//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  

uint8_t g_rs485_addr = 0;//485������ַ
uint8_t g_rfid_state = RFID_PULL_OUT;
uint8_t g_lock_state = LOCK_STATE_OFF;
uint8_t g_rfid_id[8] = {0};
uint8_t g_chipID[12] = {0};
struct cmd_recv_stru g_stru_cmd_recv = {0};

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*ʹ��microLib�ķ���*/
/* 
   int fputc(int ch, FILE *f)
   {
   USART_SendData(USART1, (uint8_t) ch);

   while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	

   return ch;
   }
   int GetKey (void)  { 

   while (!(USART1->SR & USART_FLAG_RXNE));

   return ((int)(USART1->DR & 0x1FF));
   }
 */
//////////////////////////////////////////////////////////////////////////////////
static void update_cmd_recv(struct cmd_recv_stru *p_cmd_recv_stru, uint8_t u8_recv)
{
	p_cmd_recv_stru->cmd_buffer[p_cmd_recv_stru->cmd_index] = u8_recv;
	p_cmd_recv_stru->cmd_index = (p_cmd_recv_stru->cmd_index + 1) % MAX_SERIAL_BUFFER_LENGHT;
	
	if(p_cmd_recv_stru->cmd_state != ENUM_SPP_MSG_PAYLOAD)
    	p_cmd_recv_stru->cmd_state++;
	else if(p_cmd_recv_stru->cmd_index + 2 == p_cmd_recv_stru->cmd_length)
		p_cmd_recv_stru->cmd_state++;
}

static void is_com_msg_head(struct cmd_recv_stru *p_cmd_recv_stru, uint8_t u8_recv)
{
	if (u8_recv == RECV_COM_MSG_HEAD)
    {
		p_cmd_recv_stru->cmd_recv_state = COM_CMD_RECV_INCOMPLETE;
        p_cmd_recv_stru->cmd_index = 0;
		update_cmd_recv(p_cmd_recv_stru, u8_recv);
    }
	else
	{
		p_cmd_recv_stru->cmd_index = 0;
		p_cmd_recv_stru->cmd_state = ENUM_COM_MSG_HEAD;
	}
}

void process_com_data(struct cmd_recv_stru *p_cmd_recv_stru, u8 u8_recv)
{
	switch (p_cmd_recv_stru->cmd_state)
	{
		case ENUM_COM_MSG_HEAD:
		{
			is_com_msg_head(p_cmd_recv_stru, u8_recv);
			break;
		}
		case ENUM_COM_MSG_LEN:
		{
			if (u8_recv >=0x07 && u8_recv <= 0x13)
			{
				p_cmd_recv_stru->cmd_length = u8_recv;
				update_cmd_recv(p_cmd_recv_stru, u8_recv);
			}
			else 
				is_com_msg_head(p_cmd_recv_stru, u8_recv);
			break;
		}
		case ENUM_COM_MSG_DST_ADDR:
		{
			#ifdef SIM800C_BOARD
			if(u8_recv == g_rs485_addr)
			#endif
			
			#ifdef RC522_BOARD
			if(u8_recv == COM_BROADCAST_ADDR || u8_recv == g_rs485_addr)
			#endif
			
				update_cmd_recv(p_cmd_recv_stru, u8_recv);
			else 
				is_com_msg_head(p_cmd_recv_stru, u8_recv);
			break;
		}
		case ENUM_COM_MSG_SRC_ADDR:
		{
			#ifdef SIM800C_BOARD
			if(u8_recv <= COM_MAX_SLAVE_ADDR)
			#endif
			
			#ifdef RC522_BOARD
			if(u8_recv == COM_MASTER_ADDR)
			#endif
			
				update_cmd_recv(p_cmd_recv_stru, u8_recv);
			else 
				is_com_msg_head(p_cmd_recv_stru, u8_recv);
			break;
		}
		case ENUM_COM_MSG_FUNCTION_CODE:
		case ENUM_SPP_MSG_PAYLOAD:
		case ENUM_SPP_MSG_CHECKSUM:
		{
			update_cmd_recv(p_cmd_recv_stru, u8_recv);
			break;
		}
		case ENUM_COM_MSG_TAIL:
		{
			if(u8_recv == DEFAULT_COM_MSG_TAIL)
			{
				p_cmd_recv_stru->cmd_buffer[p_cmd_recv_stru->cmd_index] = u8_recv;
				p_cmd_recv_stru->cmd_index = 0;
                p_cmd_recv_stru->cmd_state = ENUM_COM_MSG_HEAD;
				p_cmd_recv_stru->cmd_recv_state = COM_CMD_RECV_COMPLETE;
			}
			else 
				is_com_msg_head(p_cmd_recv_stru, u8_recv);
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////	 

//��ʼ��IO ����1 
//bound:������
void uart_init(u32 bound){
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
	USART_DeInit(USART1);  //��λ����1
	//USART1_TX   PA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9

	//USART1_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10

	//Usart1 NVIC ����

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	//USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USART1, &USART_InitStructure); //��ʼ������
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
	USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 

}
#if EN_USART1_RX   //���ʹ���˽���
void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8 res;
	if (time_sys - time_uart1 >= 100) //��ʱ100ms������ǰ����
		g_stru_cmd_recv.cmd_state = ENUM_COM_MSG_HEAD;
	time_uart1 = time_sys;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		//LED0 = 0;
		res =USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������
		process_com_data(&g_stru_cmd_recv, res);
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		
		#if 0 //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		if((USART_RX_STA&0x8000)==0)//����δ���	
		{
			if(USART_RX_STA&0x4000)//���յ���0x0d
			{
				if(res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
				else USART_RX_STA|=0x8000;	//��������� 
			}
			else //��û�յ�0X0D
			{	
				if(res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=res;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}
		#endif
	}
	#if 0
	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
	{
		USART_ClearITPendingBit(USART1, USART_IT_TXE);           /* Clear the USART transmit interrupt */
	}
	#endif
} 
#endif	
void USART1SendByte(unsigned char SendData)
{	   
	USART_SendData(USART1,SendData);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void RS485SendNByte(uint8_t *send_buf, uint16_t send_len)
{	
	delay_ms(time_sys%5*2*MAX_SERIAL_BUFFER_LENGHT*1000/(RS485_BaudRate/8));//�����ʱ����ֹ���߳�ͻ
	while(send_len--)
	{
		USART_SendData(USART1,*send_buf++);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	}
}

void USART1SendString(u8 *cmd,u16 len)
{
	u16 i=0;
	while((*(cmd+i)!=0)&&(i<len))
	{
		USART1SendByte(*(cmd+i));i++;
	}
}

//////////////////////////////////////////////////////////////////////////////////
static void init_RS485_addr(uint8_t * rs485_addr)
{
	#ifdef SIM800C_BOARD
	u8 i = 0;
	*rs485_addr = 0xF0;
	#endif

	#ifdef RC522_BOARD //��ȡ���뿪�ص�ֵ
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	*rs485_addr = (RS485_ADDR_HBIT<<1 + RS485_ADDR_LBIT);
	#endif
}

void RS485_init(u32 bound){
	//get_chipID(g_chipID);	
	init_RS485_addr(&g_rs485_addr);
	uart_init(bound);//����1��ʼ��
}
