#include "relay.h"

void relay_init(void)
{
  GPIO_InitTypeDef 
  GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//���
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}          

//relay on PB4
void relay_on(void)
{
  GPIO_SetBits(GPIOB,GPIO_Pin_4);
}
//relay off PB4
void relay_off(void)
{
  GPIO_ResetBits(GPIOB,GPIO_Pin_4);
}
 
