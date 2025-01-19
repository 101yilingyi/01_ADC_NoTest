#include "drv_rs485.h"
#include "stm32f1xx_hal.h"
#include "drv_gpio.h"
#include "drv_uart.h"

int Drv_RS485_Init(void)
{
	GPIO_RS485_CON_Init();
	return Drv_USART3_Init();
}

int Drv_RS485_Write(const unsigned char *pData, const unsigned int Length)
{
	// 控制RS485为发送模式
	GPIO_RS485_CON_Ctrl(0);
	int len = Length;
	while(len != 0)
	{
		unsigned short size = len;
		if(len > 0xFFFF)	size = 0xFFFF;
		if(Drv_USART3_Write(pData, size) != size)
		{
			GPIO_RS485_CON_Ctrl(1);
			return -1;
		}
		len -= size;
		pData += size;
	}
	GPIO_RS485_CON_Ctrl(1);
	return Length;
}
	
int Drv_RS485_Read(unsigned char *pData, const unsigned int Length)
{
	return Drv_USART3_Read(pData, Length);
}
