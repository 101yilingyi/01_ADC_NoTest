#ifndef __DRV_VM501_H
#define __DRV_VM501_H

int Drv_VM501_Init(void);
unsigned short Drv_VM501_ReadFrequence(const unsigned char channel);
unsigned short Drv_VM501_Read_Resistance(const unsigned char channel);
unsigned short Drv_VM501_Read_Temperature(const unsigned char channel);

#endif /* __DRV_VM501_H */
