#include "delay.h"

void delay_ns(uint16_t ns)
{
    uint16_t mult;
    while(ns--)
    {
        mult = 8;
        while(mult--);
    }
}

void delay_ms(uint16_t ms)
{
    while(ms--) delay_ns(1000);
}

void delay_s(uint16_t s)
{
    while(s--) delay_ms(1000);
}
