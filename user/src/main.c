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

/* 主函数 */
int main(void)
{
  RCC_ClocksTypeDef RCC_Clocks;

  /* STM32F4XX外设基础配置 */
  Basic_Config();

  /* 获取时钟配置，将系统时钟值转换为字符串 */	
  RCC_GetClocksFreq(&RCC_Clocks);
  Num2Str(RCC_Clocks.SYSCLK_Frequency);

  /* USART1打印系统时钟 */
  Basic_Print((uint8_t *)("SYSCLK frequency: "));
  Basic_Print(Str);
  Basic_Print((uint8_t *)("Hz\r\n"));

  /* 将HCLK时钟值转换为字符串，使用USART1输出 */
  Num2Str(RCC_Clocks.HCLK_Frequency);
  Basic_Print((uint8_t *)("HCLK frequency: "));
  Basic_Print(Str);
  Basic_Print((uint8_t *)("Hz\r\n"));

  /* 将外设1总线时钟值转换为字符串，使用USART1输出 */
  Num2Str(RCC_Clocks.PCLK1_Frequency);
  Basic_Print((uint8_t *)("PCLK1 frequency: "));
  Basic_Print(Str);
  Basic_Print((uint8_t *)("Hz\r\n"));

  /* 将外设2总线时钟值转换为字符串，使用USART1输出 */
  Num2Str(RCC_Clocks.PCLK2_Frequency);
  Basic_Print((uint8_t *)("PCLK2 frequency: "));
  Basic_Print(Str);
  Basic_Print((uint8_t *)("Hz\r\n"));



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
	
  /* 启动调度器 */
  vTaskStartScheduler();
	
  /* 正常启动调度器后，不会运行到这里来 */
  while (1);
}

/* 基础配置，运行FreeRTOS前，输出提示信息 */
void Basic_Config(void)
{
  /* RCC配置*/
  /* GPIOD --- 指示灯 */
  /* GPIOB --- PB6 --- USART1_TX */
  /*      |--- PB7 --- USART1_RX */
  /* 使能GPIOD，GPIOB时钟 */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOB, ENABLE);

  /* 使能USART1时钟 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  
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

  /* USART1_IO配置 */
  /* 复用GPIOB_Pin6, GPIOB_Pin7到USART1 */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

  /* 配置GPIOB_Pin6*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  
  /* 功能复用 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  
  /* 推挽输出 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  
  /* 50MHz翻转速度 */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;
  
  /* 上拉开启 */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

  /* 使用以上配置初始化GPIOB */
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* 配置GPIOB_Pin7 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;

  /* 功能复用 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	

  /* 使用以上配置初始化GPIOB*/
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* USART1配置 */
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

  /* 使用以上配置初始化USART1 */
  USART_Init(USART1, &USART_InitStructure);

  /* 使能USART1 */
  USART_Cmd(USART1, ENABLE);
}

/* 基础打印功能，FreeRTOS启动前输出信息 */
void Basic_Print(uint8_t *p)
{
  while(*p)
    {
      USART_SendData(USART1, *p++);
      while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
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
