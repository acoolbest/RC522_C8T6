#include "delay.h"
#include "sys.h"
#include "pwm.h"
#include "buzzer.h"

#if 0
// 定义乐曲：刘德华《恭喜发财》
const tNote MyScore1[]=
{
  {L3,TT/8},{M6,TT/4},{M5,TT/4},{M6,TT/4},{M5,TT/8},{M3,TT/8},{M3,TT/4},{L3,TT/8},{M6,TT/4},{M5,TT/4},{M6,TT/4},{M5,TT/8},{M6,TT/8},{M6,TT/2},{M3,TT/8},{M2,TT/8+TT/16},{M3,TT/16},{M2,TT/8},
  {M1,TT/8},{L6,TT/4},{M3,TT/8},{M2,TT/8+TT/16},{M3,TT/16},{M2,TT/8},{M1,TT/8},{M2,TT/4},{M2,TT/8+TT/16},{M1,TT/8},{M1,TT/4},{M2,TT/4},{M3,TT/4},{M5,TT/4},{M6,TT},{M6,TT/8+TT/16},{M5,TT/16},
  {M3,TT/8},{M5,TT/8},{M6,TT/4}, //恭喜你发财。。。礼多人不怪
  
  {L3,TT/8},{L6,TT/4},{L6,TT/8},{L5,TT/8},{L6,TT/8},{L3,TT/8},{L3,TT/8},{L5,TT/8},{L6,TT/8},{M1,TT/8},{L6,TT/8},{L5,TT/8},{L6,TT/4},{L3,TT/8},{L5,TT/8},{M1,TT/4},{M1,TT/8},{M1,TT/8},{M2,TT/8},
  {M2,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT/2},{L3,TT/8},{M2,TT/4},{M2,TT/8},{M1,TT/8},{M2,TT/8},{L6,TT/8},{L6,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},{M2,TT/8},{M1,TT/8},{L6,TT/8},
  {M1,TT/8},{M3,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},{M3,TT/4},{L5,TT/4},{L6,TT/2},//我祝满天下的女孩。。。智商充满你脑袋

  {L3,TT/8},{L6,TT/4},{L6,TT/8},{L5,TT/8},{L6,TT/8},{L3,TT/8},{L3,TT/8},{L5,TT/8},{L6,TT/8},{M1,TT/8},{L6,TT/8},{L5,TT/8},{L6,TT/4},{L3,TT/8},{L5,TT/8},{M1,TT/4},{M1,TT/8},{M1,TT/8},{M2,TT/8},
  {M2,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT/2},{L3,TT/8},{M2,TT/4},{M2,TT/8},{M1,TT/8},{M2,TT/8},{L6,TT/8},{L6,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},{M2,TT/8},{M1,TT/8},{L6,TT/8},
  {M1,TT/8},{M3,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},{M3,TT/4},{L5,TT/4},{L6,TT/2},//我祝满天下的女孩。。。智商充满你脑袋

  {L5,TT/4},{M1,TT/2+TT/4},{M1,TT/8},{M2,TT/8},{M3,TT/2+TT/4},{M3,TT/8},{M5,TT/8},{M5,TT/4+TT/8},{M3,TT/8},{M2,TT/4},{M1,TT/4},{M2,TT/2},{M2,TT/4+TT/8},{L6,TT/8},{M2,TT/4},{M3,TT/4},
  {M4,TT/8+TT/16},{M5,TT/16},{M4,TT/8},{M3,TT/8},{M2,TT/2},{M5,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/4},{L5,TT/8},{L6,TT/2},//大摇大摆。。。要喊得够豪迈

  {L3,TT/8},{M6,TT/4},{M5,TT/4},{M6,TT/4},{M5,TT/8},{M3,TT/8},{M3,TT/4},{L3,TT/8},{M6,TT/4},{M5,TT/4},{M6,TT/4},{M5,TT/8},{M6,TT/8},{M6,TT/2},{M3,TT/8},{M2,TT/8+TT/16},{M3,TT/16},{M2,TT/8},
  {M1,TT/8},{L6,TT/4},{M3,TT/8},{M2,TT/8+TT/16},{M3,TT/16},{M2,TT/8},{M1,TT/8},{M2,TT/4},{M2,TT/8+TT/16},{M1,TT/8},{M1,TT/4},{M2,TT/4},{M3,TT/4},{M5,TT/4},{M6,TT},{M6,TT/8+TT/16},{M5,TT/16},
  {M3,TT/8},{M5,TT/8},{M6,TT/4}, //恭喜你发财。。。礼多人不怪

  {L3,TT/8},{L6,TT/4},{L6,TT/8},{L5,TT/8},{L6,TT/8},{L3,TT/8},{L3,TT/8},{L5,TT/8},{L6,TT/8},{M1,TT/8},{L6,TT/8},{L5,TT/8},{L6,TT/4},{L3,TT/8},{L5,TT/8},{M1,TT/4},{M1,TT/8},{M1,TT/8},{M2,TT/8},
  {M2,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT/2},{L3,TT/8},{M2,TT/4},{M2,TT/8},{M1,TT/8},{M2,TT/8},{L6,TT/8},{L6,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},{M2,TT/8},{M1,TT/8},{L6,TT/8},
  {M1,TT/8},{M3,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},{M3,TT/4},{L5,TT/4},{L6,TT/2},//我祝满天下的女孩。。。智商充满你脑袋

  {L5,TT/4},{M1,TT/2+TT/4},{M1,TT/8},{M2,TT/8},{M3,TT/2+TT/4},{M3,TT/8},{M5,TT/8},{M5,TT/4+TT/8},{M3,TT/8},{M2,TT/4},{M1,TT/4},{M2,TT/2},{M2,TT/4+TT/8},{L6,TT/8},{M2,TT/4},{M3,TT/4},
  {M4,TT/8+TT/16},{M5,TT/16},{M4,TT/8},{M3,TT/8},{M2,TT/2},{M5,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/4},{L5,TT/8},{L6,TT/2},//大摇大摆。。。要喊得够豪迈

  {L3,TT/8},{M6,TT/4},{M5,TT/4},{M6,TT/4},{M5,TT/8},{M3,TT/8},{M3,TT/4},{L3,TT/8},{M6,TT/4},{M5,TT/4},{M6,TT/4},{M5,TT/8},{M6,TT/8},{M6,TT/2},{M3,TT/8},{M2,TT/8+TT/16},{M3,TT/16},{M2,TT/8},
  {M1,TT/8},{L6,TT/4},{M3,TT/8},{M2,TT/8+TT/16},{M3,TT/16},{M2,TT/8},{M1,TT/8},{M2,TT/4},{M2,TT/8+TT/16},{M1,TT/8},{M1,TT/4},{M2,TT/4},{M3,TT/4},{M5,TT/4},{M6,TT},{M6,TT/8+TT/16},{M5,TT/16},
  {M3,TT/8},{M5,TT/8},{M6,TT/4}, //恭喜你发财。。。礼多人不怪

  {L3,TT/8},{M6,TT/4},{M5,TT/4},{M6,TT/4},{M5,TT/8},{M3,TT/8},{M3,TT/4},{L3,TT/8},{M6,TT/4},{M5,TT/4},{M6,TT/4},{M5,TT/8},{M6,TT/8},{M6,TT/2},{M3,TT/8},{M2,TT/8+TT/16},{M3,TT/16},{M2,TT/8},
  {M1,TT/8},{L6,TT/4},{M3,TT/8},{M2,TT/8+TT/16},{M3,TT/16},{M2,TT/8},{M1,TT/8},{M2,TT/4},{M2,TT/8+TT/16},{M1,TT/8},{M1,TT/4},{M2,TT/4},{M3,TT/4},{M5,TT/4},{M6,TT},{M6,TT/8+TT/16},{M5,TT/16},
  {M3,TT/8},{M5,TT/8},{M6,TT/4}, //恭喜你发财。。。礼多人不怪
  {0,0},
};
#endif

#if 0

// 定义乐曲：《荷塘月色》
const tNote MyScore2[]=
{
  {M1,TT/8},{M1,TT/4},{L6,TT/8},{L5,TT/4},{L6,TT/4},{M1,TT/4},{M1,TT/8},{M2,TT/8},{M3,TT/2},{M2,TT/8},{M2,TT/4},{M1,TT/8},{M2,TT/4},{M2,TT/8},{M5,TT/8},{M5,TT/8},{M3,TT/8},
  {M3,TT/8},{M2,TT/8},{M3,TT/2},{M1,TT/8},{M1,TT/4},{L6,TT/8},{L5,TT/4},{M5,TT/4},{M3,TT/8},{M2,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/2},{M2,TT/8},{M2,TT/4},//苍茫的天涯是我的爱。。。最呀最摇
  {M1,TT/8},{M2,TT/8},{M2,TT/4},{M3,TT/8},{M2,TT/8},{M1,TT/8},{L6,TT/8},{M2,TT/8},{M1,TT/2},//剪一段时光。。。美丽的琴音就落在我身旁
  {M1,TT/8},{M1,TT/4},{L6,TT/8},{L5,TT/4},{L6,TT/4},{M1,TT/8},{M1,TT/4},{M2,TT/8},{M3,TT/2},{M2,TT/8},{M2,TT/4},{M1,TT/8},{M2,TT/4},{M2,TT/8},{M5,TT/8},{M5,TT/8},{M3,TT/8},
  {M3,TT/8},{M2,TT/8},{M3,TT/2},{M1,TT/8},{M1,TT/8},{M1,TT/8},{L6,TT/8},{L5,TT/4},{M5,TT/4},{M3,TT/8},{M2,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/2},{M2,TT/8},{M2,TT/4},//苍茫的天涯是我的爱。。。最呀最摇
  {M1,TT/8},{M2,TT/8},{M2,TT/4},{M3,TT/8},{M2,TT/8},{M1,TT/8},{L6,TT/8},{M2,TT/8},{M1,TT/2},//萤火虫。。。谁采下那一朵昨夜的忧伤
  {M3,TT/8},{M5,TT/4},{M5,TT/8},{M5,TT/4},{M5,TT/4},{M6,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/2},{M6,TT/8},{H1,TT/8},{M6,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},
  {L6,TT/8},{M2,TT/4},{M2,TT/8},{M3,TT/8},{M3,TT/8},{M2,TT/4+TT/8},{M3,TT/8},{M5,TT/4},{M5,TT/8},{M5,TT/4},{M5,TT/4},{M6,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},//苍茫的天涯是我的爱。。。最呀最摇
  {M1,TT/2},{L6,TT/8},{M1,TT/8},{L6,TT/8},{L5,TT/8},{M2,TT/4},{M3,TT/4},{M1,TT/2+TT/4},	//我像只鱼儿。。。等你宛在水中央
  {M1,TT/4+TT/8},{M5,TT/8},{M1,TT/8},{M5,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT},{M1,TT/4+TT/8},{M5,TT/8},{M1,TT/8},{M5,TT/8},{M1,TT/8},{M2,TT/8},{M2,TT},{M1,TT/4+TT/8},{M5,TT/8},{M1,TT/8},{M5,TT/8},
  {M2,TT/8},{M1,TT/8},{L6,TT/2},{L6,TT/8},{L5,TT/8},{M1,TT/8},{M2,TT/8},{M1,TT/4+TT/8},{M5,TT/8},{M1,TT/8},{M5,TT/8},{M1,TT/8},{L6,TT/8},{M1,TT},
  {M1,TT/8},{M1,TT/4},{L6,TT/8},{L5,TT/4},{L6,TT/4},{M1,TT/8},{M1,TT/4},{M2,TT/8},{M3,TT/2},{M2,TT/8},{M2,TT/4},{M1,TT/8},{M2,TT/4},{M2,TT/8},{M5,TT/8},{M5,TT/8},{M3,TT/8},
  {M3,TT/8},{M2,TT/8},{M3,TT/2},{M1,TT/8},{M1,TT/8},{M1,TT/8},{L6,TT/8},{L5,TT/4},{M5,TT/4},{M3,TT/8},{M2,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/2},{M2,TT/8},{M2,TT/4},//苍茫的天涯是我的爱。。。最呀最摇
  {M1,TT/8},{M2,TT/8},{M2,TT/4},{M3,TT/8},{M2,TT/8},{M1,TT/8},{L6,TT/8},{M2,TT/8},{M1,TT/2},//萤火虫。。。谁采下那一朵昨夜的忧伤
  {M3,TT/8},{M5,TT/4},{M5,TT/8},{M5,TT/4},{M5,TT/4},{M6,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/2},{M6,TT/8},{H1,TT/8},{M6,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},
  {L6,TT/8},{M2,TT/4},{M2,TT/8},{M3,TT/8},{M3,TT/8},{M2,TT/4+TT/8},{M3,TT/8},{M5,TT/4},{M5,TT/8},{M5,TT/4},{M5,TT/4},{M6,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},//苍茫的天涯是我的爱。。。最呀最摇
  {M1,TT/2},{L6,TT/8},{M1,TT/8},{L6,TT/8},{L5,TT/8},{M2,TT/4},{M3,TT/4},{M1,TT/2+TT/4},	//我像只鱼儿。。。等你宛在水中央
  {M1,TT/4+TT/8},{M5,TT/8},{M1,TT/8},{M5,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT},{M1,TT/4+TT/8},{M5,TT/8},{M1,TT/8},{M5,TT/8},{M1,TT/8},{M2,TT/8},{M2,TT},{M1,TT/4+TT/8},{M5,TT/8},{M1,TT/8},{M5,TT/8},
  {M2,TT/8},{M1,TT/8},{L6,TT/2},{L6,TT/8},{L5,TT/8},{M1,TT/8},{M2,TT/8},{M1,TT/4+TT/8},{M5,TT/8},{M1,TT/8},{M5,TT/8},{M1,TT/8},{L6,TT/8},{M1,TT},

  {0,0},
};
#endif

#if 0

// 定义乐曲：《最炫民族风》
const tNote MyScore3[]=
{
  {L6,TT/4},{L3,TT/8},{L5,TT/8},{L6,TT/4},{L6,TT/8},{M1,TT/8},{M1,TT/4},{M2,TT/8},{M1,TT/8},{L6,TT/2},{M1,TT/4},{M1,TT/8},{L5,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT/8},{M5,TT/8},
  {M5,TT/8},{M3,TT/8},{M2,TT/4},{M3,TT/2},{M6,TT/8},{M6,TT/8},{M6,TT/8},{M5,TT/8},{M3,TT/8},{M3,TT/4},{M1,TT/8},{L6,TT/8},{L6,TT/8},{L6,TT/8},{M3,TT/8},//苍茫的天涯是我的爱。。。最呀最摇
  {M2,TT/2},{M3,TT/8},{M3,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},{L6,TT/4},{L5,TT/4},{L6,TT/2}, //摆，什么样的歌声才是最开怀
  {L6,TT/4},{L3,TT/8},{L5,TT/8},{L6,TT/4},{L6,TT/8},{M1,TT/8},{M1,TT/4},{M2,TT/8},{M1,TT/8},{L6,TT/2},{M1,TT/4},{M1,TT/8},{L5,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT/8},{M5,TT/8},
  {M5,TT/8},{M3,TT/8},{M2,TT/4},{M3,TT/2},{M6,TT/8},{M6,TT/8},{M6,TT/8},{M5,TT/8},{M3,TT/8},{M3,TT/4},{M1,TT/8},{L6,TT/8},{L6,TT/8},{L6,TT/8},{M3,TT/8},//苍茫的天涯是我的爱。。。最呀最摇
  {M2,TT/2},{M3,TT/8},{M3,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},{L6,TT/4},{L5,TT/4},{L6,TT/2}, //摆，什么样的歌声才是最开怀
  {M3,TT/8},{M3,TT/8},{M5,TT/8},{M3,TT/8},{M3,TT/8},{M5,TT/8},{M5,TT/8},{M6,TT/8},{H1,TT/8},{M6,TT/8},{M5,TT/4},{M6,TT/2},{L6,TT/4},{L6,TT/8},
  {L5,TT/8},{L6,TT/4},{M1,TT/4},{M2,TT/8},{M3,TT/16},{M2,TT/16},{M1,TT/8},{M2,TT/8},{M3,TT/2},{L6,TT/8},{M6,TT/8},{M6,TT/8},{M5,TT/8},{M2,TT/8},{M3,TT/16},{M2,TT/16},
  {M1,TT/8},{M2,TT/8},{M3,TT/2},//留下来
  {M1,TT/8},{L6,TT/8},{L6,TT/8},{M1,TT/8},{M2,TT/4},{L5,TT/8},{L5,TT/8},{M3,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/2},{L6,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},
  {L5,TT/8},{L3,TT/8},{L6,TT/2},{L6,TT/4},{L6,TT/8},{L5,TT/8},{L6,TT/4},{M1,TT/4},{M2,TT/8},{M3,TT/16},{M2,TT/16},{M1,TT/8},{M2,TT/8},{M3,TT/2},{L6,TT/8},{M6,TT/8},
  {M6,TT/8},{M5,TT/8},{M2,TT/8},{M3,TT/16},{M2,TT/16},
  {M1,TT/8},{M2,TT/8},{M3,TT/2},//留下来
  {M1,TT/8},{L6,TT/8},{L6,TT/8},{M1,TT/8},{M2,TT/4},{L5,TT/8},{L5,TT/8},{M3,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/4+TT/8},{M1,TT/8},{L6,TT/8},
  {M1,TT/8},{M2,TT/8},{M3,TT/8},{M5,TT/8},{M3,TT/8},{M3,TT/8},{M5,TT/8},{M6,TT/2},{M6,TT/2},{L6,TT/4},{L6,TT/8},{L5,TT/8},{L6,TT/4},
  {L6,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT/16},{M2,TT/16},{M1,TT/8},{M2,TT/8},{M3,TT/2},
  {M6,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},{M1,TT/2},//登上天外云霄的舞台
  {L6,TT/4},{L6,TT/8},{L5,TT/8},{L6,TT/4},{M1,TT/4},{M2,TT/8},{M3,TT/16},{M2,TT/16},{M1,TT/8},{M2,TT/8},{M3,TT/2},{L6,TT/8},{M6,TT/8},{M6,TT/8},{M5,TT/8},{M2,TT/8},{M3,TT/16},{M2,TT/16},
  {M1,TT/8},{M2,TT/8},{M3,TT/2},//留下来
  {M1,TT/8},{L6,TT/8},{L6,TT/8},{M1,TT/8},{M2,TT/4},{L5,TT/8},{L5,TT/8},{M3,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/2},{L6,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},
  {L5,TT/8},{L3,TT/8},{L6,TT/2},{L6,TT/4},{L6,TT/8},{L5,TT/8},{L6,TT/4},{M1,TT/4},{M2,TT/8},{M3,TT/16},{M2,TT/16},{M1,TT/8},{M2,TT/8},{M3,TT/2},{L6,TT/8},{M6,TT/8},
  {M6,TT/8},{M5,TT/8},{M2,TT/8},{M3,TT/16},{M2,TT/16},
  {M1,TT/8},{M2,TT/8},{M3,TT/2},//留下来
  {M1,TT/8},{L6,TT/8},{L6,TT/8},{M1,TT/8},{M2,TT/4},{L5,TT/8},{L5,TT/8},{M3,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/4+TT/8},{M1,TT/8},{L6,TT/8},
  {M1,TT/8},{M2,TT/8},{M3,TT/8},{M5,TT/8},{M3,TT/8},{M3,TT/8},{M5,TT/8},{M6,TT/2},{M6,TT/2},
  {0,0},
};
#endif

#if 1

// 定义乐曲：《开锁铃声》
const tNote song_unlock[]=
{
  {M6,TT/12},{M6,TT/12},{M6,TT/12},
  {0,0},
};
#endif

#if 1

// 定义乐曲：《关锁铃声》
const tNote song_lock[]=
{
  {M5,TT/12},{M1,TT/12},{M5,TT/12},
  {0,0},
};
#endif

// 蜂鸣器停止发声
void buzzerQuiet(void)	
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	TIM_Cmd(TIM3, DISABLE);  //停止TIM3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 //PB.5 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);	 //根据设定参数初始化GPIOB.5
	GPIO_ResetBits(GPIOB,GPIO_Pin_5);	//PB.5 输出低
}

/////////////////////////////////////////////////////////	 
//蜂鸣器发出指定频率的声音
//usFreq是发声频率，取值 (系统时钟/65536)+1 ～ 20000，单位：Hz
void buzzerSound(unsigned short usFreq)	 
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	unsigned long  ulVal;
	if((usFreq<=8000000/65536UL)||(usFreq>20000))
	{
		buzzerQuiet();// 蜂鸣器静音
	}
	else
	{
		GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); //Timer3部分重映射  TIM3_CH2->PB5    
		//设置该引脚为复用输出功能,输出TIM3 CH2的PWM脉冲波形	GPIOB.5
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; //TIM_CH2
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO	  
		ulVal=8000000/usFreq;
		//TIM3->ARR =ulVal ;	//设置自动重装载寄存器周期的值（音调） 
		TIM_SetAutoreload(TIM3,ulVal);//设置自动重装载寄存器周期的值（音调） 
		TIM_SetCompare2(TIM3,ulVal/2);//音量
		TIM_Cmd(TIM3, ENABLE);  //启动TIM3
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
		buzzerQuiet(); // 蜂鸣器静音
		delay_ms(10);// 10 ms 
	} 
}

void buzzer_init(void)
{
	TIM3_PWM_Init(14399,10);	 //分频。PWM频率=72000/14400/11（Khz）
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
	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	TIM3_PWM_Init(14399,10);	 //分频。PWM频率=72000/14400/11（Khz）
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

