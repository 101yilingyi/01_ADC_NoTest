#ifndef	__DRV_GPIO_H
#define __DRV_GPIO_H

// 12V-OUT, PA8
void GPIO_12vPower_Init(void);
void GPIO_12vPower_Ctrl(const unsigned char status);

// 3V3D-M, PC0
void GPIO_3v3Power_Init(void);
void GPIO_3v3Power_Ctrl(const unsigned char status);

#endif	/* __DRV_GPIO_H */
