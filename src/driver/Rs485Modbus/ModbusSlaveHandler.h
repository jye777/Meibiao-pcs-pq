#ifndef __MODBUSSLAVEHANDLER_H__
#define __MODBUSSLAVEHANDLER_H__
#include "modbus.h"

int ModbusSlaveHandler_Handler(struct Modbus_PDU* pRecvPDU,struct Modbus_PDU* pSendPDU);
#endif