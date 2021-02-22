#include "stdint.h"
#include "modbus.h"
//#include "sample.h"

#define MODULE_NAME MODBUSMASTERHANDLER /* ����궨��������#include"debug.h"֮ǰ.�����ֲ��������� */
#include "debug.h"



/*
addr    bytes    type    meaning
0:       4       float    Uab
1        4       float    Ubc
2        4       float    Uca
3        4       float    temp1
4        4       float    temp2
5        4       float    temp3
6        4       float    temp4
7        4       float    temp5
8        4       float    temp6
9        4       float    temp7
10       4       float    temp8
11       4       float    Rp(MOhm)
12       4       float    Rm(MOhm)
13       4       float    Ia
14       4       float    Ib
15       4       float    Ic
16       4       float    Igfci
*/






int ModbusMasterHandler_ReadRegister(uint16_t startRegisterAddr,uint16_t registerQty,uint8_t* pBuff)
/* ����Ϊ�Ĵ�����ʼ��ַ,�Ĵ�������,�Ĵ���ֵ���滺��ָ��;����ֵΪ�������ֽ���.(�˺������ڵ���������ַ��ȡ) */
{
}



int ModbusMasterHandler_Handler(struct Modbus_PDU* pRecvPDU,struct Modbus_PDU* pSendPDU)
{
    float fvalue=0;
    int   ivalue=0;
    uint16_t startRegAddr=0;
    uint16_t regQty=0;
    int byteQty=0;
    switch(pRecvPDU->mpdu_funCode)
    {
        case FC_ReadHoldReg:
        {
            //��ʼ��ַ�ͼĴ����������������ֽ�
            byteQty=(pRecvPDU->mpdu_pData[0]);
            DEBUG(3,"MODBUS�������յ��ӻ��Ļظ�.\n");
        }break;
    }
    return 0;
}
