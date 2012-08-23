/**
 *
 *
 *
 */

/* �û�ͷ�ļ�*/
#include "main.h"


/* freertos�ں�ͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* ����1 */
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

/* ����2 */
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


/* ������ */
int main(void)
{
  /* STM32F4XX�������� */
  /* ʱ������ */  	
  Stm32f4xx_RCC_Config();
  /* GPIO���� */
  Stm32f4xx_GPIO_Config();
  /* �������� */
  Stm32f4xx_USART_Config();
  /* �ж����� */
  /* Stm32f4xx_NVIC_Config(); */	

  Put_String((unsigned char *)("Test FreeRTOS!!\r\n"));
 
  /* ������һ������ */
  xTaskCreate(
              /* ָ��������������ָ�� */
              vTask1,
              /* �����ı���*/
              (const signed char *)("Task 1"),
              /* ջ��� */
              100,
              /* ������� */
              NULL,
              /* ���ȼ� */
              1,
              /* ������ */
              NULL
              );
	
  /* �����ڶ������� */
  xTaskCreate(vTask2, (const signed char *)("Task 2"), 100, NULL, 1, NULL);
	
  /* ���������� */
  vTaskStartScheduler();
	
  /* ���������������󣬲������е������� */
  while (1);
}

/* �������Ĳ����޸� */
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
