#ifndef __SPI_GPIO_H__
#define __SPI_GPIO_H__

//w5500支持mode 0和mode 3.
#include "stdint.h"
#include "stm32f4xx_hal_conf.h"
enum spiModeType
{
    SPI_MODE_0=0,
    SPI_MODE_1,
    SPI_MODE_2,
    SPI_MODE_3
};

enum spiSpeedType
{
    SPI_SPEED_80MHZ,
    SPI_SPEED_25MHZ,
    SPI_SPEED_20MHZ,
    SPI_SPEED_15MHZ,
    SPI_SPEED_10MHZ,
};
struct gpioPinType
{
    GPIO_TypeDef* gp_port;
    uint16_t      gp_pin;
};

struct spiPinType
{
    struct gpioPinType spi_mosi;
    struct gpioPinType spi_miso;
    struct gpioPinType spi_sclk;
    struct gpioPinType spi_cs;
    enum spiModeType   spi_mode;
    enum spiSpeedType  spi_speed;/* spi通信支持的最大hz. */
};



void GpioSpi_Init(struct spiPinType* pSpi,uint8_t cpol,uint8_t cpha);
void GpioSpi_Init2(struct spiPinType* pSpi,uint8_t spiMode);
void GpioSpi_Init3(struct spiPinType* pSpi);
void GpioSpi_DeInit(struct spiPinType* pSpi);

void Spi_ChipSelect(struct spiPinType* pSpi);

void Spi_ChipDeselect(struct spiPinType* pSpi);

uint8_t Spi_ReadByte(struct spiPinType* pSpi);
void Spi_WriteByte(struct spiPinType* pSpi,uint8_t wb);
void Spi_ReadBurst(struct spiPinType* pSpi,uint8_t* pBuf, uint16_t len);
void Spi_WriteBurst(struct spiPinType* pSpi,uint8_t* pBuf, uint16_t len);

uint8_t Spi_TransferBytes(struct spiPinType* pSpi,uint8_t* pSendData,uint32_t sendByteQty,uint8_t* pRecvData,uint32_t recvByteQty);
uint8_t Spi_TransferBytes_V2(struct spiPinType* pSpi,uint8_t* pSendData,uint32_t sendByteQty,uint8_t* pRecvData,uint32_t recvByteQty);
uint8_t Spi_TransferBytes_V3(struct spiPinType* pSpi,uint8_t* pSendData,uint8_t* pRecvData,uint32_t totalByteQty);

#endif
