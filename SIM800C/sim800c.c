#include "sim800c.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h"   	 
#include "key.h"	 	 	 	 	 
#include "string.h"    
#include "usart2.h"
#include "my_global.h"

u8 Scan_Wtime = 0;//����ɨ����Ҫ��ʱ��
u8 BT_Scan_mode=0;//����ɨ���豸ģʽ��־

struct sim_cmd_stru g_stru_sim_recv = {0};
struct sim_cmd_stru g_stru_sim_send = {0};

u16 g_send_msg_id = 0;
u16 g_recv_unlock_msg_id = 0;


u16 sim800c_recv_msg_id = 0;

u8 g_terminal_id[32] = {0};
u16 g_terminal_len = 0;

#if 0
//usmart֧�ֲ��� 
//���յ���ATָ��Ӧ�����ݷ��ظ����Դ���
//mode:0,������USART2_RX_STA;
//     1,����USART2_RX_STA;
void sim_at_response(u8 mode)
{
	u16 len = 0;
	if(USART2_RX_STA&0X8000)		//���յ�һ��������
	{
		len = USART2_RX_STA&0X7FFF;
		USART2_RX_BUF[len]=0;//��ӽ�����
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
		USART2SendNByte(USART2_RX_BUF, len);
		#endif
		
	}
}
#endif
//////////////////////////////////////////////////////////////////////////////////
//ATK-SIM800C �������(���Ų��ԡ����Ų��ԡ�GPRS���ԡ���������)���ô���

//sim800C���������,�����յ���Ӧ��
//str:�ڴ���Ӧ����
//����ֵ:0,û�еõ��ڴ���Ӧ����
//����,�ڴ�Ӧ������λ��(str��λ��)
u8* sim800c_check_cmd(u8 *str)
{
	char *strx=0;
	if(USART2_RX_STA&0X8000)		//���յ�һ��������
	{ 
		USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;//��ӽ�����
		strx=strstr((const char*)USART2_RX_BUF,(const char*)str);
		USART1SendString(USART2_RX_BUF, USART2_RX_STA&0X7FFF);
	} 
	return (u8*)strx;
}
//��sim800C��������
//cmd:���͵������ַ���(����Ҫ��ӻس���),��cmd<0XFF��ʱ��,��������(���緢��0X1A),���ڵ�ʱ�����ַ���.
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
u8 sim800c_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART2_RX_STA=0;
	if((u32)cmd<=0XFF)
	{
		while(DMA1_Channel7->CNDTR!=0);	//�ȴ�ͨ��7�������   
		USART2->DR=(u32)cmd;
	}else u2_printf("%s\r\n",cmd);//��������
	
	if(waittime==1100)//11s����ش�������(����ɨ��ģʽ)
	{
		 Scan_Wtime = 11;  //��Ҫ��ʱ��ʱ��
		 TIM4_SetARR(9999);//����1S��ʱ�ж�
	}
	
	
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{ 
	   while(--waittime)	//�ȴ�����ʱ
	   {
		   if(BT_Scan_mode)//����ɨ��ģʽ
		   {
			   res=KEY_Scan(0);//������һ��
			   if(res==WKUP_PRES)return 2;
		   }
		   delay_ms(10);
		   if(USART2_RX_STA&0X8000)//���յ��ڴ���Ӧ����
		   {
			   if(sim800c_check_cmd(ack))break;//�õ���Ч���� 
			   USART2_RX_STA=0;
		   } 
	   }
	   if(waittime==0)res=1; 
	}
	return res;
} 

//����SIM800C�������ݣ���������ģʽ��ʹ�ã�
//request:�ڴ����������ַ���
//waittimg:�ȴ�ʱ��(��λ��10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
u8 sim800c_wait_request(u8 *request ,u16 waittime)
{
	 u8 res = 1;
	 u8 key;
	 if(request && waittime)
	 {
		while(--waittime)
		{   
		   key=KEY_Scan(0);
		   if(key==WKUP_PRES) return 2;//������һ��
		   delay_ms(10);
		   if(USART2_RX_STA &0x8000)//���յ��ڴ���Ӧ����
		   {
			  if(sim800c_check_cmd(request)) break;//�õ���Ч����
			  USART2_RX_STA=0;
		   }
		}
		if(waittime==0)res=0;
	 }
	 return res;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//GPRS���Բ��ִ���

const u8 *modetbl[2]={"TCP","UDP"};//����ģʽ

//tcp/udp����
//����������,��ά������
//mode:0:TCP����;1,UDP����)
//ipaddr:ip��ַ
//port:�˿� 
void sim800c_tcpudp_test(u8 mode, const u8* ipaddr, const u8* port)
{ 
	u8 i=0;
	u8 p[100] = {0};
	u8 p1[100] = {0};
	
	u8 *p2,*p3;

	u16 timex=0;
	u8 count=0;
	
	u8 connectsta=0;			//0,��������;1,���ӳɹ�;2,���ӹر�; 
	u8 hbeaterrcnt=0;			//�������������,����5�������ź���Ӧ��,����������

	sprintf((char*)p,"IP��ַ:%s �˿�:%s",ipaddr,port);

	USART2_RX_STA=0;
	sprintf((char*)p,"AT+CIPSTART=\"%s\",\"%s\",\"%s\"",modetbl[mode],ipaddr,port);
	if(sim800c_send_cmd(p,"OK",500))return;		//��������
	while(1)
	{ 

		if(connectsta==1 && sim800c_send_cmd("AT+CIPSEND",">",500)==0)		//��������
		{ 
				//printf("CIPSEND DATA:%s\r\n",p1);	 			//�������ݴ�ӡ������
			u2_printf("%s\r\n",p1);
			delay_ms(50);
			sim800c_send_cmd((u8*)0X1A,"SEND OK",1000);//��ȴ�10s
			delay_ms(500); 
		}else sim800c_send_cmd((u8*)0X1B,0,0);	//ESC,ȡ������ 

		if((timex%20)==0)
		{
			//LED0=!LED0;
			count++;	
			if(connectsta==2||hbeaterrcnt>8)//�����ж���,��������8������û����ȷ���ͳɹ�,����������
			{
				sim800c_send_cmd("AT+CIPCLOSE=1","CLOSE OK",500);	//�ر�����
				sim800c_send_cmd("AT+CIPSHUT","SHUT OK",500);		//�ر��ƶ����� 
				sim800c_send_cmd(p,"OK",500);						//������������
				connectsta=0;	
 				hbeaterrcnt=0;
			}
			sprintf((char*)p1,"ATK-SIM800C %s���� %d  ",modetbl[mode],count);
		}
		if(connectsta==0&&(timex%200)==0)//���ӻ�û������ʱ��,ÿ2���ѯһ��CIPSTATUS.
		{
			sim800c_send_cmd("AT+CIPSTATUS","OK",500);	//��ѯ����״̬
			if(strstr((const char*)USART2_RX_BUF,"CLOSED"))connectsta=2;
			if(strstr((const char*)USART2_RX_BUF,"CONNECT OK"))connectsta=1;
		}
		if(connectsta==1&&timex>=600)//����������ʱ��,ÿ6�뷢��һ������
		{
			timex=0;
			if(sim800c_send_cmd("AT+CIPSEND",">",200)==0)//��������
			{
				sim800c_send_cmd((u8*)0X00,0,0);	//��������:0X00  
				delay_ms(40);						//�������ʱ
				sim800c_send_cmd((u8*)0X1A,0,0);	//CTRL+Z,�������ݷ���,����һ�δ���	
			}else sim800c_send_cmd((u8*)0X1B,0,0);	//ESC,ȡ������ 		
				
			hbeaterrcnt++; 
			printf("hbeaterrcnt:%d\r\n",hbeaterrcnt);//������Դ���
		} 
		delay_ms(10);
		if(USART2_RX_STA&0X8000)		//���յ�һ��������
		{ 
			USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;	//��ӽ����� 
			printf("%s",USART2_RX_BUF);				//���͵�����  
			if(hbeaterrcnt)							//��Ҫ�������Ӧ��
			{
				if(strstr((const char*)USART2_RX_BUF,"SEND OK"))hbeaterrcnt=0;//��������
			}				
			p2=(u8*)strstr((const char*)USART2_RX_BUF,"+IPD");
			if(p2)//���յ�TCP/UDP����
			{
				p3=(u8*)strstr((const char*)p2,",");
				p2=(u8*)strstr((const char*)p2,":");
				p2[0]=0;//���������
				sprintf((char*)p1,"�յ�%s�ֽ�,��������",p3+1);//���յ����ֽ���
			}
			USART2_RX_STA=0;
		}

		timex++; 
		i++;
		if(i>10){i=0; u2_printf("AT\r\n");}//����������
	} 
}

//sim800C GPRS����
//���ڲ���TCP/UDP����
//����ֵ:0,����
//����,�������
u8 sim800c_gprs_test(void)
{
	const u8 *url = "208l8w1838.51mypc.cn";
	const u8 *port = "48438";
	
	u8 mode=0;				//0,TCP����;1,UDP����
	u8 timex=1; 
	
 	sim800c_send_cmd("AT+CIPCLOSE=1","CLOSE OK",100);	//�ر�����
	sim800c_send_cmd("AT+CIPSHUT","SHUT OK",100);		//�ر��ƶ����� 
	if(sim800c_send_cmd("AT+CGCLASS=\"B\"","OK",1000))return 1;				//����GPRS�ƶ�̨���ΪB,֧�ְ����������ݽ��� 
	if(sim800c_send_cmd("AT+CGDCONT=1,\"IP\",\"CMNET\"","OK",1000))return 2;//����PDP������,��������Э��,��������Ϣ
	if(sim800c_send_cmd("AT+CGATT=1","OK",500))return 3;					//����GPRSҵ��
	if(sim800c_send_cmd("AT+CIPCSGP=1,\"CMNET\"","OK",500))return 4;	 	//����ΪGPRS����ģʽ
	if(sim800c_send_cmd("AT+CIPHEAD=1","OK",500))return 5;	 				//���ý���������ʾIPͷ(�����ж�������Դ)
	
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
		sim_at_response(1);//���GSMģ�鷢�͹���������,��ʱ�ϴ�������
	}
	return 0;
}


/////////////////////////////////////////////////////////////////
//�������Բ��ִ���

///////////////////////////////////////////////////////////////////// 

//ATK-SIM800C GSM/GPRS�����Կ��Ʋ���
//���Խ�����UI
void sim800c_mtest_ui(u16 x,u16 y)
{
	u8 p[50] = {0};
	u8 *p1,*p2;
	
	USART2_RX_STA=0;
	if(sim800c_send_cmd("AT+CGMI","OK",200)==0)				//��ѯ����������
	{ 
		p1=(u8*)strstr((const char*)(USART2_RX_BUF+2),"\r\n");
		p1[0]=0;//���������
		sprintf((char*)p,"������:%s",USART2_RX_BUF+2);
		USART2_RX_STA=0;		
	} 
	if(sim800c_send_cmd("AT+CGMM","OK",200)==0)//��ѯģ������
	{ 
		p1=(u8*)strstr((const char*)(USART2_RX_BUF+2),"\r\n"); 
		p1[0]=0;//���������
		sprintf((char*)p,"ģ���ͺ�:%s",USART2_RX_BUF+2);
		USART2_RX_STA=0;		
	} 
	if(sim800c_send_cmd("AT+CGSN","OK",200)==0)//��ѯ��Ʒ���к�
	{ 
		p1=(u8*)strstr((const char*)(USART2_RX_BUF+2),"\r\n");//���һس�
		p1[0]=0;//��������� 
		sprintf((char*)p,"���к�:%s",USART2_RX_BUF+2);
		USART2_RX_STA=0;		
	}
	if(sim800c_send_cmd("AT+CNUM","+CNUM",200)==0)			//��ѯ��������
	{ 
		p1=(u8*)strstr((const char*)(USART2_RX_BUF),",");
		p2=(u8*)strstr((const char*)(p1+2),"\"");
		p2[0]=0;//���������
		sprintf((char*)p,"��������:%s",p1+2);
		USART2_RX_STA=0;		
	}
}

//GSM��Ϣ��ʾ(�ź�����,��ص���,����ʱ��)
//����ֵ:0,����
//����,�������
u8 sim800c_gsminfo_show(u16 x,u16 y)
{
	u8 p[50] = {0};
	u8 *p1, *p2;
	u8 res=0;

	USART2_RX_STA=0;
	if(sim800c_send_cmd("AT+CPIN?","OK",200))res|=1<<0;	//��ѯSIM���Ƿ���λ 
	USART2_RX_STA=0;  
	if(sim800c_send_cmd("AT+COPS?","OK",200)==0)		//��ѯ��Ӫ������
	{ 
		p1=(u8*)strstr((const char*)(USART2_RX_BUF),"\""); 
		if(p1)//����Ч����
		{
			p2=(u8*)strstr((const char*)(p1+1),"\"");
			p2[0]=0;//���������			
			sprintf((char*)p,"��Ӫ��:%s",p1+1);
		} 
		USART2_RX_STA=0;		
	}else res|=1<<1;
	if(sim800c_send_cmd("AT+CSQ","+CSQ:",200)==0)		//��ѯ�ź�����
	{ 
		p1=(u8*)strstr((const char*)(USART2_RX_BUF),":");
		p2=(u8*)strstr((const char*)(p1),",");
		p2[0]=0;//���������
		sprintf((char*)p,"�ź�����:%s",p1+2);
		USART2_RX_STA=0;		
	}else res|=1<<2;
	if(sim800c_send_cmd("AT+CBC","+CBC:",200)==0)		//��ѯ��ص���
	{ 
		p1=(u8*)strstr((const char*)(USART2_RX_BUF),",");
		p2=(u8*)strstr((const char*)(p1+1),",");
		p2[0]=0;p2[5]=0;//���������
		sprintf((char*)p,"��ص���:%s%%  %smV",p1+1,p2+1);
		USART2_RX_STA=0;		
	}else res|=1<<3; 
	if(sim800c_send_cmd("AT+CCLK?","+CCLK:",200)==0)		//��ѯ��ص���
	{ 
		p1=(u8*)strstr((const char*)(USART2_RX_BUF),"\"");
		p2=(u8*)strstr((const char*)(p1+1),":");
		p2[3]=0;//���������
		sprintf((char*)p,"����ʱ��:%s",p1+1);
		USART2_RX_STA=0;		
	}else res|=1<<4; 

	return res;
} 


//NTP����ͬ��ʱ��
void ntp_update(void)
{  
	 sim800c_send_cmd("AT+SAPBR=3,1,\"Contype\",\"GPRS\"","OK",200);//���ó��س���1
	 sim800c_send_cmd("AT+SAPBR=3,1,\"APN\",\"CMNET\"","OK",200);
	 sim800c_send_cmd("AT+SAPBR=1,1",0,200);                        //����һ��GPRS������
     delay_ms(5);
     sim800c_send_cmd("AT+CNTPCID=1","OK",200);                     //����CNTPʹ�õ�CID
	 sim800c_send_cmd("AT+CNTP=\"202.120.2.101\",32","OK",200);     //����NTP�������ͱ���ʱ��(32ʱ�� ʱ����׼ȷ)
     sim800c_send_cmd("AT+CNTP","+CNTP: 1",600);                    //ͬ������ʱ��
}

//sim800C�����Գ���
void sim800c_test(void)
{
	u8 timex=0;
	u8 sim_ready=0;
	
	delay_ms(50);
	while(sim800c_send_cmd("AT","OK",100))//����Ƿ�Ӧ��ATָ�� 
	{
		delay_ms(1200);
	} 	 
	
	sim800c_send_cmd("ATE0","OK",200);//������
	sim800c_mtest_ui(40,30);
	ntp_update();//����ͬ��ʱ��
	while(1)
	{
		delay_ms(10);
		sim_at_response(1);//���GSMģ�鷢�͹���������,��ʱ�ϴ�������
		#if 1
		if(sim_ready)//SIM������.
		{
			sim800c_gprs_test();//GPRS����
			//sim800c_mtest_ui(40,30);
			timex=0;
		}
		if(timex==0 || sim_ready==0)		//2.5�����Ҹ���һ��
		{
			if(sim800c_gsminfo_show(40,225)==0)sim_ready=1;
			else sim_ready=0;
		}	
		//if((timex%20)==0)LED0=!LED0;//200ms��˸ 
		timex++;
		#endif
	} 	
}

void sim800c_post_unlock_result(void)
{
	u8 i = 0;
	for(i=0;i<=COM_MAX_SLAVE_ADDR;i++)
	{
		if(g_slave_device_info[i].unlock_state != UNLOCK_STATE_RFID_UNDEFINE
			&& !g_slave_device_info[i].unlock_timeout)
		{
			//sim800c_send_cmd(send_buf,send_len);
			g_slave_device_info[i].unlock_state = UNLOCK_STATE_RFID_UNDEFINE;
		}
	}
}

void sim800c_report_device_info(void)
{
	
}

u8 deal_sim800c_recv_tcp_data(u8 *recv_data, struct sim_cmd_stru *stru_recv)
{
	u8 *p = recv_data;
	stru_recv->function_code = ((u16)(*p)) << 8 | *(p+1);
	p+=2;
	stru_recv->data_len = ((u16)(*p)) << 8 | *(p+1);
	p+=2;
	stru_recv->msg_id = ((u16)(*p)) << 8 | *(p+1);
	p+=2;

	switch(stru_recv->function_code){
		case SIM800C_CMD_UNLOCK:
		case SIM800C_CMD_REPROT_DEVICE_INFO:
			stru_recv->terminal_len = ((u16)(*p)) << 8 | *(p+1);
			p+=2;
			memcpy(stru_recv->terminal_id, p, stru_recv->terminal_len);
			p+=stru_recv->terminal_len;

			stru_recv->rfid_len = ((u16)(*p)) << 8 | *(p+1);
			p+=2;
			memcpy(stru_recv->rfid_id, p, stru_recv->rfid_len);
			p+=stru_recv->rfid_len;

			stru_recv->slave_addr = *p++;
			if(stru_recv->function_code == SIM800C_CMD_REPROT_DEVICE_INFO)
				stru_recv->result_code = *p++;
			break;
		case SIM800C_CMD_HEARTBEAT:
			stru_recv->result_code = *p++;
			break;
		default:
			break;
	}	
}

void sim_at_response(u8 mode)
{	
	u16 len = 0;
	u8 recv_data[256] = {0};
	struct sim_cmd_stru stru_recv = {0};
	
	if(USART2_RX_STA&0X8000)		//���յ�һ��������
	{
		len = USART2_RX_STA&0X7FFF;
		USART2_RX_BUF[len]=0;//��ӽ�����
		if(mode)USART2_RX_STA=0;
		deal_sim800c_recv_tcp_data(USART2_RX_BUF, &stru_recv);

		
		
		

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
		USART2SendNByte(USART2_RX_BUF, len);
		#endif
		
	}
}

u8 sim800c_send_tcp_data(u8 data, u16 len)
{
	if(connectsta==1 && sim800c_send_cmd("AT+CIPSEND=len",">",500)==0)		//��������
	{ 
			//printf("CIPSEND DATA:%s\r\n",p1);	 			//�������ݴ�ӡ������
		u2_printf("%s\r\n",p1);
		delay_ms(50);
		sim800c_send_cmd((u8*)0X1A,"SEND OK",1000);//��ȴ�10s
		delay_ms(500); 
	}else sim800c_send_cmd((u8*)0X1B,0,0);	//ESC,ȡ������

		
	u8 res=0; 
	USART2_RX_STA=0;
	if((u32)cmd<=0XFF)
	{
		while(DMA1_Channel7->CNDTR!=0);	//�ȴ�ͨ��7�������   
		USART2->DR=(u32)cmd;
	}else u2_printf("%s\r\n",cmd);//��������
	
	if(waittime==1100)//11s����ش�������(����ɨ��ģʽ)
	{
		 Scan_Wtime = 11;  //��Ҫ��ʱ��ʱ��
		 TIM4_SetARR(9999);//����1S��ʱ�ж�
	}
	
	
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{ 
	   while(--waittime)	//�ȴ�����ʱ
	   {
		   if(BT_Scan_mode)//����ɨ��ģʽ
		   {
			   res=KEY_Scan(0);//������һ��
			   if(res==WKUP_PRES)return 2;
		   }
		   delay_ms(10);
		   if(USART2_RX_STA&0X8000)//���յ��ڴ���Ӧ����
		   {
			   if(sim800c_check_cmd(ack))break;//�õ���Ч���� 
			   USART2_RX_STA=0;
		   } 
	   }
	   if(waittime==0)res=1; 
	}
	return res;


	u8 recv_data[MAX_SERIAL_BUFFER_LENGHT] = {0};
	u8 count = 0;
	u8 ret = RET_FAIL;
	for(count=0;count<3;count++)
	{
		RS485SendNByte(cmd, len,0);
		if(cmd[4] == COM_CMD_GET_SLAVE_ADDR)
		{
			rs485_broadcast_timeout = 2000;
			do
			{
				if(rs485_read_data(recv_data) == RET_SUCCESS
					&& recv_data[4] == cmd[4])
				{
					update_slave_addr(recv_data[3], SLAVE_ADDR_NEW);
					ret = RET_SUCCESS;
				}
			}while(rs485_broadcast_timeout);
		}
		else
		{
			if(rs485_read_data(recv_data) == RET_SUCCESS
				&& recv_data[4] == cmd[4]
				&& recv_data[3] == cmd[2])
			{
				ret = RET_SUCCESS;
				switch(recv_data[3])
				{
					case COM_CMD_GET_SLAVE_RFID:
						update_rfid_info(recv_data[3], recv_data[5], &recv_data[6]);
						break;
					case COM_CMD_CTRL_SLAVE_UNLOCK:
						deal_unlock_state(recv_data[3], recv_data[5]);
						break;
					case COM_CMD_GET_NEW_RFID_INFO:
						update_back_info(recv_data[3], recv_data[5], &recv_data[6]);
						break;
					default:
						ret = RET_FAIL;
						break;
				}
			}
		}
		if(ret != RET_SUCCESS) break;
	}
	if(ret == RET_FAIL)
	{
		update_slave_addr(cmd[2], SLAVE_ADDR_INVAILD);
	}
	return ret;
}

void sim800c_get_terminal_id(void)
{
	u8 p[50] = {0};
	u8 *p1,*p2;
	if(sim800c_send_cmd("AT+CGSN","OK",200)==0)//��ѯ��Ʒ���к�
	{ 
		p1=(u8*)strstr((const char*)(USART2_RX_BUF+2),"\r\n");//���һس�
		p1[0]=0;//��������� 
		sprintf((char*)p,"���к�:%s",USART2_RX_BUF+2);
		USART2_RX_STA=0;
	}
	g_terminal_len = sprintf(g_terminal_id,"%s",USART2_RX_BUF+2);
	if(sim800c_send_cmd("AT+CNUM","+CNUM",200)==0)			//��ѯ��������
	{ 
		p1=(u8*)strstr((const char*)(USART2_RX_BUF),",");
		p2=(u8*)strstr((const char*)(p1+2),"\"");
		p2[0]=0;//���������
		sprintf((char*)p,"��������:%s",p1+2);
		USART2_RX_STA=0;		
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
	u8 *p = send_buf;
	u16 data_len = 0;
	u8 rc522_num = 0;
	*p++ = (u8)(function_code >> 8);
	*p++ = (u8)function_code;
	
	switch(function_code){
		case SIM800C_CMD_UNLOCK:
			*(p += 2) = (u8)(g_recv_unlock_msg_id >> 8);
			*p++ = (u8)g_recv_unlock_msg_id;
			*p++ = (u8)(g_terminal_len >> 8);
			*p++ = (u8)g_terminal_len;
			memcpy(p, g_terminal_id, g_terminal_len);
			p += g_terminal_len;
			*p++ = 0x00;
			*p++ = 0x08;
			memcpy(p, g_slave_device_info[addr].rfid_id, 8);
			p += 8;
			*p++ = addr;
			*p++ = g_slave_device_info[addr].unlock_state == UNLOCK_STATE_OK ? 0x00:0x01;
			break;
		case SIM800C_CMD_REPROT_DEVICE_INFO:
			*(p += 2) = (u8)(g_send_msg_id >> 8);
			*p++ = (u8)g_send_msg_id;
			g_send_msg_id++;
			*p++ = (u8)(g_terminal_len >> 8);
			*p++ = (u8)g_terminal_len;
			memcpy(p, g_terminal_id, g_terminal_len);
			p += g_terminal_len;
			*p++ = 0x00;
			*p++ = 0x08;
			memcpy(p, g_slave_device_info[addr].rfid_id, 8);
			p += 8;
			*p++ = addr;
			break;
		case SIM800C_CMD_HEARTBEAT:
			*(p += 2) = (u8)(g_send_msg_id >> 8);
			*p++ = (u8)g_send_msg_id;
			g_send_msg_id++;
			*p++ = (u8)(g_terminal_len >> 8);
			*p++ = (u8)g_terminal_len;
			memcpy(p, g_terminal_id, g_terminal_len);
			p += g_terminal_len;
			rc522_num = sim800c_get_rc522_state(p+1);
			*p++ = rc522_num;
			p += (rc522_num*2);
			break;
		default:
			break;
	}
	data_len = p-send_buf;
	send_buf[2] = (u8)(data_len >> 8);
	send_buf[3] = (u8)data_len;
	return data_len;
}

void sim800c_heartbeat(void)
{
	static u16 msg_id = 0;
	static u32 time_50s = 0;
	u16 data_len = 0;
	u8 send_buf[SIM800C_SEND_MAX_LENGHT] = {0};
	if(time_50s == 0 || time_sys-time_50s >= 50*1000)
	{
		time_50s = time_sys;
		data_len = sprintf_send_tcp_data(send_buf, SIM800C_CMD_HEARTBEAT, 0x00);
		sim800c_send_tcp_data(send_buf, data_len);
	}
}

void sim800c_process(void)
{
	sim_at_response(1);
	sim800c_test(); 				//GSM����

	sim800c_report_device_info();
	sim800c_heartbeat();
}

void sim800c_reset(void)
{
	//1��ߵ�ƽ��Ȼ��͵�ƽ
	POWKEY = 1;
	delay_ms(1000);
	POWKEY = 0;
}

void sim800c_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PC�˿�ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				 //POWKEY-->PB.11 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.11
	sim800c_reset();
}


