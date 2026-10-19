#ifndef INC__H_RTC_H__
#define INC__H_RTC_H__

#include "stm32f1xx_hal.h"
#include "rtc.h"
#include "time.h"

HAL_StatusTypeDef H_HAL_RTC_SetTime(struct tm *time);
struct tm* H_HAL_RTC_GetTime(void);
void H_RTC_Init(void);

#endif // __H_RTC_H__