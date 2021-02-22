#include "stdint.h"
#include "modbus.h"
#include "sample.h"
#include "System.h"
#include "InsulationDetection.h"
#include "Voltage2Temperature.h"
#include "CurrentDetection.h"
#include "VoltageDetection.h"
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




int ModbusSlaveHandler_ReadHoldReg(uint16_t RegisterAddr,uint8_t* pBuff)
/* 参数为读取寄存器地址和保存寄存器值的缓存指针,返回值为寄存器值的字节数.(此函数用于读取单个地址) */
{
    int retValue=0;
    float fvalue=0;
    uint16_t   ivalue=0;
    int byteIndex=0;
    switch(RegisterAddr)
    {
        case 0:/* 温度通道0(10倍开式温度),总共8个通道 */
        case 1:/* 温度通道1(10倍开式温度),总共8个通道 */
        case 2:/* 温度通道2(10倍开式温度),总共8个通道 */
        case 3:/* 温度通道3(10倍开式温度),总共8个通道 */
        case 4:/* 温度通道4(10倍开式温度),总共8个通道 */
        case 5:/* 温度通道5(10倍开式温度),总共8个通道 */
        case 6:/* 温度通道6(10倍开式温度),总共8个通道 */
        case 7:/* 温度通道7(10倍开式温度),总共8个通道 */
        {
            fvalue=Temperature_GetKelvinTemperature(RegisterAddr);
            fvalue *= 10;
        }break;

        case 8:/* 三相电压之Uab(10倍伏) */
        {
            VoltageDetection_GetACRMSVoltage(&fvalue,(float*)NULL,(float*)NULL);
            fvalue *= 10;
        }break;
        case 9:/* 三相电压之Ubc(10倍伏) */
        {
            VoltageDetection_GetACRMSVoltage((float*)NULL,&fvalue,(float*)NULL);
            fvalue *= 10;
        }break;
        case 10:/* 三相电压之Uca(10倍伏) */
        {
            VoltageDetection_GetACRMSVoltage((float*)NULL,(float*)NULL,&fvalue);
            fvalue *= 10;
        }break;
        case 11: /* 正极绝缘电阻值(10倍k欧) */
        {
            InsulationDetection_GetMOhm(&fvalue,(float*)NULL);
            fvalue=(fvalue*1000);
            fvalue *= 10;
        }break;
        case 12: /* 负极绝缘电阻值(10倍k欧) */
        {
            InsulationDetection_GetMOhm((float*)NULL,&fvalue);
            fvalue=(fvalue*1000);
            fvalue *= 10;
        }break;
        case 13:/* A相电流(10倍安) */
        {
            CurrentDetection_GetACCurrent(&fvalue,(float*)NULL,(float*)NULL,(float*)NULL);
            fvalue *= 10;
        }break;
        case 14:/* B相电流(10倍安) */
        {
            CurrentDetection_GetACCurrent((float*)NULL,&fvalue,(float*)NULL,(float*)NULL);
            fvalue *= 10;
        }break;
        case 15:/* C相电流(10倍安) */
        {
            CurrentDetection_GetACCurrent((float*)NULL,(float*)NULL,&fvalue,(float*)NULL);
            fvalue *= 10;
        }break;
        case 16:/* 绝缘电流(10倍毫安) */
        {
            CurrentDetection_GetACCurrent((float*)NULL,(float*)NULL,(float*)NULL,&fvalue);
            fvalue=(fvalue*1000);/* mA */
            fvalue *= 10;
        }break;
        case 17:/* 正极对地电压(10倍伏特) */
        {
            InsulationDetection_GetPNVoltage(&fvalue,(float*)NULL);
            fvalue *= 10;
        }break;
        case 18:/* 负极对地电压(10倍伏特) */
        {
            InsulationDetection_GetPNVoltage((float*)NULL,&fvalue);
            fvalue *= 10;
        }break;

        case 90:/* 采样板代码主版本 */
        {
            fvalue = 1;
        }break;
        case 91:/* 采样板代码子版本 */
        {
            fvalue = 0;
        }break;
        case 92:/* 采样板代码贝塔版本号,正式版都为0,贝塔版为1~9 */
        {
            fvalue = 0;
        }break;
        
#if 1 /* 这一段都是测试地址 */
        case 100:
        {
            fvalue=Sample_GetVoltage(VOLTAGE_TEMPERATURE0);
            fvalue *= 100;
        }break;
        case 101:
        {
            fvalue=Sample_GetVoltage(VOLTAGE_TEMPERATURE1);
            fvalue *= 100;
        }break;
        case 102:
        {
            fvalue=Sample_GetVoltage(VOLTAGE_TEMPERATURE2);
            fvalue *= 100;
        }break;
        case 103:
        {
            fvalue=Sample_GetVoltage(VOLTAGE_TEMPERATURE3);
            fvalue *= 100;
        }break;
        case 104:
        {
            fvalue=Sample_GetVoltage(VOLTAGE_TEMPERATURE4);
            fvalue *= 100;
        }break;
        case 105:
        {
            fvalue=Sample_GetVoltage(VOLTAGE_TEMPERATURE5);
            fvalue *= 100;
        }break;
        case 106:
        {
            fvalue=Sample_GetVoltage(VOLTAGE_TEMPERATURE6);
            fvalue *= 100;
        }break;
        case 107:
        {
            fvalue=Sample_GetVoltage(VOLTAGE_TEMPERATURE7);
            fvalue *= 100;
        }break;
        case 108:
        {
            fvalue=Sample_GetVoltage(VOLTAGE_VAB);
            fvalue *= 100;
        }break;
        case 109:
        {
            fvalue=Sample_GetVoltage(VOLTAGE_VBC);
            fvalue *= 100;
        }break;
        case 110:
        {
            fvalue=Sample_GetVoltage(VOLTAGE_VCA);
            fvalue *= 100;
        }break;
        case 111:
        {
            fvalue=Sample_GetVoltage(VOLTAGE_IOSA);
            fvalue *= 100;
        }break;
        case 112:
        {
            fvalue=Sample_GetVoltage(VOLTAGE_IOSB);
            fvalue *= 100;
        }break;
        case 113:
        {
            fvalue=Sample_GetVoltage(VOLTAGE_IA);
            fvalue *= 100;
        }break;
        case 114:
        {
            fvalue=Sample_GetVoltage(VOLTAGE_IB);
            fvalue *= 100;
        }break;
        case 115:
        {
            fvalue=Sample_GetVoltage(VOLTAGE_IC);
            fvalue *= 100;
        }break;
        case 116:
        {
            fvalue=Sample_GetVoltage(VOLTAGE_GFCI);
            fvalue *= 100;
        }break;
#endif

#if 1
//测试区
        case 200:
        case 201:
        case 202:
        case 203:
        case 204:
        case 205:
        case 206:
        case 207:
        {
            fvalue=Temperature_GetKelvinTemperature(RegisterAddr-200)-273.15;
        }break;
        case 208:
        {
            VoltageDetection_GetDCVoltage(&fvalue,(float*)NULL,(float*)NULL);
        }break;
        case 209:
        {
            VoltageDetection_GetDCVoltage((float*)NULL,&fvalue,(float*)NULL);
        }break;
        case 210:
        {
            VoltageDetection_GetDCVoltage((float*)NULL,(float*)NULL,&fvalue);
        }break;
        
        case 213:
        {
            CurrentDetection_GetDCCurrent(&fvalue,(float*)NULL,(float*)NULL,(float*)NULL);
        }break;
        case 214:
        {
            CurrentDetection_GetDCCurrent((float*)NULL,&fvalue,(float*)NULL,(float*)NULL);
        }break;
        case 215:
        {
            CurrentDetection_GetDCCurrent((float*)NULL,(float*)NULL,&fvalue,(float*)NULL);
        }break;
#endif

        default:
        /* 用于处理未定义的地址 */
        {
            fvalue = 0.0;
        }break;
    }
    ivalue=(uint16_t)fvalue;
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
        case 1999:
        /* 重启控制 */
        {
            System_SocReset();
        }break;
        default:
        {

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
        {
            pSendPDU->mpdu_funCode=FC_ReadHoldReg;
            //起始地址和寄存器数量都是两个字节
            startRegAddr=(pRecvPDU->mpdu_pData[0]<<8)|(pRecvPDU->mpdu_pData[1]);
            regQty      =(pRecvPDU->mpdu_pData[2]<<8)|(pRecvPDU->mpdu_pData[3]);
            byteQty=ModbusSlaveHandler_ReadRegister(startRegAddr,regQty,pSendPDU->mpdu_pData+1);
            pSendPDU->mpdu_pData[0]=(uint8_t)byteQty;
            pSendPDU->mpdu_dataLength=byteQty+1;
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
