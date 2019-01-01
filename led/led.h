#ifndef __LED_H
#define __LED_H

#include <stdbool.h>

#include "stm32f10x.h"

typedef enum
{
    led_red   = GPIO_Pin_5,
    led_green = GPIO_Pin_0,
    led_blue  = GPIO_Pin_1
}ledcl;

void ledInit(void);
void ledSw(ledcl color, bool sw);

#endif
