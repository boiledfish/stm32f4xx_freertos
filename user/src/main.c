/**
 *
 *
 *
 */

/* 用户头文件*/
#include "main.h"

/* freertos内核头文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

uint8_t Str[10];

/* STM32F4XX外设基础配置 */
void Basic_Config(void);

/* 基础打印 */
void Basic_Print(uint8_t *p);

/* 9位整型转字符串 */
void Num2Str(uint32_t Num);



RTC_TimeTypeDef RTC_TimeStructure;
RTC_InitTypeDef RTC_InitStructure;
RTC_AlarmTypeDef  RTC_AlarmStructure;

uint32_t AsynchPrediv = 0, SynchPrediv = 0;

void RTC_Config(void);

void RTC_TimeRegulate(void);

void RTC_TimeShow(void);

/* 主函数 */
int main(void)
{
  RCC_ClocksTypeDef RCC_Clocks;

  /* STM32F4XX外设基础配置 */
  Basic_Config();

  /* 获取时钟配置，将系统时钟值转换为字符串 */	
  RCC_GetClocksFreq(&RCC_Clocks);
  Num2Str(RCC_Clocks.SYSCLK_Frequency);

  /* USART3打印系统时钟 */
  Basic_Print((uint8_t *)("SYSCLK frequency: "));
  Basic_Print(Str);
  Basic_Print((uint8_t *)("Hz\r\n"));

  /* 将HCLK时钟值转换为字符串，使用USART3输出 */
  Num2Str(RCC_Clocks.HCLK_Frequency);
  Basic_Print((uint8_t *)("HCLK frequency: "));
  Basic_Print(Str);
  Basic_Print((uint8_t *)("Hz\r\n"));

  /* 将外设1总线时钟值转换为字符串，使用USART3输出 */
  Num2Str(RCC_Clocks.PCLK1_Frequency);
  Basic_Print((uint8_t *)("PCLK1 frequency: "));
  Basic_Print(Str);
  Basic_Print((uint8_t *)("Hz\r\n"));

  /* 将外设2总线时钟值转换为字符串，使用USART3输出 */
  Num2Str(RCC_Clocks.PCLK2_Frequency);
  Basic_Print((uint8_t *)("PCLK2 frequency: "));
  Basic_Print(Str);
  Basic_Print((uint8_t *)("Hz\r\n"));

  if(RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x33F2)
    {
      /* RTC configuration  */
      RTC_Config();
      
      /* Configure the RTC data register and RTC prescaler */
      RTC_InitStructure.RTC_AsynchPrediv = AsynchPrediv;
      RTC_InitStructure.RTC_SynchPrediv = SynchPrediv;
      RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;

      /* Check on RTC init */
      if (RTC_Init(&RTC_InitStructure) == ERROR)
	Basic_Print((uint8_t *)("RTC Prescaler Config failed!\r\n")); 
      else
	{
	  Basic_Print((uint8_t *)("RTC Prescaler Config OK!\r\n"));

	  /* Configure the time register */
	  RTC_TimeRegulate(); 
	}
    }
  else
    {
      /* Check if the Power On Reset flag is set */
      if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
	{
	  Basic_Print((uint8_t *)("Power On Reset occurred....\n\r"));
	}
      /* Check if the Pin Reset flag is set */
      else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
	{
	  Basic_Print((uint8_t *)("External Reset occurred....\n\r"));
	}

      Basic_Print((uint8_t *)("No need to configure RTC....\n\r"));
    
      /* Enable the PWR clock */
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

      /* Allow access to RTC */
      PWR_BackupAccessCmd(ENABLE);

      /* Wait for RTC APB registers synchronisation */
      RTC_WaitForSynchro();

      /* Clear the RTC Alarm Flag */
      RTC_ClearFlag(RTC_FLAG_ALRAF);

      /* Clear the EXTI Line 17 Pending bit (Connected internally to RTC Alarm) */
      //EXTI_ClearITPendingBit(EXTI_Line17);
      
      /* Display the RTC Time and Alarm */
      RTC_TimeShow();
    }


  /* 创建第一个任务 */
  xTaskCreate(
              /* 指向任务任务函数到指针 */
              vTask1,
              /* 任务文本名*/
              (const signed char *)("Task 1"),
              /* 栈深度 */
              100,
              /* 任务参数 */
              NULL,
              /* 优先级 */
              1,
              /* 任务句柄 */
              NULL
              );
	
  /* 创建第二个任务 */
  xTaskCreate(vTask2, (const signed char *)("Task 2"), 100, NULL, 1, NULL);

  xTaskCreate(vTask3, (const signed char *)("Task 2"), 100, NULL, 1, NULL);
	
  /* 启动调度器 */
  vTaskStartScheduler();
	
  /* 正常启动调度器后，不会运行到这里来 */
  while (1);
}


#define I2C_SLAVE_ADDRESS7      0xA0

/* 基础配置，运行FreeRTOS前，输出提示信息，输出日期时间 */
void Basic_Config(void)
{
  /* RCC配置*/
  /* GPIOD --- 指示灯 */
  /* GPIOB --- PB6 --- USART1_TX */
  /*      |--- PB7 --- USART1_RX */
  /* 使能GPIOD时钟 */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  /* 使能USART3时钟 */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
 
  /* GPIO配置 */
  /* GPIO初始化结构体 */
  GPIO_InitTypeDef  GPIO_InitStructure;

  /* 指示灯IO配置 */
  /* PD12 --- Green, PD13 --- Red, PD14 --- Yellow, PD15 --- Blue */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;

  /* 输出模式 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;

  /* 推挽输出 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;

  /* IO速度 */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  /* 上下拉配置 */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

  /* 使用以上配置初始化GPIOD */
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* USART3_IO配置 */
  /* 复用GPIOD_Pin8, GPIOD_Pin9到USART3 */
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);

  /* 配置GPIOD_Pin8*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  
  /* 功能复用 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  
  /* 推挽输出 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  
  /* 50MHz翻转速度 */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;
  
  /* 上拉开启 */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

  /* 使用以上配置初始化GPIOD */
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* 配置GPIOD_Pin9 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;

  /* 功能复用 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	

  /* 使用以上配置初始化GPIOD*/
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* USART3配置 */
  USART_InitTypeDef USART_InitStructure;

  /* 配置波特率 */
  USART_InitStructure.USART_BaudRate = 115200;

  /* 配置字长 */
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;

  /* 配置停止位 */
  USART_InitStructure.USART_StopBits = USART_StopBits_1;

  /* 配置校验 */
  USART_InitStructure.USART_Parity = USART_Parity_No;

  /* 配置速率控制 */
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

  /* 配置USART模式 */
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  /* 使用以上配置初始化USART3 */
  USART_Init(USART3, &USART_InitStructure);

  /* 使能USART3 */
  USART_Cmd(USART3, ENABLE);

  /* 使能I2C1时钟 */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

  /* 使能GPIOB时钟 */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);

  /*配置GPIOB_Pin7，GPIOB_Pin8*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8;

  /* 功能复用 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  
  /* 开漏输出 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  
  /* 2MHz翻转速度 */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

  /* 使用以上配置初始化GPIOB*/
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* I2C1 */
  I2C_InitTypeDef I2C_InitStructure;

  /* 配置I2C速率为200K */
  I2C_InitStructure.I2C_ClockSpeed = 200000;
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = I2C_SLAVE_ADDRESS7;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

  I2C1_Cmd(I2C1, ENABLE);
  I2C_Init(I2C1, &I2C_InitStructure);

}

/* 基础打印功能，FreeRTOS启动前输出信息 */
void Basic_Print(uint8_t *p)
{
  while(*p)
    {
      USART_SendData(USART3, *p++);
      while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
    }
}

/* 9位整型转字符串 */
void Num2Str(uint32_t Num)
{
  uint8_t i;

  /* 将9位整型转换成字符串*/
  Str[0] = (Num / 100000000);
  Str[1] = (Num - Str[0] * 100000000) / 10000000;
  Str[2] = (Num - Str[0] * 100000000 - Str[1] * 10000000) / 1000000;
  Str[3] = (Num - Str[0] * 100000000 - Str[1] * 10000000 - Str[2] * 1000000) / 100000;
  Str[4] = (Num - Str[0] * 100000000 - Str[1] * 10000000 - Str[2] * 1000000 - Str[3] * 100000) / 10000;
  Str[5] = (Num - Str[0] * 100000000 - Str[1] * 10000000 - Str[2] * 1000000 - Str[3] * 100000 - Str[4] * 10000) / 1000;
  Str[6] = (Num - Str[0] * 100000000 - Str[1] * 10000000 - Str[2] * 1000000 - Str[3] * 100000 - Str[4] * 10000 - Str[5] * 1000) / 100;
  Str[7] = (Num - Str[0] * 100000000 - Str[1] * 10000000 - Str[2] * 1000000 - Str[3] * 100000 - Str[4] * 10000 - Str[5] * 1000 - Str[6] * 100) / 10;
  Str[8] = (Num - Str[0] * 100000000 - Str[1] * 10000000 - Str[2] * 1000000 - Str[3] * 100000 - Str[4] * 10000 - Str[5] * 1000 - Str[6] * 100 - Str[7] * 10);

  /* 将0～9转换成对应到ASCII码 */
  for(i = 0; i < 9; i++)
    Str[i] = Str[i] + 48;

  /* 如果前面的位是0，则修改ASCII码为“空格”值 */
  for(i = 0; i < 9; i++)
    if(Str[i] == 48)
      Str[i] = 32;
    else
      break;
}


void RTC_Config(void)
{
  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to RTC */
  PWR_BackupAccessCmd(ENABLE);

  /* Enable the LSE OSC */
  RCC_LSEConfig(RCC_LSE_ON);

  /* Wait till LSE is ready */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {
    }

  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
  
  SynchPrediv = 0xFF;
  AsynchPrediv = 0x7F;

  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();
}

void RTC_TimeRegulate(void)
{
  uint32_t tmp_hh = 10, tmp_mm = 59, tmp_ss = 30;

  RTC_TimeStructure.RTC_H12 = RTC_H12_AM;

  RTC_TimeStructure.RTC_Hours = tmp_hh;

  RTC_TimeStructure.RTC_Minutes = tmp_mm;

  RTC_TimeStructure.RTC_Seconds = tmp_ss;

  /* Configure the RTC time register */
  if(RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure) == ERROR)
    {
      Basic_Print((uint8_t *)("RTC Set Time failed.\n\r"));
    }
  else
    {
      Basic_Print((uint8_t *)("RTC Set Time success.\n\r"));
      RTC_TimeShow();
      /* Indicator for the RTC configuration */
      RTC_WriteBackupRegister(RTC_BKP_DR0, 0x32F2);
    }

  RTC_AlarmCmd(RTC_Alarm_A, DISABLE); 
}

uint8_t Str_2Byte[2];
void Num2Str_2Byte(uint8_t num)
{
  uint8_t i;
  Str_2Byte[0] = num / 10;
  Str_2Byte[1] = num - Str_2Byte[0] * 10;

  /* 将0～9转换成对应到ASCII码 */
  for(i = 0; i < 2; i++)
    Str_2Byte[i] = Str_2Byte[i] + 48;
}


void RTC_TimeShow(void)
{
  /* Get the current Time */
  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);

  Basic_Print((uint8_t *)("The current time is: "));

  Num2Str_2Byte(RTC_TimeStructure.RTC_Hours);
  Basic_Print((uint8_t *)(Str_2Byte));
  
  Basic_Print((uint8_t *)(":"));

  Num2Str_2Byte(RTC_TimeStructure.RTC_Minutes);
  Basic_Print((uint8_t *)(Str_2Byte));

  Basic_Print((uint8_t *)(":"));

  Num2Str_2Byte(RTC_TimeStructure.RTC_Seconds);
  Basic_Print((uint8_t *)(Str_2Byte));

  Basic_Print((uint8_t *)("\n\r"));
}

uint8_t I2C_ReadByte(uint8_t id, uint8_t read_address)
{
  uint8_t temp;

  /* 等待总线释放 */
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

  /* 发送START */
  I2C_GenerateSTART(I2C1, ENABLE);

  /* EV5 */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

  /* 发送地址 */
  I2C_Send7bitAddress(I2C1, id, I2C_Direction_Transmitter);

  /* EV6 */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

  /* 重置EV6 */
  I2C_Cmd(I2C1, ENABLE);

  /* 发送读地址*/
  I2C_SendData(I2C1, read_address);

  /* EV8 */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  /* 重新发送START */
  I2C_GenerateSTART(I2C1, ENABLE);

  /* EV5 */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

  /* 发送读地址 */
  I2C_Send7bitAddress(I2C1, id, I2C_Direction_Receiver);

  /* EV6 */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

  /* 关闭应答和停止条件产生  */
  I2C_AcknowledgeConfig(I2C1, DISABLE);
  I2C_Generate(I2C1, ENABLE);

  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));

  temp = I2C_ReceiveData(I2C1);
  
  I2C_AcknowledgeConfig(I2C1, ENABLE);

  return temp;
}

/* 这个后面的不用修改 */
#ifdef  USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *   where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
    {
    }
}
#endif

/**
 * @}
 */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
