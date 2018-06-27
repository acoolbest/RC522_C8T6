#ifndef __DELAY_H
#define __DELAY_H 			   
#include "sys.h"
#if 0
void delay_init(void);
void delay_ms(u16 nms);
void delay_us(u32 nus);
#endif
void SysTick_Init(void);
//void delay_us(__IO u32 nTime);
void delay_ms(__IO u32 nTime);

#endif





























