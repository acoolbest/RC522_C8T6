#include "sys.h"
#include "delay.h"
#include "usart.h"

#ifdef RC522_BOARD
#include "rc522.h"
#include "relay.h"
#include "buzzer.h"
#include "my_function.h"

int main(void)
{
	SysTick_Init();
	NVIC_Configuration();
	RS485_init(RS485_BaudRate);
	relay_init();						//�̵�����ʼ��
	buzzer_init();						//��������ʼ��
	RC522_Init();
	delay_ms(0);						//����ϵͳʱ��
	while(1)
	{
		rs485_process();
		rc522_process();
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
	USART2_Init(115200);				//sim800c���ƽӿڳ�ʼ��
	sim800c_init();
	delay_ms(0);						//����ϵͳʱ��
	while(1)
	{
		rs485_process();
		sim800c_process();
	}
}
#endif
