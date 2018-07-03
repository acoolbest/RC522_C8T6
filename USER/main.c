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
	relay_init();						//�̵�����ʼ��
	buzzer_init();						//��������ʼ��
	RC522_Init();
	delay_ms(0);						//����ϵͳʱ��
	while(1)
	{
		time_out_relay_lock();
		RC522_test();					//RC522����
		RC522_RW(RC522_READ_TYPE, RC522_buffer);
		RC522_RW(RC522_WRITE_TYPE, RC522_buffer);
		usart_process();
	}
}
#endif

#ifdef SIM800C_BOARD
#include "usart2.h"
#include "sim800c.h"

void rs485_process(void)
{
	
	usart_get_slave_addr();

	
	usart_get_slave_rfid();

	
	usart_ctrl_slave_unlock();

	
	usart_get_new_rfid_info();
}

void sim800c_process(void)
{

}

int main(void)
{
	SysTick_Init();
	NVIC_Configuration();
	RS485_init(RS485_BaudRate);
	sim800c_init();
	USART2_Init(115200);				//sim800c���ƽӿڳ�ʼ��
	delay_ms(0);						//����ϵͳʱ��
	while(1)
	{
		sim_at_response(1);
		sim800c_test(); 				//GSM����
		sim800c_process();
		rs485_process();
	}
}
#endif
