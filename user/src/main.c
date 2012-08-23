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

uint8_t Str[10];

/* STM32F4XX����������� */
void Basic_Config(void);

/* ������ӡ */
void Basic_Print(uint8_t *p);

/* 9λ����ת�ַ��� */
void Num2Str(uint32_t Num);

/* ������ */
int main(void)
{
  RCC_ClocksTypeDef RCC_Clocks;

  /* STM32F4XX����������� */
  Basic_Config();

  /* ��ȡʱ�����ã���ϵͳʱ��ֵת��Ϊ�ַ��� */	
  RCC_GetClocksFreq(&RCC_Clocks);
  Num2Str(RCC_Clocks.SYSCLK_Frequency);

  /* USART1��ӡϵͳʱ�� */
  Basic_Print((uint8_t *)("SYSCLK frequency: "));
  Basic_Print(Str);
  Basic_Print((uint8_t *)("Hz\r\n"));

  /* ��HCLKʱ��ֵת��Ϊ�ַ�����ʹ��USART1��� */
  Num2Str(RCC_Clocks.HCLK_Frequency);
  Basic_Print((uint8_t *)("HCLK frequency: "));
  Basic_Print(Str);
  Basic_Print((uint8_t *)("Hz\r\n"));

  /* ������1����ʱ��ֵת��Ϊ�ַ�����ʹ��USART1��� */
  Num2Str(RCC_Clocks.PCLK1_Frequency);
  Basic_Print((uint8_t *)("PCLK1 frequency: "));
  Basic_Print(Str);
  Basic_Print((uint8_t *)("Hz\r\n"));

  /* ������2����ʱ��ֵת��Ϊ�ַ�����ʹ��USART1��� */
  Num2Str(RCC_Clocks.PCLK2_Frequency);
  Basic_Print((uint8_t *)("PCLK2 frequency: "));
  Basic_Print(Str);
  Basic_Print((uint8_t *)("Hz\r\n"));



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

/* �������ã�����FreeRTOSǰ�������ʾ��Ϣ */
void Basic_Config(void)
{
  /* RCC����*/
  /* GPIOD --- ָʾ�� */
  /* GPIOB --- PB6 --- USART1_TX */
  /*      |--- PB7 --- USART1_RX */
  /* ʹ��GPIOD��GPIOBʱ�� */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOB, ENABLE);

  /* ʹ��USART1ʱ�� */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  
  /* GPIO���� */
  /* GPIO��ʼ���ṹ�� */
  GPIO_InitTypeDef  GPIO_InitStructure;

  /* ָʾ��IO���� */
  /* PD12 --- Green, PD13 --- Red, PD14 --- Yellow, PD15 --- Blue */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;

  /* ���ģʽ */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;

  /* ������� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;

  /* IO�ٶ� */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  /* ���������� */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

  /* ʹ���������ó�ʼ��GPIOD */
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* USART1_IO���� */
  /* ����GPIOB_Pin6, GPIOB_Pin7��USART1 */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

  /* ����GPIOB_Pin6*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  
  /* ���ܸ��� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  
  /* ������� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  
  /* 50MHz��ת�ٶ� */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;
  
  /* �������� */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

  /* ʹ���������ó�ʼ��GPIOB */
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* ����GPIOB_Pin7 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;

  /* ���ܸ��� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	

  /* ʹ���������ó�ʼ��GPIOB*/
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* USART1���� */
  USART_InitTypeDef USART_InitStructure;

  /* ���ò����� */
  USART_InitStructure.USART_BaudRate = 115200;

  /* �����ֳ� */
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;

  /* ����ֹͣλ */
  USART_InitStructure.USART_StopBits = USART_StopBits_1;

  /* ����У�� */
  USART_InitStructure.USART_Parity = USART_Parity_No;

  /* �������ʿ��� */
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

  /* ����USARTģʽ */
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  /* ʹ���������ó�ʼ��USART1 */
  USART_Init(USART1, &USART_InitStructure);

  /* ʹ��USART1 */
  USART_Cmd(USART1, ENABLE);
}

/* ������ӡ���ܣ�FreeRTOS����ǰ�����Ϣ */
void Basic_Print(uint8_t *p)
{
  while(*p)
    {
      USART_SendData(USART1, *p++);
      while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    }
}

/* 9λ����ת�ַ��� */
void Num2Str(uint32_t Num)
{
  uint8_t i;

  /* ��9λ����ת�����ַ���*/
  Str[0] = (Num / 100000000);
  Str[1] = (Num - Str[0] * 100000000) / 10000000;
  Str[2] = (Num - Str[0] * 100000000 - Str[1] * 10000000) / 1000000;
  Str[3] = (Num - Str[0] * 100000000 - Str[1] * 10000000 - Str[2] * 1000000) / 100000;
  Str[4] = (Num - Str[0] * 100000000 - Str[1] * 10000000 - Str[2] * 1000000 - Str[3] * 100000) / 10000;
  Str[5] = (Num - Str[0] * 100000000 - Str[1] * 10000000 - Str[2] * 1000000 - Str[3] * 100000 - Str[4] * 10000) / 1000;
  Str[6] = (Num - Str[0] * 100000000 - Str[1] * 10000000 - Str[2] * 1000000 - Str[3] * 100000 - Str[4] * 10000 - Str[5] * 1000) / 100;
  Str[7] = (Num - Str[0] * 100000000 - Str[1] * 10000000 - Str[2] * 1000000 - Str[3] * 100000 - Str[4] * 10000 - Str[5] * 1000 - Str[6] * 100) / 10;
  Str[8] = (Num - Str[0] * 100000000 - Str[1] * 10000000 - Str[2] * 1000000 - Str[3] * 100000 - Str[4] * 10000 - Str[5] * 1000 - Str[6] * 100 - Str[7] * 10);

  /* ��0��9ת���ɶ�Ӧ��ASCII�� */
  for(i = 0; i < 9; i++)
    Str[i] = Str[i] + 48;

  /* ���ǰ���λ��0�����޸�ASCII��Ϊ���ո�ֵ */
  for(i = 0; i < 9; i++)
    if(Str[i] == 48)
      Str[i] = 32;
    else
      break;
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
