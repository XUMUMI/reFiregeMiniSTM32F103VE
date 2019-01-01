#ifndef _SYSTICK_H_
#define _SYSTICK_H_

#include "stm32f10x.h"

void sysTickInit(void);
void sysDelay(uint32_t nTime);

#endif
