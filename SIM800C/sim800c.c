#include <stdlib.h>
#include "sim800c.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h"   	 
#include "key.h"	 	 	 	 	 
#include "string.h"    
#include "usart2.h"
#include "my_function.h"
#include "my_global.h"

u16 g_send_msg_id = 0;
u16 g_recv_unlock_msg_id[COM_MAX_SLAVE_ADDR+1] = {0};

u8 g_terminal_id[32] = {0};
u16 g_terminal_len = 0;

u8 g_sim800c_ready_state = SIM800C_NOT_READY;
u8 g_server_connect_state = SERVER_CLOSED;
u8 g_sim800c_csq = 0;
u8 g_heartbeat_err_count = 0;


const u8 *modetbl[2]={"TCP","UDP"};//连接模式
const u8 g_url[] = "208l8w1838.51mypc.cn";
const u8 g_port[] = "48438";

//////////////////////////////////////////////////////////////////////////////////
//将1个字符转换为16进制数字
//chr:字符,0~9/A~F/a~F
//返回值:chr对应的16进制数值
u8 sim800c_chr2hex(u8 chr)
{
	if(chr>='0'&&chr<='9')return chr-'0';
	if(chr>='A'&&chr<='F')return (chr-'A'+10);
	if(chr>='a'&&chr<='f')return (chr-'a'+10); 
	return 0;
}
//将1个16进制数字转换为字符
//hex:16进制数字,0~15;
//返回值:字符
u8 sim800c_hex2chr(u8 hex)
{
	if(hex<=9)return hex+'0';
	if(hex>=10&&hex<=15)return (hex-10+'A'); 
	return '0';
}

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
		//USART1SendString(USART2_RX_BUF, USART2_RX_STA&0X7FFF);
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
	sim800c_recv_unlock_data();//每次发送数据前先接收一次数据，防止漏数据
	USART2_RX_STA=0;
	if((u32)cmd<=0XFF)
	{
		while(DMA1_Channel7->CNDTR!=0);	//等待通道7传输完成   
		USART2->DR=(u32)cmd;
	}else u2_printf("%s\r\n",cmd);//发送命令
	
	if(ack&&waittime)		//需要等待应答
	{ 
	   while(--waittime)	//等待倒计时
	   {
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

/////////////////////////////////////////////////////////////////////////////////////////////////////
//GPRS测试部分代码

#if 0
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

void sim_at_response(u8 mode)
{	
	if(USART2_RX_STA&0X8000)		//接收到一次数据了
	{
		USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;//添加结束符
		if(mode)USART2_RX_STA=0;
		USART1SendString(USART2_RX_BUF, USART2_RX_STA&0X7FFF);
	}
}
#endif

void sim800c_detect(void)
{
	u8 res = 0;
	static u32 time_10s = 0;
	static u32 time_5s = 0;
	u8 p[50] = {0};
	u8 *p1, *p2;
	if(g_sim800c_ready_state == SIM800C_NOT_READY || time_10s == 0 || time_sys-time_10s >= 10*1000)
	{
		time_10s = time_sys;
		if(sim800c_send_cmd("AT+CPIN?","OK",200)) res|=1<<2;	//查询SIM卡是否在位 
		if(sim800c_send_cmd("AT+CSQ","+CSQ:",200)==0)			//查询信号质量
		{ 
			p1=(u8*)strstr((const char*)(USART2_RX_BUF),":");
			p2=(u8*)strstr((const char*)(p1),",");
			p2[0]=0;//加入结束符
			g_sim800c_csq = atoi((const char *)(p1+2));
			USART2_RX_STA=0;
		}else res|=1<<3;
		if(res)
		{
			g_sim800c_ready_state = SIM800C_NOT_READY;
			g_server_connect_state = SERVER_CLOSED;
		}
		else g_sim800c_ready_state = SIM800C_READY;
	}

	if(g_sim800c_ready_state != SIM800C_READY) return;

	if(g_server_connect_state == SERVER_CLOSED || g_heartbeat_err_count > 3)//连接中断了,或者连续3次心跳没有正确发送成功,则重新连接
	{
		printf("TCP RESET connect_state[%d], heartbeat_err[%d]\n",g_server_connect_state,g_heartbeat_err_count);
		sim800c_send_cmd("AT+CIPCLOSE=1","CLOSE OK",500);	//关闭连接
		sim800c_send_cmd("AT+CIPSHUT","SHUT OK",500);		//关闭移动场景
		sprintf((char*)p,"AT+CIPSTART=\"%s\",\"%s\",\"%s\"",modetbl[0],g_url,g_port);
		sim800c_send_cmd(p,"OK",500);						//尝试重新连接
		g_server_connect_state = SERVER_CONNECTING;
		g_heartbeat_err_count = 0;
	}

	if(g_server_connect_state == SERVER_CONNECTING && (time_5s == 0 || time_sys-time_5s >= 5*1000))
	{
		time_5s = time_sys;
		sim800c_send_cmd("AT+CIPSTATUS","OK",500);	//查询连接状态
		if(strstr((const char*)USART2_RX_BUF,"CLOSED"))
		{
			g_server_connect_state = SERVER_CLOSED;
			printf("TCP CONNECTING CLOSED\n");
		}
			
		if(strstr((const char*)USART2_RX_BUF,"CONNECT OK"))
		{
			g_server_connect_state = SERVER_CONNECTED;
			printf("TCP CONNECT OK\n");
		}			
	}
	if(g_server_connect_state == SERVER_CONNECTED && USART2_RX_STA&0X8000)
	{
		USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;//添加结束符
		if(strstr((const char*)USART2_RX_BUF,"CLOSED"))
		{
			g_server_connect_state = SERVER_CLOSED;
			printf("TCP CONNECTED CLOSED\n");
		}
	}
}

u16 sprintf_recv_tcp_data(u8 *send_buf, u8* recv_buf)
{
	// if unlock, return 0
	u16 * p16 = (u16 *)send_buf;
	u16 function_code = HtoNs(*p16++);
	u16 send_len = HtoNs(*p16++);
	u16 recv_len = 0;
	switch (function_code)
	{
		case SIM800C_CMD_REPROT_DEVICE_INFO:
			memcpy(recv_buf, send_buf, send_len);
			p16 = (u16 *)recv_buf;
			*(++p16) = HtoNs(send_len+1);
			recv_len = send_len+1+4;
			return recv_len;
		case SIM800C_CMD_HEARTBEAT:
			memcpy(recv_buf, send_buf, send_len);
			p16 = (u16 *)recv_buf;
			*(++p16) = HtoNs(2+1);
			recv_len = 2+2+2+1;
			return recv_len;
		case SIM800C_CMD_UNLOCK:
		default:
			return 0;
	}
}

u8 sim800c_send_tcp_data(u8 *data, u16 data_len)
{
	u8 ret = RET_FAIL;
	u16 waittime = 3000;//3000ms
	u8 buf[32] = {0};
	u16 sprintf_len = 0;
	u8 recv_buf[256] = {0};
	u16 recv_len = 0;
	u8 ack_state = 0;
	char * p = NULL;
	
	sim800c_recv_unlock_data();//每次发送数据前先接收一次数据，防止漏数据

	sprintf_len = sprintf_recv_tcp_data(data, recv_buf);
	
	sprintf((char *)buf, "AT+CIPSEND=%d", data_len);
	if(sim800c_send_cmd(buf,">",500)==0)
	{
		delay_ms(50);
		USART2_RX_STA=0;
		USART2SendNByte(data,data_len);
		while(--waittime && ret)//等待倒计时
		{
			if(USART2_RX_STA&0X8000)//接收到期待的应答结果
			{
				recv_len = USART2_RX_STA&0X7FFF;
				if(ack_state == 0)
				{
					USART2_RX_BUF[recv_len]=0;//添加结束符
					p = strstr((const char*)USART2_RX_BUF,"SEND OK");
					if(p){
						ack_state = 1;
					}
				}
				if(ack_state == 1)
				{
					if(sprintf_len == 0 ||(sprintf_len == recv_len && !memcmp(USART2_RX_BUF, recv_buf, recv_len)))
					{
						ret = RET_SUCCESS;
					}
				}
				USART2_RX_STA=0;
			}
			delay_ms(1);
		}
	}
	else
	{
		sim800c_send_cmd((u8*)0X1B,0,0);
	}
	return ret;
}

void sim800c_recv_unlock_data(void)
{
	u16 recv_len = 0;
	u8 recv_data[256] = {0};
	u16 * p16 = NULL;
	u8 * p = NULL;
	struct sim_cmd_stru stru_recv = {0};
	
	if(USART2_RX_STA&0X8000)		//接收到一次数据了
	{
		recv_len = USART2_RX_STA&0X7FFF;
		if(recv_len > 256) 
		{
			USART2_RX_STA=0;
			return;
		}
		memcpy(recv_data, USART2_RX_BUF, recv_len);
		USART2_RX_STA=0;
		
		p16 = (u16 *)recv_data;
		stru_recv.function_code = HtoNs(*p16++);
		if(stru_recv.function_code != SIM800C_CMD_UNLOCK) return;
		
		stru_recv.data_len = HtoNs(*p16++);
		stru_recv.msg_id = HtoNs(*p16++);
		
		stru_recv.terminal_len = HtoNs(*p16++);
		p=(u8 *)p16;
		memcpy(stru_recv.terminal_id, p, stru_recv.terminal_len);
		stru_recv.terminal_id[stru_recv.terminal_len] = '\0';
		p+=stru_recv.terminal_len;
		
		p16=(u16 *)p;
		stru_recv.rfid_len = HtoNs(*p16++);
		p=(u8 *)p16;
		memcpy(stru_recv.rfid_id, p, stru_recv.rfid_len);
		stru_recv.rfid_id[stru_recv.rfid_len] = '\0';
		p+=stru_recv.rfid_len;
		stru_recv.slave_addr = *p++;
		
		g_recv_unlock_msg_id[stru_recv.slave_addr] = stru_recv.msg_id;

		if(stru_recv.data_len + 4 !=  recv_len
			|| stru_recv.terminal_len != g_terminal_len
			|| memcmp(stru_recv.terminal_id, g_terminal_id, stru_recv.terminal_len)
			|| stru_recv.slave_addr >= COM_MAX_SLAVE_ADDR
			|| stru_recv.rfid_len != strlen((const char *)g_slave_device_info[stru_recv.slave_addr].rfid_id)
			|| memcmp(stru_recv.rfid_id, g_slave_device_info[stru_recv.slave_addr].rfid_id, stru_recv.rfid_len))
		{
			//接收数据出错，直接回复开锁失败
			deal_unlock_state(stru_recv.slave_addr, UNLOCK_STATE_ERR);
			printf("recv unlock err:0x%04x, 0x%04x, 0x%04x, 0x%04x, %s, 0x%04x, %s, 0x%02x\n", 
				stru_recv.function_code, 
				stru_recv.data_len, 
				stru_recv.msg_id, 
				stru_recv.terminal_len, 
				stru_recv.terminal_id, 
				stru_recv.rfid_len, 
				stru_recv.rfid_id, 
				stru_recv.slave_addr);
		}
		else
		{
			printf("recv unlock ok\n");
			usart_ctrl_slave_unlock(stru_recv.slave_addr, stru_recv.rfid_id);
		}
	}
}

u8 sim800c_get_rc522_state(u8 * rc522_state)
{
	static u8 rc522_index_start = '0';
	static u8 rc522_state_ok = '1';
	static u8 rc522_state_err = '0';
	u8 *p = rc522_state;
	u8 i = 0;
	u8 rc522_num = 0;
	for(i=0;i<=COM_MAX_SLAVE_ADDR;i++)
	{
		if(g_slave_device_info[i].addr_state != SLAVE_ADDR_UNDEFINED)
		{
			*p++ = i+ rc522_index_start;
			if(g_slave_device_info[i].addr_state == SLAVE_ADDR_INVAILD)
				*p++ = rc522_state_err;
			else
				*p++ = rc522_state_ok;
			rc522_num++;
		}
	}
	return rc522_num;
}

u16 sprintf_send_tcp_data(u8 *send_buf, u16 function_code, u8 addr)
{
	u16 data_len = 0;
	u8 rc522_num = 0;
	u16 rfid_len = 0;
	u8 *p = NULL;
	u16 *p16=(u16 *)send_buf;
	*p16++ = HtoNs(function_code);
	
	switch(function_code){
		case SIM800C_CMD_UNLOCK://若因接收数据不匹配造成的开锁失败，将回复正确数据
			rfid_len = strlen((const char *)g_slave_device_info[addr].rfid_id);
			*p16++ = HtoNs(2+2+g_terminal_len+2+rfid_len+1+1);
			*p16++ = HtoNs(g_recv_unlock_msg_id[addr]);
			*p16++ = HtoNs(g_terminal_len);
			p=(u8 *)p16;
			memcpy(p, g_terminal_id, g_terminal_len);
			p += g_terminal_len;
			
			p16 = (u16 *)p;
			*p16++ = HtoNs(rfid_len);
			p=(u8 *)p16;
			memcpy(p, g_slave_device_info[addr].rfid_id, rfid_len);
			p += rfid_len;
			
			*p++ = addr;
			
			*p++ = g_slave_device_info[addr].unlock_state == UNLOCK_STATE_OK ? 0x00:0x01;
			
			data_len = p-send_buf;
			break;
		case SIM800C_CMD_REPROT_DEVICE_INFO:
			rfid_len = strlen((const char *)g_slave_device_info[addr].rfid_id);
			*p16++ = HtoNs(2+2+g_terminal_len+2+rfid_len+1);
			*p16++ = HtoNs(g_send_msg_id); 
			g_send_msg_id++;
			*p16++ = HtoNs(g_terminal_len);
			p=(u8 *)p16;
			memcpy(p, g_terminal_id, g_terminal_len);
			p += g_terminal_len;
			
			p16 = (u16 *)p;
			*p16++ = HtoNs(rfid_len);
			p=(u8 *)p16;
			memcpy(p, g_slave_device_info[addr].rfid_id, rfid_len);
			p += rfid_len;
			
			*p++ = addr;
			
			data_len = p-send_buf;
			break;
		case SIM800C_CMD_HEARTBEAT:
			p = send_buf+2+2+2+2+g_terminal_len+1;
			rc522_num = sim800c_get_rc522_state(p);
			*p16++ = HtoNs(2+2+g_terminal_len+1+rc522_num*2);
			*p16++ = HtoNs(g_send_msg_id);
			g_send_msg_id++;
			*p16++ = HtoNs(g_terminal_len);

			p=(u8 *)p16;
			memcpy(p, g_terminal_id, g_terminal_len);
			p += g_terminal_len;
			
			*p++ = rc522_num;
			p += (rc522_num*2);
			
			data_len = p-send_buf;
			break;
		default:
			break;
	}
	return data_len;
}

void sim800c_post_unlock_result(void)
{
	u8 i = 0;
	u16 data_len = 0;
	u8 send_buf[SIM800C_SEND_MAX_LENGHT] = {0};
	for(i=0;i<=COM_MAX_SLAVE_ADDR;i++)
	{
		if(g_slave_device_info[i].unlock_state != UNLOCK_STATE_RFID_UNDEFINE
			&& !g_slave_device_info[i].unlock_timeout)
		{
			data_len = sprintf_send_tcp_data(send_buf, SIM800C_CMD_UNLOCK, i);
			if(sim800c_send_tcp_data(send_buf, data_len) == RET_SUCCESS)
				g_slave_device_info[i].unlock_state = UNLOCK_STATE_RFID_UNDEFINE;
		}
	}
}

void sim800c_report_device_info(void)
{
	u8 i = 0;
	u16 data_len = 0;
	u8 send_buf[SIM800C_SEND_MAX_LENGHT] = {0};
	for(i=0;i<=COM_MAX_SLAVE_ADDR;i++)
	{
		if(g_slave_device_info[i].rfid_state == RFID_INSERT)
		{
			data_len = sprintf_send_tcp_data(send_buf, SIM800C_CMD_REPROT_DEVICE_INFO, i);
			if(sim800c_send_tcp_data(send_buf, data_len) == RET_SUCCESS)
				g_slave_device_info[i].rfid_state = RFID_EXIST;
		}
	}
}

void sim800c_heartbeat(void)
{
	static u32 time_50s = 0;
	u16 data_len = 0;
	u8 send_buf[SIM800C_SEND_MAX_LENGHT] = {0};
	if(g_heartbeat_err_count || time_50s == 0 || time_sys-time_50s >= 50*1000)
	{
		time_50s = time_sys;
		data_len = sprintf_send_tcp_data(send_buf, SIM800C_CMD_HEARTBEAT, 0x00);
		if(sim800c_send_tcp_data(send_buf, data_len) == RET_SUCCESS)
			g_heartbeat_err_count = 0;
		else
			g_heartbeat_err_count++;
	}
}

u16 sim800c_init_tcp_env(void)
{
	u16 res=0;
	u8 *p1, *p2;
	g_server_connect_state = SERVER_CLOSED;
	g_sim800c_ready_state = SIM800C_NOT_READY;
	while(sim800c_send_cmd("AT","OK",100))				//检测是否应答AT指令 
	{
		delay_ms(1200);
	}
	
	if(sim800c_send_cmd("ATE0","OK",200)) return (res|=1<<0);	//不回显
	
	if(sim800c_send_cmd("AT+CGSN","OK",200)==0)			//查询产品序列号
	{
		p1=(u8*)strstr((const char*)(USART2_RX_BUF+2),"\r\n");//查找回车
		p1[0]=0;//加入结束符 
		g_terminal_len = sprintf((char *)g_terminal_id,"%s",USART2_RX_BUF+2);
		USART2_RX_STA=0;
	}else return (res|=1<<1);

	
	if(sim800c_send_cmd("AT+CPIN?","OK",200)) return (res|=1<<2);	//查询SIM卡是否在位 
	if(sim800c_send_cmd("AT+CSQ","+CSQ:",200)==0)			//查询信号质量
	{ 
		p1=(u8*)strstr((const char*)(USART2_RX_BUF),":");
		p2=(u8*)strstr((const char*)(p1),",");
		p2[0]=0;//加入结束符
		g_sim800c_csq = atoi((const char *)p1+2);
		USART2_RX_STA=0;
	}else return (res|=1<<3);

	
	sim800c_send_cmd("AT+CIPCLOSE=1","CLOSE OK",100);	//关闭连接
	sim800c_send_cmd("AT+CIPSHUT","SHUT OK",100);		//关闭移动场景
	if(sim800c_send_cmd("AT+CGCLASS=\"B\"","OK",1000)) return (res|=1<<4);				//设置GPRS移动台类别为B,支持包交换和数据交换 
	if(sim800c_send_cmd("AT+CGDCONT=1,\"IP\",\"CMNET\"","OK",1000)) return (res|=1<<5);	//设置PDP上下文,互联网接协议,接入点等信息
	if(sim800c_send_cmd("AT+CGATT=1","OK",500)) return (res|=1<<6);						//附着GPRS业务
	if(sim800c_send_cmd("AT+CIPCSGP=1,\"CMNET\"","OK",500)) return (res|=1<<7);	 		//设置为GPRS连接模式
	if(sim800c_send_cmd("AT+CIPHEAD=0","OK",500)) return (res|=1<<8);	 				//设置接收数据不显示IP头(去掉多余数据)
	return res;
}

void sim800c_process(void)
{
	sim800c_detect();
	sim800c_recv_unlock_data();
	if(g_sim800c_ready_state == SIM800C_READY && g_server_connect_state == SERVER_CONNECTED)
	{
		sim800c_post_unlock_result();
		sim800c_report_device_info();
		sim800c_heartbeat();
	}
}

void sim800c_reset(void)
{
	//1秒高电平，然后低电平
	POWKEY = 1;
	delay_ms(1000);
	POWKEY = 0;
	
	while(sim800c_init_tcp_env()) 
	{
		//USART1SendString("1",1);
		delay_ms(1000);
	}
	printf("sim800c_reset done\n");
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


