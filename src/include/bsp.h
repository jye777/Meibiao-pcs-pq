
#ifndef BSP_H
#define BSP_H

#include <stdint.h>
#include "stm32f4xx.h"

#include "led.h"

extern ADC_HandleTypeDef adc2;
extern ADC_HandleTypeDef adc3;
extern DMA_HandleTypeDef dma_adc2;
extern DMA_HandleTypeDef dma_adc3;
extern CAN_HandleTypeDef can1;
extern I2C_HandleTypeDef i2c1;
extern IWDG_HandleTypeDef iwdg;
extern RTC_HandleTypeDef rtc;
extern SPI_HandleTypeDef spi2;
extern UART_HandleTypeDef uart1;
extern UART_HandleTypeDef uart6;
extern DMA_HandleTypeDef dma_usart1_rx;
extern DMA_HandleTypeDef dma_usart1_tx;
extern DMA_HandleTypeDef dma_usart3_rx;
extern DMA_HandleTypeDef dma_usart3_tx;
extern DMA_HandleTypeDef dma_usart6_rx;
extern DMA_HandleTypeDef dma_usart6_tx;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void bsp_Init(void);

void watch_dog_init(void);
void watch_dog_feed(void);

int stdout_putchar (int ch);
int stdin_getchar(void);
int stderr_putchar (int ch);
void  ttywrch (int ch);
int try_getchar(char *ch);

#define RS485_1_CTRL_PORT   (GPIOC)
#define RS485_1_CTRL_PIN    (GPIO_PIN_12)

#define RS485_2_CTRL_PORT   (GPIOC)
#define RS485_2_CTRL_PIN    (GPIO_PIN_8)

#endif /* LED_H */

