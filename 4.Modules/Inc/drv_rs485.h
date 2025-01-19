#ifndef __DRV_RS485_H
#define __DRV_RS485_H

int Drv_RS485_Init(void);
int Drv_RS485_Write(const unsigned char *pData, const unsigned int Length);
int Drv_RS485_Read(unsigned char *pData, const unsigned int Length);

#endif /* __DRV_RS485_H */
