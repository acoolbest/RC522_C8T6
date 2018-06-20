//*******************************************
//dwgl for stm32f1XX
//V1.1 20160401
//*******************************************
#include "stm32f10x.h"
#include "my_global.h"
u32 time_sys = 0;
u32 time_uart1 = 0;
u32 time_lock = 0;
u8 rc522_req_type = RC522_REQ_ALL;


