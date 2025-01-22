#include "drv_vm501.h"
#include "stm32f1xx_hal.h"
#include "drv_sw_i2c.h"
#include "drv_gpio.h"

static void Write_Byte(const unsigned char reg, const unsigned char data);
static void Write_Bytes(const uint8_t reg, const uint8_t* pData, const uint8_t len);
static unsigned char Read_Byte(const unsigned char reg);
static void Read_Bytes(const uint8_t reg, uint8_t* pData, const uint8_t len);

int Drv_VM501_Init(void)
{
	GPIO_VM501Channel_Init();
	Drv_SW_I2C2_Init();
	return 0;
}

// ÆµÂÊ
unsigned short Drv_VM501_ReadFrequence(const unsigned char channel)
{
	GPIO_VM501Channel_Enable(channel);
	HAL_Delay(1000);
	unsigned short data = 0;
	Read_Bytes(0x29, (unsigned char *)&data, 2);
	GPIO_VM501Channel_Disable(channel);
	return data;
}

// µç×è
unsigned short Drv_VM501_Read_Resistance(const unsigned char channel)
{
	GPIO_VM501Channel_Enable(channel);
	HAL_Delay(1000);
	unsigned short data = 0;
	Read_Bytes(0x23, (unsigned char *)&data, 2);
	GPIO_VM501Channel_Disable(channel);
	return data;
}

// ÎÂ¶È
unsigned short Drv_VM501_Read_Temperature(const unsigned char channel)
{
	GPIO_VM501Channel_Enable(channel);
	HAL_Delay(1000);
	unsigned short data = 0;
	Read_Bytes(0x27, (unsigned char *)&data, 2);
	GPIO_VM501Channel_Disable(channel);
	return data;
}

static void Write_Byte(const unsigned char reg, const unsigned char data)
{
	Drv_SW_I2C2_Start();
	Drv_SW_I2C2_Write(0xA0);
	Drv_SW_I2C2_Write(reg);
	Drv_SW_I2C2_Write(data);
	Drv_SW_I2C2_Stop();
}

static void Write_Bytes(const uint8_t reg, const uint8_t* pData, const uint8_t len)
{
	Drv_SW_I2C2_Start();
	Drv_SW_I2C2_Write(0xA0);
	Drv_SW_I2C2_Write(reg);
	for(uint8_t i = 0; i < len; i++)
	{
		Drv_SW_I2C2_Write(pData[i]);
	}
	Drv_SW_I2C2_Stop();
}

static unsigned char Read_Byte(const unsigned char reg)
{
	Drv_SW_I2C2_Start();
    Drv_SW_I2C2_Write(0xA0);
    Drv_SW_I2C2_Write(reg);
    Drv_SW_I2C2_Start();
    Drv_SW_I2C2_Write(0xA1);
    unsigned char data = Drv_SW_I2C2_Read(1);
    Drv_SW_I2C2_Stop();
    return data;
}

static void Read_Bytes(const uint8_t reg, uint8_t* pData, const uint8_t len)
{
	Drv_SW_I2C2_Start();
    Drv_SW_I2C2_Write(0xA0);
    Drv_SW_I2C2_Write(reg);
    Drv_SW_I2C2_Start();
    Drv_SW_I2C2_Write(0xA1);
    for(unsigned char i=(len-1); i>0; i--)
    {
        pData[i] = Drv_SW_I2C2_Read(0);
    }
    pData[0] = Drv_SW_I2C2_Read(1);
    Drv_SW_I2C2_Stop();
}

