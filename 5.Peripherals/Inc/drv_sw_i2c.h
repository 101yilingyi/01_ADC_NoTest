#ifndef __DRV_SW_I2C_H
#define __DRV_SW_I2C_H

void Drv_SW_I2C2_Init(void);
void Drv_SW_I2C2_Start(void);
void Drv_SW_I2C2_Stop(void);
unsigned char Drv_SW_I2C2_Write(unsigned char data);
unsigned char Drv_SW_I2C2_Read(unsigned char isAck);

#endif /* __DRV_SW_I2C_H */
