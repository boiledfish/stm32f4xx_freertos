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



RTC_TimeTypeDef RTC_TimeStructure;
RTC_InitTypeDef RTC_InitStructure;
RTC_AlarmTypeDef  RTC_AlarmStructure;

uint32_t AsynchPrediv = 0, SynchPrediv = 0;

void RTC_Config(void);

void RTC_TimeRegulate(void);

void RTC_TimeShow(void);

/* ������ */
int main(void)
{
  RCC_ClocksTypeDef RCC_Clocks;

  /* STM32F4XX����������� */
  Basic_Config();

  /* ��ȡʱ�����ã���ϵͳʱ��ֵת��Ϊ�ַ��� */	
  RCC_GetClocksFreq(&RCC_Clocks);
  Num2Str(RCC_Clocks.SYSCLK_Frequency);

  /* USART3��ӡϵͳʱ�� */
  Basic_Print((uint8_t *)("SYSCLK frequency: "));
  Basic_Print(Str);
  Basic_Print((uint8_t *)("Hz\r\n"));

  /* ��HCLKʱ��ֵת��Ϊ�ַ�����ʹ��USART3��� */
  Num2Str(RCC_Clocks.HCLK_Frequency);
  Basic_Print((uint8_t *)("HCLK frequency: "));
  Basic_Print(Str);
  Basic_Print((uint8_t *)("Hz\r\n"));

  /* ������1����ʱ��ֵת��Ϊ�ַ�����ʹ��USART3��� */
  Num2Str(RCC_Clocks.PCLK1_Frequency);
  Basic_Print((uint8_t *)("PCLK1 frequency: "));
  Basic_Print(Str);
  Basic_Print((uint8_t *)("Hz\r\n"));

  /* ������2����ʱ��ֵת��Ϊ�ַ�����ʹ��USART3��� */
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

  xTaskCreate(vTask3, (const signed char *)("Task 2"), 100, NULL, 1, NULL);
	
  /* ���������� */
  vTaskStartScheduler();
	
  /* ���������������󣬲������е������� */
  while (1);
}


#define I2C_SLAVE_ADDRESS7      0xA0

/* �������ã�����FreeRTOSǰ�������ʾ��Ϣ���������ʱ�� */
void Basic_Config(void)
{
  /* RCC����*/
  /* GPIOD --- ָʾ�� */
  /* GPIOB --- PB6 --- USART1_TX */
  /*      |--- PB7 --- USART1_RX */
  /* ʹ��GPIODʱ�� */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  /* ʹ��USART3ʱ�� */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
 
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

  /* USART3_IO���� */
  /* ����GPIOD_Pin8, GPIOD_Pin9��USART3 */
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);

  /* ����GPIOD_Pin8*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  
  /* ���ܸ��� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  
  /* ������� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  
  /* 50MHz��ת�ٶ� */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;
  
  /* �������� */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

  /* ʹ���������ó�ʼ��GPIOD */
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* ����GPIOD_Pin9 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;

  /* ���ܸ��� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	

  /* ʹ���������ó�ʼ��GPIOD*/
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* USART3���� */
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

  /* ʹ���������ó�ʼ��USART3 */
  USART_Init(USART3, &USART_InitStructure);

  /* ʹ��USART3 */
  USART_Cmd(USART3, ENABLE);

  /* ʹ��I2C1ʱ�� */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

  /* ʹ��GPIOBʱ�� */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);

  /*����GPIOB_Pin7��GPIOB_Pin8*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8;

  /* ���ܸ��� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  
  /* ��©��� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  
  /* 2MHz��ת�ٶ� */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

  /* ʹ���������ó�ʼ��GPIOB*/
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* I2C1 */
  I2C_InitTypeDef I2C_InitStructure;

  /* ����I2C����Ϊ200K */
  I2C_InitStructure.I2C_ClockSpeed = 200000;
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = I2C_SLAVE_ADDRESS7;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

  I2C1_Cmd(I2C1, ENABLE);
  I2C_Init(I2C1, &I2C_InitStructure);

}

/* ������ӡ���ܣ�FreeRTOS����ǰ�����Ϣ */
void Basic_Print(uint8_t *p)
{
  while(*p)
    {
      USART_SendData(USART3, *p++);
      while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
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

  /* ��0��9ת���ɶ�Ӧ��ASCII�� */
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

  /* �ȴ������ͷ� */
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

  /* ����START */
  I2C_GenerateSTART(I2C1, ENABLE);

  /* EV5 */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

  /* ���͵�ַ */
  I2C_Send7bitAddress(I2C1, id, I2C_Direction_Transmitter);

  /* EV6 */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

  /* ����EV6 */
  I2C_Cmd(I2C1, ENABLE);

  /* ���Ͷ���ַ*/
  I2C_SendData(I2C1, read_address);

  /* EV8 */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  /* ���·���START */
  I2C_GenerateSTART(I2C1, ENABLE);

  /* EV5 */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

  /* ���Ͷ���ַ */
  I2C_Send7bitAddress(I2C1, id, I2C_Direction_Receiver);

  /* EV6 */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

  /* �ر�Ӧ���ֹͣ��������  */
  I2C_AcknowledgeConfig(I2C1, DISABLE);
  I2C_Generate(I2C1, ENABLE);

  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));

  temp = I2C_ReceiveData(I2C1);
  
  I2C_AcknowledgeConfig(I2C1, ENABLE);

  return temp;
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
