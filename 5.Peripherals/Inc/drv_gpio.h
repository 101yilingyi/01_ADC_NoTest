#ifndef	__DRV_GPIO_H
#define __DRV_GPIO_H

// 12V-OUT, PA8
void GPIO_12vPower_Init(void);
void GPIO_12vPower_Ctrl(const unsigned char status);

// 3V3D-M, PC0
void GPIO_3v3Power_Init(void);
void GPIO_3v3Power_Ctrl(const unsigned char status);

// status LED 
void GPIO_4GModule_RS485_StatusLED_Init(void);
void GPIO_4GModule_StatusLED_Ctrl(const unsigned char status);
void GPIO_RS485_StatusLED_Ctrl(const unsigned char status);

/* WatchDog Pins: PB1-WD-WAKE, PB8-WD-DONE, RESET */
void GPIO_WatchDog_Init(void);
void WatchDog_Feed(void);

/* RS485 */
void GPIO_RS485_CON_Init(void);
void GPIO_RS485_CON_Ctrl(const unsigned char status);

/* RTC */
void GPIO_RTC_WKUP_Init(void);

#endif	/* __DRV_GPIO_H */
