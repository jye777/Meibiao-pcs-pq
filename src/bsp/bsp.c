#include "cmsis_os.h"
#include "bsp.h"
#include "cpu.h"
#include "Driver_USART.h"
#include "Driver_SPI.h"
#include "can.h"
#include "rte_device.h"
#include "fpga.h"

ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc3;
DMA_HandleTypeDef hdma_adc1;
DMA_HandleTypeDef hdma_adc3;

CAN_HandleTypeDef can1;
I2C_HandleTypeDef i2c1;
IWDG_HandleTypeDef iwdg;
RTC_HandleTypeDef rtc;
SPI_HandleTypeDef spi2;
WWDG_HandleTypeDef hwwdg;

/* USART Driver */
/* USART Driver */
extern ARM_DRIVER_USART Driver_USART1;
extern ARM_DRIVER_USART Driver_USART3;
extern ARM_DRIVER_USART Driver_USART6;

extern void usart3_callback(uint32_t event);
extern void usart6_callback(uint32_t event);


FSMC_NAND_InitTypeDef       NandInit;

int stdout_putchar(int ch)
{
    extern ARM_DRIVER_USART Driver_USART1;
    CPU_SR cpu_sr;
    ARM_DRIVER_USART *drv = &Driver_USART1;

    while(1)
    {
        CPU_CRITICAL_ENTER();

        if(drv->GetStatus().tx_busy)
        {
            CPU_CRITICAL_EXIT();
            continue;
        }

        drv->Send(&ch, 1);
        CPU_CRITICAL_EXIT();
        break;
    }

    return ch;
};

int try_getchar(char *ch)
{
    extern ARM_DRIVER_USART Driver_USART1;
    int recv = 0;
    static char buf = 0;
    ARM_DRIVER_USART *drv = &Driver_USART1;

    if(buf != 0)
    {
        *ch  = buf;
        recv = 1;
    }

    drv->Control(ARM_USART_ABORT_RECEIVE, 0);
    drv->Receive(&buf, 1);

    return recv;
};

int stdin_getchar(void)
{
    uint8_t ch;
    static char buf[50];
    static int offset = 0;
    extern ARM_DRIVER_USART Driver_USART1;

    ARM_DRIVER_USART *drv = &Driver_USART1;

    if(drv->GetRxCount() > offset)
    {
        ch = buf[offset++];
        return ch;
    }

    drv->Control(ARM_USART_ABORT_RECEIVE, 0);
    drv->Receive(buf, sizeof(buf));
    offset = 0;

    while(drv->GetRxCount() == 0)
    {
        osDelay(1);
    }

    ch = buf[0];
    offset++;

    return ch;
};

int stderr_putchar (int ch)
{
    return stdout_putchar(ch);
};

void ttywrch (int ch)
{
    stdout_putchar(ch);
};

/* CAN1 init function */
void CAN1_Init(void)
{

    can1.Instance = CAN1;
    can1.Init.Prescaler = 16;
    can1.Init.Mode = CAN_MODE_NORMAL;
    can1.Init.SJW = CAN_SJW_1TQ;
    can1.Init.BS1 = CAN_BS1_1TQ;
    can1.Init.BS2 = CAN_BS2_1TQ;
    can1.Init.TTCM = DISABLE;
    can1.Init.ABOM = DISABLE;
    can1.Init.AWUM = DISABLE;
    can1.Init.NART = DISABLE;
    can1.Init.RFLM = DISABLE;
    can1.Init.TXFP = DISABLE;
    HAL_CAN_Init(&can1);

}

/* I2C1 init function */
void I2C1_Init(void)
{

    i2c1.Instance = I2C1;
    i2c1.Init.ClockSpeed = 100000;
    i2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    i2c1.Init.OwnAddress1 = 0;
    i2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    i2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
    i2c1.Init.OwnAddress2 = 0;
    i2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
    i2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;
    HAL_I2C_Init(&i2c1);

}

/* IWDG init function */
void IWDG_Init(void)
{

    iwdg.Instance = IWDG;
    iwdg.Init.Prescaler = IWDG_PRESCALER_64;
    iwdg.Init.Reload = 4095;
    HAL_IWDG_Init(&iwdg);

}

/* RTC init function */
void RTC_Init(void)
{
    rtc.Instance = RTC;
    rtc.Init.HourFormat = RTC_HOURFORMAT_24;
    rtc.Init.AsynchPrediv = 127;
    rtc.Init.SynchPrediv = 255;
    rtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    rtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    rtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    HAL_RTC_Init(&rtc);
}

/* SPI1 init function */
void SPI1_Init(void)
{
    extern ARM_DRIVER_SPI Driver_SPI1;

    ARM_DRIVER_SPI *spi_drv = &Driver_SPI1;
    spi_drv->Initialize(NULL);
    spi_drv->PowerControl(ARM_POWER_FULL);
    spi_drv->Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL0_CPHA0 | ARM_SPI_MSB_LSB | ARM_SPI_SS_MASTER_SW | ARM_SPI_DATA_BITS(8), 20000000);
    spi_drv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);

}


/* SPI2 init function */
void SPI2_Init(void)
{
#if 0 /* old style spi init. */
    spi2.Instance = SPI2;
    spi2.Init.Mode = SPI_MODE_MASTER;
    spi2.Init.Direction = SPI_DIRECTION_2LINES;
    spi2.Init.DataSize = SPI_DATASIZE_8BIT;
    spi2.Init.CLKPolarity = SPI_POLARITY_LOW;
    spi2.Init.CLKPhase = SPI_PHASE_1EDGE;
    spi2.Init.NSS = SPI_NSS_HARD_OUTPUT;
    spi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    spi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
    spi2.Init.TIMode = SPI_TIMODE_DISABLED;
    spi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
    HAL_SPI_Init(&spi2);
#endif
    
    extern ARM_DRIVER_SPI Driver_SPI2;

    ARM_DRIVER_SPI *spi_drv = &Driver_SPI2;
    spi_drv->Initialize(NULL);
    spi_drv->PowerControl(ARM_POWER_FULL);
    spi_drv->Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL0_CPHA0 | ARM_SPI_MSB_LSB | ARM_SPI_SS_MASTER_SW | ARM_SPI_DATA_BITS(8), 20000000);
    spi_drv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
}


/* USART1 init function */
void USART1_UART_Init(void)
{
    extern ARM_DRIVER_USART Driver_USART1;
    ARM_DRIVER_USART * USARTdrv = &Driver_USART1;

    USARTdrv->Initialize(NULL);
    USARTdrv->PowerControl(ARM_POWER_FULL);
    USARTdrv->Control(ARM_USART_MODE_ASYNCHRONOUS |
                      ARM_USART_DATA_BITS_8 |
                      ARM_USART_PARITY_NONE |
                      ARM_USART_STOP_BITS_1 |
                      ARM_USART_FLOW_CONTROL_NONE,
                      115200);
    USARTdrv->Control(ARM_USART_CONTROL_TX, 1);
    USARTdrv->Control(ARM_USART_CONTROL_RX, 1);
}

/* USART3 init function */
void USART3_UART_Init(void)
/* communicate with screen. */
{
#if RTE_USART3
    extern ARM_DRIVER_USART Driver_USART3;
    ARM_DRIVER_USART * USARTdrv = &Driver_USART3;
    
    extern void RS485_Callback(uint32_t event);

    USARTdrv->Initialize(RS485_Callback);//usart3_callback);
    USARTdrv->PowerControl(ARM_POWER_FULL);
    USARTdrv->Control(ARM_USART_MODE_ASYNCHRONOUS |
                      ARM_USART_DATA_BITS_8 |
                      ARM_USART_PARITY_NONE |
                      ARM_USART_STOP_BITS_1 |
                      ARM_USART_FLOW_CONTROL_NONE,
                      9600);
    USARTdrv->Control(ARM_USART_CONTROL_TX, 1);
    USARTdrv->Control(ARM_USART_CONTROL_RX, 1);

    HAL_GPIO_WritePin(RS485_1_CTRL_PORT, RS485_1_CTRL_PIN, GPIO_PIN_RESET);
#endif
}

/* USART3 init function */

void USART4_UART_Init(void)
{
#if RTE_UART4
    extern ARM_DRIVER_USART Driver_USART4;
    ARM_DRIVER_USART * USARTdrv = &Driver_USART4;
    
    USARTdrv->Initialize(NULL);//usart3_callback);
    USARTdrv->PowerControl(ARM_POWER_FULL);
    USARTdrv->Control(ARM_USART_MODE_ASYNCHRONOUS |
                      ARM_USART_DATA_BITS_8 |
                      ARM_USART_PARITY_NONE |
                      ARM_USART_STOP_BITS_1 |
                      ARM_USART_FLOW_CONTROL_NONE,
                      115200);
    USARTdrv->Control(ARM_USART_CONTROL_TX, 1);
    USARTdrv->Control(ARM_USART_CONTROL_RX, 1);

    HAL_GPIO_WritePin(RS485_1_CTRL_PORT, RS485_1_CTRL_PIN, GPIO_PIN_RESET);
#endif
}


/* USART6 init function */
void USART6_UART_Init(void)
/* communicate with extended board. */
{
#if RTE_USART6
    extern ARM_DRIVER_USART Driver_USART6;
    ARM_DRIVER_USART * USARTdrv = &Driver_USART6;	
    USARTdrv->Initialize(usart6_callback);
    USARTdrv->PowerControl(ARM_POWER_FULL);	
	
    USARTdrv->Control(ARM_USART_MODE_ASYNCHRONOUS | 
                    ARM_USART_DATA_BITS_8 | 
                    ARM_USART_PARITY_NONE| 
                    ARM_USART_STOP_BITS_1 | 
                    ARM_USART_FLOW_CONTROL_NONE, 
                    19200);
    USARTdrv->Control(ARM_USART_CONTROL_TX, 1);
    USARTdrv->Control(ARM_USART_CONTROL_RX, 1);
    HAL_GPIO_WritePin(RS485_2_CTRL_PORT, RS485_2_CTRL_PIN, GPIO_PIN_RESET);
#endif
}

/**
  * Enable DMA controller clock
  */
void DMA_Init(void)
{
    /* DMA controller clock enable */
    __HAL_RCC_DMA2_CLK_ENABLE();

    /* DMA interrupt init */
    /* DMA2_Stream0_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
    /* DMA2_Stream4_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream4_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream4_IRQn);
}

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void GPIO_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStruct;

    /* GPIO Ports Clock Enable */
    __GPIOA_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();
    __GPIOC_CLK_ENABLE();
    __GPIOD_CLK_ENABLE();
    __GPIOE_CLK_ENABLE();
    __GPIOF_CLK_ENABLE();
    __GPIOG_CLK_ENABLE();
    __GPIOH_CLK_ENABLE();

    /* Configure RS485_CON2:PC8 RS485_CON1:PC12 */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);



    /* Configure F_NCE:PG7 F_NCONFIG:PG8 */
    GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
}

#if 0
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
    /* USER CODE BEGIN MspInit 0 */

    /* USER CODE END MspInit 0 */

    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    /* System interrupt init*/
    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

    /* USER CODE BEGIN MspInit 1 */

    /* USER CODE END MspInit 1 */
}
#endif

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler */
    /* User can add his own implementation to report the HAL error return state */
    while(1)
    {
    }
    /* USER CODE END Error_Handler */
}

/* ADC1 init function */
void MX_ADC1_Init(void)
{
    ADC_ChannelConfTypeDef sConfig;

    /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
    */
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = ENABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 7;
    hadc1.Init.DMAContinuousRequests = ENABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
        Error_Handler();
    }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
    sConfig.Channel = ADC_CHANNEL_5;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
    sConfig.Channel = ADC_CHANNEL_6;
    sConfig.Rank = 2;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
    sConfig.Channel = ADC_CHANNEL_8;
    sConfig.Rank = 3;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
    sConfig.Channel = ADC_CHANNEL_9;
    sConfig.Rank = 4;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
    sConfig.Channel = ADC_CHANNEL_12;
    sConfig.Rank = 5;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
    sConfig.Channel = ADC_CHANNEL_13;
    sConfig.Rank = 6;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
    sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = 7;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

}
/* ADC3 init function */
void MX_ADC3_Init(void)
{
    ADC_ChannelConfTypeDef sConfig;

    /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
    */
    hadc3.Instance = ADC3;
    hadc3.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc3.Init.Resolution = ADC_RESOLUTION_12B;
    hadc3.Init.ScanConvMode = ENABLE;
    hadc3.Init.ContinuousConvMode = DISABLE;
    hadc3.Init.DiscontinuousConvMode = DISABLE;
    hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc3.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc3.Init.NbrOfConversion = 7;
    hadc3.Init.DMAContinuousRequests = ENABLE;
    hadc3.Init.EOCSelection = ADC_EOC_SEQ_CONV;
    if (HAL_ADC_Init(&hadc3) != HAL_OK)
    {
        Error_Handler();
    }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
    sConfig.Channel = ADC_CHANNEL_4;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
    sConfig.Channel = ADC_CHANNEL_5;
    sConfig.Rank = 2;
    if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
    sConfig.Channel = ADC_CHANNEL_6;
    sConfig.Rank = 3;
    if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
    sConfig.Channel = ADC_CHANNEL_7;
    sConfig.Rank = 4;
    if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
    sConfig.Channel = ADC_CHANNEL_8;
    sConfig.Rank = 5;
    if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
    sConfig.Channel = ADC_CHANNEL_10;
    sConfig.Rank = 6;
    if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
    sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = 7;
    if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

    GPIO_InitTypeDef GPIO_InitStruct;
    if(adcHandle->Instance==ADC1)
    {
        /* USER CODE BEGIN ADC1_MspInit 0 */

        /* USER CODE END ADC1_MspInit 0 */
        /* Peripheral clock enable */
        __HAL_RCC_ADC1_CLK_ENABLE();

        /**ADC1 GPIO Configuration
	        PC2     ------> ADC1_IN12
	        PC3     ------> ADC1_IN13
	        PA0-WKUP     ------> ADC1_IN0
	        PA5     ------> ADC1_IN5  //被占用
	        PA6     ------> ADC1_IN6  //被占用
	        PB0     ------> ADC1_IN8
	        PB1     ------> ADC1_IN9
	        */
        GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_0;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* Peripheral DMA init*/

        hdma_adc1.Instance = DMA2_Stream4;
        hdma_adc1.Init.Channel = DMA_CHANNEL_0;
        hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
        hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hdma_adc1.Init.Mode = DMA_NORMAL;
        hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
        hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
        {
            Error_Handler();
        }

        __HAL_LINKDMA(adcHandle,DMA_Handle,hdma_adc1);

        /* USER CODE BEGIN ADC1_MspInit 1 */

        /* USER CODE END ADC1_MspInit 1 */
    }
    else if(adcHandle->Instance==ADC3)
    {
        /* USER CODE BEGIN ADC3_MspInit 0 */

        /* USER CODE END ADC3_MspInit 0 */
        /* Peripheral clock enable */
        __HAL_RCC_ADC3_CLK_ENABLE();

        /**ADC3 GPIO Configuration
	        PF6     ------> ADC3_IN4
	        PF7     ------> ADC3_IN5
	        PF8     ------> ADC3_IN6
	        PF9     ------> ADC3_IN7
	        PF10     ------> ADC3_IN8
	        PC0     ------> ADC3_IN10
	        PA0-WKUP     ------> ADC3_IN0
	        */
        GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_0;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        //GPIO_InitStruct.Pin = GPIO_PIN_0;
        //GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        //GPIO_InitStruct.Pull = GPIO_NOPULL;
        //HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* Peripheral DMA init*/

        hdma_adc3.Instance = DMA2_Stream0;
        hdma_adc3.Init.Channel = DMA_CHANNEL_2;
        hdma_adc3.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_adc3.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_adc3.Init.MemInc = DMA_MINC_ENABLE;
        hdma_adc3.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_adc3.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hdma_adc3.Init.Mode = DMA_NORMAL;
        hdma_adc3.Init.Priority = DMA_PRIORITY_LOW;
        hdma_adc3.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_adc3) != HAL_OK)
        {
            Error_Handler();
        }

        __HAL_LINKDMA(adcHandle,DMA_Handle,hdma_adc3);

        /* USER CODE BEGIN ADC3_MspInit 1 */

        /* USER CODE END ADC3_MspInit 1 */
    }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{

    if(adcHandle->Instance==ADC1)
    {
        /* USER CODE BEGIN ADC1_MspDeInit 0 */

        /* USER CODE END ADC1_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_ADC1_CLK_DISABLE();

        /**ADC1 GPIO Configuration
	        PC2     ------> ADC1_IN12
	        PC3     ------> ADC1_IN13
	        PA0-WKUP     ------> ADC1_IN0
	        PA5     ------> ADC1_IN5
	        PA6     ------> ADC1_IN6
	        PB0     ------> ADC1_IN8
	        PB1     ------> ADC1_IN9
	        */
        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_2|GPIO_PIN_3);

        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0);

        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0|GPIO_PIN_1);

        /* Peripheral DMA DeInit*/
        HAL_DMA_DeInit(adcHandle->DMA_Handle);
        /* USER CODE BEGIN ADC1_MspDeInit 1 */

        /* USER CODE END ADC1_MspDeInit 1 */
    }
    else if(adcHandle->Instance==ADC3)
    {
        /* USER CODE BEGIN ADC3_MspDeInit 0 */

        /* USER CODE END ADC3_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_ADC3_CLK_DISABLE();

        /**ADC3 GPIO Configuration
	        PF6     ------> ADC3_IN4
	        PF7     ------> ADC3_IN5
	        PF8     ------> ADC3_IN6
	        PF9     ------> ADC3_IN7
	        PF10     ------> ADC3_IN8
	        PC0     ------> ADC3_IN10
	        PC2     ------> ADC3_IN12
	        PC3     ------> ADC3_IN13
	        PA0-WKUP     ------> ADC3_IN0
	        */
        HAL_GPIO_DeInit(GPIOF, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9
                        |GPIO_PIN_10);

        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0);

        //HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0);

        /* Peripheral DMA DeInit*/
        HAL_DMA_DeInit(adcHandle->DMA_Handle);
        /* USER CODE BEGIN ADC3_MspDeInit 1 */

        /* USER CODE END ADC3_MspDeInit 1 */
    }
}

void HAL_WWDG_MspInit(WWDG_HandleTypeDef* wwdgHandle)
{

    if(wwdgHandle->Instance==WWDG)
    {
        /* USER CODE BEGIN WWDG_MspInit 0 */

        /* USER CODE END WWDG_MspInit 0 */
        /* Peripheral clock enable */
        __HAL_RCC_WWDG_CLK_ENABLE();
        /* USER CODE BEGIN WWDG_MspInit 1 */

        /* USER CODE END WWDG_MspInit 1 */
    }
}

//void watch_dog_init(void)
//{
//    hwwdg.Instance = WWDG;
//    hwwdg.Init.Prescaler = WWDG_PRESCALER_8;
//    hwwdg.Init.Window = 127;
//    hwwdg.Init.Counter = 127;
//    hwwdg.Init.EWIMode = 0;

//    if(HAL_WWDG_Init(&hwwdg) != HAL_OK)
//    {
//        Error_Handler();
//    }
//}

void watch_dog_feed(void)
{
//    HAL_WWDG_Refresh(&hwwdg);
	HAL_IWDG_Refresh(&iwdg);	//使用独立看门狗
}

void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan)
{

    GPIO_InitTypeDef GPIO_InitStruct;
    if(hcan->Instance==CAN1)
    {
        /* USER CODE BEGIN CAN1_MspInit 0 */

        /* USER CODE END CAN1_MspInit 0 */
        /* Peripheral clock enable */
        __CAN1_CLK_ENABLE();

        /**CAN1 GPIO Configuration
        PA11     ------> CAN1_RX
        PA12     ------> CAN1_TX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
        GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* System interrupt init*/
        HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
        HAL_NVIC_SetPriority(CAN1_SCE_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);
        HAL_NVIC_SetPriority(CAN1_TX_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
        HAL_NVIC_SetPriority(CAN1_RX1_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
        /* USER CODE BEGIN CAN1_MspInit 1 */

        /* USER CODE END CAN1_MspInit 1 */
    }

}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* hcan)
{

    if(hcan->Instance==CAN1)
    {
        /* USER CODE BEGIN CAN1_MspDeInit 0 */

        /* USER CODE END CAN1_MspDeInit 0 */
        /* Peripheral clock disable */
        __CAN1_CLK_DISABLE();

        /**CAN1 GPIO Configuration
        PA11     ------> CAN1_RX
        PA12     ------> CAN1_TX
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

        /* Peripheral interrupt DeInit*/
        HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);

        HAL_NVIC_DisableIRQ(CAN1_SCE_IRQn);

        HAL_NVIC_DisableIRQ(CAN1_TX_IRQn);

        HAL_NVIC_DisableIRQ(CAN1_RX1_IRQn);

        /* USER CODE BEGIN CAN1_MspDeInit 1 */

        /* USER CODE END CAN1_MspDeInit 1 */
    }

}

void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{

    GPIO_InitTypeDef GPIO_InitStruct;
    if(hi2c->Instance==I2C1)
    {
        /* USER CODE BEGIN I2C1_MspInit 0 */

        /* USER CODE END I2C1_MspInit 0 */
        /* Peripheral clock enable */
        __I2C1_CLK_ENABLE();

        /**I2C1 GPIO Configuration
        PB6     ------> I2C1_SCL
        PB7     ------> I2C1_SDA
        */
        GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
        GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* USER CODE BEGIN I2C1_MspInit 1 */

        /* USER CODE END I2C1_MspInit 1 */
    }

}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
{

    if(hi2c->Instance==I2C1)
    {
        /* USER CODE BEGIN I2C1_MspDeInit 0 */

        /* USER CODE END I2C1_MspDeInit 0 */
        /* Peripheral clock disable */
        __I2C1_CLK_DISABLE();

        /**I2C1 GPIO Configuration
        PB6     ------> I2C1_SCL
        PB7     ------> I2C1_SDA
        */
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_7);

        /* USER CODE BEGIN I2C1_MspDeInit 1 */

        /* USER CODE END I2C1_MspDeInit 1 */
    }

}

void HAL_IWDG_MspInit(IWDG_HandleTypeDef* iwdg)
{

}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtc)
{

    if(rtc->Instance==RTC)
    {
        /* USER CODE BEGIN RTC_MspInit 0 */

        /* USER CODE END RTC_MspInit 0 */
        /* Peripheral clock enable */
        __HAL_RCC_RTC_ENABLE();
        /* USER CODE BEGIN RTC_MspInit 1 */

        /* USER CODE END RTC_MspInit 1 */
    }

}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtc)
{

    if(rtc->Instance==RTC)
    {
        /* USER CODE BEGIN RTC_MspDeInit 0 */

        /* USER CODE END RTC_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_RTC_DISABLE();
        /* USER CODE BEGIN RTC_MspDeInit 1 */

        /* USER CODE END RTC_MspDeInit 1 */
    }

}

void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{

    GPIO_InitTypeDef GPIO_InitStruct;
    if(hspi->Instance==SPI2)
    {
        /* USER CODE BEGIN SPI2_MspInit 0 */

        /* USER CODE END SPI2_MspInit 0 */
        /* Peripheral clock enable */
        __SPI2_CLK_ENABLE();

        /**SPI2 GPIO Configuration
        PB10     ------> SPI2_SCK
        PB14     ------> SPI2_MISO
        PB15     ------> SPI2_MOSI
        PB9     ------> SPI2_NSS
        */
        GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_14|GPIO_PIN_15;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* USER CODE BEGIN SPI2_MspInit 1 */

        /* USER CODE END SPI2_MspInit 1 */
    }

}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{

    if(hspi->Instance==SPI2)
    {
        /* USER CODE BEGIN SPI2_MspDeInit 0 */

        /* USER CODE END SPI2_MspDeInit 0 */
        /* Peripheral clock disable */
        __SPI2_CLK_DISABLE();

        /**SPI2 GPIO Configuration
        PB10     ------> SPI2_SCK
        PB14     ------> SPI2_MISO
        PB15     ------> SPI2_MOSI
        PB9     ------> SPI2_NSS
        */
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_9);

        /* USER CODE BEGIN SPI2_MspDeInit 1 */

        /* USER CODE END SPI2_MspDeInit 1 */
    }

}

void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    __PWR_CLK_ENABLE();

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
#ifdef BOARD_ARMFLY
    RCC_OscInitStruct.PLL.PLLM = 25;
#else
    RCC_OscInitStruct.PLL.PLLM = 12;
#endif
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 4;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1
                                  |RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;/* 168Mhz主频，不要分频. */
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
}

void MemoryBus_Config(void)
{
    FSMC_NORSRAM_InitTypeDef FInit;
    FSMC_NORSRAM_TimingTypeDef FTiming;
    FSMC_NAND_PCC_TimingTypeDef ComSpaceTiming;
    FSMC_NAND_PCC_TimingTypeDef AttSpaceTiming;
    GPIO_InitTypeDef            GPIO_InitStruct;

    /* Peripheral clock enable */
    __FSMC_CLK_ENABLE();

    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_FSMC;

    GPIO_InitStruct.Pin = GPIO_PIN_0  |GPIO_PIN_1  | GPIO_PIN_4 | GPIO_PIN_5 |
                          GPIO_PIN_7 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_14 |
                          GPIO_PIN_15;

    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_FSMC;

    GPIO_InitStruct.Pin = GPIO_PIN_7  | GPIO_PIN_8  | GPIO_PIN_9 | GPIO_PIN_10;

    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_FSMC;

    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_12;

    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    /* FPGA interface Init */
    FInit.NSBank = FSMC_NORSRAM_BANK4;
    FInit.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;
    FInit.MemoryType = FSMC_MEMORY_TYPE_SRAM;
    FInit.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;
    FInit.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;
    FInit.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;
    FInit.WrapMode = FSMC_WRAP_MODE_DISABLE;
    FInit.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;
    FInit.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;
    FInit.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;
    FInit.ExtendedMode = FSMC_EXTENDED_MODE_DISABLE;
    FInit.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
    FInit.WriteBurst = FSMC_WRITE_BURST_DISABLE;
    FInit.PageSize = FSMC_PAGE_SIZE_NONE;

    FTiming.AddressSetupTime = 14;
    FTiming.AddressHoldTime = 0;
    FTiming.DataSetupTime = 16;
    FTiming.BusTurnAroundDuration = 0;
    FTiming.CLKDivision = 0;
    FTiming.DataLatency = 0;
    FTiming.AccessMode = FSMC_ACCESS_MODE_A;

    FSMC_NORSRAM_Init(FSMC_NORSRAM_DEVICE, &FInit);
    FSMC_NORSRAM_Timing_Init(FSMC_NORSRAM_DEVICE, &FTiming, FSMC_NORSRAM_BANK4);
    __FMC_NORSRAM_ENABLE(FSMC_NORSRAM_DEVICE, FSMC_NORSRAM_BANK4);
#if 0
    printf("bcr2: 0x%x\n", FSMC_NORSRAM_DEVICE->BTCR[2]);
    printf("btr2: 0x%x\n", FSMC_NORSRAM_DEVICE->BTCR[3]);

    FSMC_NORSRAM_DEVICE->BTCR[6] = 0x1015;
    printf("bcr4: 0x%x\n", FSMC_NORSRAM_DEVICE->BTCR[6]);
    printf("btr4: 0x%x\n", FSMC_NORSRAM_DEVICE->BTCR[7]);
#endif

    /* NAND Bank Init Structure */
    NandInit.NandBank        = FSMC_NAND_BANK2;
    NandInit.Waitfeature     = FSMC_NAND_PCC_WAIT_FEATURE_DISABLE;
    NandInit.MemoryDataWidth = FSMC_NAND_PCC_MEM_BUS_WIDTH_8;
    NandInit.EccComputation  = FSMC_NAND_ECC_DISABLE;
    NandInit.ECCPageSize     = FSMC_NAND_ECC_PAGE_SIZE_2048BYTE;
    NandInit.TCLRSetupTime   = 1;
    NandInit.TARSetupTime    = 1;

    /* ComSpaceTiming */
    ComSpaceTiming.SetupTime     = 1;// 1
    ComSpaceTiming.WaitSetupTime = 5;// 3
    ComSpaceTiming.HoldSetupTime = 5;// 2
    ComSpaceTiming.HiZSetupTime  = 2;// 1
    /* AttSpaceTiming */
    AttSpaceTiming.SetupTime     = 1;// 1
    AttSpaceTiming.WaitSetupTime = 5;// 3
    AttSpaceTiming.HoldSetupTime = 5;// 2
    AttSpaceTiming.HiZSetupTime  = 2;// 1

    /* Initialize NAND control Interface */
    FMC_NAND_Init(FSMC_NAND_DEVICE, &(NandInit));

    /* Initialize NAND common space timing Interface */
    FMC_NAND_CommonSpace_Timing_Init(FSMC_NAND_DEVICE, &ComSpaceTiming, NandInit.NandBank);

    /* Initialize NAND attribute space timing Interface */
    FMC_NAND_AttributeSpace_Timing_Init(FSMC_NAND_DEVICE, &AttSpaceTiming, NandInit.NandBank);

    /* Enable the NAND device */
    __FMC_NAND_ENABLE(FSMC_NAND_DEVICE, NandInit.NandBank);
}

void bsp_Init(void)
{
    /* config by bootloader */
    SystemClock_Config();
    //HAL_Init();
		IWDG_Init();
    GPIO_Init();
    DMA_Init();
    RTC_Init();
    MX_ADC1_Init();
    MX_ADC3_Init();
    SPI1_Init();
    SPI2_Init();
    USART1_UART_Init();
    USART3_UART_Init();
    USART4_UART_Init();
    USART6_UART_Init();
    LED_Initialize();
    MemoryBus_Config();
#if(TEST_MODE == 0)	
	CAN_Init();
#endif

//		CAN_Initialize();
//		if(CAN_Initialize() == false) 
//		{ 
//			Error_Handler ();
//		}
}

