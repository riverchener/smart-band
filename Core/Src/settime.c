#include "stm32f103xb.h"
#include "stm32f1xx_hal.h"
#include "oled.h"
#include "h_rtc.h"
#include "key.h"
#include <stdint.h>
#include <stdio.h>

void Show_Settime_FirstUI(void)
{
  char month[50] = ""; // HH:MM:SS
  char date[50] = ""; // YYYY/MM/DD
  char year[50] = "";
  struct tm *now = H_HAL_RTC_GetTime();
  snprintf(year, sizeof(year), "年:%04d", now->tm_year + 1900);
  snprintf(month, sizeof(month), "月:%02d", now->tm_mon + 1);
  snprintf(date, sizeof(date), "日:%02d", now->tm_mday);
  OLED_DrawImage(0, 0, &return1Img, OLED_COLOR_NORMAL);
  OLED_PrintString(0, 16, year, &font16x16, OLED_COLOR_NORMAL);
  OLED_PrintString(0, 32, month, &font16x16, OLED_COLOR_NORMAL);
  OLED_PrintString(0, 48, date, &font16x16, OLED_COLOR_NORMAL);
}


void Show_Settime_SecondUI(void)
{
  char hour[50] = ""; // HH:MM:SS
  char min[50] = ""; // YYYY/MM/DD
  char sec[50] = "";
  struct tm *now = H_HAL_RTC_GetTime();
  snprintf(hour, sizeof(hour), "时:%02d", now->tm_hour);
  snprintf(min, sizeof(min), "分:%02d", now->tm_min);
  snprintf(sec, sizeof(sec), "秒:%02d", now->tm_sec);
  OLED_PrintString(0, 0, hour, &font16x16, OLED_COLOR_NORMAL);
  OLED_PrintString(0, 16, min, &font16x16, OLED_COLOR_NORMAL);
  OLED_PrintString(0, 32, sec, &font16x16, OLED_COLOR_NORMAL);
}

void Change_RTC_Time(uint8_t i,uint8_t flag)//flag 0为加一 1为减一;i为设置项 1年 2月 3日 4时 5分 6秒
{
  struct tm *now = H_HAL_RTC_GetTime();
  if(flag == 1){
    switch (i)
    {
      case 1:
        now->tm_year++;
        break;
      case 2:
        now->tm_mon++;
        break;
      case 3:
        now->tm_mday++;
        break;
      case 4:
        now->tm_hour++;
        break;
      case 5:
        now->tm_min++;
        break;
      case 6:
        now->tm_sec++;
        break;
    }
  }
  else if(flag == 0){
    switch (i)
    {
      case 1:
        now->tm_year--;
        break;
      case 2:
        now->tm_mon--;
        break;
      case 3:
        now->tm_mday--;
        break;
      case 4:
        now->tm_hour--;
        break;
      case 5:
        now->tm_min--;
        break;
      case 6:
        now->tm_sec--;
        break;
    }
  }
  H_HAL_RTC_SetTime(now);
}


extern uint8_t KeyNum;
int set_time_flag = 1;

int SetYear(void)
{
  while(1){
    KeyNum = Key_GetNum();
    if (KeyNum == 1)// 数值加一
    {
      Change_RTC_Time(1, 1);
    }
    else if (KeyNum == 2)// 数值减一
    {
      Change_RTC_Time(1, 0);
    }
    else if (KeyNum == 3)// 确认
    {
      
      return 0; // 返回设置界面标志
     }
   Show_Settime_FirstUI();
   OLED_ReverseArea(24, 16, 32, 16); 
   OLED_ShowFrame();
  }
}


int SetMonth(void)
{
   while(1){
    KeyNum = Key_GetNum();
    if (KeyNum == 1)// 数值加一
    {
      Change_RTC_Time(2, 1);
      struct tm *now = H_HAL_RTC_GetTime();
      if(now->tm_mon == 0)//O为1月，所以月份从十二月加到一月时需要年份减一，保持年份不变。
      {
        now->tm_year--;
        H_HAL_RTC_SetTime(now);
      }
    }
    else if (KeyNum == 2)// 数值减一
    {
      Change_RTC_Time(2, 0);
      struct tm *now = H_HAL_RTC_GetTime();
      if(now->tm_mon == 11)//11为12月，所以月份从一月减到十二月时需要年份加一，保持年份不变。
      {
        now->tm_year++;
        H_HAL_RTC_SetTime(now);
      }
    }
    else if (KeyNum == 3)// 确认
    {
      
      return 0; // 返回设置界面标志
     }
   Show_Settime_FirstUI();
   OLED_ReverseArea(24, 32, 16, 16); 
   OLED_ShowFrame();
    }
}

int SetDate(void)
{
   while(1){
    KeyNum = Key_GetNum();
    if (KeyNum == 1)// 数值加一
    {
      Change_RTC_Time(3, 1);
    }
    else if (KeyNum == 2)// 数值减一
    {
      Change_RTC_Time(3, 0);
    }
    else if (KeyNum == 3)// 确认
    {
      return 0; // 返回设置界面标志
     }
   Show_Settime_FirstUI();
   OLED_ReverseArea(24, 48, 16, 16); 
   OLED_ShowFrame();
  }
}


int SetHour(void)
{
   while(1){
    KeyNum = Key_GetNum();
    if (KeyNum == 1)// 数值加一
    {
      Change_RTC_Time(4, 1);
      /*struct tm *now = H_HAL_RTC_GetTime();
      if(now->tm_hour >= 24){
        now->tm_hour = 0;
        H_HAL_RTC_SetTime(now);
      }*/
    }
    else if (KeyNum == 2)// 数值减一
    {
      Change_RTC_Time(4, 0);
      /*struct tm *now = H_HAL_RTC_GetTime();
      if(now->tm_hour < 0){
        now->tm_hour = 23;
        H_HAL_RTC_SetTime(now);
      }*/
    }
    else if (KeyNum == 3)// 确认
    {
      return 0; // 返回设置界面标志
     }
   Show_Settime_SecondUI();
   OLED_ReverseArea(24, 0, 16, 16); 
   OLED_ShowFrame();
    }
}


int SetMin(void)
{
   while(1){
    KeyNum = Key_GetNum();
    if (KeyNum == 1)// 数值加一
    {
      Change_RTC_Time(5, 1);
      /*struct tm *now = H_HAL_RTC_GetTime();
      if(now->tm_min >= 60){
        now->tm_min = 0;
        H_HAL_RTC_SetTime(now);
      }*/
    }
    else if (KeyNum == 2)// 数值减一
    {
      Change_RTC_Time(5, 0);
      /*struct tm *now = H_HAL_RTC_GetTime();
      if(now->tm_min < 0){
        now->tm_min = 59;
        H_HAL_RTC_SetTime(now);
      }*/
    }
    else if (KeyNum == 3)// 确认
    {
      return 0; // 返回设置界面标志
     }
   Show_Settime_SecondUI();
   OLED_ReverseArea(24, 16, 16, 16); 
   OLED_ShowFrame();
    }
}

int SetSec(void)
{
   while(1){
    KeyNum = Key_GetNum();
    if (KeyNum == 1)// 数值加一
    {
      Change_RTC_Time(6, 1);
      /*struct tm *now = H_HAL_RTC_GetTime();
      if(now->tm_sec >= 60){
        now->tm_sec = 0;
        H_HAL_RTC_SetTime(now);
      }*/
    }
    else if (KeyNum == 2)// 数值减一
    {
      Change_RTC_Time(6, 0);
      /*struct tm *now = H_HAL_RTC_GetTime();
      if(now->tm_sec < 0){
        now->tm_sec = 59;
        H_HAL_RTC_SetTime(now);
      }*/
    }
    else if (KeyNum == 3)// 确认
    {
      return 0; // 返回设置界面标志
     }
   Show_Settime_SecondUI();
   OLED_ReverseArea(24, 32, 16, 16); 
   OLED_ShowFrame();
    }
}

int SetTime(void)
{
   while (1)
  {
    uint8_t set_time_flag_temp = 0;
    KeyNum = Key_GetNum();
    if (KeyNum == 1)// 上一项
    {
      set_time_flag--;
      if (set_time_flag <= 0)
      {
        set_time_flag = 7; 
      } 
    }
    else if (KeyNum == 2)// 下一项
    {
      set_time_flag++;
      if (set_time_flag >= 8)
      {
        set_time_flag = 1;
      }
    }
    else if (KeyNum == 3)// 确认
    {
      OLED_NewFrame();
      OLED_ShowFrame();
      set_time_flag_temp = set_time_flag;
      if(set_time_flag_temp == 1)
      {
        return 0; // 返回设置界面标志
      }
      else if(set_time_flag_temp == 2)
      {
        SetYear(); // 返回设置界面标志
      }
      else if(set_time_flag_temp == 3)
      {
        SetMonth(); // 返回设置界面标志
      }
      else if(set_time_flag_temp == 4)
      {
        SetDate(); // 返回设置界面标志
      }
      else if(set_time_flag_temp == 5)
      {
        SetHour(); // 返回设置界面标志
      }
      else if(set_time_flag_temp == 6)
      {
        SetMin(); // 返回设置界面标志
      }
      else if(set_time_flag_temp == 7)
      {
        SetSec(); // 返回设置界面标志
      }
     }

    switch (set_time_flag)
     {
      case 1:
        OLED_NewFrame();
        Show_Settime_FirstUI();
        OLED_ReverseArea(0, 0, 16, 16); // 反转“菜单”区域
        OLED_ShowFrame();
        break;
      case 2:
        OLED_NewFrame();
        Show_Settime_FirstUI();
        OLED_ReverseArea(0, 16, 16, 16); // 反转“设置”区域
        OLED_ShowFrame(); 
        break;
      case 3:
        OLED_NewFrame();
        Show_Settime_FirstUI();
        OLED_ReverseArea(0, 32, 16, 16); // 反转“设置”区域
        OLED_ShowFrame(); 
        break;
      case 4:
        OLED_NewFrame();
        Show_Settime_FirstUI();
        OLED_ReverseArea(0, 48, 16, 16); // 反转“设置”区域
        OLED_ShowFrame(); 
        break;
      case 5:
        OLED_NewFrame();
        Show_Settime_SecondUI();
        OLED_ReverseArea(0, 0, 16, 16); // 反转“设置”区域
        OLED_ShowFrame(); 
        break;
      case 6:
        OLED_NewFrame();
        Show_Settime_SecondUI();
        OLED_ReverseArea(0, 16, 16, 16); // 反转“设置”区域
        OLED_ShowFrame(); 
        break;
      case 7:
        OLED_NewFrame();    
        Show_Settime_SecondUI();
        OLED_ReverseArea(0, 32, 16, 16); // 反转“设置”区域
        OLED_ShowFrame(); 
        break;
     }
  }
}
