#include "stdint.h"
#include "modbus.h"
#include "modbusSlaveHandler.h"
#include "modbusMasterHandler.h"
#include "comm_offline.h"


//#include "SampleBoard.h"
int XXModbus_Slave1Handler(struct Modbus_PDU* pRecvPDU,struct Modbus_PDU* pSendPDU)
/* ע��:����buff�ͷ���buff��ͬһ��(struct Modbus_PDU.mpdu_pData),������buffû�ж�ȡ��ʱ�����������buff */
{
    /* ����(��û�оͲ���Ҫ) */

    /* ����ָ�� */
    /* Ϊ�˴���ģ��Ķ�����,����ʹ��XXModbus_Slave2Handler�ķ�ʽ������Ϣ */
    switch(pRecvPDU->mpdu_funCode)
    /* ���������Ŀ����ʦ�������.(������Ӵ����յ���sceen��������ָ��) */
    {
        default:
        {
            //printf("�յ�����ָ��.\n");
            pSendPDU->mpdu_dataLength=0;
        }break;
    }

    return 0;
}

int XXModbus_Slave2Handler(struct Modbus_PDU* pRecvPDU,struct Modbus_PDU* pSendPDU)
/* ע��:����buff�ͷ���buff��ͬһ��(struct Modbus_PDU.mpdu_pData),������buffû�ж�ȡ��ʱ�����������buff */
{
    /* ����(��û�оͲ���Ҫ) */

    /* ����ָ�� */
    /* ���������Ŀ����ʦ�������.(������Ӵ����յ���sceen��������ָ��) */
    hmiOffline.hbCur++;
    ModbusSlaveHandler_Handler(pRecvPDU,pSendPDU);

    return 0;
}



int XXModbus_Master1Handler(struct Modbus_PDU* pRecvPDU,struct Modbus_PDU* pSendPDU)
{
    (void)pSendPDU;
    /* ����(��û�оͲ���Ҫ) */

    /* ����ָ�� */
    /* Ϊ�˴���ģ��Ķ�����,����ʹ��XXModbus_Master2Handler�ķ�ʽ������Ϣ */
    switch(pRecvPDU->mpdu_funCode)
    {
        default:
        {
            //printf("�յ��ӻ�Ӧ��.OVER\n");
        }break;
    }
    return 0;
}

int XXModbus_Master2Handler(struct Modbus_PDU* pRecvPDU,struct Modbus_PDU* pSendPDU)
{
    (void)pSendPDU;
    /* ����(��û�оͲ���Ҫ) */

    /* ����ָ�� */
    /* ������������������handler,���籣������Ĳ�����ظ��ĵ�ѹ,�¶���Ϣ��. */
    ModbusMasterHandler_Handler(pRecvPDU,pSendPDU);
    
    return 0;
}

