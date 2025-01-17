#include "drv_gpio.h"
#include "stm32f1xx_hal.h"

// 12V-OUT, PA8
void GPIO_12vPower_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOA_CLK_ENABLE();
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET); 		// 默认输出高电平
}	

void GPIO_12vPower_Ctrl(const unsigned char status)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, (GPIO_PinState)status);
}

// 3V3D-M, PC0
void GPIO_3v3Power_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOC_CLK_ENABLE();
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET); 		// 默认输出高电平
}

void GPIO_3v3Power_Ctrl(const unsigned char status)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, (GPIO_PinState)status);
}

