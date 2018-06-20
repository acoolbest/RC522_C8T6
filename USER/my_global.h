#ifndef __MY_GLOBAL_H
#define	__MY_GLOBAL_H

#include "stm32f10x.h"
#include "sys.h"

extern u32 time_sys;
extern u32 time_uart1;
extern u32 time_lock;

#define RC522_REQ_IDL           0x26               //寻天线区内未进入休眠状态
#define RC522_REQ_ALL           0x52               //寻天线区内全部卡
extern u8 rc522_req_type;

#define LED0 PCout(13)	// PC13


#endif
