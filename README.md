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