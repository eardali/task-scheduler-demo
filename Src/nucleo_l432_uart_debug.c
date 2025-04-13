/*
 * nucleo_l432_uart_debug.c
 *
 *  Created on: Mar 2025
 *      Author: eardali
 */

#include "nucleo_l432_uart_debug.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

static uint8_t rx_buffer[TX_RX_BUFFER_SIZE];
static uint8_t tx_buffer[TX_RX_BUFFER_SIZE];
static uint16_t rx_head = 0, rx_tail = 0;
static uint16_t tx_head = 0, tx_tail = 0;
static uint16_t tx_data_size;
static bool tx_in_progress = false;

//init printf/scanf over uart (nonblocking, interrupt based)
void UART_Debug_Init(void){
	HAL_UART_Init(&UartHandle); //this call HAL_UART_MspInit() implicitly
	HAL_UART_Receive_IT(&UartHandle, &rx_buffer[rx_head], 1); //once call RX explicitly
}

//simple test for uart, echo received char
//call in a while(true) in main
void TestEcho(void){
    int a = getchar();
    if (a != -1){
      printf("%c", a);
    }
}

/* UART handler declaration */
UART_HandleTypeDef UartHandle;

/**
  * @brief UART MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  *           - NVIC configuration for UART interrupt request enable
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{  
  GPIO_InitTypeDef  GPIO_InitStruct;
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  USARTx_TX_GPIO_CLK_ENABLE();
  USARTx_RX_GPIO_CLK_ENABLE();


  /* Enable USARTx clock */
  USARTx_CLK_ENABLE(); 
  
  /*##-2- Configure peripheral GPIO ##########################################*/  
  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = USARTx_TX_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = USARTx_TX_AF;

  HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct);

  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = USARTx_RX_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Alternate = USARTx_RX_AF;

  HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStruct);
    
  /*##-3- Configure the NVIC for UART ########################################*/
  /* NVIC for USART */
  HAL_NVIC_SetPriority(USARTx_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USARTx_IRQn);

  /*##-4- Configure the UART peripheral ######################################*/

  UartHandle.Instance        = USARTx;

  UartHandle.Init.BaudRate   = UART_BAUD_RATE;
  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits   = UART_STOPBITS_1;
  UartHandle.Init.Parity     = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode       = UART_MODE_TX_RX;

  //HAL_UART_Receive_IT(&UartHandle, &rx_buffer[rx_head], 1);
}

/**
 * @brief UART MSP De-Initialization 
 *        This function frees the hardware resources used in this example:
 *          - Disable the Peripheral's clock
 *          - Revert GPIO and NVIC configuration to their default state
 * @param huart: UART handle pointer
 * @retval None
 */
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
  /*##-1- Reset peripherals ##################################################*/
  USARTx_FORCE_RESET();
  USARTx_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks #################################*/
  /* Configure UART Tx as alternate function  */
  HAL_GPIO_DeInit(USARTx_TX_GPIO_PORT, USARTx_TX_PIN);
  /* Configure UART Rx as alternate function  */
  HAL_GPIO_DeInit(USARTx_RX_GPIO_PORT, USARTx_RX_PIN);
  
  /*##-3- Disable the NVIC for UART ##########################################*/
  HAL_NVIC_DisableIRQ(USARTx_IRQn);
}

static void UART_TxBuffer_Process(void){
  if (!(tx_in_progress) && (tx_head != tx_tail)){
    tx_in_progress = true;
    if(tx_head > tx_tail){
      tx_data_size = tx_head - tx_tail;
    }else{
      tx_data_size = TX_RX_BUFFER_SIZE - tx_tail; //in this case, transmits in 2 passes, first [tail: end], then [0: head]
    }
    HAL_UART_Transmit_IT(&UartHandle, &tx_buffer[tx_tail], tx_data_size);
  }
}

/**
  * @brief  Tx Transfer completed callback
  * @param  UartHandle: UART handle.
  * @note   This example shows a simple way to report end of IT Tx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  /* Set transmission flag: transfer complete */
  if (UartHandle->Instance == USARTx){
    tx_tail = (tx_tail + tx_data_size)%TX_RX_BUFFER_SIZE;
    tx_in_progress = false;
    UART_TxBuffer_Process();
  }
}

//redirect printf/putch to serial
int _write(int file, char *ptr, int len){
  int i = 0;
  HAL_NVIC_DisableIRQ(USARTx_IRQn); //while updating indices prevent disturbing by interrupt
  while (i < len){
    tx_buffer[tx_head] = ptr[i++];
    tx_head = (tx_head + 1) % TX_RX_BUFFER_SIZE;
  }
  HAL_NVIC_EnableIRQ(USARTx_IRQn);
  UART_TxBuffer_Process();
  return i;
}

/**
* @brief  Rx Transfer completed callback
* @param  UartHandle: UART handle
* @note   This example shows a simple way to report end of DMA Rx transfer, and
*         you can add your own implementation.
* @retval None
*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  /* receive complete */
  if (UartHandle->Instance == USARTx){
    rx_head = (rx_head + 1) % TX_RX_BUFFER_SIZE;
    HAL_UART_Receive_IT(UartHandle, &rx_buffer[rx_head], 1);  // Restart reception
  }
}

//redirect scanf/getch to serial
int _read(int file, char *ptr, int len){
  int i = 0;
  if (rx_tail == rx_head) {
    return -1;  // Buffer empty
  }
  //UART_StartReceive();
  while (i < len && rx_tail != rx_head){
    ptr[i++] = rx_buffer[rx_tail];
    rx_tail = (rx_tail + 1) % TX_RX_BUFFER_SIZE;
  }
  return i;  // Return bytes read
}

/**
* @brief  UART error callbacks
* @param  UartHandle: UART handle
* @note   This example shows a simple way to report transfer error, and you can
*         add your own implementation.
* @retval None
*/
void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
{
    /*uart error*/
}
