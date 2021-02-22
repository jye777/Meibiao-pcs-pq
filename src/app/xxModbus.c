#include "stdint.h"
#include "modbus.h"
#include "modbusSlaveHandler.h"
#include "modbusMasterHandler.h"
#include "comm_offline.h"


//#include "SampleBoard.h"
int XXModbus_Slave1Handler(struct Modbus_PDU* pRecvPDU,struct Modbus_PDU* pSendPDU)
/* 注意:接收buff和发送buff是同一个(struct Modbus_PDU.mpdu_pData),当接收buff没有读取完时请勿操作发送buff */
{
    /* 解密(若没有就不需要) */

    /* 处理指令 */
    /* 为了代码模块的独立性,建议使用XXModbus_Slave2Handler的方式处理信息 */
    switch(pRecvPDU->mpdu_funCode)
    /* 命令处理由项目工程师自行添加.(比如添加处理收到的sceen发过来的指令) */
    {
        default:
        {
            //printf("收到主机指令.\n");
            pSendPDU->mpdu_dataLength=0;
        }break;
    }

    return 0;
}

int XXModbus_Slave2Handler(struct Modbus_PDU* pRecvPDU,struct Modbus_PDU* pSendPDU)
/* 注意:接收buff和发送buff是同一个(struct Modbus_PDU.mpdu_pData),当接收buff没有读取完时请勿操作发送buff */
{
    /* 解密(若没有就不需要) */

    /* 处理指令 */
    /* 命令处理由项目工程师自行添加.(比如添加处理收到的sceen发过来的指令) */
    hmiOffline.hbCur++;
    ModbusSlaveHandler_Handler(pRecvPDU,pSendPDU);

    return 0;
}



int XXModbus_Master1Handler(struct Modbus_PDU* pRecvPDU,struct Modbus_PDU* pSendPDU)
{
    (void)pSendPDU;
    /* 解密(若没有就不需要) */

    /* 处理指令 */
    /* 为了代码模块的独立性,建议使用XXModbus_Master2Handler的方式处理信息 */
    switch(pRecvPDU->mpdu_funCode)
    {
        default:
        {
            //printf("收到从机应答.OVER\n");
        }break;
    }
    return 0;
}

int XXModbus_Master2Handler(struct Modbus_PDU* pRecvPDU,struct Modbus_PDU* pSendPDU)
{
    (void)pSendPDU;
    /* 解密(若没有就不需要) */

    /* 处理指令 */
    /* 请自行添加命令处理代码或handler,比如保存读到的采样板回复的电压,温度信息等. */
    ModbusMasterHandler_Handler(pRecvPDU,pSendPDU);
    
    return 0;
}

