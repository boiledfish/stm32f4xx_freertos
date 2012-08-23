#include "stm32f4xx.h"

void Stm32f4xx_USART_Config(void)
{
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
