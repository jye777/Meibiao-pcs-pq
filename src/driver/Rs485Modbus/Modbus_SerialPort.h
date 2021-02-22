#ifndef __MODBUS_SERIALPORT_H__
#define __MODBUS_SERIALPORT_H__

#include "modbus.h"
struct ModbusSP_PDU
{
    uint8_t           msppdu_addr;  //modbus serial port pdu address part.
    struct Modbus_PDU msppdu_pdu;   //modbus pdu
    //uint16_t          msppdu_crc16; //crc16Ð£ÑéÂë
};
void ModbusSP_BuildPDU(struct Modbus_PDU* pMBPdu,struct ModbusSP_PDU* pMBSPPdu,uint8_t domain);
struct ModbusSP_PDU ModbusSP_FastBuildPDU(struct Modbus_PDU* pMBPdu,uint8_t domain);

struct ModbusSP_PDU ModbusSP_FastBuild03RequestPDU(uint8_t* pBuff,uint8_t domain,uint16_t startRegisterAddr,uint16_t registerQty);
void ModbusSP_UnPackage(struct ModbusSP_PDU* pPDU,uint8_t* pBuff,int* pLength);
int ModbusSP_AppendCRC16(uint8_t* pBuff,int* pLength);

void ModbusSP_SlaveHandler(uint8_t* pBuff,int* pLength,ModbusHandler handler);
void ModbusSP_MasterHandler(uint8_t* pBuff,int* pLength,ModbusHandler handler);

#endif
