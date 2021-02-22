#include "stm32f4xx.h"
#include "cmsis_os.h"
#include "pinAssignment.h"
#include "Driver_Usart.h"
#include "rs485.h"


void RS485_Callback(uint32_t event)
{
    (void)event;
}


int RS485_Send(struct RS485Type* pRs485,uint8_t* pBuff, uint16_t dataLength)
{
    if((int)pRs485*(int)pBuff*(int)dataLength==0)
    {
        return 0;
    }
    ARM_DRIVER_USART *pUartDrv = pRs485->rt_pUartDrv;

    if(RS485_CON_LSHR == pRs485->rt_mode)
    {
        HAL_GPIO_WritePin(pRs485->rt_pPin->port, pRs485->rt_pPin->pin, GPIO_PIN_RESET);
    }
    else if(RS485_CON_HSLR == pRs485->rt_mode)
    {
        HAL_GPIO_WritePin(pRs485->rt_pPin->port, pRs485->rt_pPin->pin, GPIO_PIN_SET);
    }

    while(1)
    {
        pUartDrv->Send(pBuff, dataLength);
        while(pUartDrv->GetStatus().tx_busy)
        /*  �ȴ��������. */
        {
            osThreadYield();
        }
        break;
    }

    return 0;
}

int RS485_Recv(struct RS485Type* pRs485,uint8_t* pBuff, uint16_t buffLen, uint16_t* pRecvLen)
/* pRecvLenΪ��������,��ʾʵ�ʽ����ֽ���. */
{
    if((int)pRs485*(int)pBuff*(int)buffLen*(int)pRecvLen==0)
    {
        return 0;
    }
    uint16_t toRecvLen=0,recvedLen=0;
    ARM_DRIVER_USART *pUartDrv = pRs485->rt_pUartDrv;

    /* ֻ�з�������˲Ž������ģʽ. */
    while(pUartDrv->GetStatus().tx_busy)
    {
        osThreadYield();
    }
    
    if(RS485_CON_LSHR == pRs485->rt_mode)
    {
        HAL_GPIO_WritePin(pRs485->rt_pPin->port, pRs485->rt_pPin->pin, GPIO_PIN_SET);
    }
    else if(RS485_CON_HSLR == pRs485->rt_mode)
    {
        HAL_GPIO_WritePin(pRs485->rt_pPin->port, pRs485->rt_pPin->pin, GPIO_PIN_RESET);
    }
    
    pUartDrv->Control(ARM_USART_ABORT_RECEIVE, 0);
    pUartDrv->Receive(pBuff, buffLen);

    /* �ȴ����ݰ�������� */
    uint32_t recvedLen1=0,recvedLen2=0;
    while(1)
    {
        /* ����ʱһ���������� */
        /* ע��:�����ղ�������,����������ʵ���Ӽ���Yield */
        osThreadYield();
        if(recvedLen1=pUartDrv->GetRxCount())
        {
            /* ����ʱ1���������� */
            osThreadYield();
            osDelay(20);
            /* �жϽ����Ƿ�ֹͣ */
            if(recvedLen1 ==(recvedLen2=pUartDrv->GetRxCount()))
            /* �ѽ������ݲ��ٱ仯,��ʾ�������,����ѭ��. */
            {
                break;
            }
            else
            /* ����,�����ȴ� */
            {
            
            }
        }
    }
    *pRecvLen = recvedLen1;
    return recvedLen;
}




