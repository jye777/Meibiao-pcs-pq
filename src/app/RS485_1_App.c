#include "stdint.h"
#include "cmsis_os.h"
#include "stm32f4xx.h"
#include "rs485.h"
#include "xxModbus.h"
#include "crc.h"
#include "modbus_serialport.h"
#include "pinAssignment.h"

//#define MODULE_NAME RS485_1APP /* ����궨��������#include"debug.h"֮ǰ.�����ֲ��������� */
//#include "debug.h"

#define DEBUG(...)

#if 0
extern ARM_DRIVER_USART Driver_USART4;

static const struct RS485Type rs485Conf[]=
{
    {
        .rt_pUartDrv=&Driver_USART4,
        .rt_pPin    =&PIN_INDEX_113,
        .rt_mode    =RS485_CON_LSHR,
    }
};

void RS485_1_Send(uint8_t * pBuff, uint16_t dataLength)
{
    struct RS485Type* pRs485=(struct RS485Type*)rs485Conf;
    RS485_Send(pRs485,pBuff,dataLength);
}


int RS485_1_Recv(uint8_t* pBuff, uint16_t buffLen)
/* ������ʾ����buffer��ͷָ�뼰�ܳ���,����ֵ��ʾ���յ����ֽ���. */
{
    uint16_t recvedLen=0;
    struct RS485Type* pRs485=(struct RS485Type*)rs485Conf;
    RS485_Recv(pRs485,pBuff,buffLen,&recvedLen);
    return recvedLen;
}

#define BUFFER_LENGTH (1000)
static uint8_t buffer[BUFFER_LENGTH];


void RS485_1_XXCMDPackage(uint8_t* pBuff,int* pLength)
/* modbus������װָ������Զ��� */
{
    /* slave station address */
    pBuff[0]=1;
    /* cmd */
    pBuff[1]=2;
    /* data */
    pBuff[2]=3;
    /* crc */
    uint16_t crc16Value=crc16((unsigned char*)pBuff,3);
    pBuff[3]=(uint8_t)(crc16Value>>8);
    pBuff[4]=(uint8_t)crc16Value;
    *pLength = 5;
}

void RS485_1_Thread(const void* pPara)
/* modbus�����߳�ʵ�� */
{
    int length=0;
    uint8_t* pBuff=buffer;
    while(1)
    {
        osDelay(3000);
        /* ���������������� */
        RS485_1_XXCMDPackage(pBuff,&length);
        RS485_1_Send(pBuff,length);
        DEBUG(5,"RS485_1�����ѷ���%d�ֽ�:%x %x %x %x %x.\n",length,pBuff[0],pBuff[1],pBuff[2],pBuff[3],pBuff[4]);
        /* �ȴ����մӻ�Ӧ�� */
        #if 1
        if(length=RS485_1_Recv(pBuff,BUFFER_LENGTH))
        {
            ModbusSP_MasterHandler(pBuff,&length,XXModbus_Master1Handler);
        }
        #endif
    }
}
#endif
