//*******************************************
//dwgl for stm32f1XX
//V1.1 20160401
//*******************************************

#include "my_global.h"
u32 time_sys = 0;
u32 time_uart1 = 0;
u32 time_lock = 0;
u8 rc522_req_type = RC522_REQ_ALL;
#ifdef SIM800C_BOARD
struct slave_device_info g_slave_device_info[COM_MAX_SLAVE_ADDR+1] = {0};
#endif


