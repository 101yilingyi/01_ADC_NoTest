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
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET); 		// Ĭ������ߵ�ƽ
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
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET); 		// Ĭ������ߵ�ƽ
}

void GPIO_3v3Power_Ctrl(const unsigned char status)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, (GPIO_PinState)status);
}

// PC4-4G-STATUS, PC5-RS485-STATUS, �͵�ƽ����
void GPIO_4GModule_RS485_StatusLED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOC_CLK_ENABLE();
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4 | GPIO_PIN_5, GPIO_PIN_SET); 		// Ĭ������ߵ�ƽ
}

void GPIO_4GModule_StatusLED_Ctrl(const unsigned char status)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, (GPIO_PinState)status);
}

void GPIO_RS485_StatusLED_Ctrl(const unsigned char status)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, (GPIO_PinState)status);
}

/* WatchDog Pins: PB1-WD-WAKE, PB8-WD-DONE, RESET 
 * TPL5010 ͨ�� WAKE �� MCU ���ڵķ��������ź��Ի��� MCU , TPL5010 ������Ϊ 2h 
 * MCU �����ڽ��յ���һ�� WAKE ������ 20ms ��ͨ�� DONE ����һ�ߵ�ƽ�ź�, �� MCU ���յ� WAKE ���� DONE �ź�
 * MCU û���͵Ļ�, ��TPL5010ͨ�� RSTn ���� MCU
 */
void GPIO_WatchDog_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pin = GPIO_PIN_1;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pin = GPIO_PIN_8;
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void EXTI1_IRQHandler(void)
{
	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_1) != 0x00u){
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_1);
		// ι��
		
	}
}

void WatchDog_Feed(void)	// DONE 
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
}

/* RS485: PD10-CON-RS485 */
void GPIO_RS485_CON_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOD_CLK_ENABLE();
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET); 		// Ĭ������ߵ�ƽ, ����
}

/* 0-����, 1-���� */
void GPIO_RS485_CON_Ctrl(const unsigned char status)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, (GPIO_PinState)status);
}

