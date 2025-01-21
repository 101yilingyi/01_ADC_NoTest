#include "drv_net.h"
#include "stm32f1xx_hal.h"
#include "drv_uart.h"
#include "drv_gpio.h"

int Drv_4GModule_Init(void)
{
	GPIO_4GModule_Init();
	return Drv_USART2_Init();
}

