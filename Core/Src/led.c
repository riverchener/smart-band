#include "gpio.h"
#include "delay.h"

// void LED_Init(void)
// {
//     GPIO_InitTypeDef GPIO_InitStruct = {0};

//     __HAL_RCC_GPIOB_CLK_ENABLE();

//     HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
    
//     GPIO_InitStruct.Pin = LED_Pin;
//     GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//     GPIO_InitStruct.Pull = GPIO_NOPULL;
//     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//     HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);
// }

void LED_On(void)
{
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET); // 低电平点亮LED

}

void LED_Off(void)
{
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET); // 高电平关闭LED
}