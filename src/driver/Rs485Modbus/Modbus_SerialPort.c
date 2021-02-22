#include "stdint.h"
#include "stdio.h"
#include "Modbus_SerialPort.h"
#include "xxModbus.h"
#include "CRC.h"

/* 注意:所谓包就是指PDU结构体 */
/* 组包pBuff,(发送pBuff/接收pBuff)是不同的buff;但是(发送pBuff/接收pBuff)是同一个buff */

void ModbusSP_BuildPDU(struct Modbus_PDU* pMBPdu,struct ModbusSP_PDU* pMBSPPdu,uint8_t domain)
/* 将modbus PDU构建为modbus serial port PDU */
{
    pMBSPPdu->msppdu_addr=domain;
    pMBSPPdu->msppdu_pdu=(*pMBPdu);
    /* CRC部分在发送前再计算 */
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
/* 将pbuff放入pdu中(将收到的数据放入modbus串口PDU中) */
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
/* 将pdu拆包后放入pbuff中(将modbus串口PDU解包到buff中) */
{    
    pBuff[0]=pPDU->msppdu_addr;
    pBuff[1]=pPDU->msppdu_pdu.mpdu_funCode;
    if((pBuff+2) != pPDU->msppdu_pdu.mpdu_pData)
    {
        memcpy(pBuff+2,pPDU->msppdu_pdu.mpdu_pData,pPDU->msppdu_pdu.mpdu_dataLength);
    }
    //crc数据稍后再填写
    *pLength = pPDU->msppdu_pdu.mpdu_dataLength+2;
}


int ModbusSP_CheckPDUCRC(uint8_t* pBuff,int* pLength)
/* 检查接收到的modbus数据包的CRC值 */
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
    /* 计算CRC16 */
    calcCRC = crc16(pBuff,(length-2));
    crcData = (pBuff[length-2]<<8)|(pBuff[length-1]);
    //printf("[check step1]calcCRC=0x%04X,crcData=0x%04X\n",calcCRC,crcData);


    if(calcCRC == crcData)
    {
        return 1;
    }
    printf("校验失败,丢弃.\n");
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
    /* 计算地址CRC16 */
    calcCRC = crc16(pBuff,length);
    //printf("[calc step1]calcCRC=0x%04X\n",calcCRC);

    pBuff[length] = (uint8_t)(calcCRC>>8);
    pBuff[length+1] = (uint8_t)(calcCRC);
    
    *pLength = length+2;
    
    return (0);
}



void ModbusSP_SlaveHandler(uint8_t* pBuff,int* pLength,ModbusHandler handler)
/* 这个函数一般是modbus的从站调用,用来处理从主站接收到的指令. */
/* 注意:接收和发送使用的是同一个buff,在读完接收数据之前别写发送数据 */
{
    static struct ModbusSP_PDU recvPdu;
    static struct ModbusSP_PDU sendPdu;
    
    struct ModbusSP_PDU* pRecvPdu=&recvPdu;
    struct ModbusSP_PDU* pSendPdu=&sendPdu;
    if(ModbusSP_CheckPDUCRC(pBuff,pLength)>0)
    /* crc校验正确才进行后续操作,否则直接丢弃 */
    {
        ModbusSP_Package(pRecvPdu,pBuff,pLength);
        *pSendPdu = *pRecvPdu;
        Modbus_Handler(&(pRecvPdu->msppdu_pdu),&(pSendPdu->msppdu_pdu),handler);
        /* 将应答数据填充到数组中以便后续发送 */
        ModbusSP_UnPackage(pSendPdu,pBuff,pLength);
        /* 计算应答数据的CRC16值 */
        ModbusSP_AppendCRC16(pBuff,pLength);
    }
    else
    {
        *pLength = 0;
    }
}

void ModbusSP_MasterHandler(uint8_t* pBuff,int* pLength,ModbusHandler handler)
/* 这个函数一般是modbus的主站调用,用来处理接收到的从站的相应. */
/* 注意:接收和发送使用的是同一个buff,在读完接收数据之前别写发送数据 */
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
    /* crc校验正确才进行后续操作,否则直接丢弃 */
    {
        ModbusSP_Package(pRecvPdu,pBuff,pLength);
        *pSendPdu = *pRecvPdu;
        Modbus_Handler(&(pRecvPdu->msppdu_pdu),&(pSendPdu->msppdu_pdu),handler);
    }
}


