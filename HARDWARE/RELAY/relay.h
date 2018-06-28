#ifndef __RELAY_H
#define __RELAY_H	 
#include "sys.h"

void relay_init(void);//≥ı ºªØ

#define RELAY_ON	1
#define RELAY_OFF	0
#define RELAY_CTRL PBout(7)	// PB7
				    
#endif
