#include "stdlib.h"
#include "stdint.h"
#include "string.h"
#include "bsp.h"
#include "spi_gpio.h"
#include "stm32f4xx_hal_conf.h"



static void GpioSpi_SetPin(struct gpioPinType* pGpioPin)
{
    HAL_GPIO_WritePin(pGpioPin->gp_port,pGpioPin->gp_pin,GPIO_PIN_SET);
}

static void GpioSpi_ResetPin(struct gpioPinType* pGpioPin)
{
    HAL_GPIO_WritePin(pGpioPin->gp_port,pGpioPin->gp_pin,GPIO_PIN_RESET);
}

static void GpioSpi_TogglePin(struct gpioPinType* pGpioPin)
/* toggle pin level */
{
    HAL_GPIO_TogglePin(pGpioPin->gp_port,pGpioPin->gp_pin);
}

static GPIO_PinState GpioSpi_GetPin(struct gpioPinType* pGpioPin)
{
    return HAL_GPIO_ReadPin(pGpioPin->gp_port,pGpioPin->gp_pin);
}


static void GpioSpi_Delay(enum spiSpeedType spiSpeed)
{
    uint32_t delayCount=0;
    /* 系统cpu主频为168Mhz */
    switch(spiSpeed)
    {
        case SPI_SPEED_10MHZ:
        case SPI_SPEED_15MHZ:
        {
            /* 这句语句大概被翻译成6条汇编指令, */
            while(++delayCount < 3);
        }
        break;
        case SPI_SPEED_20MHZ:
        {
            while(++delayCount < 2);
        }
        break;
        case SPI_SPEED_25MHZ:
        {
            while(++delayCount < 1);
        }
        break;
        default:break;
    }
}


/*
* SPI_MODE_0:  cpol = 0,cpha = 0
* SPI_MODE_1:  cpol = 0,cpha = 1
* SPI_MODE_2:  cpol = 1,cpha = 0
* SPI_MODE_3:  cpol = 1,cpha = 1
*/
void GpioSpi_Init(struct spiPinType* pSpi,uint8_t cpol,uint8_t cpha)
{
    if(! pSpi )
    {
        return;
    }
    /* init gpio pins. */

    /* init mode */
    if(cpol == 0)
    {
        if(cpha == 0)
        {
            pSpi->spi_mode = SPI_MODE_0;
        }
        else if(cpha)
        {
            pSpi->spi_mode = SPI_MODE_1;
        }
    }
    else if(cpol)
    {
        if(cpha)
        {
            pSpi->spi_mode = SPI_MODE_3;
        }
        else if(cpha == 0)
        {
            pSpi->spi_mode = SPI_MODE_2;
        }
    }
}

/*
* SPI_MODE_0:  cpol = 0,cpha = 0
* SPI_MODE_1:  cpol = 0,cpha = 1
* SPI_MODE_2:  cpol = 1,cpha = 0
* SPI_MODE_3:  cpol = 1,cpha = 1
*/
void GpioSpi_Init2(struct spiPinType* pSpi,uint8_t spiMode)
{
    if(! pSpi || spiMode > SPI_MODE_3)
    {
        return;
    }
    /* init gpio pins. */

    GPIO_InitTypeDef GPIO_InitStruct;

    /* GPIO Ports Clock Enable */
    struct gpioPinType gpioPin;
    
    gpioPin = pSpi->spi_cs;
    GPIO_InitStruct.Pin = gpioPin.gp_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(gpioPin.gp_port, &GPIO_InitStruct);
    
    gpioPin = pSpi->spi_sclk;
    GPIO_InitStruct.Pin = gpioPin.gp_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(gpioPin.gp_port, &GPIO_InitStruct);
    
    gpioPin = pSpi->spi_mosi;
    GPIO_InitStruct.Pin = gpioPin.gp_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(gpioPin.gp_port, &GPIO_InitStruct);
    
    gpioPin = pSpi->spi_miso;
    GPIO_InitStruct.Pin = gpioPin.gp_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(gpioPin.gp_port, &GPIO_InitStruct);

    /* init mode */
    pSpi->spi_mode = (enum spiModeType)spiMode;
}


void GpioSpi_Init3(struct spiPinType* pSpi)
{
    if(! pSpi)
    {
        return;
    }
    /* init gpio pins. */

    GPIO_InitTypeDef GPIO_InitStruct;

    /* GPIO Ports Clock Enable */
    struct gpioPinType gpioPin;
    
    gpioPin = pSpi->spi_cs;
    GPIO_InitStruct.Pin = gpioPin.gp_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(gpioPin.gp_port, &GPIO_InitStruct);
    
    gpioPin = pSpi->spi_sclk;
    GPIO_InitStruct.Pin = gpioPin.gp_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(gpioPin.gp_port, &GPIO_InitStruct);
    
    gpioPin = pSpi->spi_mosi;
    GPIO_InitStruct.Pin = gpioPin.gp_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(gpioPin.gp_port, &GPIO_InitStruct);
    
    gpioPin = pSpi->spi_miso;
    GPIO_InitStruct.Pin = gpioPin.gp_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(gpioPin.gp_port, &GPIO_InitStruct);

    pSpi->spi_mode = pSpi->spi_mode;
}

void GpioSpi_DeInit(struct spiPinType* pSpi)
{
    if(! pSpi)
    {
        return;
    }
    /* init gpio pins. */

    GPIO_InitTypeDef GPIO_InitStruct;

    /* GPIO Ports Clock Enable */
    struct gpioPinType gpioPin;
    
    gpioPin = pSpi->spi_cs;
    HAL_GPIO_DeInit(gpioPin.gp_port, gpioPin.gp_pin);
    
    gpioPin = pSpi->spi_sclk;
    HAL_GPIO_DeInit(gpioPin.gp_port, gpioPin.gp_pin);
    
    gpioPin = pSpi->spi_mosi;
    HAL_GPIO_DeInit(gpioPin.gp_port, gpioPin.gp_pin);
    
    gpioPin = pSpi->spi_miso;
    HAL_GPIO_DeInit(gpioPin.gp_port, gpioPin.gp_pin);

}


#define SPI_CS_H()    do{GpioSpi_SetPin(&(pSpi->spi_cs));}while(0);
#define SPI_CS_L()    do{GpioSpi_ResetPin(&(pSpi->spi_cs));}while(0);
#define SPI_SCLK_H()  do{GpioSpi_SetPin(&(pSpi->spi_sclk));}while(0);
#define SPI_SCLK_L()  do{GpioSpi_ResetPin(&(pSpi->spi_sclk));}while(0);
#define SPI_SCLK_T()  do{GpioSpi_TogglePin(&(pSpi->spi_sclk));}while(0);

#define SPI_MOSI_H()  do{GpioSpi_SetPin(&(pSpi->spi_mosi));}while(0);
#define SPI_MOSI_L()  do{GpioSpi_ResetPin(&(pSpi->spi_mosi));}while(0);
#define SPI_MISO()    GpioSpi_GetPin(&(pSpi->spi_miso))
#define SPI_MISO_DUMMY() 
#define SPI_MOSI_DUMMY()  SPI_MOSI_L()




void Spi_ChipSelect(struct spiPinType* pSpi)
{
    SPI_CS_L();
}

void Spi_ChipDeselect(struct spiPinType* pSpi)
{
    SPI_CS_H();
}


uint8_t Spi_ReadByte(struct spiPinType* pSpi)
{
    return 0;
}

void Spi_WriteByte(struct spiPinType* pSpi,uint8_t wb)
{}



void Spi_ReadBurst(struct spiPinType* pSpi,uint8_t* pBuf, uint16_t len)
{}

void Spi_WriteBurst(struct spiPinType* pSpi,uint8_t* pBuf, uint16_t len)
{}


/* 
* (这个函数已更正)first sending all to-be-sent data.start recving as soon as finishing sending.
* 片选和传输函数是独立开了的
*/

uint8_t Spi_TransferBytes(struct spiPinType* pSpi,uint8_t* pSendData,uint32_t sendByteQty,uint8_t* pRecvData,uint32_t recvByteQty)
{
    uint32_t byteQty = sendByteQty + recvByteQty;
    if(pRecvData && recvByteQty)
    {
        memset((void*)pRecvData,0,recvByteQty);
    }

    if(pSpi->spi_mode <= SPI_MODE_1)
    {
        SPI_SCLK_L();
    }
    else
    {
        SPI_SCLK_H();
    }
    //SPI_CS_L();
    if(pSpi->spi_mode == SPI_MODE_0 || pSpi->spi_mode == SPI_MODE_2)
    {
        for(uint32_t byteIndex=0;byteIndex<byteQty;byteIndex++)
        {
            for(uint8_t bitIndex=0;bitIndex<8;bitIndex++)
            {
                /* make data ready */
                if(byteIndex < sendByteQty)
                {
                    if(pSendData)
                    {
                        if((pSendData[byteIndex] << bitIndex) & 0x80)
                        {
                            SPI_MOSI_H();
                        }
                        else
                        {
                            SPI_MOSI_L();
                        }
                    }
                    else
                    {
                        SPI_MOSI_DUMMY();
                    }
                }
                else
                {
                    SPI_MOSI_DUMMY();
                }
                GpioSpi_Delay(pSpi->spi_speed);
                SPI_SCLK_T();/* 第一个边沿时采样 */

                /* start recving after finished sending all data. */
                if(byteIndex >= sendByteQty)
                {
                    if(pRecvData)
                    {
                        pRecvData[byteIndex-sendByteQty] |= ((SPI_MISO()==GPIO_PIN_SET)?(1):(0))<<(7-bitIndex);
                    }
                    else
                    {
                        SPI_MISO_DUMMY();
                    }
                }
                else
                {
                    /* 此时采集到的从机的数据没有任何意义 */
                    SPI_MISO_DUMMY();
                }
                GpioSpi_Delay(pSpi->spi_speed);
                SPI_SCLK_T();/* 第2个边沿后准备数据 */
            }
        }

    }

    if(pSpi->spi_mode == SPI_MODE_1 || pSpi->spi_mode == SPI_MODE_3)
    {
        for(uint32_t byteIndex=0;byteIndex<byteQty;byteIndex++)
        {

            for(uint8_t bitIndex=0;bitIndex<8;bitIndex++)
            {
                SPI_SCLK_T();/* 第1个边沿后准备数据 */
                /* send first. */
                if(byteIndex < sendByteQty)
                {
                    if(pSendData)
                    {
                        if((pSendData[byteIndex] << bitIndex) & 0x80)
                        {
                            SPI_MOSI_H();
                        }
                        else
                        {
                            SPI_MOSI_L();
                        }
                    }
                    else
                    {
                        SPI_MOSI_DUMMY();
                    }
                }
                else
                {
                    SPI_MOSI_DUMMY();
                }
                GpioSpi_Delay(pSpi->spi_speed);
                SPI_SCLK_T();/* 第2个边沿时采样 */

                /* start recving after finished sending all data. */
                if(byteIndex >= sendByteQty)
                {
                    if(pRecvData)
                    {
                        pRecvData[byteIndex-sendByteQty] |= ((SPI_MISO()==GPIO_PIN_SET)?(1):(0))<<(7-bitIndex);
                    }
                    else
                    {
                        SPI_MISO_DUMMY();
                    }
                }
                else
                {
                    /* 此时读到的数据没有意义 */
                    SPI_MISO_DUMMY();
                }
                GpioSpi_Delay(pSpi->spi_speed);
            }
        }

    }

    //SPI_CS_H();
    if(pSpi->spi_mode <= SPI_MODE_1)
    {
        SPI_SCLK_L();
    }
    else
    {
        SPI_SCLK_H();
    }
    return 0;
}


/* 
* first sending all to-be-sent data.start recving as soon as finishing sending.
*/

uint8_t Spi_TransferBytes_V2(struct spiPinType* pSpi,uint8_t* pSendData,uint32_t sendByteQty,uint8_t* pRecvData,uint32_t recvByteQty)
{

    return 0;
}

/* (这个函数待更正)must make sure pSendData and pRecvData are in the same array, pSendData is at head and pRecvData followed. */
uint8_t Spi_TransferBytes_V3(struct spiPinType* pSpi,uint8_t* pSendData,uint8_t* pRecvData,uint32_t totalByteQty)
{
    return 0;
}


