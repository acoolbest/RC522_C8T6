#ifndef __MY_GLOBAL_H
#define	__MY_GLOBAL_H

#include "stm32f10x.h"
#include "sys.h"

extern u32 time_sys;
extern u32 time_uart1;
extern u32 time_lock;
extern u32 rs485_broadcast_timeout;
extern u32 rs485_read_timeout;



#define			RC522_REQ_IDL           0x26               //寻天线区内未进入休眠状态
#define 		RC522_REQ_ALL           0x52               //寻天线区内全部卡
extern u8 rc522_req_type;

struct slave_device_info{
	u8 addr;
	u8 addr_state;
	u8 rfid_state;
	u8 unlock_state;
	u32 unlock_timeout;
	u8 rfid_id[8];
};

#define			COM_MAX_SLAVE_ADDR		0x1F

extern struct slave_device_info g_slave_device_info[COM_MAX_SLAVE_ADDR+1];

#define LED0 PCout(13)	// PC13


#endif
