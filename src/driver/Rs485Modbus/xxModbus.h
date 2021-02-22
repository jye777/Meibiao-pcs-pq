#ifndef __XXMODBUS_H__
#define __XXMODBUS_H__
#include "modbus.h"
int XXModbus_Slave1Handler(struct Modbus_PDU*,struct Modbus_PDU*);
int XXModbus_Master1Handler(struct Modbus_PDU* pRecvPDU,struct Modbus_PDU* pSendPDU);
int XXModbus_Slave2Handler(struct Modbus_PDU*,struct Modbus_PDU*);
int XXModbus_Master2Handler(struct Modbus_PDU* pRecvPDU,struct Modbus_PDU* pSendPDU);

#endif
