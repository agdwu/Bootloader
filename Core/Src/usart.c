/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
uint8_t 	UART4_Rx_Buff[UART4_RX_LEN] 	__attribute__		((at(0x20001000)));
uint8_t 	UART4_Tx_Buff[UART4_TX_LEN] = {0};
uint16_t 	Uart4_Rx_length;
uint8_t		rxData;
uint32_t 	Rx_Count=0;
/* USER CODE END 0 */

UART_HandleTypeDef huart4;

/* UART4 init function */
void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */
	HAL_UART_Receive_IT(&huart4,&rxData,1);
  /* USER CODE END UART4_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==UART4)
  {
  /* USER CODE BEGIN UART4_MspInit 0 */

  /* USER CODE END UART4_MspInit 0 */
    /* UART4 clock enable */
    __HAL_RCC_UART4_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**UART4 GPIO Configuration
    PA0-WKUP     ------> UART4_TX
    PA1     ------> UART4_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* UART4 interrupt Init */
    HAL_NVIC_SetPriority(UART4_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(UART4_IRQn);
  /* USER CODE BEGIN UART4_MspInit 1 */

  /* USER CODE END UART4_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==UART4)
  {
  /* USER CODE BEGIN UART4_MspDeInit 0 */

  /* USER CODE END UART4_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_UART4_CLK_DISABLE();

    /**UART4 GPIO Configuration
    PA0-WKUP     ------> UART4_TX
    PA1     ------> UART4_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|GPIO_PIN_1);

    /* UART4 interrupt Deinit */
    HAL_NVIC_DisableIRQ(UART4_IRQn);
  /* USER CODE BEGIN UART4_MspDeInit 1 */

  /* USER CODE END UART4_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
#include <stdio.h>// 包含标准输入输出头文件
 
int fputc(int ch,FILE *f)
{
//采用轮询方式发送1字节数据，超时时间设置为无限等待
	HAL_UART_Transmit(&huart4,(uint8_t *)&ch,1,HAL_MAX_DELAY);
	return ch;
}

int fgetc(FILE *f)
{
	uint8_t ch;
	// 采用轮询方式接收 1字节数据，超时时间设置为无限等待
	HAL_UART_Receive( &huart4,(uint8_t*)&ch,1, HAL_MAX_DELAY );
	return ch;
}
 
void Uart4_CRC_Send(uint16_t size)
{
	uint16_t i;
	for(i=0;i<size;i++)
	{
		HAL_UART_Transmit(&huart4,&UART4_Rx_Buff[i],1,HAL_MAX_DELAY);
		while(HAL_IS_BIT_SET(huart4.Instance->SR,USART_SR_TC)!=SET);//等待数据发送完成
		
	}
	memset(UART4_Rx_Buff,0,sizeof(UART4_Rx_Buff));
}



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==UART4)
	{
		if(Rx_Count<UART4_RX_LEN)
		{
			UART4_Rx_Buff[Rx_Count]=rxData;
			Uart4_Rx_length++;
		}
		HAL_UART_Receive_IT(&huart4,&rxData,1);
	}
}


/* USER CODE END 1 */
