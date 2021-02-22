#include <string.h>
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"
#include "epcs.h"
//#include "debug.h"
#include "spi_gpio.h"

#if EPCS_DEBUG_ENABLE
#define EPCS16_DEBUG Debug_Printf
#else
#define EPCS16_DEBUG(...)
#endif

#define FPGA_NCONFIG_PORT        GPIOG //PA4
#define FPGA_NCONFIG_PIN        GPIO_PIN_8 //PA4
#define FPGA_NCE_PORT            GPIOG  //PF11
#define FPGA_NCE_PIN            GPIO_PIN_7  //PF11


struct spiPinType epcs16Spi=
{
    .spi_mosi={GPIOG,GPIO_PIN_10},
    .spi_miso={GPIOD,GPIO_PIN_2},
    .spi_sclk={GPIOG,GPIO_PIN_11},
    .spi_cs={GPIOD,GPIO_PIN_3},
    .spi_mode=SPI_MODE_0,
    .spi_speed=SPI_SPEED_20MHZ,
};


void EpcsSpi_Init(void)
{
    GpioSpi_Init3(&epcs16Spi);
    
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = FPGA_NCONFIG_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(FPGA_NCONFIG_PORT, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = FPGA_NCE_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(FPGA_NCE_PORT,&GPIO_InitStruct);

    HAL_GPIO_WritePin(FPGA_NCONFIG_PORT, FPGA_NCONFIG_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(FPGA_NCE_PORT, FPGA_NCE_PIN, GPIO_PIN_RESET);
}


void EpcsSpi_Close(void)
{

    GpioSpi_DeInit(&epcs16Spi);

    /* Configure F_NCE:PG7 F_NCONFIG:PG8 */
    HAL_GPIO_DeInit(FPGA_NCE_PORT, FPGA_NCE_PIN);
    HAL_GPIO_DeInit(FPGA_NCONFIG_PORT, FPGA_NCONFIG_PIN);
}

void EpcsSpi_Reset(void)
{
    GpioSpi_DeInit(&epcs16Spi);
    HAL_GPIO_WritePin(FPGA_NCE_PORT, FPGA_NCE_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(FPGA_NCONFIG_PORT, FPGA_NCONFIG_PIN, GPIO_PIN_SET);
}



int EpcsSpi_WriteRead(uint8_t* pWriteBuff,uint32_t writeLength,
                            uint8_t* pReadBuff,uint32_t readLength)
{
    struct spiPinType * pSpi= &epcs16Spi;
    //Spi_ChipSelect(pSpi);
    Spi_TransferBytes(pSpi,pWriteBuff,writeLength,pReadBuff,readLength);
    //Spi_ChipDeselect(pSpi);
    return 0;
}
int EpcsSpi_ChipSelect(void)
{
    struct spiPinType * pSpi= &epcs16Spi;
    Spi_ChipSelect(pSpi);
    //Spi_ChipDeselect(pSpi);
    return 0;
}
int EpcsSpi_ChipDeselect(void)
{
    struct spiPinType * pSpi= &epcs16Spi;
    //Spi_ChipSelect(pSpi);
    Spi_ChipDeselect(pSpi);
    return 0;
}





