#include "stdint.h"
#include "modbus.h"

#define MODULE_NAME MODBUSSLAVEHANDLER /* 这个宏定义必须放在#include"debug.h"之前.且名字不能随便更改 */
#include "debug.h"


/*
addr    bytes    type    meaning
0:       4       float    Uab
1        4       float    Ubc
2        4       float    Uca
3        4       float    temp1
4        4       float    temp2
5        4       float    temp3
6        4       float    temp4
7        4       float    temp5
8        4       float    temp6
9        4       float    temp7
10       4       float    temp8
11       4       float    Rp(MOhm)
12       4       float    Rm(MOhm)
13       4       float    Ia
14       4       float    Ib
15       4       float    Ic
16       4       float    Igfci
*/

#if 1
extern unsigned short modbus_read_reg_u0(unsigned short addr, unsigned char* err);
extern unsigned char modbus_write_reg_u0(unsigned short addr, short value);
#endif




int ModbusSlaveHandler_ReadHoldReg(uint16_t RegisterAddr,uint8_t* pBuff)
/* 参数为读取寄存器地址和保存寄存器值的缓存指针,返回值为寄存器值的字节数.(此函数用于读取单个地址) */
{
    int retValue=0;
    float fvalue=0;
    uint16_t   ivalue=0;
    int byteIndex=0;
    switch(RegisterAddr)
    {
        #if 0
        default:
        {
            fvalue=RegisterAddr;
            fvalue *= 10;
        }break;
        #else
        default:
        {
            uint8_t errorCode=0;
            ivalue=modbus_read_reg_u0(RegisterAddr,&errorCode);
        }break;
        #endif
    }
    //ivalue=(uint16_t)fvalue;
    pBuff[byteIndex++]=(uint8_t)(ivalue>>(1*8));
    pBuff[byteIndex++]=(uint8_t)(ivalue>>(0*8));
    retValue=byteIndex;

    return retValue;
}

int ModbusSlaveHandler_ReadRegister(uint16_t startRegisterAddr,uint16_t registerQty,uint8_t* pBuff)
/* 参数为寄存器起始地址,寄存器数量,寄存器值保存缓存指针;返回值为读到的字节数.(此函数用于单个或多个地址读取) */
{
    uint16_t regIndex=0;
    int byteQty=0;
    int totalByteQty=0;
    uint8_t* pReadBuff=pBuff;
    while(regIndex<registerQty)
    {
        pReadBuff += byteQty;
        byteQty=ModbusSlaveHandler_ReadHoldReg(startRegisterAddr+regIndex,pReadBuff);
        totalByteQty += byteQty;
        regIndex++;
    }
    return totalByteQty;
}




int ModbusSlaveHandler_ReadSamplePoint(uint8_t channel,uint8_t* pBuff,uint16_t buffAllowByte)
{
    //return IDDEBUG_GetSamplePoint(channel,pBuff,buffAllowByte);
}
int ModbusSlaveHandler_ReadSamplePointAverage(uint8_t channel,uint8_t* pBuff)
{
    //return IDDEBUG_GetSamplePointAverage(channel,pBuff);
}

int ModbusSlaveHandler_WriteSingleReg(uint16_t RegisterAddr,uint8_t* pBuff)
{
    int retValue=0;
    float fvalue=0;
    uint16_t   ivalue=0;
    int byteIndex=0;
    switch(RegisterAddr)
    {
        default:
        {
            ivalue=(pBuff[1]<<8)|(pBuff[2]);
            modbus_write_reg_u0(RegisterAddr,ivalue);
        }break;
    }
    retValue=2;
    return retValue;
}

int ModbusSlaveHandler_WriteMultiReg(uint16_t startRegisterAddr,uint16_t registerQty,uint8_t* pBuff)
/* 参数为寄存器起始地址,寄存器数量,寄存器值保存缓存指针;返回值为读到的字节数.(此函数用于单个或多个地址读取) */
{
    uint16_t regIndex=0;
    int byteQty=0;
    uint8_t* pWriteBuff=pBuff;
    while(regIndex<registerQty)
    {
        pWriteBuff += byteQty;
        byteQty=ModbusSlaveHandler_WriteSingleReg(startRegisterAddr+regIndex,pWriteBuff);
        regIndex++;
    }
    return registerQty;
}


int ModbusSlaveHandler_Handler(struct Modbus_PDU* pRecvPDU,struct Modbus_PDU* pSendPDU)
{
    float fvalue=0;
    int   ivalue=0;
    uint16_t startRegAddr=0;
    uint16_t regQty=0;
    int byteQty=0;
    switch(pRecvPDU->mpdu_funCode)
    {
        case FC_ReadHoldReg:
        case FC_ReadInputReg:
        {
            pSendPDU->mpdu_funCode=pRecvPDU->mpdu_funCode;
            //起始地址和寄存器数量都是两个字节
            startRegAddr=(pRecvPDU->mpdu_pData[0]<<8)|(pRecvPDU->mpdu_pData[1]);
            regQty      =(pRecvPDU->mpdu_pData[2]<<8)|(pRecvPDU->mpdu_pData[3]);
            byteQty=ModbusSlaveHandler_ReadRegister(startRegAddr,regQty,pSendPDU->mpdu_pData+1);
            pSendPDU->mpdu_pData[0]=(uint8_t)byteQty;
            pSendPDU->mpdu_dataLength=byteQty+1;
        }break;
        case FC_WriteSingleReg:
        {
            pSendPDU->mpdu_funCode=FC_WriteSingleReg;
            //起始地址和寄存器数量都是两个字节
            startRegAddr=(pRecvPDU->mpdu_pData[0]<<8)|(pRecvPDU->mpdu_pData[1]);
            uint8_t regValue[2];
            regValue[0]=pRecvPDU->mpdu_pData[2];
            regValue[1]=pRecvPDU->mpdu_pData[3];
            ModbusSlaveHandler_WriteSingleReg(startRegAddr,regValue);
            pSendPDU->mpdu_pData[0]=pRecvPDU->mpdu_pData[0];
            pSendPDU->mpdu_pData[1]=pRecvPDU->mpdu_pData[1];
            pSendPDU->mpdu_pData[2]=pRecvPDU->mpdu_pData[2];
            pSendPDU->mpdu_pData[3]=pRecvPDU->mpdu_pData[3];
            pSendPDU->mpdu_dataLength=4;
        }break;
        case FC_WriteMultiReg:
        {
            pSendPDU->mpdu_funCode=FC_WriteMultiReg;
            //起始地址和寄存器数量都是两个字节
            startRegAddr=(pRecvPDU->mpdu_pData[0]<<8)|(pRecvPDU->mpdu_pData[1]);
            regQty      =(pRecvPDU->mpdu_pData[2]<<8)|(pRecvPDU->mpdu_pData[3]);
            byteQty=ModbusSlaveHandler_WriteMultiReg(startRegAddr,regQty,pSendPDU->mpdu_pData+4);
            pSendPDU->mpdu_pData[0]=(uint8_t)(startRegAddr>>8);
            pSendPDU->mpdu_pData[1]=(uint8_t)(startRegAddr);
            pSendPDU->mpdu_pData[2]=(uint8_t)(regQty>>8);
            pSendPDU->mpdu_pData[3]=(uint8_t)(regQty);
            pSendPDU->mpdu_dataLength=4;
        }break;
        #if 0
        case FC_ReadSamplePoint:
        {
            uint8_t channelId=pRecvPDU->mpdu_pData[0];
            uint16_t buffByteQty=(pRecvPDU->mpdu_pData[1]<<8)|(pRecvPDU->mpdu_pData[2]);
            byteQty=ModbusSlaveHandler_ReadSamplePoint(channelId,pSendPDU->mpdu_pData,buffByteQty);
            pSendPDU->mpdu_dataLength=byteQty;
        }break;
        case FC_ReadSamplePointAverage:
        {
            uint8_t channelId=pRecvPDU->mpdu_pData[0];
            byteQty=ModbusSlaveHandler_ReadSamplePointAverage(channelId,pSendPDU->mpdu_pData);
            pSendPDU->mpdu_dataLength=byteQty;
        }break;
        #endif
    }
    return 0;
}
