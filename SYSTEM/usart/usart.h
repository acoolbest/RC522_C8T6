#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h"
/*
 * ֧����Ӧ��ͬƵ���µĴ��ڲ���������.
 * �����˶�printf��֧��
 * �����˴��ڽ��������.
 * ������printf��һ���ַ���ʧ��bug
 * V1.4�޸�˵��
 * 1,�޸Ĵ��ڳ�ʼ��IO��bug
 * 2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
 * 3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
 * 4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
 */
 
#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����

extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���

#define RS485_BaudRate			(9600)	//RS485������


/*
 * PB12��PB13 �������룬�������ƽ�ߵ���������ַ��2��IO��ʵ�ֵĵ�ַΪ00��01��10��11
 */
#define RS485_ADDR_HBIT  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)
#define RS485_ADDR_LBIT  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)

#define			RECV_COM_MSG_HEAD					0x67
#define			SEND_COM_MSG_HEAD					0x68
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

#define			LOCK_STATE_OFF						0x00
#define			LOCK_STATE_ON						0x01

struct cmd_recv_stru
{
	u8 cmd_buffer[MAX_SERIAL_BUFFER_LENGHT];
	u16 cmd_length;
	u16 cmd_index;
	u8 cmd_state;
	u8 cmd_recv_state;
};

struct com_send_stru
{
	u8 cmd_type;
	u8 dst_addr;
	u8 confirm_flag;
	u8 rfid_state;
	u8 rfid_id[8];
};

enum enum_rfid_state
{
	RFID_PULL_OUT = 0,
	RFID_INSERT,
	RFID_EXIST
};


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

void uart_init(u32 bound);
void USART1SendString(u8 *cmd,u16 len);
void RS485SendNByte(uint8_t *send_buf, uint16_t send_len);

void RS485_init(u32 bound);
void time_out_relay_lock(void);

#endif


