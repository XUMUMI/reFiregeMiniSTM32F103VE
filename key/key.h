#ifndef _KEY_H_
#define _KEY_H_

#include "stm32f10x.h"

#define KEY1_EXTI_Line EXTI_Line0
#define KEY2_EXTI_Line EXTI_Line13

#define KEY1_IRQHandler EXTI0_IRQHandler
#define KEY2_IRQHandler EXTI15_10_IRQHandler

void keyInit(void);

#endif
