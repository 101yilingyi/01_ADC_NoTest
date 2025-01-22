#include "drv_sw_i2c.h"
#include "main.h"


// VM501
#define I2C2_SCL_PIN        GPIO_PIN_10
#define I2C2_SDA_PIN        GPIO_PIN_11

#define I2C2_PORT           GPIOB

#define SDA_IN()            HAL_GPIO_ReadPin(I2C2_PORT, I2C2_SDA_PIN)
#define SCL_LOW()           HAL_GPIO_WritePin(I2C2_PORT, I2C2_SCL_PIN, GPIO_PIN_RESET)
#define SCL_HIGH()           HAL_GPIO_WritePin(I2C2_PORT, I2C2_SCL_PIN, GPIO_PIN_SET)
#define SDA_LOW()           HAL_GPIO_WritePin(I2C2_PORT, I2C2_SDA_PIN, GPIO_PIN_RESET)
#define SDA_HIGH()           HAL_GPIO_WritePin(I2C2_PORT, I2C2_SDA_PIN, GPIO_PIN_SET)

void Drv_SW_I2C2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10 | GPIO_PIN_11, GPIO_PIN_SET);
    
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
    
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

// I2C开始信号：在SCL为高电平的情况下，SDA由高到低变化
void Drv_SW_I2C2_Start(void)
{
    SCL_LOW();
    SDA_HIGH();
    SCL_HIGH();
    SDA_LOW();
}

// I2C停止信号：在SCL为高电平的情况下，SDA由低到高变化
void Drv_SW_I2C2_Stop(void)
{
    SCL_LOW();
    SDA_LOW();
    SCL_HIGH();
    SDA_HIGH();
}

// 返回值为0代表接收方给了应答信号，为1表示给了非应答信号
unsigned char Drv_SW_I2C2_Write(unsigned char data)
{
    // 8个时钟发送一个字节的8个bit，从高bit到低bit依次发送
    for(char i = 0; i<8; i++)
    {
        SCL_LOW();
        if(data&0x80) SDA_HIGH();
        else SDA_LOW();
        data = data<<1;
        SCL_HIGH();
    }
    // 第9个时钟读取SDA的电平，判断是否得到了应答
    SCL_LOW();
    SDA_HIGH(); // 释放对SDA的控制权以便读取SDA被外部改变的电平
    SCL_HIGH();
    return SDA_IN();
}

// isAck=0 --> 给应答信号
// isAck=1 --> 给非应答信号
unsigned char Drv_SW_I2C2_Read(unsigned char isAck)
{
    SCL_LOW();  // 是为了避免产生一个开始信号或者停止信号
    SDA_HIGH(); // 释放对SDA的控制权以便读取SDA被外部改变的电平
    unsigned char data = 0;
    for(char i=0; i<8; i++)
    {
        SCL_LOW();
        SCL_HIGH();
        data <<= 1; // 先将原来的数据按位移到高位，留出最低位保存新的逻辑电平数据
        data |= SDA_IN();   // 将新的逻辑电平数据保存到数据的最低位
    }
    // 第9个时钟根据isAck来改变SDA的电平达到给应答/非应答信号的目的
    SCL_LOW();
    (isAck==0)?SDA_LOW():SDA_HIGH();
    SCL_HIGH();
    return data;
}
