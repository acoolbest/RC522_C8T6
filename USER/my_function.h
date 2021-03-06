#ifndef __MY_FUNCTION_H
#define	__MY_FUNCTION_H

#include "stm32f10x.h"
#include "sys.h"

#define			RET_SUCCESS							0x00
#define			RET_FAIL							0x01

#define			SLAVE_ADDR_UNDEFINED				0x00
#define			SLAVE_ADDR_NEW						0x01
#define			SLAVE_ADDR_NORMAL					0x02
#define			SLAVE_ADDR_INVAILD					0x03

#define			LOCK_STATE_OFF						0x00
#define			LOCK_STATE_ON						0x01

// 短整型大小端互换
#define BigLittleSwap16(A)  ((((u16)(A) & 0xff00) >> 8) | \
                                                 (((u16)(A) & 0x00ff) << 8))
// 长整型大小端互换
#define BigLittleSwap32(A)  ((((u32)(A) & 0xff000000) >> 24) | \
                                                 (((u32)(A) & 0x00ff0000) >> 8) | \
                                                 (((u32)(A) & 0x0000ff00) << 8) | \
                                                 (((u32)(A) & 0x000000ff) << 24))
u32 HtoNl(u32 h);
u32 NtoHl(u32 n);
u16 HtoNs(u16 h);
u16 NtoHs(u16 n);

enum enum_unlock_state
{
	UNLOCK_STATE_RFID_UNDEFINE = 0x00,			//未定义
	UNLOCK_STATE_RFID_ERR,						//RFID编号不匹配
	UNLOCK_STATE_RFID_PULL_OUT,					//RFID借出状态
	UNLOCK_STATE_ERR,							//开锁失败
	UNLOCK_STATE_OK,							//开锁成功
	UNLOCK_STATE_ONGOING = 0xFF					//开锁进行中
};

enum enum_rfid_state
{
	RFID_PULL_OUT = 0,
	RFID_INSERT,
	RFID_EXIST
};

void usart_process(void);

void usart_ctrl_slave_unlock(u8 addr, u8 * rfid_id);
void unlock_timeout_increase(void);
void update_slave_addr(u8 addr, u8 addr_state);
void deal_unlock_state(u8 addr, u8 state);
u8 rs485_send_cmd(u8 *cmd, u16 len);
void rs485_process(void);

#endif
