#ifndef __KEY_H
#define __KEY_H

#include "stm32f1xx_hal.h"

// void Key_Init(void);
uint8_t Key_GetNum(void);
void Key_Tick(void);

#endif /* __KEY_H */