#ifndef	__DRV_I2C_H
#define __DRV_I2C_H

int Drv_I2C1_Init(void);
int Drv_I2C1_Write(const unsigned char address, const unsigned char* pdata, const unsigned short length);
int Drv_I2C1_Read(const unsigned char address, unsigned char* pdata, const unsigned short length);

#endif	/* __DRV_I2C_H */
