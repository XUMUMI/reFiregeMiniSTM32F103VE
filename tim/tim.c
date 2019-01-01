#include "tim.h"

void TIMNVICConf(void);

void TIMInit(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = (1000 - 1);
    TIM_TimeBaseStructure.TIM_Prescaler = (72 - 1);
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
    TIMNVICConf();
    TIM_Cmd(TIM6, ENABLE);
}

void TIMDelay(uint32_t ms)
{
    TIM_ClearFlag(TIM6, TIM_FLAG_Update);
    TIM_SetCounter(TIM6, 0);
    TIM_Cmd(TIM6, ENABLE);
    for(uint32_t i = 0; i < ms; i++)
    {
        while(!TIM_GetFlagStatus(TIM6, TIM_FLAG_Update));
        TIM_ClearFlag(TIM6, TIM_FLAG_Update);
    }
    TIM_Cmd(TIM6, DISABLE);
}

void TIMNVICConf(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
