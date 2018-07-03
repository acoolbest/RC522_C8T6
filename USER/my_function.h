#ifndef __MY_FUNCTION_H
#define	__MY_FUNCTION_H

#include "stm32f10x.h"
#include "sys.h"

#define RET_SUCCESS		0x00
#define RET_FAIL		0x01

#define SLAVE_ADDR_UNDEFINED	0x00
#define SLAVE_ADDR_NEW			0x01
#define SLAVE_ADDR_OLD			0x02

u8 rs485_send_cmd(u8 *cmd, u16 len);

#endif
