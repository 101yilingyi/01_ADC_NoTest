#include "drv_i2c.h"
#include "stm32f1xx_hal.h"

// RTC
static I2C_HandleTypeDef hi2c1;

int Drv_I2C1_Init(void)
{
    hi2c1.Instance = I2C1;          // 硬件I2C模块
    hi2c1.Init.ClockSpeed = 100000; // I2C通信速率100KHz~400KHz
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2; // 空读周期
    hi2c1.Init.OwnAddress1 = 0;     // 当MCU作为I2C从机的时候的设备地址
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;    // 地址格式
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;   // 双地址模式不使能
    hi2c1.Init.OwnAddress2 = 0; // 当MCU作为I2C从机的时候的第二个设备地址
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;   // 作为从机的时候才有这个功能
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;   // 钳制模式
    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        return -1;
    }
    return 0;
}

int Drv_I2C1_Write(const unsigned char address, const unsigned char* pdata, const unsigned short length)
{
    HAL_StatusTypeDef ret = HAL_I2C_Master_Transmit(&hi2c1, address, (unsigned char*)pdata, length, HAL_MAX_DELAY);
    if(HAL_OK != ret)   return -1;
    return length;
}

int Drv_I2C1_Read(const unsigned char address, unsigned char* pdata, const unsigned short length)
{
    HAL_StatusTypeDef ret = HAL_I2C_Master_Receive(&hi2c1, address, (unsigned char*)pdata, length, HAL_MAX_DELAY);
    if(HAL_OK != ret)   return -1;
    return length;
}


void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(i2cHandle->Instance==I2C1)
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();
        /* I2C1 GPIO Configuration
           PB6     ------> I2C1_SCL
           PB7     ------> I2C1_SDA  */
        GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* I2C1 clock enable */
        __HAL_RCC_I2C1_CLK_ENABLE();
    }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{
    if(i2cHandle->Instance==I2C1)
    {
        /* Peripheral clock disable */
        __HAL_RCC_I2C1_CLK_DISABLE();

        /* I2C1 GPIO Configuration
           PB6     ------> I2C1_SCL
           PB7     ------> I2C1_SDA		*/
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);
    }
}

