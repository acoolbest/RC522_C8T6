#include "delay.h"
#include "sys.h"
#include "pwm.h"
#include "buzzer.h"

#if 0
// �������������»�����ϲ���ơ�
const tNote MyScore1[]=
{
  {L3,TT/8},{M6,TT/4},{M5,TT/4},{M6,TT/4},{M5,TT/8},{M3,TT/8},{M3,TT/4},{L3,TT/8},{M6,TT/4},{M5,TT/4},{M6,TT/4},{M5,TT/8},{M6,TT/8},{M6,TT/2},{M3,TT/8},{M2,TT/8+TT/16},{M3,TT/16},{M2,TT/8},
  {M1,TT/8},{L6,TT/4},{M3,TT/8},{M2,TT/8+TT/16},{M3,TT/16},{M2,TT/8},{M1,TT/8},{M2,TT/4},{M2,TT/8+TT/16},{M1,TT/8},{M1,TT/4},{M2,TT/4},{M3,TT/4},{M5,TT/4},{M6,TT},{M6,TT/8+TT/16},{M5,TT/16},
  {M3,TT/8},{M5,TT/8},{M6,TT/4}, //��ϲ�㷢�ơ���������˲���
  
  {L3,TT/8},{L6,TT/4},{L6,TT/8},{L5,TT/8},{L6,TT/8},{L3,TT/8},{L3,TT/8},{L5,TT/8},{L6,TT/8},{M1,TT/8},{L6,TT/8},{L5,TT/8},{L6,TT/4},{L3,TT/8},{L5,TT/8},{M1,TT/4},{M1,TT/8},{M1,TT/8},{M2,TT/8},
  {M2,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT/2},{L3,TT/8},{M2,TT/4},{M2,TT/8},{M1,TT/8},{M2,TT/8},{L6,TT/8},{L6,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},{M2,TT/8},{M1,TT/8},{L6,TT/8},
  {M1,TT/8},{M3,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},{M3,TT/4},{L5,TT/4},{L6,TT/2},//��ף�����µ�Ů�����������̳������Դ�

  {L3,TT/8},{L6,TT/4},{L6,TT/8},{L5,TT/8},{L6,TT/8},{L3,TT/8},{L3,TT/8},{L5,TT/8},{L6,TT/8},{M1,TT/8},{L6,TT/8},{L5,TT/8},{L6,TT/4},{L3,TT/8},{L5,TT/8},{M1,TT/4},{M1,TT/8},{M1,TT/8},{M2,TT/8},
  {M2,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT/2},{L3,TT/8},{M2,TT/4},{M2,TT/8},{M1,TT/8},{M2,TT/8},{L6,TT/8},{L6,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},{M2,TT/8},{M1,TT/8},{L6,TT/8},
  {M1,TT/8},{M3,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},{M3,TT/4},{L5,TT/4},{L6,TT/2},//��ף�����µ�Ů�����������̳������Դ�

  {L5,TT/4},{M1,TT/2+TT/4},{M1,TT/8},{M2,TT/8},{M3,TT/2+TT/4},{M3,TT/8},{M5,TT/8},{M5,TT/4+TT/8},{M3,TT/8},{M2,TT/4},{M1,TT/4},{M2,TT/2},{M2,TT/4+TT/8},{L6,TT/8},{M2,TT/4},{M3,TT/4},
  {M4,TT/8+TT/16},{M5,TT/16},{M4,TT/8},{M3,TT/8},{M2,TT/2},{M5,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/4},{L5,TT/8},{L6,TT/2},//��ҡ��ڡ�����Ҫ���ù�����

  {L3,TT/8},{M6,TT/4},{M5,TT/4},{M6,TT/4},{M5,TT/8},{M3,TT/8},{M3,TT/4},{L3,TT/8},{M6,TT/4},{M5,TT/4},{M6,TT/4},{M5,TT/8},{M6,TT/8},{M6,TT/2},{M3,TT/8},{M2,TT/8+TT/16},{M3,TT/16},{M2,TT/8},
  {M1,TT/8},{L6,TT/4},{M3,TT/8},{M2,TT/8+TT/16},{M3,TT/16},{M2,TT/8},{M1,TT/8},{M2,TT/4},{M2,TT/8+TT/16},{M1,TT/8},{M1,TT/4},{M2,TT/4},{M3,TT/4},{M5,TT/4},{M6,TT},{M6,TT/8+TT/16},{M5,TT/16},
  {M3,TT/8},{M5,TT/8},{M6,TT/4}, //��ϲ�㷢�ơ���������˲���

  {L3,TT/8},{L6,TT/4},{L6,TT/8},{L5,TT/8},{L6,TT/8},{L3,TT/8},{L3,TT/8},{L5,TT/8},{L6,TT/8},{M1,TT/8},{L6,TT/8},{L5,TT/8},{L6,TT/4},{L3,TT/8},{L5,TT/8},{M1,TT/4},{M1,TT/8},{M1,TT/8},{M2,TT/8},
  {M2,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT/2},{L3,TT/8},{M2,TT/4},{M2,TT/8},{M1,TT/8},{M2,TT/8},{L6,TT/8},{L6,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},{M2,TT/8},{M1,TT/8},{L6,TT/8},
  {M1,TT/8},{M3,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},{M3,TT/4},{L5,TT/4},{L6,TT/2},//��ף�����µ�Ů�����������̳������Դ�

  {L5,TT/4},{M1,TT/2+TT/4},{M1,TT/8},{M2,TT/8},{M3,TT/2+TT/4},{M3,TT/8},{M5,TT/8},{M5,TT/4+TT/8},{M3,TT/8},{M2,TT/4},{M1,TT/4},{M2,TT/2},{M2,TT/4+TT/8},{L6,TT/8},{M2,TT/4},{M3,TT/4},
  {M4,TT/8+TT/16},{M5,TT/16},{M4,TT/8},{M3,TT/8},{M2,TT/2},{M5,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/4},{L5,TT/8},{L6,TT/2},//��ҡ��ڡ�����Ҫ���ù�����

  {L3,TT/8},{M6,TT/4},{M5,TT/4},{M6,TT/4},{M5,TT/8},{M3,TT/8},{M3,TT/4},{L3,TT/8},{M6,TT/4},{M5,TT/4},{M6,TT/4},{M5,TT/8},{M6,TT/8},{M6,TT/2},{M3,TT/8},{M2,TT/8+TT/16},{M3,TT/16},{M2,TT/8},
  {M1,TT/8},{L6,TT/4},{M3,TT/8},{M2,TT/8+TT/16},{M3,TT/16},{M2,TT/8},{M1,TT/8},{M2,TT/4},{M2,TT/8+TT/16},{M1,TT/8},{M1,TT/4},{M2,TT/4},{M3,TT/4},{M5,TT/4},{M6,TT},{M6,TT/8+TT/16},{M5,TT/16},
  {M3,TT/8},{M5,TT/8},{M6,TT/4}, //��ϲ�㷢�ơ���������˲���

  {L3,TT/8},{M6,TT/4},{M5,TT/4},{M6,TT/4},{M5,TT/8},{M3,TT/8},{M3,TT/4},{L3,TT/8},{M6,TT/4},{M5,TT/4},{M6,TT/4},{M5,TT/8},{M6,TT/8},{M6,TT/2},{M3,TT/8},{M2,TT/8+TT/16},{M3,TT/16},{M2,TT/8},
  {M1,TT/8},{L6,TT/4},{M3,TT/8},{M2,TT/8+TT/16},{M3,TT/16},{M2,TT/8},{M1,TT/8},{M2,TT/4},{M2,TT/8+TT/16},{M1,TT/8},{M1,TT/4},{M2,TT/4},{M3,TT/4},{M5,TT/4},{M6,TT},{M6,TT/8+TT/16},{M5,TT/16},
  {M3,TT/8},{M5,TT/8},{M6,TT/4}, //��ϲ�㷢�ơ���������˲���
  {0,0},
};
#endif

#if 0

// ������������������ɫ��
const tNote MyScore2[]=
{
  {M1,TT/8},{M1,TT/4},{L6,TT/8},{L5,TT/4},{L6,TT/4},{M1,TT/4},{M1,TT/8},{M2,TT/8},{M3,TT/2},{M2,TT/8},{M2,TT/4},{M1,TT/8},{M2,TT/4},{M2,TT/8},{M5,TT/8},{M5,TT/8},{M3,TT/8},
  {M3,TT/8},{M2,TT/8},{M3,TT/2},{M1,TT/8},{M1,TT/4},{L6,TT/8},{L5,TT/4},{M5,TT/4},{M3,TT/8},{M2,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/2},{M2,TT/8},{M2,TT/4},//��ã���������ҵİ���������ѽ��ҡ
  {M1,TT/8},{M2,TT/8},{M2,TT/4},{M3,TT/8},{M2,TT/8},{M1,TT/8},{L6,TT/8},{M2,TT/8},{M1,TT/2},//��һ��ʱ�⡣��������������������������
  {M1,TT/8},{M1,TT/4},{L6,TT/8},{L5,TT/4},{L6,TT/4},{M1,TT/8},{M1,TT/4},{M2,TT/8},{M3,TT/2},{M2,TT/8},{M2,TT/4},{M1,TT/8},{M2,TT/4},{M2,TT/8},{M5,TT/8},{M5,TT/8},{M3,TT/8},
  {M3,TT/8},{M2,TT/8},{M3,TT/2},{M1,TT/8},{M1,TT/8},{M1,TT/8},{L6,TT/8},{L5,TT/4},{M5,TT/4},{M3,TT/8},{M2,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/2},{M2,TT/8},{M2,TT/4},//��ã���������ҵİ���������ѽ��ҡ
  {M1,TT/8},{M2,TT/8},{M2,TT/4},{M3,TT/8},{M2,TT/8},{M1,TT/8},{L6,TT/8},{M2,TT/8},{M1,TT/2},//ө��档����˭������һ����ҹ������
  {M3,TT/8},{M5,TT/4},{M5,TT/8},{M5,TT/4},{M5,TT/4},{M6,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/2},{M6,TT/8},{H1,TT/8},{M6,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},
  {L6,TT/8},{M2,TT/4},{M2,TT/8},{M3,TT/8},{M3,TT/8},{M2,TT/4+TT/8},{M3,TT/8},{M5,TT/4},{M5,TT/8},{M5,TT/4},{M5,TT/4},{M6,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},//��ã���������ҵİ���������ѽ��ҡ
  {M1,TT/2},{L6,TT/8},{M1,TT/8},{L6,TT/8},{L5,TT/8},{M2,TT/4},{M3,TT/4},{M1,TT/2+TT/4},	//����ֻ�����������������ˮ����
  {M1,TT/4+TT/8},{M5,TT/8},{M1,TT/8},{M5,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT},{M1,TT/4+TT/8},{M5,TT/8},{M1,TT/8},{M5,TT/8},{M1,TT/8},{M2,TT/8},{M2,TT},{M1,TT/4+TT/8},{M5,TT/8},{M1,TT/8},{M5,TT/8},
  {M2,TT/8},{M1,TT/8},{L6,TT/2},{L6,TT/8},{L5,TT/8},{M1,TT/8},{M2,TT/8},{M1,TT/4+TT/8},{M5,TT/8},{M1,TT/8},{M5,TT/8},{M1,TT/8},{L6,TT/8},{M1,TT},
  {M1,TT/8},{M1,TT/4},{L6,TT/8},{L5,TT/4},{L6,TT/4},{M1,TT/8},{M1,TT/4},{M2,TT/8},{M3,TT/2},{M2,TT/8},{M2,TT/4},{M1,TT/8},{M2,TT/4},{M2,TT/8},{M5,TT/8},{M5,TT/8},{M3,TT/8},
  {M3,TT/8},{M2,TT/8},{M3,TT/2},{M1,TT/8},{M1,TT/8},{M1,TT/8},{L6,TT/8},{L5,TT/4},{M5,TT/4},{M3,TT/8},{M2,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/2},{M2,TT/8},{M2,TT/4},//��ã���������ҵİ���������ѽ��ҡ
  {M1,TT/8},{M2,TT/8},{M2,TT/4},{M3,TT/8},{M2,TT/8},{M1,TT/8},{L6,TT/8},{M2,TT/8},{M1,TT/2},//ө��档����˭������һ����ҹ������
  {M3,TT/8},{M5,TT/4},{M5,TT/8},{M5,TT/4},{M5,TT/4},{M6,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/2},{M6,TT/8},{H1,TT/8},{M6,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},
  {L6,TT/8},{M2,TT/4},{M2,TT/8},{M3,TT/8},{M3,TT/8},{M2,TT/4+TT/8},{M3,TT/8},{M5,TT/4},{M5,TT/8},{M5,TT/4},{M5,TT/4},{M6,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},//��ã���������ҵİ���������ѽ��ҡ
  {M1,TT/2},{L6,TT/8},{M1,TT/8},{L6,TT/8},{L5,TT/8},{M2,TT/4},{M3,TT/4},{M1,TT/2+TT/4},	//����ֻ�����������������ˮ����
  {M1,TT/4+TT/8},{M5,TT/8},{M1,TT/8},{M5,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT},{M1,TT/4+TT/8},{M5,TT/8},{M1,TT/8},{M5,TT/8},{M1,TT/8},{M2,TT/8},{M2,TT},{M1,TT/4+TT/8},{M5,TT/8},{M1,TT/8},{M5,TT/8},
  {M2,TT/8},{M1,TT/8},{L6,TT/2},{L6,TT/8},{L5,TT/8},{M1,TT/8},{M2,TT/8},{M1,TT/4+TT/8},{M5,TT/8},{M1,TT/8},{M5,TT/8},{M1,TT/8},{L6,TT/8},{M1,TT},

  {0,0},
};
#endif

#if 0

// ��������������������硷
const tNote MyScore3[]=
{
  {L6,TT/4},{L3,TT/8},{L5,TT/8},{L6,TT/4},{L6,TT/8},{M1,TT/8},{M1,TT/4},{M2,TT/8},{M1,TT/8},{L6,TT/2},{M1,TT/4},{M1,TT/8},{L5,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT/8},{M5,TT/8},
  {M5,TT/8},{M3,TT/8},{M2,TT/4},{M3,TT/2},{M6,TT/8},{M6,TT/8},{M6,TT/8},{M5,TT/8},{M3,TT/8},{M3,TT/4},{M1,TT/8},{L6,TT/8},{L6,TT/8},{L6,TT/8},{M3,TT/8},//��ã���������ҵİ���������ѽ��ҡ
  {M2,TT/2},{M3,TT/8},{M3,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},{L6,TT/4},{L5,TT/4},{L6,TT/2}, //�ڣ�ʲô���ĸ����������
  {L6,TT/4},{L3,TT/8},{L5,TT/8},{L6,TT/4},{L6,TT/8},{M1,TT/8},{M1,TT/4},{M2,TT/8},{M1,TT/8},{L6,TT/2},{M1,TT/4},{M1,TT/8},{L5,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT/8},{M5,TT/8},
  {M5,TT/8},{M3,TT/8},{M2,TT/4},{M3,TT/2},{M6,TT/8},{M6,TT/8},{M6,TT/8},{M5,TT/8},{M3,TT/8},{M3,TT/4},{M1,TT/8},{L6,TT/8},{L6,TT/8},{L6,TT/8},{M3,TT/8},//��ã���������ҵİ���������ѽ��ҡ
  {M2,TT/2},{M3,TT/8},{M3,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},{L6,TT/4},{L5,TT/4},{L6,TT/2}, //�ڣ�ʲô���ĸ����������
  {M3,TT/8},{M3,TT/8},{M5,TT/8},{M3,TT/8},{M3,TT/8},{M5,TT/8},{M5,TT/8},{M6,TT/8},{H1,TT/8},{M6,TT/8},{M5,TT/4},{M6,TT/2},{L6,TT/4},{L6,TT/8},
  {L5,TT/8},{L6,TT/4},{M1,TT/4},{M2,TT/8},{M3,TT/16},{M2,TT/16},{M1,TT/8},{M2,TT/8},{M3,TT/2},{L6,TT/8},{M6,TT/8},{M6,TT/8},{M5,TT/8},{M2,TT/8},{M3,TT/16},{M2,TT/16},
  {M1,TT/8},{M2,TT/8},{M3,TT/2},//������
  {M1,TT/8},{L6,TT/8},{L6,TT/8},{M1,TT/8},{M2,TT/4},{L5,TT/8},{L5,TT/8},{M3,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/2},{L6,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},
  {L5,TT/8},{L3,TT/8},{L6,TT/2},{L6,TT/4},{L6,TT/8},{L5,TT/8},{L6,TT/4},{M1,TT/4},{M2,TT/8},{M3,TT/16},{M2,TT/16},{M1,TT/8},{M2,TT/8},{M3,TT/2},{L6,TT/8},{M6,TT/8},
  {M6,TT/8},{M5,TT/8},{M2,TT/8},{M3,TT/16},{M2,TT/16},
  {M1,TT/8},{M2,TT/8},{M3,TT/2},//������
  {M1,TT/8},{L6,TT/8},{L6,TT/8},{M1,TT/8},{M2,TT/4},{L5,TT/8},{L5,TT/8},{M3,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/4+TT/8},{M1,TT/8},{L6,TT/8},
  {M1,TT/8},{M2,TT/8},{M3,TT/8},{M5,TT/8},{M3,TT/8},{M3,TT/8},{M5,TT/8},{M6,TT/2},{M6,TT/2},{L6,TT/4},{L6,TT/8},{L5,TT/8},{L6,TT/4},
  {L6,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT/16},{M2,TT/16},{M1,TT/8},{M2,TT/8},{M3,TT/2},
  {M6,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},{M1,TT/2},//����������������̨
  {L6,TT/4},{L6,TT/8},{L5,TT/8},{L6,TT/4},{M1,TT/4},{M2,TT/8},{M3,TT/16},{M2,TT/16},{M1,TT/8},{M2,TT/8},{M3,TT/2},{L6,TT/8},{M6,TT/8},{M6,TT/8},{M5,TT/8},{M2,TT/8},{M3,TT/16},{M2,TT/16},
  {M1,TT/8},{M2,TT/8},{M3,TT/2},//������
  {M1,TT/8},{L6,TT/8},{L6,TT/8},{M1,TT/8},{M2,TT/4},{L5,TT/8},{L5,TT/8},{M3,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/2},{L6,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},
  {L5,TT/8},{L3,TT/8},{L6,TT/2},{L6,TT/4},{L6,TT/8},{L5,TT/8},{L6,TT/4},{M1,TT/4},{M2,TT/8},{M3,TT/16},{M2,TT/16},{M1,TT/8},{M2,TT/8},{M3,TT/2},{L6,TT/8},{M6,TT/8},
  {M6,TT/8},{M5,TT/8},{M2,TT/8},{M3,TT/16},{M2,TT/16},
  {M1,TT/8},{M2,TT/8},{M3,TT/2},//������
  {M1,TT/8},{L6,TT/8},{L6,TT/8},{M1,TT/8},{M2,TT/4},{L5,TT/8},{L5,TT/8},{M3,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/4+TT/8},{M1,TT/8},{L6,TT/8},
  {M1,TT/8},{M2,TT/8},{M3,TT/8},{M5,TT/8},{M3,TT/8},{M3,TT/8},{M5,TT/8},{M6,TT/2},{M6,TT/2},
  {0,0},
};
#endif

#if 1

// ����������������������
const tNote song_unlock[]=
{
  {M6,TT/12},{M6,TT/12},{M6,TT/12},
  {0,0},
};
#endif

#if 1

// ����������������������
const tNote song_lock[]=
{
  {M5,TT/12},{M1,TT/12},{M5,TT/12},
  {0,0},
};
#endif

// ������ֹͣ����
void buzzerQuiet(void)	
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	TIM_Cmd(TIM3, DISABLE);  //ֹͣTIM3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 //PB.5 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);	 //�����趨������ʼ��GPIOB.5
	GPIO_ResetBits(GPIOB,GPIO_Pin_5);	//PB.5 �����
}

/////////////////////////////////////////////////////////	 
//����������ָ��Ƶ�ʵ�����
//usFreq�Ƿ���Ƶ�ʣ�ȡֵ (ϵͳʱ��/65536)+1 �� 20000����λ��Hz
void buzzerSound(unsigned short usFreq)	 
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	unsigned long  ulVal;
	if((usFreq<=8000000/65536UL)||(usFreq>20000))
	{
		buzzerQuiet();// ����������
	}
	else
	{
		GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); //Timer3������ӳ��  TIM3_CH2->PB5    
		//���ø�����Ϊ�����������,���TIM3 CH2��PWM���岨��	GPIOB.5
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; //TIM_CH2
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIO	  
		ulVal=8000000/usFreq;
		//TIM3->ARR =ulVal ;	//�����Զ���װ�ؼĴ������ڵ�ֵ�������� 
		TIM_SetAutoreload(TIM3,ulVal);//�����Զ���װ�ؼĴ������ڵ�ֵ�������� 
		TIM_SetCompare2(TIM3,ulVal/2);//����
		TIM_Cmd(TIM3, ENABLE);  //����TIM3
	}  
}

void musicPlay(const tNote * MyScore)
{ 
	u8 i=0; 
	while(1) 
	{
		if (MyScore[i].mTime == 0) break; 
		buzzerSound(MyScore[i].mName); 
		delay_ms(MyScore[i].mTime);
		i++; 
		buzzerQuiet(); // ����������
		delay_ms(10);// 10 ms 
	} 
}

void buzzer_init(void)
{
	TIM3_PWM_Init(14399,10);	 //��Ƶ��PWMƵ��=72000/14400/11��Khz��
}

void buzzer_play(uint8_t type)
{
	switch(type)
	{
		case BUZZER_PLAY_UNLOCK:
			musicPlay(song_unlock);
			break;
		case BUZZER_PLAY_LOCK:
			musicPlay(song_lock);
			break;
		default:
			break;
	}
}

#if 0
int main(void) 
{ 
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	TIM3_PWM_Init(14399,10);	 //��Ƶ��PWMƵ��=72000/14400/11��Khz��
	for (;;) 
	{
		musicPlay(MyScore3);
		delay_ms(3000);
		buzzer_play(BUZZER_PLAY_UNLOCK);
		delay_ms(1000);
		buzzer_play(BUZZER_PLAY_LOCK);
		delay_ms(3000);
	} 
}
#endif

