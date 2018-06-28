/*
 *支持适应不同频率下的串口波特率设置.
 *加入了对printf的支持
 *增加了串口接收命令功能.
 *修正了printf第一个字符丢失的bug
 *V1.4修改说明
 *1,修改串口初始化IO的bug
 *2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
 *3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
 *4,修改了EN_USART1_RX的使能方式
 */

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "relay.h"
#include "my_global.h"


//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  

uint8_t g_rs485_addr = 0;//485本机地址
uint8_t g_rfid_state = RFID_PULL_OUT;
uint8_t g_lock_state = LOCK_STATE_OFF;
uint8_t g_rfid_id[8] = {0};
uint8_t g_chipID[12] = {0};
struct cmd_recv_stru g_stru_cmd_recv = {0};

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*使用microLib的方法*/
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
			if(u8_recv == COM_BROADCAST_ADDR || u8_recv == g_rs485_addr || g_rs485_addr == 0)
				update_cmd_recv(p_cmd_recv_stru, u8_recv);
			else 
				is_com_msg_head(p_cmd_recv_stru, u8_recv);
			break;
		}
		case ENUM_COM_MSG_SRC_ADDR:
		{
			if(u8_recv == COM_MASTER_ADDR)
			{
				update_cmd_recv(p_cmd_recv_stru, u8_recv);
			}
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

uint8_t get_checksum(uint8_t * p_frame, uint16_t frame_len)
{
	uint8_t sum = 0;
	uint16_t i = 0;
	
	if(p_frame == NULL || frame_len == 0)
		return 0;

	for (i = 1; i < frame_len - 2; i++) sum += p_frame[i];
	return sum;
}

void rfid_ctrl_lock(uint8_t ctrl_type)
{
	if(ctrl_type == LOCK_STATE_ON) relay_on();
	else relay_off();

	g_lock_state = ctrl_type;
	time_lock = ctrl_type;//开启5s定时器
}

void time_out_relay_lock(void)
{
	if(time_lock >= 50000000)//5s
	{
		rfid_ctrl_lock(LOCK_STATE_OFF);
		rc522_req_type = RC522_REQ_ALL;//关锁后重新读取RFID卡信息
	}
}

void deal_cmd_data(struct cmd_recv_stru *p_cmd_recv_stru)
{
	if(p_cmd_recv_stru->cmd_recv_state == COM_CMD_RECV_COMPLETE)
	{
		u8 send_buf[MAX_SERIAL_BUFFER_LENGHT] = {0};
		u8 *p = send_buf;
		u8 send_flag = 1;
		u16 recv_len = p_cmd_recv_stru->cmd_length;
		u8 recv_data[MAX_SERIAL_BUFFER_LENGHT] = {0};
		memcpy(recv_data, p_cmd_recv_stru->cmd_buffer, recv_len);
		if((recv_data[0] == RECV_COM_MSG_HEAD)
			&& (recv_data[recv_len-1] == DEFAULT_COM_MSG_TAIL)
			&& (recv_data[recv_len-2] == get_checksum(recv_data, recv_len)))
		{
			*p = SEND_COM_MSG_HEAD;
			*(p += 2) = recv_data[3];
			*(++p) = g_rs485_addr;
			*(++p) = recv_data[4];
			switch(recv_data[4])
			{
				case COM_CMD_GET_SLAVE_ADDR://获取从机地址
					break;
				case COM_CMD_GET_SLAVE_RFID://获取从机RFID
					*(++p) = g_rfid_state == RFID_PULL_OUT ? 0x00:0x01;
					if(*p)
					{
						memcpy(p+1,g_rfid_id,8);
						p += 8;
					}
					break;
				case COM_CMD_CTRL_SLAVE_UNLOCK://控制从机开锁
				//XX(确认字)
					if(memcmp(&recv_data[5], g_rfid_id, 8))
						*(++p) = 0x01;//RFID编号不匹配
					else
					{
						if(g_rfid_state == RFID_PULL_OUT)
							*(++p) = 0x02;//RFID借出状态
						else
						{
							rfid_ctrl_lock(LOCK_STATE_ON);
							*(++p) = 0xFF;//RFID开锁进行中
						}
					}
					break;
				case COM_CMD_GET_NEW_RFID_INFO://获取RFID归还信息
				//XX(设备状态)  u64(64位RFID号)
					*(++p) = g_rfid_state == RFID_INSERT ? 0x01:0x00;
					if(*p)
					{
						memcpy(p+1,g_rfid_id,8);
						p += 8;
					}
					break;
				default:
					send_flag = 0;
					break;		
			}
			if(send_flag){
				*(p += 2) = DEFAULT_COM_MSG_TAIL;
				send_buf[1] = (p - send_buf + 1);
				*(p - 1) = get_checksum(send_buf, send_buf[1]);
				RS485SendNByte(send_buf,send_buf[1]);
			}
		}
		p_cmd_recv_stru->cmd_recv_state = COM_CMD_RECV_INCOMPLETE;
	}
}

//////////////////////////////////////////////////////////////////////////////////	 

//初始化IO 串口1 
//bound:波特率
void uart_init(u32 bound){
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
	USART_DeInit(USART1);  //复位串口1
	//USART1_TX   PA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9

	//USART1_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10

	//Usart1 NVIC 配置

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	//USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART1, &USART_InitStructure); //初始化串口
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
	USART_Cmd(USART1, ENABLE);                    //使能串口 

}
#if EN_USART1_RX   //如果使能了接收
void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 res;
	if (time_sys - time_uart1 >= 100) //超时100ms则丢弃当前数据
		g_stru_cmd_recv.cmd_state = ENUM_COM_MSG_HEAD;
	time_uart1 = time_sys;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		//LED0 = 0;
		res =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
		process_com_data(&g_stru_cmd_recv, res);
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		
		#if 0 //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		if((USART_RX_STA&0x8000)==0)//接收未完成	
		{
			if(USART_RX_STA&0x4000)//接收到了0x0d
			{
				if(res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x8000;	//接收完成了 
			}
			else //还没收到0X0D
			{	
				if(res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=res;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
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
	delay_ms(time_sys%5*2*MAX_SERIAL_BUFFER_LENGHT*1000/(RS485_BaudRate/8));//随机延时，防止总线冲突
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
static void get_chipID(uint8_t * chipID)
{
	u32 temp0,temp1,temp2;
	temp0 = *(__IO u32*)(0x1FFFF7E8);    //产品唯一身份标识寄存器（96位）
	temp1 = *(__IO u32*)(0x1FFFF7EC);
	temp2 = *(__IO u32*)(0x1FFFF7F0);
                                  
//ID码地址：0x1FFFF7E8 0x1FFFF7EC 0x1FFFF7F0，只需要读取这个地址中的数据就可以了。

    chipID[0] = (u8)(temp0 & 0x000000FF);
    chipID[1] = (u8)((temp0 & 0x0000FF00)>>8);
    chipID[2] = (u8)((temp0 & 0x00FF0000)>>16);
    chipID[3] = (u8)((temp0 & 0xFF000000)>>24);
    chipID[4] = (u8)(temp1 & 0x000000FF);
    chipID[5] = (u8)((temp1 & 0x0000FF00)>>8);
    chipID[6] = (u8)((temp1 & 0x00FF0000)>>16);
    chipID[7] = (u8)((temp1 & 0xFF000000)>>24);
    chipID[8] = (u8)(temp2 & 0x000000FF);
    chipID[9] = (u8)((temp2 & 0x0000FF00)>>8);
    chipID[10] = (u8)((temp2 & 0x00FF0000)>>16);
    chipID[11] = (u8)((temp2 & 0xFF000000)>>24);
}

static void init_RS485_addr(uint8_t * rs485_addr)
{
	#ifdef SIM800C_BOARD
	*rs485_addr = 0xF0;
	#endif

	#ifdef RC522_BOARD //读取拨码开关的值
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
	uart_init(bound);//串口1初始化
}

