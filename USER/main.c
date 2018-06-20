#if 0
#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lcd.h"
#include "key.h"
#include "usmart.h" 
#include "malloc.h"  
#include "MMC_SD.h" 
#include "ff.h"  
#include "exfuns.h"
#include "fontupd.h"
#include "text.h"	
#include "sim800c.h"	
#include "touch.h"	
#include "usart2.h"	
#else
#include "sys.h"
#include "delay.h"
#include "led.h"
#include "usart.h"
#include "usart2.h"
#include "usart3.h"
#include "sim800c.h"
#include "rc522.h"
#include "relay.h"
#endif
/************************************************
 ALIENTEK Mini STM32开发板 扩展实验17
 ATK-SIM800C GSM/GPRS模块测试实验  
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/
 #if 0
 int main(void)
 { 
	u8 key,fontok=0; 
    NVIC_Configuration();	 
	delay_init();	    	         //延时函数初始化	  
	uart_init(115200);	 	         //串口初始化为9600	
    usmart_dev.init(72);		     //初始化USMART		 
	LCD_Init();				         //初始化液晶 
	LED_Init();         	         //LED初始化	 
	KEY_Init();				         //按键初始化	  													    
 	USART2_Init(115200);	         //初始化串口2 
	tp_dev.init();			         //触摸屏初始化
 	mem_init();				         //初始化内存池	    
 	exfuns_init();			         //为fatfs相关变量申请内存  
    f_mount(fs[0],"0:",1); 	         //挂载SD卡 
 	f_mount(fs[1],"1:",1); 	         //挂载FLASH.
	key=KEY_Scan(0);  
	if(key==KEY0_PRES)		         //强制校准
	{
	   LCD_Clear(WHITE);	         //清屏
	   tp_dev.adjust();  	         //屏幕校准  
	   LCD_Clear(WHITE);	         //清屏
	}
	fontok=font_init();		         //检查字库是否OK
	if(fontok||key==KEY1_PRES)       //需要更新字库（字库不存在/KEY1按下）			 
	{
		LCD_Clear(WHITE);		   	 //清屏
 		POINT_COLOR=RED;			 //设置字体为红色	   	   	  
		LCD_ShowString(60,50,200,16,16,"ALIENTEK STM32");
		while(SD_Initialize())		 //检测SD卡
		{
			LCD_ShowString(60,70,200,16,16,"SD Card Failed!");
			delay_ms(200);
			LCD_Fill(60,70,200+60,70+16,WHITE);
			delay_ms(200);		    
		}								 						    
		LCD_ShowString(60,70,200,16,16,"SD Card OK");
		LCD_ShowString(60,90,200,16,16,"Font Updating...");
		key=update_font(20,110,16); //更新字库
		while(key)                  //更新失败		
		{			 		  
			LCD_ShowString(60,110,200,16,16,"Font Update Failed!");
			delay_ms(200);
			LCD_Fill(20,110,200+20,110+16,WHITE);
			delay_ms(200);		       
		} 		  
		LCD_ShowString(60,110,200,16,16,"Font Update Success!");
		delay_ms(1500);	
		LCD_Clear(WHITE);           //清屏	       
	}  
	sim800c_test();                 //GSM测试
}

#else

int main(void)
{
	uint8_t RC522_buffer[16] = {0};
	SysTick_Init();
	LED_Init();
	NVIC_Configuration();
	//uart_init(115200);
	RS485_init(RS485_BaudRate);
	relay_init();						//继电器初始化
	RC522_Init();
 	USART2_Init(115200);
 	USART3_Init(9600);
	delay_ms(0);						//启动系统时钟
	
	while(1)
	{
		time_out_relay_lock();
		//sim_at_response(1);
		sim800c_test();					//GSM测试
		RC522_test();					//GSM测试
		RC522_RW(RC522_READ_TYPE, RC522_buffer);
		RC522_RW(RC522_WRITE_TYPE, RC522_buffer);
	}
}
#endif
