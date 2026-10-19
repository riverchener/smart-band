#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_adc.h"
#include "stm32f1xx_hal_adc_ex.h"
#include "menu.h"
#include "oled.h"
#include "h_rtc.h"
//#include "font.h"
// #include <stdio.h>
// #include <stdint.h>
// #include <sys/types.h>
#include "key.h"
#include "settime.h"
#include "led.h"
#include "mpu6050.h"
#include "delay.h"
#include "math.h"
#include "dino.h"
#include "adc.h"
#include "format.h"


uint8_t KeyNum;
void Peripheral_Init(void)
{
  H_RTC_Init();
  // Key_Init();
  // LED_Init(); // MX_GPIO_Init已初始化
  MPU6050_Init();
  // ADC_Init();// MX_ADC1_Init已初始化
  HAL_ADCEx_Calibration_Start(&hadc1);//自动校准
  HAL_ADC_Start(&hadc1);//开启ADC
  HAL_ADC_PollForConversion(&hadc1,HAL_MAX_DELAY);//等待转换完成
}

/*-------------------------------------------首页时钟-----------------------------------------------------*/
uint16_t ADValue;
float VBAT;
char batt[10] = "";
char AD[10] = "";
int Battery_Capacity;

void Show_Battery(void)
{
  int sum = 0;

  for(int i = 0; i < 3000; i++)//采样3000次
  {
    sum += HAL_ADC_GetValue(&hadc1);
  }

  ADValue = sum / 3000;
  //VBAT = (float)ADValue / 4095 * 3.3;
  Battery_Capacity = (ADValue - 3275) * 100 / 819;
  if(Battery_Capacity < 0) Battery_Capacity = 0;
  
  //format_u16(AD, ADValue);
  format_u16_3(batt, Battery_Capacity);

  //OLED_PrintASCIIString(64, 0 ,AD,&afont8x6, OLED_COLOR_NORMAL);
  OLED_PrintASCIIString(85, 4 ,batt,&afont8x6, OLED_COLOR_NORMAL);
  OLED_PrintASCIIChar(103,4,'%', &afont8x6, OLED_COLOR_NORMAL);
  if(Battery_Capacity == 100)OLED_DrawImage(110, 0 , &batteryImg, OLED_COLOR_NORMAL);
  else if(Battery_Capacity>=10&&Battery_Capacity<100)
	{
		OLED_DrawImage(110, 0 , &batteryImg, OLED_COLOR_NORMAL);
		OLED_ClearArea((112+Battery_Capacity/10),5,(10-Battery_Capacity/10),6);
    OLED_ClearArea(85,4,6,8);
	}
	
	else
	{
		OLED_DrawImage(110, 0 , &batteryImg, OLED_COLOR_NORMAL);
		OLED_ClearArea(112,5,10,6);
    OLED_ClearArea(85,4,12,8);
	}
}


void Show_Clock_UI(void)
{
  char time[50] = ""; // HH:MM:SS
  char date[50] = ""; // YYYY/MM/DD
  struct tm *now = H_HAL_RTC_GetTime();
  format_hms(time, now->tm_hour, now->tm_min, now->tm_sec);
  format_ymd(date, now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);
  OLED_NewFrame();
  Show_Battery();
  OLED_PrintASCIIString(0, 0, date, &afont8x6, OLED_COLOR_NORMAL);
  OLED_PrintASCIIString(16, 16, time, &afont24x12, OLED_COLOR_NORMAL);
  OLED_PrintString(0, 48, "菜单", &font16x16, OLED_COLOR_NORMAL);
  OLED_PrintString(96, 48, "设置", &font16x16, OLED_COLOR_NORMAL);
}

int clkflag = 1;

int First_Page_Clock(void)
{
  while (1)
  {
    KeyNum = Key_GetNum();
    if (KeyNum == 1)// 上一项
    {
      clkflag--;
      if (clkflag <= 0)
      {
        clkflag = 2; 
      } 
    }
    else if (KeyNum == 2)// 下一项
    {
      clkflag++;
      if (clkflag >= 3)
      {
        clkflag = 1;
      } 
    }
    else if (KeyNum == 3)// 确认
    {
      OLED_NewFrame();
      OLED_ShowFrame();
      return clkflag;
     }

    switch (clkflag)
     {
      case 1:
        Show_Clock_UI();
        OLED_ReverseArea(0, 48, 32, 16); // 反转“菜单”区域
        OLED_ShowFrame();
        break;
      case 2:
        Show_Clock_UI();
        OLED_ReverseArea(96, 48, 32, 16); // 反转“设置”区域
        OLED_ShowFrame(); 
        break;
     }
  }
}

/*---------------------------------------------------设置界面-----------------------------------------------------*/

void Show_Settingpage_UI(void)
{
  OLED_DrawImage(0, 0, &return1Img, OLED_COLOR_NORMAL);
  OLED_PrintString(0, 16, "日期时间设置", &font16x16, OLED_COLOR_NORMAL);
}

int setflag = 1;

int SettingPage(void)
{
   while (1)
  {
    KeyNum = Key_GetNum();
    uint8_t setflag_temp = 0;
    if (KeyNum == 1)// 上一项
    {
      setflag--;
      if (setflag <= 0)
      {
        setflag = 2; 
      } 
    }
    else if (KeyNum == 2)// 下一项
    {
      setflag++;
      if (setflag >= 3)
      {
        setflag = 1;
      } 
    }
    else if (KeyNum == 3)// 确认
    {
      OLED_NewFrame();
      OLED_ShowFrame();
      setflag_temp = setflag;
     }
    
    if (setflag_temp == 1)
    {
      return 0; // 返回设置界面标志
    }
    else if (setflag_temp == 2)
    {
      SetTime(); // 返回设置界面标志
    }
    switch (setflag)
     {
      case 1:
        Show_Settingpage_UI();
        OLED_ReverseArea(0, 0, 16, 16); // 反转“菜单”区域
        OLED_ShowFrame();
        break;
      case 2:
        Show_Settingpage_UI();
        OLED_ReverseArea(0, 16, 96, 16); // 反转“设置”区域
        OLED_ShowFrame(); 
        break;
     }
  }
}


/*-------------------------------------------滑动菜单界面-----------------------------------------------------*/
uint8_t pre_selection;//当前选中的logo
uint8_t target_selection;//目标选中的logo
uint8_t x_step = 48;//logo当前的x坐标
uint8_t Speed = 4;//速度
uint8_t move_flag;//是否正在移动



void Menu_Animation(void)//菜单动画,通过调整logo的x坐标实现logo的移动,实现选中logo的替换。
{
  OLED_NewFrame();
  OLED_DrawImage(42,10,&cheekImg, OLED_COLOR_NORMAL);

  if(pre_selection < target_selection)// logo向左移动，按key2
  {
    x_step -= Speed;
    if(x_step == 0)
    {
      pre_selection++;
      move_flag = 0;
      x_step = 48;//重置x坐标，使x坐标追踪目标logo
    }
  }

  else if(pre_selection > target_selection)// logo向右移动,按key1
  {
    x_step += Speed;
    if(x_step == 96)
    {
      pre_selection--;
      move_flag = 0;
      x_step = 48;//重置x坐标,使x坐标追踪目标logo
    }
  }

  if(pre_selection >= 1){
    OLED_DrawImage(x_step-48,16,icons[pre_selection-1], OLED_COLOR_NORMAL);
  }

  if(pre_selection >= 2){
    OLED_DrawImage(x_step-96,16,icons[pre_selection-2], OLED_COLOR_NORMAL);
  }

  OLED_DrawImage(x_step,16,icons[pre_selection], OLED_COLOR_NORMAL);//显示当前选中logo

  if(pre_selection <= 6){
    OLED_DrawImage(x_step+48,16,icons[pre_selection+1], OLED_COLOR_NORMAL);
  }
  else{
    OLED_DrawImage(x_step+48,16,icons[0], OLED_COLOR_NORMAL); 
  }
  if(pre_selection <= 5){
    OLED_DrawImage(x_step+96,16,icons[pre_selection+2], OLED_COLOR_NORMAL);
  }
  else if(pre_selection == 6){
    OLED_DrawImage(x_step+96,16,icons[0], OLED_COLOR_NORMAL); 
  }
  OLED_ShowFrame();
}

void Set_Selection(uint8_t move_flag, uint8_t Pre_Selection, uint8_t Target_Selection)//设置选中logo,并调用动画函数实现logo的移动
{
    if(move_flag == 1)
    {
      pre_selection = Pre_Selection;
      target_selection = Target_Selection;
    } 
     Menu_Animation();
}

void MenuToFunction(void)//根据当前选中的logo下滑进入对应的功能界面
{
  for(uint8_t i = 0; i <= 6; i++){
  OLED_NewFrame();
    if(pre_selection >= 1){
      OLED_DrawImage(x_step-48,16+8*i,icons[pre_selection-1], OLED_COLOR_NORMAL);
    }

    OLED_DrawImage(x_step,16+8*i,icons[pre_selection], OLED_COLOR_NORMAL);//显示当前选中logo

    if(pre_selection <= 6){
      OLED_DrawImage(x_step+48,16+8*i,icons[pre_selection+1], OLED_COLOR_NORMAL);
    }
    else{
      OLED_DrawImage(x_step+48,16+8*i,icons[0], OLED_COLOR_NORMAL); 
    }
    OLED_ShowFrame();
  }
}

uint8_t menu_flag = 1;//目标项

int Menu(void)//菜单界面,通过调整menu_flag的值来确定当前选中的logo,并调用动画函数实现logo的移动
{
  move_flag = 1;
  uint8_t DirectFlag = 2; // 1: 框向左移动,logo向右移动 2: 框向右移动,logo向左移动
  while (1)
  {
    KeyNum = Key_GetNum();
    uint8_t menuflag_temp = 0;
    if (KeyNum == 1)// 框向左移动,logo向右移动，上一项
    {
      DirectFlag = 1;
      move_flag = 1;
      menu_flag--;
      if (menu_flag <= 0)
      {
        menu_flag = 7; 
      } 
    }
    else if (KeyNum == 2)// 框向右移动,logo向左移动，下一项
    {
      DirectFlag = 2;
      move_flag = 1;
      menu_flag++;
      if (menu_flag >= 8)
      {
        menu_flag = 1;
      } 
    }
    else if (KeyNum == 3)// 确认
    {
      OLED_NewFrame();
      OLED_ShowFrame();
      menuflag_temp = menu_flag;
     }
    
    if (menuflag_temp == 1)
    {
      return 0; // 返回设置界面标志
    }
    else if (menuflag_temp == 2)
    {
      MenuToFunction();
      StopWatch(); 
    }
    else if (menuflag_temp == 3)
    {
       MenuToFunction();
       LED(); 
    }
    else if (menuflag_temp == 4)
    {
      MenuToFunction();
      MPU6050();
    }
    else if (menuflag_temp == 5)
    {
      MenuToFunction();
      Game();
    }
    else if (menuflag_temp == 6)
    {
      MenuToFunction();
      Emoji();
    }
    else if (menuflag_temp == 7)
    {
      MenuToFunction();
      Gradienter();
    }
    /*switch (menu_flag)
     {
      case 1:
        if(DirectFlag == 1)
        {
          Set_Selection(move_flag, 1, 0);
        }
        else if(DirectFlag == 2)
        {
          Set_Selection(move_flag,0,0);
        }
        break;
      case 2:
        if(DirectFlag == 1)
        {
          Set_Selection(move_flag, 2, 1);
        }
        else if(DirectFlag == 2)
        {
          Set_Selection(move_flag,0,1);
        }
        break;
      case 3:
          if(DirectFlag == 1)
        {
          Set_Selection(move_flag, 3, 2);
        }
        else if(DirectFlag == 2)
        {
          Set_Selection(move_flag,1,2);
        }
        break;
      case 4:
          if(DirectFlag == 1)
        {
          Set_Selection(move_flag, 4, 3);
        }
        else if(DirectFlag == 2)
        {
          Set_Selection(move_flag,2,3);
        }
        break;
      case 5:
          if(DirectFlag == 1)
        {
          Set_Selection(move_flag, 5, 4);
        }
        else if(DirectFlag == 2)
        {
          Set_Selection(move_flag,3,4);
        }
        break;
      case 6:
          if(DirectFlag == 1)
        {
          Set_Selection(move_flag, 6, 5);
        }
        else if(DirectFlag == 2)
        {
          Set_Selection(move_flag,4,5);
        }
        break;
      case 7:
          if(DirectFlag == 1)
        {
          Set_Selection(move_flag, 7, 6);
        }
        else if(DirectFlag == 2)
        {
          Set_Selection(move_flag,5,6);
        }
        break;
     }*/
     if(menu_flag == 1)
     {
        if(DirectFlag == 1)
        {
          Set_Selection(move_flag, 1, 0);
        }
        else if(DirectFlag == 2)
        {
          Set_Selection(move_flag,0,0);
        }
     }
     else {
        if(DirectFlag == 1)
        {
          Set_Selection(move_flag, menu_flag, menu_flag-1);
        }
        else if(DirectFlag == 2)
        {
          Set_Selection(move_flag,menu_flag-2,menu_flag-1);
        }
     }
  }
}

/*---------------------------------------------------秒表界面-----------------------------------------------------*/

uint8_t hour ,min ,sec;

void Show_Stopwatch_UI(void)
{
  char time[50] = ""; // HH:MM:SS
  OLED_NewFrame();
  OLED_DrawImage(0, 0, &return1Img, OLED_COLOR_NORMAL);
  format_hms(time, hour, min, sec);
  OLED_PrintASCIIString(32, 20, time, &afont16x8, OLED_COLOR_NORMAL);
  OLED_PrintString(8, 44, "开始", &font16x16, OLED_COLOR_NORMAL);
  OLED_PrintString(48, 44, "停止", &font16x16, OLED_COLOR_NORMAL);
  OLED_PrintString(88, 44, "清除", &font16x16, OLED_COLOR_NORMAL);
}

uint8_t start_timing_flag = 0;

void StopWatch_Tick(void)
{
  static uint16_t Count;
  Count++;
    if(Count >= 1000)//每1000ms，时间加一秒
    {
      Count = 0;
    if(start_timing_flag == 1)
    {
      sec++;
      if(sec >= 60)
      {
        sec = 0;
        min++;
        if(min == 60)
        {
          min = 0;
          hour++;
          if(hour > 99)
          {
            hour = 0;
          }
        }
      }
      Show_Stopwatch_UI();
    }
  }
}

uint8_t stopwatch_flag = 1;

int StopWatch(void)
{
  while (1)
  {
    KeyNum = Key_GetNum();
    uint8_t stopwatch_flag_temp = 0;
    if (KeyNum == 1)// 上一项
    {
      stopwatch_flag--;
      if (stopwatch_flag <= 0)
      {
        stopwatch_flag = 4; 
      } 
    }
    else if (KeyNum == 2)// 下一项
    {
      stopwatch_flag++;
      if (stopwatch_flag >= 5)
      {
        stopwatch_flag = 1;
      } 
    }
    else if (KeyNum == 3)// 确认
    {
      stopwatch_flag_temp = stopwatch_flag;
     }
    
    if (stopwatch_flag_temp == 1)
    {
      return 0; // 返回设置界面标志
    }
    switch (stopwatch_flag)
     {
      case 1:
        Show_Stopwatch_UI();
        OLED_ReverseArea(0, 0, 16, 16); // 反转“菜单”区域
        OLED_ShowFrame();
        break;
      case 2:
        Show_Stopwatch_UI();
        start_timing_flag = 1;
        OLED_ReverseArea(8, 44, 32, 16); // 反转“设置”区域
        OLED_ShowFrame(); 
        break;
      case 3:
        Show_Stopwatch_UI();
        start_timing_flag = 0;
        OLED_ReverseArea(48, 44, 32, 16); // 反转“设置”区域
        OLED_ShowFrame(); 
        break;
      case 4:
        Show_Stopwatch_UI();
        start_timing_flag = 0;
        hour = 0;
        min = 0; 
        sec = 0; 
        OLED_ReverseArea(88, 44, 32, 16); // 反转“设置”区域
        OLED_ShowFrame();
        break;
     }
  }
}

/*---------------------------------------------------手电筒界面-----------------------------------------------------*/

void Show_LED_UI(void)
{
  OLED_NewFrame();
  OLED_DrawImage(0, 0, &return1Img, OLED_COLOR_NORMAL);
  OLED_PrintASCIIString(20, 20, "OFF", &afont24x12, OLED_COLOR_NORMAL);
  OLED_PrintASCIIString(72, 20, "ON", &afont24x12, OLED_COLOR_NORMAL);
}

uint8_t led_flag = 1;


int LED(void)
{
   while (1)
  {
    KeyNum = Key_GetNum();
    uint8_t led_flag_temp = 0;
    if (KeyNum == 1)// 上一项
    {
      led_flag--;
      if (led_flag <= 0)
      {
        led_flag = 3; 
      } 
    }
    else if (KeyNum == 2)// 下一项
    {
      led_flag++;
      if (led_flag >= 4)
      {
        led_flag = 1;
      } 
    }
    else if (KeyNum == 3)// 确认
    {
      OLED_NewFrame();
      OLED_ShowFrame();
      led_flag_temp = led_flag;
     }
    
    if (led_flag_temp == 1)
    {
      return 0; // 返回设置界面标志
    }
    switch (led_flag)
     {
      case 1:
        Show_LED_UI();
        OLED_ReverseArea(0, 0, 16, 16); // 反转“菜单”区域
        OLED_ShowFrame();
        break;
      case 2:
        Show_LED_UI();
        LED_Off();
        OLED_ReverseArea(20, 20, 36, 24); // 反转“设置”区域
        OLED_ShowFrame(); 
        break;
      case 3:
        Show_LED_UI();
        LED_On();
        OLED_ReverseArea(72, 20, 24, 24); // 反转“设置”区域
        OLED_ShowFrame(); 
        break;
    
     }
  }
}

/*-------------------------------------------MPU6050-----------------------------------------------------*/

int16_t ax,ay,az,gx,gy,gz;
float roll_g,pitch_g,yaw_g;
float roll_a,pitch_a;
float roll,pitch,yaw;
float a=0.9;
float Delta_t=0.005;
float pi=3.1415927f;

void MPU6050_Calculation(void)
{
	Delay_ms(5);
	MPU6050_GetData(&ax,&ay,&az,&gx,&gy,&gz);
	

	roll_g=roll+(float)gx*Delta_t;
	pitch_g=pitch+(float)gy*Delta_t;
	yaw_g=yaw+(float)gz*Delta_t;

	pitch_a=atan2f((float)(-ax), (float)az) * 180.0f / pi;
	roll_a=atan2f((float)ay, (float)az) * 180.0f / pi;

	roll=a*roll_g+(1-a)*roll_a;
	pitch=a*pitch_g+(1-a)*pitch_a;
	yaw=a*yaw_g;
	
}

void Show_MPU6050_UI(void)
{
  OLED_NewFrame();
  char Roll[50] = ""; 
  char Pitch[50] = ""; 
  char Yaw[50] = ""; 
  format_fixed2(Roll, "Roll:", roll);
  format_fixed2(Pitch, "Pitch:", pitch);
  format_fixed2(Yaw, "Yaw:", yaw);
  OLED_DrawImage(0, 0, &return1Img, OLED_COLOR_NORMAL);
  OLED_PrintASCIIString(0, 16, Roll, &afont16x8, OLED_COLOR_NORMAL);
  OLED_PrintASCIIString(0, 32, Pitch, &afont16x8, OLED_COLOR_NORMAL);
  OLED_PrintASCIIString(0, 48, Yaw, &afont16x8, OLED_COLOR_NORMAL);
}

int MPU6050(void)
{
  while(1){
    KeyNum = Key_GetNum();
    if(KeyNum == 3){
      return 0;
    }
    MPU6050_Calculation();
    Show_MPU6050_UI();
    OLED_ReverseArea(0,0,16, 16);
    OLED_ShowFrame();
   }
}

/*------------------------------------------游戏-----------------------------------------------------*/
void Show_Game_UI(void)
{
	OLED_NewFrame();
  OLED_DrawImage(0, 0, &return1Img, OLED_COLOR_NORMAL);
	OLED_PrintString(0, 16, "谷歌小恐龙", &font16x16, OLED_COLOR_NORMAL);
}

uint8_t game_flag=1;
int Game(void)
{
	while(1)
	{
		KeyNum=Key_GetNum();
		uint8_t game_flag_temp=0;
		if(KeyNum==1)//ÉÏÒ»Ïî
		{
			game_flag--;
			if(game_flag<=0)game_flag=2;
		}
		else if(KeyNum==2)//ÏÂÒ»Ïî
		{
			game_flag++;
			if(game_flag>=3)game_flag=1;
		}
		else if(KeyNum==3)//È·ÈÏ
		{
			game_flag_temp=game_flag;
		}
		
		if(game_flag_temp==1){return 0;}
		else if(game_flag_temp==2){DinoGame_Pos_Init();DinoGame_Animation();}
		
		switch(game_flag)
		{
			case 1:
				Show_Game_UI();
				OLED_ReverseArea(0,0,16,16);
				OLED_ShowFrame();
				break;
			
			case 2:
				Show_Game_UI();
				OLED_ReverseArea(0,16,80,16);
				OLED_ShowFrame();
				break;
		}
	}
}

/*----------------------------------动态表情包-------------------------------------*/

void Show_Emoji_UI(void)
{
	for(uint8_t i=0;i<3;i++)
	{
		OLED_NewFrame();
    OLED_DrawImage(30, 10+i, eyebrows[0], OLED_COLOR_NORMAL);
    OLED_DrawImage(82, 10+i, eyebrows[1], OLED_COLOR_NORMAL);
		OLED_DrawEllipse(40,32,6,6-i,OLED_COLOR_NORMAL);//×óÑÛ
		OLED_DrawEllipse(88,32,6,6-i,OLED_COLOR_NORMAL);//ÓÒÑÛ
    OLED_DrawImage(54,40,&mouthImg,OLED_COLOR_NORMAL);
		OLED_ShowFrame();
		Delay_ms(100);
	}
	
	/*ÕöÑÛ*/
	for(uint8_t i=0;i<3;i++)
	{
		OLED_NewFrame();
    OLED_DrawImage(30, 12-i, eyebrows[0], OLED_COLOR_NORMAL);
    OLED_DrawImage(82, 12-i, eyebrows[1], OLED_COLOR_NORMAL);
		OLED_DrawEllipse(40,32,6,4+i,OLED_COLOR_NORMAL);//×óÑÛ
		OLED_DrawEllipse(88,32,6,4+i,OLED_COLOR_NORMAL);//ÓÒÑÛ
    OLED_DrawImage(54,40,&mouthImg,OLED_COLOR_NORMAL);
		OLED_ShowFrame();
		Delay_ms(100);
	}
	
	Delay_ms(500);
	
}

int Emoji(void)
{
	while(1)
	{
		KeyNum=Key_GetNum();
		if(KeyNum==3)
		{
			OLED_NewFrame();
      OLED_ShowFrame();
			return 0;
		}
		Show_Emoji_UI();
	}
}

/*----------------------------------水平仪------------------------------------*/

void Show_Gradienter_UI(void)
{
	MPU6050_Calculation();
	OLED_DrawCircle(64,32,30,OLED_COLOR_NORMAL);
	OLED_DrawCircle(64-roll,32+pitch,4,OLED_COLOR_NORMAL);
}

int Gradienter(void)
{
	while(1)
	{
		KeyNum=Key_GetNum();
		if(KeyNum==3)
		{
			OLED_NewFrame();
      OLED_ShowFrame();
			return 0;
		}
		OLED_NewFrame();
		Show_Gradienter_UI();
		OLED_ShowFrame();
	}
}
