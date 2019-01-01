#include "beep.h"
#include "stm32f10x.h"

void beepInit(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    beepSw(false);
}

void beepSw(bool sw)
{
	if(sw) GPIO_SetBits(GPIOA, GPIO_Pin_8);
	else GPIO_ResetBits(GPIOA, GPIO_Pin_8);
}
