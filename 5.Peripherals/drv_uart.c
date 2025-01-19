#include "drv_uart.h"
#include "stm32f1xx_hal.h"
#include "RingBuffer.h"

static UART_HandleTypeDef huart3;
static volatile unsigned char gRxData3 = 0;
static RingBuffer *pBuffer3 = NULL;

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

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART3)
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
        /* USART3 clock enable */
        __HAL_RCC_USART1_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**USART3 GPIO Configuration
        PA9     ------> USART1_TX
        PA10    ------> USART1_RX
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
    else if(uartHandle->Instance==USART3)
    {
        /* USART3 clock enable */
        __HAL_RCC_USART3_CLK_ENABLE();

        __HAL_RCC_GPIOD_CLK_ENABLE();
        /**USART3 GPIO Configuration
        PD8     ------> USART3_TX
        PD9     ------> USART3_RX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_8;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

        __HAL_AFIO_REMAP_USART3_ENABLE();

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

        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);
    }
    else if(uartHandle->Instance==USART3)
    {
        /* Peripheral clock disable */
        __HAL_RCC_USART3_CLK_DISABLE();

        /**USART3 GPIO Configuration
        PD8     ------> USART3_TX
        PD9     ------> USART3_RX
        */
        HAL_GPIO_DeInit(GPIOD, GPIO_PIN_8|GPIO_PIN_9);

        /* USART3 interrupt Deinit */
        HAL_NVIC_DisableIRQ(USART3_IRQn);
    }
}

