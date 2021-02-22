/**
 * @file			epcs.h
 * @brief			
 * @author			wangpeng
 * @date			2016/11/11
 * @version			Initial Draft
 * @par				Copyright (C),  2013-2023, SCNEE
 * @par History:
 * 1.Date: 			2016/11/11
 *   Author: 			wangpeng
 *   Modification: 		Created file
*/

#ifndef __EPCS_COMMON_H__
#define __EPCS_COMMON_H__

#include <stdint.h>



#define EPCS_SPI_CMD_WRITE_ENABLE		0x06
#define EPCS_SPI_CMD_WRITE_DISABLE		0x04
#define EPCS_SPI_CMD_READ_STATUS		0x05
#define EPCS_SPI_CMD_READ_BYTES			0x03
#define EPCS_SPI_CMD_READ_SILICON_ID	0xAB
#define EPCS_SPI_CMD_FAST_READ			0x0B
#define EPCS_SPI_CMD_WRITE_STATUS		0x01
#define EPCS_SPI_CMD_WRITE_BYTES		0x02
#define EPCS_SPI_CMD_ERASE_BULK			0xC7
#define EPCS_SPI_CMD_ERASE_SECTOR		0xD8
#define EPCS_SPI_CMD_READ_DEVID			0x9F

#define EPCS1_SILICON_ID				0x10
#define EPCS4_SILICON_ID				0x12
#define EPCS16_SILICON_ID				0x14
#define EPCS64_SILICON_ID				0x15

#define EPCS_STATUS_MASK   0X1F
#define EPCS_STATUS_BLOCK_PROTECT_MASK  (7<<2)
#define EPCS_STATUS_WRITE_ENABLE_MASK  (1<<1)
#define EPCS_STATUS_WRITE_ENABLE  (1<<1)
#define EPCS_STATUS_WRITE_DISABLE (0<<1)
#define EPCS_STATUS_BUSY_MASK  (1)
#define EPCS_STATUS_IS_BUSY    (1)
#define EPCS_STATUS_NOT_BUSY   (0)


enum epcsOperateIndex
{
    IDX_EPCS_OP_WRITE_ENABLE=0,
    IDX_EPCS_OP_WRITE_DISABLE,
    IDX_EPCS_OP_READ_STATUS,
    IDX_EPCS_OP_READ_BYTES,
    IDX_EPCS_OP_READ_SILICON_ID,
    IDX_EPCS_OP_FAST_READ,
    IDX_EPCS_OP_WRITE_STATUS,
    IDX_EPCS_OP_WRITE_BYTES,
    IDX_EPCS_OP_ERASE_BULK,
    IDX_EPCS_OP_ERASE_SECTOR,
    QTY_OF_EPCS_OP,
};



int Epcs_WriteEnable(void);

int Epcs_WriteDisable(void);

int Epcs_ReadStatus(uint8_t* pStatus);

int Epcs_ReadBytes(uint32_t startAddress,uint8_t* pBuff,uint32_t readLength);


int Epcs_ReadSiliconId(uint8_t* pId);


int Epcs_FastRead(uint32_t startAddress,uint8_t* pBuff,uint32_t readLength);

int Epcs_WriteStatus(uint8_t newStatus);

int Epcs_WriteBytes(uint32_t startAddress,uint8_t* pBuff,uint32_t writeLength);

int Epcs_EraseBulk(void);

int Epcs_EraseSector(uint32_t startAddress);
int Epcs_ByteMsbToLsb(uint8_t* pByte,uint32_t length);

int Epcs_Write(uint32_t startAddress, uint8_t* pBuff, uint32_t length);

int Epcs_Read(uint32_t startAddress, uint8_t* pBuff, uint32_t length);

#endif /* EPCS_H */

