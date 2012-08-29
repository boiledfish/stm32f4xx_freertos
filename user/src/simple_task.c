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

void vTask3(void *pvParameters)
{
  while(1)
    {
      RTC_TimeShow();
      vTaskDelay(2000);
    }
}
