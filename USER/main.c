#include "sys.h"
#include "delay.h"
#include "usart.h"

#ifdef RC522_BOARD
#include "rc522.h"
#include "relay.h"
#include "buzzer.h"

int main(void)
{
	uint8_t RC522_buffer[16] = {0};
	SysTick_Init();
	NVIC_Configuration();
	RS485_init(RS485_BaudRate);
	relay_init();						//继电器初始化
	buzzer_init();						//蜂鸣器初始化
	RC522_Init();
	delay_ms(0);						//启动系统时钟
	while(1)
	{
		time_out_relay_lock();
		RC522_test();					//RC522测试
		RC522_RW(RC522_READ_TYPE, RC522_buffer);
		RC522_RW(RC522_WRITE_TYPE, RC522_buffer);
		usart_process();
	}
}
#endif

#ifdef SIM800C_BOARD
#include "usart2.h"
#include "sim800c.h"
#include "my_function.h"

int main(void)
{
	SysTick_Init();
	NVIC_Configuration();
	RS485_init(RS485_BaudRate);
	sim800c_init();
	USART2_Init(115200);				//sim800c控制接口初始化
	delay_ms(0);						//启动系统时钟
	while(1)
	{
		sim800c_process();
		rs485_process();
	}
}
#endif
