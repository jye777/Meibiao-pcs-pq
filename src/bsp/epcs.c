#include <string.h>
#include "cmsis_os.h"
#include "Driver_SPI.h"
#include "stm32f4xx_hal.h"
#include "tty.h"
#include "c_md5.h"
#include "epcs16.h"
#include "epcs.h"
#include "epcs_common.h"
#include "spi_gpio.h"


extern ARM_DRIVER_SPI Driver_SPI2;
extern uint8_t pof_head[POF_HEAD_LENGTH];
extern uint8_t pof_tail[POF_TAIL_LENGTH];

osSemaphoreId epcs_sem = NULL;
osSemaphoreDef(epcs_sem);

uint8_t epcs_tempbuf[0x10010];
uint8_t epcs_databuf[0x10000];

#define FPGA_NCONFIG_PIN        GPIO_PIN_8
#define FPGA_NCE_PIN            GPIO_PIN_7

#define epcs_dbg        tty_printf

void spi_callback(uint32_t event)
{
    switch (event)
    {
    case ARM_SPI_EVENT_TRANSFER_COMPLETE:
        osSemaphoreRelease(epcs_sem);
        break;
    case ARM_SPI_EVENT_DATA_LOST:
        break;
    case ARM_SPI_EVENT_MODE_FAULT:
        break;
    }
}

static void delay(void)
{
    uint32_t tick;
    uint32_t n = 0;

    tick = osKernelSysTick();

    while(n < 1)
    {
        if(osKernelSysTick() != tick)
        {
            n++;
        }
    }
}



void epcs_init(void)
{
    if(epcs_sem == NULL)
    {
        epcs_sem = osSemaphoreCreate(osSemaphore(epcs_sem), 1);
    }

    osSemaphoreWait(epcs_sem, 1);
    EpcsSpi_Init();
}

void epcs_close(void)
{
    EpcsSpi_Close();
}
void epcs_Reset(void)
{
    EpcsSpi_Reset();
}


int epcs_read_silicon_id(uint8_t *id)
{
    int err;
    err = Epcs_ReadSiliconId(id);

    return err;
}

int epcs_read_status(uint8_t *status)
{
    int err;

    err = Epcs_ReadStatus(status);


    return err;
}

int epcs_write_status(uint8_t status)
{
    int err;

    err = Epcs_WriteStatus(status);

    return err;
}

int epcs_write_enable(void)
{
    int err;
    err = Epcs_WriteEnable();

    return err;
}

int epcs_write_disable(void)
{
    int err;
    err = Epcs_WriteDisable();

    return err;
}

int epcs_need_erase(uint32_t addr)
{
    return Epcs_NeedErase(addr);
}

int epcs_erase_sector(uint32_t addr)
{
    int err;
    err = Epcs_EraseSector(addr);

    return err;
}

int epcs_erase_bulk(void)
{
    int err;

    err = Epcs_EraseBulk();

    return err;
}

int epcs_write_bytes(uint8_t *buf, uint32_t addr, uint32_t size)
{
    int err;

    err = Epcs_WriteBytes(addr, buf, size);

    return err;
}

int epcs_read_bytes(unsigned char *buf, unsigned int addr, unsigned int size)
{
    int err;

    err = Epcs_ReadBytes(addr, buf, size);
    return err;
}

int epcs_transdata(unsigned char *buf, unsigned int len)
{
    Epcs_ByteMsbToLsb(buf,len);

    return 0;
}

int epcs_read(unsigned char *buf, unsigned int addr, unsigned int len)
{
    Epcs_Read(addr,buf,len);


    return 0;
}

int epcs_write(unsigned char *buf, unsigned int addr, unsigned int len)
{
    return Epcs_Write(addr,buf,len);
}

#define MD5_BUF_SIZE        sizeof(epcs_databuf)
//#define MD5_BUF_SIZE        0x400

int epcs_get_md5(uint32_t codesize, uint8_t md5[16]) //POF_LOAD
{
    uint32_t offset = 0, size = MD5_BUF_SIZE;
    uint8_t *buf = epcs_databuf;
    MD5_CTX context;

    MD5Init(&context);

    size = MD5_BUF_SIZE;
    while(1)
    {

        if(offset == 0)
        {
            size = MD5_BUF_SIZE - POF_HEAD_LENGTH;
            if(epcs_read(buf + POF_HEAD_LENGTH, offset, size))
            {
                break;
            }
            epcs_transdata(buf, MD5_BUF_SIZE);
            memcpy(buf, pof_head, POF_HEAD_LENGTH);
            offset += size;
            MD5Update(&context, buf, MD5_BUF_SIZE);
        }

        else if(offset + POF_HEAD_LENGTH >= POF_CODE_LENGTH)
        {
            size = POF_HEAD_LENGTH;
            if(epcs_read(buf, offset, POF_HEAD_LENGTH))
            {
                break;
            }
            epcs_transdata(buf, POF_HEAD_LENGTH + POF_TAIL_LENGTH);
            memcpy(buf + POF_HEAD_LENGTH, pof_tail, POF_TAIL_LENGTH);
            MD5Update(&context, buf, POF_HEAD_LENGTH + POF_TAIL_LENGTH);
            break;
        }

        else
        {
            size = MD5_BUF_SIZE;
            if(epcs_read(buf, offset, size))
            {
                break;
            }
            epcs_transdata(buf, size);
            offset += size;
            MD5Update(&context, buf, size);
        }
				osDelay(1);
    }

    MD5Final(md5, &context);

    return 0;
}

