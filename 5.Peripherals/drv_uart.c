#include "drv_uart.h"
#include "stm32f1xx_hal.h"
#include "RingBuffer.h"

#include <stdio.h>

static UART_HandleTypeDef huart1;
static UART_HandleTypeDef huart2;
static UART_HandleTypeDef huart3;

static volatile unsigned char gRxData2 = 0;
static volatile unsigned char gRxData3 = 0;

static RingBuffer *pBuffer2 = NULL;
static RingBuffer *pBuffer3 = NULL;

int Drv_USART1_Init(void)
{
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		return -1;
	}
	return 0;
}

// 4G Module
int Drv_USART2_Init(void)
{
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart2) != HAL_OK){
		return -1;
	}
	pBuffer2 = RingBufferNew(512);
	if(pBuffer2 == NULL)	return -1;
	
	if(HAL_UART_Receive_IT(&huart2, (unsigned char *)&gRxData2, 1) != HAL_OK)
	{
		return -1;
	}
	return 0;
}

int Drv_USART2_Write(const unsigned char *pData, const unsigned short Length)
{
	HAL_StatusTypeDef ret = HAL_UART_Transmit(&huart2, pData, Length, HAL_MAX_DELAY);
	if(ret != HAL_OK)	return -1;
	return Length;
}

int Drv_USART2_Read(unsigned char *pData, const unsigned int Length)
{
	if(pBuffer2 == NULL)	return -1;
	return pBuffer2->Read(pBuffer2, pData, Length);
}

void USART2_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart2);
}


// RS485
int Drv_USART3_Init(void)
{
    huart3.Instance = USART3;
    huart3.Init.BaudRate = 9600;
    huart3.Init.WordLength = UART_WORDLENGTH_8B;
    huart3.Init.StopBits = UART_STOPBITS_1;
    huart3.Init.Parity = UART_PARITY_NONE;
    huart3.Init.Mode = UART_MODE_TX_RX;
    huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart3.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart3) != HAL_OK){
        return -1;
    }
    
    pBuffer3 = RingBufferNew(512);
    if(NULL == pBuffer3){
        return -1;
    }
    
    if(HAL_UART_Receive_IT(&huart3, (unsigned char*)&gRxData3, 1) != HAL_OK){
        return -1;
    }
    return 0;
}

int Drv_USART3_Write(const unsigned char *pData, const unsigned short Length)
{
	HAL_StatusTypeDef ret = HAL_UART_Transmit(&huart3, pData, Length, HAL_MAX_DELAY);
	if(ret != HAL_OK)	return -1;
	return Length;
}

int Drv_USART3_Read(unsigned char *pData, const unsigned int Length)
{
	if(pBuffer3 == NULL)	return -1;
	return pBuffer3->Read(pBuffer3, pData, Length);
}

void UART3_IRQHandler(void)
{
	HAL_UART_IRQHandler(&huart3);
}

// ÖØ¶¨Ïòprintf
struct __FILE{
    int handle;
};

FILE __stdout;

int fputc(int ch, FILE* f)
{
    if(HAL_UART_Transmit(&huart1, (unsigned char*)&ch, 1, HAL_MAX_DELAY) != HAL_OK)
    {
        return 0;
    }
    return ch;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART2)
	{
		if(pBuffer2 != NULL)
		{
			pBuffer2->Write(pBuffer2, (unsigned char *)&gRxData2, 1);
		}
		HAL_UART_Receive_IT(&huart2, (unsigned char *)&gRxData2, 1);
	}
	else if(huart->Instance == USART3)
	{
		if(pBuffer3 != NULL)
		{
			pBuffer3->Write(pBuffer3, (unsigned char *)&gRxData3, 1);
		}
		HAL_UART_Receive_IT(&huart3, (unsigned char *)&gRxData3, 1);
	}
}
	
void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(uartHandle->Instance==USART1)
		{
		/* USART1 clock enable */
		__HAL_RCC_USART1_CLK_ENABLE();
		
		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**USART1 GPIO Configuration
		PA9     ------> USART1_TX
		PA10     ------> USART1_RX
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_9;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		
		GPIO_InitStruct.Pin = GPIO_PIN_10;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	}
	else if(uartHandle->Instance==USART2)
	{
		/* USART2 clock enable */
		__HAL_RCC_USART2_CLK_ENABLE();
	
		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**USART2 GPIO Configuration
		PA2     ------> USART2_TX
		PA3     ------> USART2_RX
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_2;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
		GPIO_InitStruct.Pin = GPIO_PIN_3;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
		/* USART2 interrupt Init */
		HAL_NVIC_SetPriority(USART2_IRQn, 5, 0);
		HAL_NVIC_EnableIRQ(USART2_IRQn);
	}
	else if(uartHandle->Instance==USART3)
	{
		/* USART3 clock enable */
		__HAL_RCC_USART3_CLK_ENABLE();
		
		__HAL_RCC_GPIOB_CLK_ENABLE();
		/**USART3 GPIO Configuration
		PB10     ------> USART3_TX
		PB11     ------> USART3_RX
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_10;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		
		GPIO_InitStruct.Pin = GPIO_PIN_11;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		
		/* USART3 interrupt Init */
		HAL_NVIC_SetPriority(USART3_IRQn, 5, 0);
		HAL_NVIC_EnableIRQ(USART3_IRQn);
	}
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{
	if(uartHandle->Instance==USART1)
	{
		/* Peripheral clock disable */
		__HAL_RCC_USART1_CLK_DISABLE();
		
		/**USART1 GPIO Configuration
		PA9     ------> USART1_TX
		PA10     ------> USART1_RX
		*/
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);
	}
	else if(uartHandle->Instance==USART2)
	{
		/* Peripheral clock disable */
		__HAL_RCC_USART2_CLK_DISABLE();
		
		/**USART2 GPIO Configuration
		PA2     ------> USART2_TX
		PA3     ------> USART2_RX
		*/
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);
		
		/* USART2 interrupt Deinit */
		HAL_NVIC_DisableIRQ(USART2_IRQn);
	}
	else if(uartHandle->Instance==USART3)
	{
		/* Peripheral clock disable */
		__HAL_RCC_USART3_CLK_DISABLE();
		
		/**USART3 GPIO Configuration
		PB10     ------> USART3_TX
		PB11     ------> USART3_RX
		*/
		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_11);
		
		/* USART3 interrupt Deinit */
		HAL_NVIC_DisableIRQ(USART3_IRQn);
	}
}

