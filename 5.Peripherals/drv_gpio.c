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

// PC4-4G-STATUS, PC5-RS485-STATUS, 低电平点亮
void GPIO_4GModule_RS485_StatusLED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOC_CLK_ENABLE();
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4 | GPIO_PIN_5, GPIO_PIN_SET); 		// 默认输出高电平
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
 * TPL5010 通过 WAKE 向 MCU 周期的发送脉冲信号以唤醒 MCU , TPL5010 的周期为 2h 
 * MCU 必须在接收到下一个 WAKE 的脉冲 20ms 内通过 DONE 发送一高电平信号, 即 MCU 接收到 WAKE 后发送 DONE 信号
 * MCU 没发送的话, 则TPL5010通过 RSTn 重启 MCU
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
		// 喂狗
		
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
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET); 		// 默认输出高电平, 接收
}

/* 0-发送, 1-接收 */
void GPIO_RS485_CON_Ctrl(const unsigned char status)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, (GPIO_PinState)status);
}

/* PA11: 通信方式选择 */
void GPIO_CommunicationMode_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOA_CLK_ENABLE();
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pin = GPIO_PIN_11;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);	
}

/* 1-RS485, 0-4G Module */

unsigned char Get_CommunicationMode(void)
{
	return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11);
}

/* RTC GPIO 驱动
 * /INT:PA0-WKUP, 当 RX8111CE 检测带定时器、闹钟等事件, 会拉低此引脚
 */
void GPIO_RTC_WKUP_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOA_CLK_ENABLE();
	
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

void EXTI0_IRQHandler(void)
{
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_0) != 0x00u){
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
        // 从低功耗模式唤醒后的动作
    }
}

/* 4G Module: PC1-RESET, PC2-RELOAD */
void GPIO_4GModule_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOC_CLK_ENABLE();
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1 | GPIO_PIN_2, GPIO_PIN_SET);
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

// PC2-RELOAD, 用来使模块恢复出厂设置
void GPIO_4GModule_Reload(void)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
	HAL_Delay(5000);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
}

// PC1-RESET, 用来重启网络模块
void GPIO_4GModule_Reset(void)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
	HAL_Delay(50);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
}

// VM501
static unsigned short gVM501ChannelPins[] = {GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_14, GPIO_PIN_13, GPIO_PIN_12,
                                                   GPIO_PIN_5, GPIO_PIN_4, GPIO_PIN_3, GPIO_PIN_2, GPIO_PIN_11, GPIO_PIN_1, GPIO_PIN_0, GPIO_PIN_15};
static GPIO_TypeDef* gVM501ChannelPorts[] = {GPIOF, GPIOF, GPIOF, GPIOF, GPIOF, GPIOF, GPIOF, GPIOF,
                                                   GPIOG, GPIOG, GPIOG, GPIOG, GPIOD, GPIOG, GPIOG, GPIOF};
void GPIO_VM501Channel_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
    
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 
                            | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, GPIO_PIN_RESET);
    
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    
    GPIO_InitStruct.Pin = GPIO_PIN_11;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 
                            | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
}

void GPIO_VM501Channel_Enable(const unsigned char channel)
{
	HAL_GPIO_WritePin(gVM501ChannelPorts[channel - 1], gVM501ChannelPins[channel - 1], GPIO_PIN_SET);

}
void GPIO_VM501Channel_Disable(const unsigned char channel)
{
	HAL_GPIO_WritePin(gVM501ChannelPorts[channel - 1], gVM501ChannelPins[channel - 1], GPIO_PIN_RESET);
}

