#ifndef __SIM800C_H__
#define __SIM800C_H__	 
#include "sys.h"

#define swap16(x) (x&0XFF)<<8|(x&0XFF00)>>8	//高低字节交换宏定义

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
u8 sim800c_call_test(void);			 //拨号测试
void sim800c_sms_read_test(void);	 //读短信测试
void sim800c_sms_send_test(void);	 //发短信测试 
void sim800c_sms_ui(u16 x,u16 y);	 //短信测试UI界面函数
u8 sim800c_sms_test(void);			 //短信测试
void sim800c_GB_ui(void);            //BT、GPRSUI界面函数
void sim800c_GB_test(void);          //BT、GPRS选择
void sim800c_spp_ui(u16 x,u16 y);    //蓝牙测试UI界面函数
u8 sim800c_spp_test(void);           //蓝牙spp测试
u8 sim800c_spp_mode(u8 mode);        //连接模式选择
void sim800c_mtest_ui(u16 x,u16 y);	 //SIM800C GSM/GPRS主测试UI
u8 sim800c_gsminfo_show(u16 x,u16 y);//显示GSM模块信息
void ntp_update(void);               //网络同步时间
void sim800c_test(void);			 //SIM800C主测试函数



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
AT                             检查模块工作

AT+IPR=38400                   设置波特率

AT&W                          保存设置

AT+CPIN?                       查询网络注册情况

AT+CSQ                         查询信号质量。

AT+CREG?                       查询GSM网络注册状态

AT+CGREG?                      查询GPRS网络注册状态

AT+CGATT=1                     使模块附着GPRS网络

AT+CIPMODE=1                  设置透传模式

AT+CSTT=CMNET                 设置APN

AT+CIICR                        激活移动场景，建立无线连接

AT+CIFSR                        获取模块IP地址

AT+CIPSTART=” TCP”,” yuganghua.3322.org”,”65005”  设置要连接的远程服务器域名和端口
#endif

#endif
