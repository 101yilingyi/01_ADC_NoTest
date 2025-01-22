版本信息

1. 12V、3V3、ADC 驱动（未测试）
   ```c
   // 12V-OUT, PA8
   void GPIO_12vPower_Init(void);
   void GPIO_12vPower_Ctrl(const unsigned char status);
   
   // 3V3D-M, PC0
   void GPIO_3v3Power_Init(void);
   void GPIO_3v3Power_Ctrl(const unsigned char status);
   
   static unsigned short ADC_InputSample(void)
   int ADC1_Init(void);
   float ADC_InputGet(void);
   ```

2. `WatchDog`驱动程序（未实现）

   ```c
   void GPIO_WatchDog_Init(void);
   void WatchDog_Feed(void);
   ```

   `EXTI`优先级与`HAL_Delay`冲突

3. `RS485`模块更新

   `UART3`,  不测试

   `RTC`的`I2C1`

4. `RTC`驱动

   `GPIO`：`PA0`，

   测试程序，读写`RCT`的寄存器

   ```c
   int main(void)
   {
       HAL_Init();
       SystemClock_Config();
       
       ... // power Init, Status-LED Init, WatchDog Init, WKUP_GPIO Init, I2C1 Init
       unsigned char flagReg = 0x1E;		// Flag Register Address	
       unsigned char flag = 1;
       // 读操作(两步: Slave addr, Register addr, Slave addr, Data)
       drv_i2c1_write(0x64, &flagReg, 1);	// Slave address, Register address
       drv_i2c1_read(0x64, &flag, 1);
       
       // 写操作, 写 Control Register 的 Stop 位
       unsigned char CtrlReg = 0x1F;
       flag = 0;
       drv_i2c1_write(0x64, &CtrlReg, 1);
       drv_i2c1_read(0x64, &flag, 1);	// 读
       
       unsigned char stop = ctrl & 0x01;
       HAL_Delay(2000);
       unsigned char data[2] = {0x1F, 0x01};
       drv_i2c1_write(0x64, data, 2);
       
       drv_i2c1_write(0x64, &CtrlReg, 1);
       drv_i2c1_read(0x64, &flag, 1);	// 读
       ...
   }
   ```

5. `4G Module`

   `GPIO`--`Communication Mode`

   `UART2_IT`

   ```c
   int Drv_4GModule_Init(void);
   // 4G Module 无 AT 测试
   ```

6. `VM501`测试

   `drv_vm501.c`, `drv_vm501.h`, `drv_sw_i2c.c`文件

   `main.c`中打印`VM501`的一频率
   
7. `Flash`驱动
