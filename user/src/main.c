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

/* 任务1 */
void vTask1(void *pvParameters)
{
  while(1)
    {
      GPIOD->BSRRL = GPIO_Pin_12 | GPIO_Pin_13;
      vTaskDelay(1000);
      GPIOD->BSRRH = GPIO_Pin_12 | GPIO_Pin_13;
      vTaskDelay(1000);
    }
}

/* 任务2 */
void vTask2(void *pvParameters)
{
  while(1)
    {
      GPIOD->BSRRL = GPIO_Pin_14 | GPIO_Pin_15;
      vTaskDelay(500);
      GPIOD->BSRRH = GPIO_Pin_14 | GPIO_Pin_15;
      vTaskDelay(500);
    }
}

void Put_String(unsigned char  *p)
{
  while(*p)
    {
      USART_SendData(USART1, *p++);
      while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    }
}


/* 主函数 */
int main(void)
{
  /* STM32F4XX外设配置 */
  /* 时钟配置 */  	
  Stm32f4xx_RCC_Config();
  /* GPIO配置 */
  Stm32f4xx_GPIO_Config();
  /* 串口配置 */
  Stm32f4xx_USART_Config();
  /* 中断配置 */
  /* Stm32f4xx_NVIC_Config(); */	

  Put_String((unsigned char *)("Test FreeRTOS!!\r\n"));
 
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
