#include "sim800c.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h"   	 
#include "key.h"	 	 	 	 	 
#include "string.h"    
#include "usart2.h"


u8 Scan_Wtime = 0;//保存扫描需要的时间
u8 BT_Scan_mode=0;//蓝牙扫描设备模式标志

struct sim_recv_stru g_stru_sim_recv = {0};

//usmart支持部分 
//将收到的AT指令应答数据返回给电脑串口
//mode:0,不清零USART2_RX_STA;
//     1,清零USART2_RX_STA;
void sim_at_response(u8 mode)
{
	u16 len = 0;
	if(USART2_RX_STA&0X8000)		//接收到一次数据了
	{
		len = USART2_RX_STA&0X7FFF;
		USART2_RX_BUF[len]=0;//添加结束符
		if(mode)USART2_RX_STA=0;
		//memcpy(recv_data, USART2_RX_BUF, len);
		
		memcpy(&g_stru_sim_recv.function_code, USART2_RX_BUF, 2);
		switch(g_stru_sim_recv.function_code){
			case SIM800C_CMD_UNLOCK:
				break;
			case SIM800C_CMD_REPROT_DEVICE_INFO:
				break;
			case SIM800C_CMD_HEARTBEAT:
				break;
			default:
				break;
		}
		#if 1
		
		#else
		USART2SendString(USART2_RX_BUF, len);
		#endif
		
	}
}
//////////////////////////////////////////////////////////////////////////////////
//ATK-SIM800C 各项测试(拨号测试、短信测试、GPRS测试、蓝牙测试)共用代码

//sim800C发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//其他,期待应答结果的位置(str的位置)
u8* sim800c_check_cmd(u8 *str)
{
	char *strx=0;
	if(USART2_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;//添加结束符
		strx=strstr((const char*)USART2_RX_BUF,(const char*)str);
		USART1SendString(USART2_RX_BUF, USART2_RX_STA&0X7FFF);
	} 
	return (u8*)strx;
}
//向sim800C发送命令
//cmd:发送的命令字符串(不需要添加回车了),当cmd<0XFF的时候,发送数字(比如发送0X1A),大于的时候发送字符串.
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 sim800c_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART2_RX_STA=0;
	if((u32)cmd<=0XFF)
	{
		while(DMA1_Channel7->CNDTR!=0);	//等待通道7传输完成   
		USART2->DR=(u32)cmd;
	}else u2_printf("%s\r\n",cmd);//发送命令
	
	if(waittime==1100)//11s后读回串口数据(蓝牙扫描模式)
	{
		 Scan_Wtime = 11;  //需要定时的时间
		 TIM4_SetARR(9999);//产生1S定时中断
	}
	
	
	if(ack&&waittime)		//需要等待应答
	{ 
	   while(--waittime)	//等待倒计时
	   {
		   if(BT_Scan_mode)//蓝牙扫描模式
		   {
			   res=KEY_Scan(0);//返回上一级
			   if(res==WKUP_PRES)return 2;
		   }
		   delay_ms(10);
		   if(USART2_RX_STA&0X8000)//接收到期待的应答结果
		   {
			   if(sim800c_check_cmd(ack))break;//得到有效数据 
			   USART2_RX_STA=0;
		   } 
	   }
	   if(waittime==0)res=1; 
	}
	return res;
} 

//接收SIM800C返回数据（蓝牙测试模式下使用）
//request:期待接收命令字符串
//waittimg:等待时间(单位：10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 sim800c_wait_request(u8 *request ,u16 waittime)
{
	 u8 res = 1;
	 u8 key;
	 if(request && waittime)
	 {
		while(--waittime)
		{   
		   key=KEY_Scan(0);
		   if(key==WKUP_PRES) return 2;//返回上一级
		   delay_ms(10);
		   if(USART2_RX_STA &0x8000)//接收到期待的应答结果
		   {
			  if(sim800c_check_cmd(request)) break;//得到有效数据
			  USART2_RX_STA=0;
		   }
		}
		if(waittime==0)res=0;
	 }
	 return res;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//GPRS测试部分代码

const u8 *modetbl[2]={"TCP","UDP"};//连接模式

//tcp/udp测试
//带心跳功能,以维持连接
//mode:0:TCP测试;1,UDP测试)
//ipaddr:ip地址
//port:端口 
void sim800c_tcpudp_test(u8 mode, const u8* ipaddr, const u8* port)
{ 
	u8 i=0;
	u8 p[100] = {0};
	u8 p1[100] = {0};
	
	u8 *p2,*p3;

	u16 timex=0;
	u8 count=0;
	
	u8 connectsta=0;			//0,正在连接;1,连接成功;2,连接关闭; 
	u8 hbeaterrcnt=0;			//心跳错误计数器,连续5次心跳信号无应答,则重新连接

	sprintf((char*)p,"IP地址:%s 端口:%s",ipaddr,port);

	USART2_RX_STA=0;
	sprintf((char*)p,"AT+CIPSTART=\"%s\",\"%s\",\"%s\"",modetbl[mode],ipaddr,port);
	if(sim800c_send_cmd(p,"OK",500))return;		//发起连接
	while(1)
	{ 

		if(connectsta==1 && sim800c_send_cmd("AT+CIPSEND",">",500)==0)		//发送数据
		{ 
				//printf("CIPSEND DATA:%s\r\n",p1);	 			//发送数据打印到串口
			u2_printf("%s\r\n",p1);
			delay_ms(50);
			sim800c_send_cmd((u8*)0X1A,"SEND OK",1000);//最长等待10s
			delay_ms(500); 
		}else sim800c_send_cmd((u8*)0X1B,0,0);	//ESC,取消发送 

		if((timex%20)==0)
		{
			//LED0=!LED0;
			count++;	
			if(connectsta==2||hbeaterrcnt>8)//连接中断了,或者连续8次心跳没有正确发送成功,则重新连接
			{
				sim800c_send_cmd("AT+CIPCLOSE=1","CLOSE OK",500);	//关闭连接
				sim800c_send_cmd("AT+CIPSHUT","SHUT OK",500);		//关闭移动场景 
				sim800c_send_cmd(p,"OK",500);						//尝试重新连接
				connectsta=0;	
 				hbeaterrcnt=0;
			}
			sprintf((char*)p1,"ATK-SIM800C %s测试 %d  ",modetbl[mode],count);
		}
		if(connectsta==0&&(timex%200)==0)//连接还没建立的时候,每2秒查询一次CIPSTATUS.
		{
			sim800c_send_cmd("AT+CIPSTATUS","OK",500);	//查询连接状态
			if(strstr((const char*)USART2_RX_BUF,"CLOSED"))connectsta=2;
			if(strstr((const char*)USART2_RX_BUF,"CONNECT OK"))connectsta=1;
		}
		if(connectsta==1&&timex>=600)//连接正常的时候,每6秒发送一次心跳
		{
			timex=0;
			if(sim800c_send_cmd("AT+CIPSEND",">",200)==0)//发送数据
			{
				sim800c_send_cmd((u8*)0X00,0,0);	//发送数据:0X00  
				delay_ms(40);						//必须加延时
				sim800c_send_cmd((u8*)0X1A,0,0);	//CTRL+Z,结束数据发送,启动一次传输	
			}else sim800c_send_cmd((u8*)0X1B,0,0);	//ESC,取消发送 		
				
			hbeaterrcnt++; 
			printf("hbeaterrcnt:%d\r\n",hbeaterrcnt);//方便调试代码
		} 
		delay_ms(10);
		if(USART2_RX_STA&0X8000)		//接收到一次数据了
		{ 
			USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;	//添加结束符 
			printf("%s",USART2_RX_BUF);				//发送到串口  
			if(hbeaterrcnt)							//需要检测心跳应答
			{
				if(strstr((const char*)USART2_RX_BUF,"SEND OK"))hbeaterrcnt=0;//心跳正常
			}				
			p2=(u8*)strstr((const char*)USART2_RX_BUF,"+IPD");
			if(p2)//接收到TCP/UDP数据
			{
				p3=(u8*)strstr((const char*)p2,",");
				p2=(u8*)strstr((const char*)p2,":");
				p2[0]=0;//加入结束符
				sprintf((char*)p1,"收到%s字节,内容如下",p3+1);//接收到的字节数
			}
			USART2_RX_STA=0;
		}

		timex++; 
		i++;
		if(i>10){i=0; u2_printf("AT\r\n");}//必须加上这句
	} 
}

//sim800C GPRS测试
//用于测试TCP/UDP连接
//返回值:0,正常
//其他,错误代码
u8 sim800c_gprs_test(void)
{
	const u8 *url = "208l8w1838.51mypc.cn";
	const u8 *port = "48438";
	
	u8 mode=0;				//0,TCP连接;1,UDP连接
	u8 timex=1; 
	
 	sim800c_send_cmd("AT+CIPCLOSE=1","CLOSE OK",100);	//关闭连接
	sim800c_send_cmd("AT+CIPSHUT","SHUT OK",100);		//关闭移动场景 
	if(sim800c_send_cmd("AT+CGCLASS=\"B\"","OK",1000))return 1;				//设置GPRS移动台类别为B,支持包交换和数据交换 
	if(sim800c_send_cmd("AT+CGDCONT=1,\"IP\",\"CMNET\"","OK",1000))return 2;//设置PDP上下文,互联网接协议,接入点等信息
	if(sim800c_send_cmd("AT+CGATT=1","OK",500))return 3;					//附着GPRS业务
	if(sim800c_send_cmd("AT+CIPCSGP=1,\"CMNET\"","OK",500))return 4;	 	//设置为GPRS连接模式
	if(sim800c_send_cmd("AT+CIPHEAD=1","OK",500))return 5;	 				//设置接收数据显示IP头(方便判断数据来源)
	
	while(timex && timex%19)
	{
		sim800c_tcpudp_test(mode,url,port);
		timex++;
		if(timex==20)
		{
			timex=0;
			//LED0=!LED0;
		}
		delay_ms(10);
		sim_at_response(1);//检查GSM模块发送过来的数据,及时上传给电脑
	}
	return 0;
}


/////////////////////////////////////////////////////////////////
//蓝牙测试部分代码

///////////////////////////////////////////////////////////////////// 

//ATK-SIM800C GSM/GPRS主测试控制部分
//测试界面主UI
void sim800c_mtest_ui(u16 x,u16 y)
{
	u8 p[50] = {0};
	u8 *p1,*p2;
	
	USART2_RX_STA=0;
	if(sim800c_send_cmd("AT+CGMI","OK",200)==0)				//查询制造商名称
	{ 
		p1=(u8*)strstr((const char*)(USART2_RX_BUF+2),"\r\n");
		p1[0]=0;//加入结束符
		sprintf((char*)p,"制造商:%s",USART2_RX_BUF+2);
		USART2_RX_STA=0;		
	} 
	if(sim800c_send_cmd("AT+CGMM","OK",200)==0)//查询模块名字
	{ 
		p1=(u8*)strstr((const char*)(USART2_RX_BUF+2),"\r\n"); 
		p1[0]=0;//加入结束符
		sprintf((char*)p,"模块型号:%s",USART2_RX_BUF+2);
		USART2_RX_STA=0;		
	} 
	if(sim800c_send_cmd("AT+CGSN","OK",200)==0)//查询产品序列号
	{ 
		p1=(u8*)strstr((const char*)(USART2_RX_BUF+2),"\r\n");//查找回车
		p1[0]=0;//加入结束符 
		sprintf((char*)p,"序列号:%s",USART2_RX_BUF+2);
		USART2_RX_STA=0;		
	}
	if(sim800c_send_cmd("AT+CNUM","+CNUM",200)==0)			//查询本机号码
	{ 
		p1=(u8*)strstr((const char*)(USART2_RX_BUF),",");
		p2=(u8*)strstr((const char*)(p1+2),"\"");
		p2[0]=0;//加入结束符
		sprintf((char*)p,"本机号码:%s",p1+2);
		USART2_RX_STA=0;		
	}
}

//GSM信息显示(信号质量,电池电量,日期时间)
//返回值:0,正常
//其他,错误代码
u8 sim800c_gsminfo_show(u16 x,u16 y)
{
	u8 p[50] = {0};
	u8 *p1, *p2;
	u8 res=0;

	USART2_RX_STA=0;
	if(sim800c_send_cmd("AT+CPIN?","OK",200))res|=1<<0;	//查询SIM卡是否在位 
	USART2_RX_STA=0;  
	if(sim800c_send_cmd("AT+COPS?","OK",200)==0)		//查询运营商名字
	{ 
		p1=(u8*)strstr((const char*)(USART2_RX_BUF),"\""); 
		if(p1)//有有效数据
		{
			p2=(u8*)strstr((const char*)(p1+1),"\"");
			p2[0]=0;//加入结束符			
			sprintf((char*)p,"运营商:%s",p1+1);
		} 
		USART2_RX_STA=0;		
	}else res|=1<<1;
	if(sim800c_send_cmd("AT+CSQ","+CSQ:",200)==0)		//查询信号质量
	{ 
		p1=(u8*)strstr((const char*)(USART2_RX_BUF),":");
		p2=(u8*)strstr((const char*)(p1),",");
		p2[0]=0;//加入结束符
		sprintf((char*)p,"信号质量:%s",p1+2);
		USART2_RX_STA=0;		
	}else res|=1<<2;
	if(sim800c_send_cmd("AT+CBC","+CBC:",200)==0)		//查询电池电量
	{ 
		p1=(u8*)strstr((const char*)(USART2_RX_BUF),",");
		p2=(u8*)strstr((const char*)(p1+1),",");
		p2[0]=0;p2[5]=0;//加入结束符
		sprintf((char*)p,"电池电量:%s%%  %smV",p1+1,p2+1);
		USART2_RX_STA=0;		
	}else res|=1<<3; 
	if(sim800c_send_cmd("AT+CCLK?","+CCLK:",200)==0)		//查询电池电量
	{ 
		p1=(u8*)strstr((const char*)(USART2_RX_BUF),"\"");
		p2=(u8*)strstr((const char*)(p1+1),":");
		p2[3]=0;//加入结束符
		sprintf((char*)p,"日期时间:%s",p1+1);
		USART2_RX_STA=0;		
	}else res|=1<<4; 

	return res;
} 


//NTP网络同步时间
void ntp_update(void)
{  
	 sim800c_send_cmd("AT+SAPBR=3,1,\"Contype\",\"GPRS\"","OK",200);//配置承载场景1
	 sim800c_send_cmd("AT+SAPBR=3,1,\"APN\",\"CMNET\"","OK",200);
	 sim800c_send_cmd("AT+SAPBR=1,1",0,200);                        //激活一个GPRS上下文
     delay_ms(5);
     sim800c_send_cmd("AT+CNTPCID=1","OK",200);                     //设置CNTP使用的CID
	 sim800c_send_cmd("AT+CNTP=\"202.120.2.101\",32","OK",200);     //设置NTP服务器和本地时区(32时区 时间最准确)
     sim800c_send_cmd("AT+CNTP","+CNTP: 1",600);                    //同步网络时间
}

//sim800C主测试程序
void sim800c_test(void)
{
	u8 timex=0;
	u8 sim_ready=0;
	
	delay_ms(50);
	while(sim800c_send_cmd("AT","OK",100))//检测是否应答AT指令 
	{
		delay_ms(1200);
	} 	 
	
	sim800c_send_cmd("ATE0","OK",200);//不回显
	sim800c_mtest_ui(40,30);
	ntp_update();//网络同步时间
	while(1)
	{
		delay_ms(10);
		sim_at_response(1);//检查GSM模块发送过来的数据,及时上传给电脑
		#if 1
		if(sim_ready)//SIM卡就绪.
		{
			sim800c_gprs_test();//GPRS测试
			//sim800c_mtest_ui(40,30);
			timex=0;
		}
		if(timex==0 || sim_ready==0)		//2.5秒左右更新一次
		{
			if(sim800c_gsminfo_show(40,225)==0)sim_ready=1;
			else sim_ready=0;
		}	
		//if((timex%20)==0)LED0=!LED0;//200ms闪烁 
		timex++;
		#endif
	} 	
}


void sim800c_reset(void)
{
	//1秒高电平，然后低电平
	POWKEY = 1;
	delay_ms(1000);
	POWKEY = 0;
}

void sim800c_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PC端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				 //POWKEY-->PB.11 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.11
	sim800c_reset();
}


