#ifndef __SIM800C_H__
#define __SIM800C_H__	 
#include "sys.h"

#define swap16(x) (x&0XFF)<<8|(x&0XFF00)>>8	//�ߵ��ֽڽ����궨��

#define POWKEY PBout(11)	// PB11

#define MAX_SIM800C_BUFFER_LENGHT 200

extern u8 Scan_Wtime;

void sim_send_sms(u8*phonenumber,u8*msg);
void sim_at_response(u8 mode);	
u8* sim800c_check_cmd(u8 *str);
u8 sim800c_send_cmd(u8 *cmd,u8 *ack,u16 waittime);
u8 sim800c_wait_request(u8 *request ,u16 waittime);
u8 sim800c_chr2hex(u8 chr);
u8 sim800c_hex2chr(u8 hex);
void sim800c_unigbk_exchange(u8 *src,u8 *dst,u8 mode);
void sim800c_load_keyboard(u16 x,u16 y,u8 **kbtbl);
void sim800c_key_staset(u16 x,u16 y,u8 keyx,u8 sta);
u8 sim800c_get_keynum(u16 x,u16 y);
u8 sim800c_call_test(void);			 //���Ų���
void sim800c_sms_read_test(void);	 //�����Ų���
void sim800c_sms_send_test(void);	 //�����Ų��� 
void sim800c_sms_ui(u16 x,u16 y);	 //���Ų���UI���溯��
u8 sim800c_sms_test(void);			 //���Ų���
void sim800c_GB_ui(void);            //BT��GPRSUI���溯��
void sim800c_GB_test(void);          //BT��GPRSѡ��
void sim800c_spp_ui(u16 x,u16 y);    //��������UI���溯��
u8 sim800c_spp_test(void);           //����spp����
u8 sim800c_spp_mode(u8 mode);        //����ģʽѡ��
void sim800c_mtest_ui(u16 x,u16 y);	 //SIM800C GSM/GPRS������UI
u8 sim800c_gsminfo_show(u16 x,u16 y);//��ʾGSMģ����Ϣ
void ntp_update(void);               //����ͬ��ʱ��
void sim800c_test(void);			 //SIM800C�����Ժ���



void sim800c_process(void);
void sim800c_init(void);

#define			SIM800C_CMD_UNLOCK					0x0101
#define			SIM800C_CMD_REPROT_DEVICE_INFO		0x0102
#define			SIM800C_CMD_HEARTBEAT				0x0103

#define			SIM800C_SEND_MAX_LENGHT				256

enum server_connect_state{
	SERVER_DISCONNECTED = 0,
	SERVER_CONNECTED,
	SERVER_CLOSED
};

enum sim800c_ready_state{
	SIM800C_NOT_READY = 0,
	SIM800C_READY
};


enum sim_tcp_send_state{
	TCP_SEND_OK = 0,
	TCP_SENDING,
	TCP_SEND_ERR
};

struct sim_cmd_head_stru{
	u16 function_code;
	u16 data_len;
	u16 msg_id;
};

struct sim_cmd_stru
{
	u16 function_code;
	u16 data_len;
	u16 msg_id;
	u16 terminal_len;
	u8 terminal_id[32];
	u16 rfid_len;
	u8 rfid_id[20];
	u8 rfid_num;
	u8 rfid_state[64];//(COM_MAX_SLAVE_ADDR+1)*2
	u8 slave_addr;
	
	u8 result_code;
};
#if 0
AT                             ���ģ�鹤��

AT+IPR=38400                   ���ò�����

AT&W                          ��������

AT+CPIN?                       ��ѯ����ע�����

AT+CSQ                         ��ѯ�ź�������

AT+CREG?                       ��ѯGSM����ע��״̬

AT+CGREG?                      ��ѯGPRS����ע��״̬

AT+CGATT=1                     ʹģ�鸽��GPRS����

AT+CIPMODE=1                  ����͸��ģʽ

AT+CSTT=CMNET                 ����APN

AT+CIICR                        �����ƶ�������������������

AT+CIFSR                        ��ȡģ��IP��ַ

AT+CIPSTART=�� TCP��,�� yuganghua.3322.org��,��65005��  ����Ҫ���ӵ�Զ�̷����������Ͷ˿�
#endif

#endif
