#include <stdint.h>
#include <sys/types.h>
#include "key.h"
#include "gpio.h"

uint8_t Key_Num;

// void Key_Init(void)
// {
//    GPIO_InitTypeDef GPIO_InitStruct = {0};

//   __HAL_RCC_GPIOA_CLK_ENABLE();
//   __HAL_RCC_GPIOB_CLK_ENABLE();

//   GPIO_InitStruct.Pin = KEY3_Pin|KEY2_Pin;
//   GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//   GPIO_InitStruct.Pull = GPIO_PULLUP;
//   HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


//   GPIO_InitStruct.Pin = KEY1_Pin;
//   GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//   GPIO_InitStruct.Pull = GPIO_PULLUP;
//   HAL_GPIO_Init(KEY1_GPIO_Port, &GPIO_InitStruct);

//   //MX_GPIO_Init();
// }


uint8_t Key_GetNum(void)
{
  uint8_t temp;
  if(Key_Num)
  {
    temp = Key_Num;
    Key_Num = 0; // 读取后清零
    return temp; // 返回按键编号
  }
  else{
    return 0; // 没有按键事件
  }
}


uint8_t Key_Getstate(void)
{
  uint8_t state = 0;
  if(HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET)
  {
    state = 1; // KEY1按下
  }
  if(HAL_GPIO_ReadPin(GPIOA,KEY2_Pin) == GPIO_PIN_RESET)
  {
    state = 2; // KEY2按下
  }   
  if(HAL_GPIO_ReadPin(GPIOA, KEY3_Pin) == GPIO_PIN_RESET)
  {
    state = 3; // KEY3按下
  }

  return state;
}

void Key_Tick(void)
{
  static uint8_t Count;
  static uint8_t CurrentState,PreState;
  Count++;
  if(Count >= 20) // 每10ms检测一次按键状态
  {
    Count = 0;
    PreState = CurrentState;
    CurrentState = Key_Getstate();
    if(CurrentState == 0 && PreState != 0) // 按键释放事件
    {
      Key_Num = PreState; // 记录按键编号
    }
  }
}