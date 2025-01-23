#include "drv_config.h"
#include "drv_flash.h"
#include "stm32f1xx_hal.h"

#define CONFIG_ADDRESS		(0x0807F800)

typedef struct DeviceConfig
{
	unsigned int isNetConfiged;	// 0x99-configed; other-not config
}DeviceConfig;

unsigned int Drv_Read_NetConfig(void)
{
	DeviceConfig config;
	Drv_Flash_Read(CONFIG_ADDRESS, (unsigned char *)&config, sizeof(config));
	return config.isNetConfiged;
}

int Drv_Save_NetConfig(void)
{
	DeviceConfig config;
	Drv_Flash_Read(CONFIG_ADDRESS, (unsigned char *)&config, sizeof(config));
	
	config.isNetConfiged = 0x99;
	return Drv_Flash_Write(CONFIG_ADDRESS, (unsigned char *)&config, sizeof(config));
}

