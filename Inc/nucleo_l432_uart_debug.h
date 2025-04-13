/*
 * nucleo_l432_uart_debug.h
 *
 *  Created on: Mar 2025
 *      Author: eardali
 */

#ifndef INC_NUCLEO_L432_UART_DEBUG_H_
#define INC_NUCLEO_L432_UART_DEBUG_H_

#include "stm32l4xx_hal.h"

#define TX_RX_BUFFER_SIZE 256
extern UART_HandleTypeDef UartHandle;

void UART_Debug_Init(void);
void TestEcho(void);

//Define baudrate
#define UART_BAUD_RATE 115200

/* Definition for USARTx clock resources */
#define USARTx                           USART2
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART2_CLK_ENABLE()
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define USARTx_FORCE_RESET()             __HAL_RCC_USART2_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __HAL_RCC_USART2_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_2
#define USARTx_TX_GPIO_PORT              GPIOA
#define USARTx_TX_AF                     GPIO_AF7_USART2
#define USARTx_RX_PIN                    GPIO_PIN_15
#define USARTx_RX_GPIO_PORT              GPIOA
#define USARTx_RX_AF                     GPIO_AF3_USART2

/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      USART2_IRQn
#define USARTx_IRQHandler                USART2_IRQHandler


#endif /* INC_STM32L4XX_UART_DEBUG_H_ */
