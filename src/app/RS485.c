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
        /*  等待发送完成. */
        {
            osThreadYield();
        }
        break;
    }

    return 0;
}

int RS485_Recv(struct RS485Type* pRs485,uint8_t* pBuff, uint16_t buffLen, uint16_t* pRecvLen)
/* pRecvLen为传出参数,表示实际接收字节数. */
{
    if((int)pRs485*(int)pBuff*(int)buffLen*(int)pRecvLen==0)
    {
        return 0;
    }
    uint16_t toRecvLen=0,recvedLen=0;
    ARM_DRIVER_USART *pUartDrv = pRs485->rt_pUartDrv;

    /* 只有发送完成了才进入接收模式. */
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

    /* 等待数据包接收完成 */
    uint32_t recvedLen1=0,recvedLen2=0;
    while(1)
    {
        /* 先延时一个调度周期 */
        /* 注意:若接收不到数据,这里可以再适当多加几个Yield */
        osThreadYield();
        if(recvedLen1=pUartDrv->GetRxCount())
        {
            /* 再延时1个调度周期 */
            osThreadYield();
            osDelay(20);
            /* 判断接收是否停止 */
            if(recvedLen1 ==(recvedLen2=pUartDrv->GetRxCount()))
            /* 已接收数据不再变化,表示接收完成,跳出循环. */
            {
                break;
            }
            else
            /* 否则,继续等待 */
            {
            
            }
        }
    }
    *pRecvLen = recvedLen1;
    return recvedLen;
}




