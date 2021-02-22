#include <string.h>
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"

#include "c_md5.h"
#include "epcs16.h"
#include "epcs_common.h"
//#include "debug.h"

#if EPCS_DEBUG_ENABLE
#define EPCS_DEBUG Debug_Printf
#else
#define EPCS_DEBUG(...)
#endif
//#define EPCS_DEBUG printf
struct epcsOperateFrame
{
    enum epcsOperateIndex eof_operateIdx;
    uint8_t               eof_operateCode;
    uint8_t               eof_addressBytes;
    uint8_t               eof_dummyBytes;
};


const struct epcsOperateFrame epcsAllOperation[QTY_OF_EPCS_OP] =
{
    {
        .eof_operateIdx   = IDX_EPCS_OP_WRITE_ENABLE,
        .eof_operateCode  = EPCS_SPI_CMD_WRITE_ENABLE,//0x06,
        .eof_addressBytes = 0,
        .eof_dummyBytes   = 0
    },
    
    {
        .eof_operateIdx   = IDX_EPCS_OP_WRITE_DISABLE,
        .eof_operateCode  = EPCS_SPI_CMD_WRITE_DISABLE,//0x04,
        .eof_addressBytes = 0,
        .eof_dummyBytes   = 0,
    },
    
    {
        .eof_operateIdx   = IDX_EPCS_OP_READ_STATUS,
        .eof_operateCode  = EPCS_SPI_CMD_READ_STATUS,//0x05,
        .eof_addressBytes = 0,
        .eof_dummyBytes   = 0,
    },
    
    {
        .eof_operateIdx   = IDX_EPCS_OP_READ_BYTES,
        .eof_operateCode  = EPCS_SPI_CMD_READ_BYTES,//0x03,
        .eof_addressBytes = 3,
        .eof_dummyBytes   = 0,
    },
    
    {
        .eof_operateIdx   = IDX_EPCS_OP_READ_SILICON_ID,
        .eof_operateCode  = EPCS_SPI_CMD_READ_SILICON_ID,//0xab,
        .eof_addressBytes = 0,
        .eof_dummyBytes   = 3,
    },
    
    {
        .eof_operateIdx   = IDX_EPCS_OP_FAST_READ,
        .eof_operateCode  = EPCS_SPI_CMD_FAST_READ,//0x0b,
        .eof_addressBytes = 3,
        .eof_dummyBytes   = 1,
    },
    
    {
        .eof_operateIdx   = IDX_EPCS_OP_WRITE_STATUS,
        .eof_operateCode  = EPCS_SPI_CMD_WRITE_STATUS,//0x01,
        .eof_addressBytes = 0,
        .eof_dummyBytes   = 0,
    },
    
    {
        .eof_operateIdx   = IDX_EPCS_OP_WRITE_BYTES,
        .eof_operateCode  = EPCS_SPI_CMD_WRITE_BYTES,//0x02,
        .eof_addressBytes = 3,
        .eof_dummyBytes   = 0,
    },
    
    {
        .eof_operateIdx   = IDX_EPCS_OP_ERASE_BULK,
        .eof_operateCode  = EPCS_SPI_CMD_ERASE_BULK,//0xc7,
        .eof_addressBytes = 0,
        .eof_dummyBytes   = 0,
    },
    
    {
        .eof_operateIdx   = IDX_EPCS_OP_ERASE_SECTOR,
        .eof_operateCode  = EPCS_SPI_CMD_ERASE_SECTOR,//0xd8,
        .eof_addressBytes = 3,
        .eof_dummyBytes   = 0,
    }
};





static uint32_t Epcs_PackageOperationCode(const struct epcsOperateFrame* pEpcsOp,
                                                        uint32_t startAddress,
                                                        uint8_t* pBuff)
{
    int index=0;
    
    uint32_t address=startAddress;
    
    /* operation code */
    pBuff[index++] = pEpcsOp->eof_operateCode;
    /* address */
    if(pEpcsOp->eof_addressBytes)
    {
      pBuff[index++] = (uint8_t)((address >> 16) & 0xff);
      pBuff[index++] = (uint8_t)((address >> 8) & 0xff);
      pBuff[index++] = (uint8_t)((address >> 0) & 0xff);
    }
    /* dummy bytes */
    if(pEpcsOp->eof_dummyBytes)
    {
      memset(pBuff+index,0,pEpcsOp->eof_dummyBytes);
      index += pEpcsOp->eof_dummyBytes;
    }
    return index;
}


int Epcs_WriteEnable(void)
{
    uint8_t wbuf[7];
    uint32_t length=0;
    uint32_t address=0;
    const struct epcsOperateFrame* pEpcsOp=epcsAllOperation+IDX_EPCS_OP_WRITE_ENABLE;

    length=Epcs_PackageOperationCode(pEpcsOp,address,wbuf);
    EpcsSpi_ChipSelect();
    EpcsSpi_WriteRead(wbuf,length,NULL,0);
    EpcsSpi_ChipDeselect();

    return 0;
}

int Epcs_WriteDisable(void)
{
    uint8_t wbuf[7];
    uint32_t length=0;
    uint32_t address=0;
    const struct epcsOperateFrame* pEpcsOp=epcsAllOperation+IDX_EPCS_OP_WRITE_DISABLE;

    length=Epcs_PackageOperationCode(pEpcsOp,address,wbuf);
    EpcsSpi_ChipSelect();
    EpcsSpi_WriteRead(wbuf,length,NULL,0);
    EpcsSpi_ChipDeselect();

    return 0;
}

int Epcs_ReadStatus(uint8_t* pStatus)
{
    uint8_t wbuf[7];
    uint32_t length=0;
    uint32_t address=0;
    const struct epcsOperateFrame* pEpcsOp=epcsAllOperation+IDX_EPCS_OP_READ_STATUS;

    length=Epcs_PackageOperationCode(pEpcsOp,address,wbuf);
#if 0
    EpcsSpi_WriteRead(wbuf,length,NULL,0);
    EpcsSpi_WriteRead(NULL,0,pStatus,1);
#else
    EpcsSpi_ChipSelect();
    EpcsSpi_WriteRead(wbuf,length,pStatus,1);
    EpcsSpi_ChipDeselect();
#endif
    return 0;
}

int Epcs_ReadBytes(uint32_t startAddress,uint8_t* pBuff,uint32_t readLength)
{
    uint8_t wbuf[7];
    uint32_t length=0;
    uint32_t address=startAddress;
    const struct epcsOperateFrame* pEpcsOp=epcsAllOperation+IDX_EPCS_OP_READ_BYTES;

    length=Epcs_PackageOperationCode(pEpcsOp,address,wbuf);
#if 0
    EpcsSpi_WriteRead(wbuf,length,NULL,0);
    EpcsSpi_WriteRead(NULL,0,pBuff,readLength);
#else
    EpcsSpi_ChipSelect();
    EpcsSpi_WriteRead(wbuf,length,pBuff,readLength);
    EpcsSpi_ChipDeselect();
#endif

    return 0;
}


int Epcs_ReadSiliconId(uint8_t* pId)
{
    uint8_t wbuf[7];
    uint32_t length=0;
    uint32_t address=0;
    const struct epcsOperateFrame* pEpcsOp=epcsAllOperation+IDX_EPCS_OP_READ_SILICON_ID;
    
    length=Epcs_PackageOperationCode(pEpcsOp,address,wbuf);
#if 0
    EpcsSpi_WriteRead(wbuf,length,NULL,0);
    EpcsSpi_WriteRead(NULL,0,pId,1);
#else
    EpcsSpi_ChipSelect();
    EpcsSpi_WriteRead(wbuf,length,pId,1);
    EpcsSpi_ChipDeselect();
#endif
    return 0;
}


int Epcs_FastRead(uint32_t startAddress,uint8_t* pBuff,uint32_t readLength)
{
    uint8_t wbuf[7];
    uint32_t length=0;
    uint32_t address=startAddress;
    const struct epcsOperateFrame* pEpcsOp=epcsAllOperation+IDX_EPCS_OP_FAST_READ;

    length=Epcs_PackageOperationCode(pEpcsOp,address,wbuf);
#if 0
    EpcsSpi_WriteRead(wbuf,length,NULL,0);
    EpcsSpi_WriteRead(NULL,0,pBuff,readLength);
#else
    EpcsSpi_ChipSelect();
    EpcsSpi_WriteRead(wbuf,length,pBuff,readLength);
    EpcsSpi_ChipDeselect();
#endif

    return 0;
}

int Epcs_WriteStatus(uint8_t newStatus)
{
    uint8_t wbuf[7];
    uint32_t length=0;
    uint32_t address=0;
    const struct epcsOperateFrame* pEpcsOp=epcsAllOperation+IDX_EPCS_OP_WRITE_STATUS;

    length=Epcs_PackageOperationCode(pEpcsOp,address,wbuf);
#if 0
    EpcsSpi_WriteRead(wbuf,length,NULL,0);
    EpcsSpi_WriteRead(&newStatus,1,NULL,0);
#else
    EpcsSpi_ChipSelect();
    EpcsSpi_WriteRead(wbuf,length,NULL,0);
    newStatus =(newStatus)?(2):(0);
    EpcsSpi_WriteRead(&newStatus,1,NULL,0);
    EpcsSpi_ChipDeselect();
#endif

    return 0;
}

int Epcs_WriteBytes(uint32_t startAddress,uint8_t* pBuff,uint32_t writeLength)
{
    uint8_t wbuf[7];
    uint32_t length=0;
    uint32_t address=startAddress;
    const struct epcsOperateFrame* pEpcsOp=epcsAllOperation+IDX_EPCS_OP_WRITE_BYTES;

    length=Epcs_PackageOperationCode(pEpcsOp,address,wbuf);
#if 0
    EpcsSpi_WriteRead(wbuf,length,NULL,0);
    EpcsSpi_WriteRead(pBuff,writeLength,NULL,0);
#else
    EpcsSpi_ChipSelect();
    EpcsSpi_WriteRead(wbuf,length,NULL,0);
    EpcsSpi_WriteRead(pBuff,writeLength,NULL,0);
    EpcsSpi_ChipDeselect();
#endif
    return 0;
}

int Epcs_EraseBulk(void)
{
    uint8_t wbuf[7];
    uint32_t length=0;
    uint32_t address=0;
    const struct epcsOperateFrame* pEpcsOp=epcsAllOperation+IDX_EPCS_OP_ERASE_BULK;

    length=Epcs_PackageOperationCode(pEpcsOp,address,wbuf);
    EpcsSpi_ChipSelect();
    EpcsSpi_WriteRead(wbuf,length,NULL,0);
    EpcsSpi_ChipDeselect();
    return 0;
}

int Epcs_EraseSector(uint32_t startAddress)
{
    uint8_t wbuf[7];
    uint32_t length=0;
    uint32_t address=startAddress;
    const struct epcsOperateFrame* pEpcsOp=epcsAllOperation+IDX_EPCS_OP_ERASE_SECTOR;

    length=Epcs_PackageOperationCode(pEpcsOp,address,wbuf);
    EpcsSpi_ChipSelect();
    EpcsSpi_WriteRead(wbuf,length,NULL,0);
    EpcsSpi_ChipDeselect();
    return 0;
}

int Epcs_NeedErase(uint32_t address)
{
    if(address % 0x10000 == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/* exchange one byte's msb with lsb. */
int Epcs_ByteMsbToLsb(uint8_t* pByte,uint32_t length)
{
    uint32_t index;
    uint8_t temp;

    for(index = 0; index < length; index++)
    {
        temp = pByte[index];

        pByte[index] = 
                        ((temp & 0x1) << 7) |
                        ((temp & 0x2) << 5) |
                        ((temp & 0x4) << 3) |
                        ((temp & 0x8) << 1) |
                        ((temp & 0x10) >> 1) |
                        ((temp & 0x20) >> 3) |
                        ((temp & 0x40) >> 5) |
                        ((temp & 0x80) >> 7);
    }

    return 0;
}


int Epcs_Write(uint32_t startAddress, uint8_t* pBuff, uint32_t length)
{
    int32_t retValue=0;
    uint32_t startTick=0,curTick=0;
    uint8_t status=0;
    uint32_t maxByteQtyThisTime=0;
    uint32_t writeByteQtyThisTime=0;
    uint32_t offset=0;
    uint8_t* pWriteBuffThisTime=NULL;
    uint32_t needWriteBytes=0;
    uint32_t startAddressThisTime=0;

    needWriteBytes=length;
    offset = 0;
    EPCS_DEBUG("[EPCS16]<WRITE> ADDR=%d,LEN=%d\n",startAddress,length);
    
    while(needWriteBytes)
    {
        pWriteBuffThisTime = pBuff + offset;
        startAddressThisTime = startAddress + offset;
        /* 防跨页 */
        maxByteQtyThisTime=256-(startAddressThisTime)%256; //本次允许的最大传输size
        writeByteQtyThisTime=(needWriteBytes<maxByteQtyThisTime)?(needWriteBytes):(maxByteQtyThisTime); //计算本次传输的size
        
        if(Epcs_NeedErase(startAddressThisTime))
        {
            Epcs_WriteEnable();
            Epcs_EraseSector(startAddressThisTime);
            //Epcs_WriteDisable();//THIS STATEMENT IS NOT NESECCORY
            startTick = osKernelSysTick();
            while(1)
            {
                Epcs_ReadStatus(&status);
                if((status & EPCS_STATUS_BUSY_MASK) == EPCS_STATUS_NOT_BUSY)
                {
                    EPCS_DEBUG("[EPCS16]擦除成功.status:%d\n",status);
                    break;
                }
                /* 超时3s判断 */
                curTick = osKernelSysTick();
                if((curTick-startTick)>osKernelSysTickMicroSec(1000*1000))
                {
                    EPCS_DEBUG("[EPCS16]擦除失败.AT:%d\n",__LINE__);
                    goto epcs_write_return;
                }
            }
        }

        Epcs_WriteEnable();
#if 0 //debug
        Epcs_ReadStatus(&status);
        EPCS_DEBUG("[EPCS16]READ STATUS.status:%d\n",status);
        if((status & EPCS_STATUS_WRITE_ENABLE_MASK) == EPCS_STATUS_WRITE_ENABLE)
        {
            EPCS_DEBUG("[EPCS16]WRITE ENABLE成功.status:%d\n",status);
            break;
        }

#endif
        Epcs_WriteBytes(startAddressThisTime,pWriteBuffThisTime,writeByteQtyThisTime);
        //Epcs_WriteDisable();//THIS STATEMENT IS NOT NESECCORY

        offset += writeByteQtyThisTime;
        needWriteBytes -= writeByteQtyThisTime;
        
        startTick = osKernelSysTick();
        while(1)
        {
            Epcs_ReadStatus(&status);

            if((status & EPCS_STATUS_BUSY_MASK) == EPCS_STATUS_NOT_BUSY)
            {
                EPCS_DEBUG("[EPCS16]WRITE BYTES成功.status:%d\n",status);
                break;
            }
            /* 超时2Ms判断 */
            curTick = osKernelSysTick();
            if((curTick-startTick)>osKernelSysTickMicroSec(1000*1000))
            {
                EPCS_DEBUG("[EPCS16]WRITE BYTES失败.AT:%d\n",__LINE__);
                goto epcs_write_return;
            }
        }
    }

epcs_write_return:
    Epcs_WriteDisable();//THIS STATEMENT IS NOT NESECCORY
    return retValue;
}



int Epcs_Read(uint32_t startAddress, uint8_t* pBuff, uint32_t length)
{
    uint32_t maxByteQtyThisTime=0;
    uint32_t readByteQtyThisTime=0;
    uint32_t offset=0;
    uint8_t* pReadBuffThisTime=NULL;
    uint32_t needReadBytes=0;
    uint32_t startAddressThisTime=0;

    needReadBytes = length;
    offset = 0;

    while(needReadBytes)
    {
        //onesize = size > 0xffff ? 0xffff : size;
        /* 一个sector一个sector的读 */
        //一次性读取的值不要太大，超过0x8000的值没有测试过。
        maxByteQtyThisTime = 0x8000;
        readByteQtyThisTime = (needReadBytes > maxByteQtyThisTime)?(maxByteQtyThisTime):(needReadBytes);
        startAddressThisTime = startAddress + offset;
        pReadBuffThisTime = pBuff + offset;
        
        //EPCS_DEBUG("[EPCS16]从ADDR=0x%X,读取LEN=0x%X字节\n",startAddressThisTime,readByteQtyThisTime);
        EPCS_DEBUG("[EPCS16]从ADDR=%d,读取LEN=%d字节\n",startAddressThisTime,readByteQtyThisTime);
    #if 1
        Epcs_ReadBytes(startAddressThisTime,pReadBuffThisTime, readByteQtyThisTime);
    #else
        Epcs_FastRead(startAddressThisTime,pReadBuffThisTime, readByteQtyThisTime);
    #endif
        offset += readByteQtyThisTime;
        needReadBytes -= readByteQtyThisTime;
    }

    return 0;
}



