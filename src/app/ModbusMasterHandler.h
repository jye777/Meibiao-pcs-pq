#ifndef __MODBUSMASTERHANDLER_H__
#define __MODBUSMASTERHANDLER_H__
#include "modbus.h"

int ModbusMasterHandler_Handler(struct Modbus_PDU* pRecvPDU,struct Modbus_PDU* pSendPDU);
#endif
