#include "stdint.h"
#include "stdio.h"
#include "Modbus_SerialPort.h"
#include "xxModbus.h"
#include "CRC.h"

/* ע��:��ν������ָPDU�ṹ�� */
/* ���pBuff,(����pBuff/����pBuff)�ǲ�ͬ��buff;����(����pBuff/����pBuff)��ͬһ��buff */

void ModbusSP_BuildPDU(struct Modbus_PDU* pMBPdu,struct ModbusSP_PDU* pMBSPPdu,uint8_t domain)
/* ��modbus PDU����Ϊmodbus serial port PDU */
{
    pMBSPPdu->msppdu_addr=domain;
    pMBSPPdu->msppdu_pdu=(*pMBPdu);
    /* CRC�����ڷ���ǰ�ټ��� */
}

struct ModbusSP_PDU ModbusSP_FastBuildPDU(struct Modbus_PDU* pMBPdu,uint8_t domain)
{
    struct ModbusSP_PDU mbspPdu;
    mbspPdu.msppdu_addr=domain;
    mbspPdu.msppdu_pdu=(*pMBPdu);
}

struct ModbusSP_PDU ModbusSP_FastBuild03RequestPDU(uint8_t* pBuff,uint8_t domain,uint16_t startRegisterAddr,uint16_t registerQty)
{
    struct ModbusSP_PDU mbspPdu;
    struct Modbus_PDU mbPdu;
    struct FC03Data data;
    data.fc03_startReg=startRegisterAddr;
    data.fc03_regQty=registerQty;
    Modbus_BuildRequestPDU(pBuff,&mbPdu,FC_ReadHoldReg,&data);
    ModbusSP_BuildPDU(&mbPdu,&mbspPdu,domain);
    return mbspPdu;
}


void ModbusSP_Package(struct ModbusSP_PDU* pPDU,uint8_t* pBuff,int* pLength)
/* ��pbuff����pdu��(���յ������ݷ���modbus����PDU��) */
{
    int length = *pLength;
    if(length<4)
    {
        return;
    }
    pPDU->msppdu_addr=pBuff[0];
    pPDU->msppdu_pdu.mpdu_funCode=pBuff[1];
    pPDU->msppdu_pdu.mpdu_pData=pBuff+2;
    pPDU->msppdu_pdu.mpdu_dataLength=length-4;/* domain(1byte)+funcode(1)+crc16(2) */
}

void ModbusSP_UnPackage(struct ModbusSP_PDU* pPDU,uint8_t* pBuff,int* pLength)
/* ��pdu��������pbuff��(��modbus����PDU�����buff��) */
{    
    pBuff[0]=pPDU->msppdu_addr;
    pBuff[1]=pPDU->msppdu_pdu.mpdu_funCode;
    if((pBuff+2) != pPDU->msppdu_pdu.mpdu_pData)
    {
        memcpy(pBuff+2,pPDU->msppdu_pdu.mpdu_pData,pPDU->msppdu_pdu.mpdu_dataLength);
    }
    //crc�����Ժ�����д
    *pLength = pPDU->msppdu_pdu.mpdu_dataLength+2;
}


int ModbusSP_CheckPDUCRC(uint8_t* pBuff,int* pLength)
/* �����յ���modbus���ݰ���CRCֵ */
{
#define SLAVE_STATION_ADDR  (2)
#define BROADCAST_ADDR      (0)
    int length=*pLength;
    uint16_t calcCRC=0,crcData=0;
    if(pBuff[0] != BROADCAST_ADDR && pBuff[0] != SLAVE_STATION_ADDR)
    {
        return (0);
    }
    if(length<4)
    {
        return (-2);
    }
    /* ����CRC16 */
    calcCRC = crc16(pBuff,(length-2));
    crcData = (pBuff[length-2]<<8)|(pBuff[length-1]);
    //printf("[check step1]calcCRC=0x%04X,crcData=0x%04X\n",calcCRC,crcData);


    if(calcCRC == crcData)
    {
        return 1;
    }
    printf("У��ʧ��,����.\n");
    return (-1);
}

int ModbusSP_AppendCRC16(uint8_t* pBuff,int* pLength)
{
#define SLAVE_STATION_ADDR  (2)
#define BROADCAST_ADDR      (0)

    uint16_t calcCRC=0;
    int length=*pLength;
    if(pBuff[0] != BROADCAST_ADDR && pBuff[0] != SLAVE_STATION_ADDR)
    {
        return (0);
    }
    /* �����ַCRC16 */
    calcCRC = crc16(pBuff,length);
    //printf("[calc step1]calcCRC=0x%04X\n",calcCRC);

    pBuff[length] = (uint8_t)(calcCRC>>8);
    pBuff[length+1] = (uint8_t)(calcCRC);
    
    *pLength = length+2;
    
    return (0);
}



void ModbusSP_SlaveHandler(uint8_t* pBuff,int* pLength,ModbusHandler handler)
/* �������һ����modbus�Ĵ�վ����,�����������վ���յ���ָ��. */
/* ע��:���պͷ���ʹ�õ���ͬһ��buff,�ڶ����������֮ǰ��д�������� */
{
    static struct ModbusSP_PDU recvPdu;
    static struct ModbusSP_PDU sendPdu;
    
    struct ModbusSP_PDU* pRecvPdu=&recvPdu;
    struct ModbusSP_PDU* pSendPdu=&sendPdu;
    if(ModbusSP_CheckPDUCRC(pBuff,pLength)>0)
    /* crcУ����ȷ�Ž��к�������,����ֱ�Ӷ��� */
    {
        ModbusSP_Package(pRecvPdu,pBuff,pLength);
        *pSendPdu = *pRecvPdu;
        Modbus_Handler(&(pRecvPdu->msppdu_pdu),&(pSendPdu->msppdu_pdu),handler);
        /* ��Ӧ��������䵽�������Ա�������� */
        ModbusSP_UnPackage(pSendPdu,pBuff,pLength);
        /* ����Ӧ�����ݵ�CRC16ֵ */
        ModbusSP_AppendCRC16(pBuff,pLength);
    }
    else
    {
        *pLength = 0;
    }
}

void ModbusSP_MasterHandler(uint8_t* pBuff,int* pLength,ModbusHandler handler)
/* �������һ����modbus����վ����,����������յ��Ĵ�վ����Ӧ. */
/* ע��:���պͷ���ʹ�õ���ͬһ��buff,�ڶ����������֮ǰ��д�������� */
{
    struct ModbusSP_PDU recvPdu;
    struct ModbusSP_PDU sendPdu;
    
    struct ModbusSP_PDU* pRecvPdu=&recvPdu;
    struct ModbusSP_PDU* pSendPdu=&sendPdu;
    
    if((int)pBuff*(int)pLength*(int)handler == 0)
    {
        return;
    }
    
    if(ModbusSP_CheckPDUCRC(pBuff,pLength)>0)
    /* crcУ����ȷ�Ž��к�������,����ֱ�Ӷ��� */
    {
        ModbusSP_Package(pRecvPdu,pBuff,pLength);
        *pSendPdu = *pRecvPdu;
        Modbus_Handler(&(pRecvPdu->msppdu_pdu),&(pSendPdu->msppdu_pdu),handler);
    }
}


