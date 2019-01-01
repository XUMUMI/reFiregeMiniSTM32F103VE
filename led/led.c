#include "led.h"

void ledGPIOConf(void);

void ledInit(void)
{
    ledGPIOConf();
    GPIO_SetBits(GPIOB, led_red | led_green | led_blue); 
}

void ledSw(ledcl color, bool sw)
{
    if(sw) GPIO_ResetBits(GPIOB, color);
    else   GPIO_SetBits  (GPIOB, color);
}

void ledGPIOConf(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitTypeDef gpiostr;
    gpiostr.GPIO_Pin = led_red | led_green | led_blue;
    gpiostr.GPIO_Mode = GPIO_Mode_Out_PP;
    gpiostr.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &gpiostr);
}
