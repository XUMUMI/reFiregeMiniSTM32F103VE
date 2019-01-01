#include "systick.h"

uint32_t SYSTICK_TIMING_DELAY;

void sysTickInit(void)
{
    SysTick_Config(SystemCoreClock / 1000);
}

void sysDelay(uint32_t nTime)
{ 
    SYSTICK_TIMING_DELAY = nTime;

    while(SYSTICK_TIMING_DELAY != 0);
}

void SysTick_Handler(void)
{
    if(SYSTICK_TIMING_DELAY) SYSTICK_TIMING_DELAY--;
}
