#include "main.h"
#include <stdio.h>
#include "drv_gpio.h"
#include "drv_adc.h"
#include "drv_rs485.h"
#include "drv_i2c.h"
#include "drv_uart.h"
#include "drv_net.h"
#include "drv_vm501.h"
#include "drv_flash.h"

#include "FreeRTOS.h"
#include "task.h"

void SystemClock_Config(void);

void test(void *param)
{
    while(Get_MQTT_State() != 0);
	static unsigned int i = 0; 
	while(1)
	{
		i++;
		float Voltage = ADC1_GetVoltage();
		printf("Voltage is %f\r", Voltage);
		vTaskDelay(1000);
		
		char buf[64] = {0};
        int ret = MQTT_Get_Msg((unsigned char*)buf, sizeof(buf));
        if(ret > 0)
        {
            printf("%s", buf);
        }
		
//		GPIO_4GModule_StatusLED_Ctrl(0);
//		GPIO_RS485_StatusLED_Ctrl(0);
		vTaskDelay(1);
	}
}

int main(void)
{
	HAL_Init();
	SystemClock_Config();
	GPIO_12vPower_Init();
	GPIO_3v3Power_Init();
	GPIO_4GModule_RS485_StatusLED_Init();
	Drv_I2C1_Init();	// RTC
	GPIO_RTC_WKUP_Init();
	Drv_USART1_Init();	// RS232
	
	ADC1_Init();
	Drv_VM501_Init();
	
	unsigned short Freq = Drv_VM501_ReadFrequence(1);
	
	printf("Frequency is %d\r\n", Freq);
	
	printf("Hello World!\r\n");		// RS232 ���ڲ���
	
	unsigned char CommunicationMode = Get_CommunicationMode();
	if(CommunicationMode == 1)//RS485
	{
		Drv_RS485_Init();
	}
	else if(CommunicationMode == 0)
	{
		Drv_4GModule_Init();
	}
	
	printf("Communicate Mode: %d\r\n", Get_CommunicationMode());
	
	xTaskCreate(test, "test", 128, NULL, 1, NULL);
	
	vTaskStartScheduler();
	
	return 0;
}

void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Initializes the RCC Oscillators according to the specified parameters
	* in the RCC_OscInitTypeDef structure.
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
	                            |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		Error_Handler();
	}
}


void Error_Handler(void)
{
	__disable_irq();
	while (1)
	{
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM8) {
		HAL_IncTick();
	}
}

