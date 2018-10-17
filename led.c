#include "stm32f10x.h"
#include "led.h"

void ledGPIOConf(void);

void ledInit(void)
{
    ledGPIOConf();
    
    GPIO_SetBits(GPIOB,GPIO_Pin_5);    
    GPIO_SetBits(GPIOB,GPIO_Pin_0);
    GPIO_SetBits(GPIOB,GPIO_Pin_1);    
}

void ledSet(ledColor color, bool sw)
{
    uint16_t GPIO_Pin;
    switch(color)
    {
        case Color_Red:   GPIO_Pin = GPIO_Pin_5; break;
        case Color_Green: GPIO_Pin = GPIO_Pin_0; break;
        case Color_Blue:  GPIO_Pin = GPIO_Pin_1; break;
    }
    if(sw) GPIO_ResetBits(GPIOB,GPIO_Pin);
    else GPIO_SetBits(GPIOB,GPIO_Pin);
}

void ledGPIOConf(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitTypeDef gpiostr;
    gpiostr.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_5;
    gpiostr.GPIO_Mode = GPIO_Mode_Out_PP;
    gpiostr.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &gpiostr);
}
