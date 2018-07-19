#ifndef __RELAY_H
#define __RELAY_H	 
#include "sys.h"

#define RELAY_ON	1
#define RELAY_OFF	0
#define RELAY_CTRL PBout(4)	// PB4

void relay_on(void);
void relay_off(void);
void relay_init(void);

#endif
