#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h"

#if 0
#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记
#endif

#define RS485_BaudRate			(9600)	//RS485波特率

/*
 * PB12、PB13 上拉输入，根据其电平高低来决定地址，2个IO能实现的地址为00、01、10、11
 */
#define RS485_ADDR_HBIT  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)
#define RS485_ADDR_LBIT  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)

#ifdef SIM800C_BOARD
#define			RECV_COM_MSG_HEAD					0x68
#define			SEND_COM_MSG_HEAD					0x67
#endif

#ifdef RC522_BOARD
#define			RECV_COM_MSG_HEAD					0x67
#define			SEND_COM_MSG_HEAD					0x68
#endif

#define			DEFAULT_COM_MSG_TAIL				0x99

#define			COM_MASTER_ADDR						0xF0
#define			COM_BROADCAST_ADDR					0xFF

#define			COM_CMD_GET_SLAVE_ADDR				0x51
#define			COM_CMD_SET_SLAVE_ADDR				0x52
#define			COM_CMD_GET_SLAVE_RFID				0x53
#define			COM_CMD_CTRL_SLAVE_UNLOCK			0x54
#define			COM_CMD_GET_NEW_RFID_INFO			0x55

#define			MAX_SERIAL_BUFFER_LENGHT            (16*2)

#define			COM_CMD_RECV_INCOMPLETE				0x00
#define			COM_CMD_RECV_COMPLETE				0x01

struct cmd_recv_stru
{
	u8 cmd_buffer[MAX_SERIAL_BUFFER_LENGHT];
	u16 cmd_length;
	u16 cmd_index;
	u8 cmd_state;
	u8 cmd_recv_state;
};
extern struct cmd_recv_stru g_stru_cmd_recv;

enum enum_com_msg_state
{
	ENUM_COM_MSG_HEAD = 0,		//0x67, 0x68
	ENUM_COM_MSG_LEN,
	ENUM_COM_MSG_DST_ADDR,		//0x00~0x0F; 0xF0
	ENUM_COM_MSG_SRC_ADDR,		//0xF0; 0x00~0x0F;
	ENUM_COM_MSG_FUNCTION_CODE,	//0x51~0x55
	ENUM_SPP_MSG_PAYLOAD,
	ENUM_SPP_MSG_CHECKSUM,
	ENUM_COM_MSG_TAIL			//0x99
};

extern u8 g_rs485_addr;//485本机地址



void RS485_init(u32 bound);
void RS485SendNByte(u8 *send_buf, u16 send_len, u32 delay_time);
void USART1SendString(u8 *cmd,u16 len);

#endif


