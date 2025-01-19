#ifndef	__DRV_UART_H
#define __DRV_UART_H

int Drv_USART3_Init(void);
int Drv_USART3_Write(const unsigned char *pData, const unsigned short Length);
int Drv_USART3_Read(unsigned char *pData, const unsigned int Length);

#endif	/* __DRV_UART_H */
