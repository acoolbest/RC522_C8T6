#include "sys.h"
#include "delay.h"
#include "led.h"
#include "usart.h"
#include "usart2.h"
#include "usart3.h"
#include "sim800c.h"
#include "rc522.h"
#include "relay.h"
#include "buzzer.h"

int main(void)
{
	uint8_t RC522_buffer[16] = {0};
	SysTick_Init();
	NVIC_Configuration();
	RS485_init(RS485_BaudRate);
	
	#ifdef SIM800C_BOARD
	sim800c_init();
	USART2_Init(115200);
	#endif
	
	#ifdef RC522_BOARD
	relay_init();						//继电器初始化
	buzzer_init();						//蜂鸣器初始化
	RC522_Init();
	#endif
	
	delay_ms(0);						//启动系统时钟

	while(1)
	{
		#ifdef SIM800C_BOARD
		sim_at_response(1);
		sim800c_test();					//GSM测试
		#endif

		#ifdef RC522_BOARD
		time_out_relay_lock();
		RC522_test();					//RC522测试
		RC522_RW(RC522_READ_TYPE, RC522_buffer);
		RC522_RW(RC522_WRITE_TYPE, RC522_buffer);
		#endif
		usart_process();
	}
}
