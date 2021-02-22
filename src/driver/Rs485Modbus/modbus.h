#ifndef __MODBUS_H__
#define __MODBUS_H__
#include "stdint.h"

#define FUNCTION_CODE_ERROR 0x80
enum FunctionCode
{
    FC_ReadHoldReg=3,
    FC_ReadInputReg=4,
    FC_WriteSingleReg=6,
    FC_WriteMultiReg=16,/*0x10*/
    FC_ReadWriteMultiReg=23,/*0x17*/
    FC_TransRealtimeWave=100,/*0x64*/
    FC_TransErrorRecording=106,
    FC_TransLog=102,
    FC_TransAllPara=103,
    FC_FirmwareUpgrade=104,
    FC_ReadSamplePoint=105,
    FC_ReadSamplePointAverage=106,
    
    FC_WritePara=107,
    FC_Heartbeat=0xff
};

enum ErrorCode
{
    EC_IllegalFunction=1,
    EC_IllegalAddress=2,
    EC_IllegalValue=3,
    EC_ServerError=4,
    EC_ServerBusy=5,


    EC_PduFormatError=127,
};

struct FC03Data
{
    uint16_t fc03_startReg;
    uint16_t fc03_regQty;
};



struct Modbus_PDU
{
    uint8_t   mpdu_funCode; //modbus pdu function code.
    uint8_t*  mpdu_pData; //modbus pdu data part.
    int       mpdu_dataLength;//modbus pdu data bytes qty.
};

typedef int (* ModbusHandler)(struct Modbus_PDU*,struct Modbus_PDU*);
void Modbus_BuildRequestPDU(uint8_t* pBuff,struct Modbus_PDU* pPdu,uint8_t funCode,void* pData);
int Modbus_Handler(struct Modbus_PDU* pRecvPdu,struct Modbus_PDU* pSendPdu,ModbusHandler handler);
#endif
