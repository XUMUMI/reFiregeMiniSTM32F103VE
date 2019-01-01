#include "usart.h"

void usartGPIOConf(void);
void usartConf(void);
void usartNVICConf(void);

void usartInit(void)
{
    usartGPIOConf();
    usartConf();
    usartNVICConf();
    
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);
}

void usartGPIOConf(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    GPIO_InitTypeDef gpiostr;
    
    gpiostr.GPIO_Mode = GPIO_Mode_AF_PP;
    gpiostr.GPIO_Pin = GPIO_Pin_9;
    gpiostr.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpiostr);
    
    gpiostr.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpiostr.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &gpiostr);
}

void usartConf(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    
    USART_InitTypeDef usartstr;
    usartstr.USART_BaudRate = 115200;
    usartstr.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usartstr.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    usartstr.USART_Parity = USART_Parity_No;
    usartstr.USART_StopBits = USART_StopBits_1;
    usartstr.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1, &usartstr);
}

void usartNVICConf(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
    NVIC_InitTypeDef NVICstr;
    NVICstr.NVIC_IRQChannel = USART1_IRQn;
    NVICstr.NVIC_IRQChannelCmd = ENABLE;
    NVICstr.NVIC_IRQChannelPreemptionPriority = 1;
    NVICstr.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVICstr);
}

void usartSendByte(uint16_t byte)
{
    USART_SendData(USART1, byte);
    while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE)) continue;
}

void usartSendStr(char *str)
{
    while(*str != '\0') usartSendByte(*(str++));
    while(!USART_GetFlagStatus(USART1, USART_FLAG_TC)) continue;
}

int fputc(int ch, FILE *f)
{
    usartSendByte(ch);
    return ch;
}

int fgetc(FILE *f)
{
    while(!USART_GetFlagStatus(USART1, USART_FLAG_RXNE));
    return (int)USART_ReceiveData(USART1);
}
