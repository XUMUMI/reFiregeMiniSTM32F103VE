#ifndef __USATR_H
#define __USATR_H

#include <stdio.h>
#include <stdint.h>

#include "stm32f10x.h"

void usartInit(void);
void usartSendByte(uint16_t byte);
void usartSendStr(char *str);

#endif
