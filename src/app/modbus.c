
#include "stdint.h"
#include "modbus.h"



void Modbus_BuildRequestPDU(uint8_t* pBuff,struct Modbus_PDU* pPdu,uint8_t funCode,void* pData)
/* 封装为modbus pdu(用于主机发送指令前打包) */
{
    int buffIndex=0;
    /* 将功能码和数据缓存指针存入相关结构体变量中 */
    pPdu->mpdu_funCode=funCode;
    pPdu->mpdu_pData=pBuff;
    (enum FunctionCode)(funCode);
    switch(funCode)
    {
        case FC_ReadHoldReg:
        {
            struct FC03Data* pFc03Data=((struct FC03Data*)(pData));
            uint16_t startReg=pFc03Data->fc03_startReg;
            uint16_t regQty  =pFc03Data->fc03_regQty;
            pPdu->mpdu_pData[buffIndex++]=(uint8_t)(startReg>>8);
            pPdu->mpdu_pData[buffIndex++]=(uint8_t)startReg;
            pPdu->mpdu_pData[buffIndex++]=(uint8_t)(regQty>>8);
            pPdu->mpdu_pData[buffIndex++]=(uint8_t)regQty;
            pPdu->mpdu_dataLength=buffIndex;
        }break;
        
    }
}



int Modbus_Handler(struct Modbus_PDU* pRecvPdu,struct Modbus_PDU* pSendPdu,ModbusHandler handler)
/* 返回值表示回应主机的pdu,参数表示接收到的主机发来的pdu */
{

    if(handler)
    {
        handler(pRecvPdu,pSendPdu);
    }
    return 0;
}

