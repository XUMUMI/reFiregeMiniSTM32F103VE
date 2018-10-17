#ifndef __LED_H
#define __LED_H

#include <stdbool.h>

typedef enum
{
    Color_Red = 0,
    Color_Green,
    Color_Blue
}ledColor;

void ledInit(void);
void ledSet(ledColor color, bool sw);

#endif
