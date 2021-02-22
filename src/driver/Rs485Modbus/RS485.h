#ifndef __RS485_H__
#define __RS485_H__
#include "stdint.h"
#include "stm32f1xx.h"
#include "pinAssignment.h"
#include "driver_usart.h"


#define RS485_CON_LSHR  0  /* ���ƽŵ͵�ƽ��ʾ���� */
#define RS485_CON_HSLR  1  /* ���ƽŸߵ�ƽ��ʾ���� */

struct RS485Type
{
    const ARM_DRIVER_USART* rt_pUartDrv;
    const struct pinType*   rt_pPin;
    const int               rt_mode;
};


int RS485_Send(struct RS485Type* pRs485,uint8_t* pBuff, uint16_t dataLength);
int RS485_Recv(struct RS485Type* pRs485,uint8_t* pBuff, uint16_t buffLen, uint16_t* pRecvLen);


#endif
