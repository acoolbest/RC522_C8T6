//*******************************************
//dwgl for stm32f1XX
//V1.1 20160401
//*******************************************
#include "my_function.h"

void rfid_ctrl_lock(uint8_t ctrl_type)
{
	if(ctrl_type == LOCK_STATE_ON) relay_on();
	else relay_off();

	g_lock_state = ctrl_type;
	time_lock = ctrl_type;//开启5s定时器
}

void time_out_relay_lock(void)
{
	if(time_lock >= 5000)//5s
	{
		rfid_ctrl_lock(LOCK_STATE_OFF);
		rc522_req_type = RC522_REQ_ALL;//关锁后重新读取RFID卡信息
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

void get_chipID(uint8_t * chipID)
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

#ifdef SIM800C_BOARD
void deal_cmd_data(struct cmd_recv_stru *p_cmd_recv_stru)
{
	if(p_cmd_recv_stru->cmd_recv_state == COM_CMD_RECV_COMPLETE)
	{
		u16 recv_len = p_cmd_recv_stru->cmd_length;
		u8 recv_data[MAX_SERIAL_BUFFER_LENGHT] = {0};
		memcpy(recv_data, p_cmd_recv_stru->cmd_buffer, recv_len);
		if((recv_data[0] == RECV_COM_MSG_HEAD)
			&& (recv_data[recv_len-1] == DEFAULT_COM_MSG_TAIL)
			&& (recv_data[recv_len-2] == get_checksum(recv_data, recv_len)))
		{
			switch(recv_data[4])
			{
				case COM_CMD_GET_SLAVE_ADDR://获取从机地址
					g_slave_device_info[recv_data[3]].addr = recv_data[3];
					break;
				case COM_CMD_GET_SLAVE_RFID://获取从机RFID
					if(recv_data[5])
					{
						if(g_slave_device_info[recv_data[3]].rfid_state == RFID_PULL_OUT)
						{
							g_slave_device_info[recv_data[3]].rfid_state = RFID_INSERT;
							memcpy(g_slave_device_info[recv_data[3]].rfid_id, &recv_data[6], 8);
						}
					}
					break;
				case COM_CMD_CTRL_SLAVE_UNLOCK://控制从机开锁
					g_slave_device_info[recv_data[3]].unlock_state = recv_data[5];
					break;
				case COM_CMD_GET_NEW_RFID_INFO://获取RFID归还信息
					if(recv_data[5])
					{
						g_slave_device_info[recv_data[3]].rfid_state = RFID_INSERT;
						memcpy(g_slave_device_info[recv_data[3]].rfid_id, &recv_data[6], 8);
					}
					break;
				default:
					break;		
			}
		}
		p_cmd_recv_stru->cmd_recv_state = COM_CMD_RECV_INCOMPLETE;
	}
}
#endif

#ifdef RC522_BOARD
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
						*(++p) = UNLOCK_STATE_RFID_ERR;
					else
					{
						if(g_rfid_state == RFID_PULL_OUT)
							*(++p) = UNLOCK_STATE_RFID_PULL_OUT;
						else
						{
							rfid_ctrl_lock(LOCK_STATE_ON);
							*(++p) = UNLOCK_STATE_ONGOING;
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
#endif

void usart_get_slave_addr(void)// one time per 5min
{
	static u32 time_5min = 0;
	u8 i = 0;
	static u8 send_buf[0x07] = {0x67,0x07,0xFF,0xF0,0x51,0x00,0x99};
	if(time_5min == 0 || time_sys-time_5min >= 5*60*1000)
	{
		time_5min = time_sys;
		send_buf[sizeof(send_buf)-2] = get_checksum(send_buf, send_buf[1]);
		RS485SendNByte(send_buf,send_buf[1]);
	}
}

void usart_get_slave_rfid(void)
{

}

void usart_ctrl_slave_unlock(void)
{

}

void usart_get_new_rfid_info(void)
{

}

void usart_process(void)
{
	deal_cmd_data(&g_stru_cmd_recv);
}

//将1个字符转换为16进制数字
//chr:字符,0~9/A~F/a~F
//返回值:chr对应的16进制数值
u8 sim800c_chr2hex(u8 chr)
{
	if(chr>='0'&&chr<='9')return chr-'0';
	if(chr>='A'&&chr<='F')return (chr-'A'+10);
	if(chr>='a'&&chr<='f')return (chr-'a'+10); 
	return 0;
}
//将1个16进制数字转换为字符
//hex:16进制数字,0~15;
//返回值:字符
u8 sim800c_hex2chr(u8 hex)
{
	if(hex<=9)return hex+'0';
	if(hex>=10&&hex<=15)return (hex-10+'A'); 
	return '0';
}

