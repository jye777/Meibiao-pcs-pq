#include "stdint.h"
#include "cmsis_os.h"
#include "stm32f4xx.h"
#include "rs485.h"
#include "xxModbus.h"
#include "modbus_serialport.h"
#include "crc.h"
#include "pinAssignment.h"

#define MODULE_NAME RS485_2APP /* 这个宏定义必须放在#include"debug.h"之前.且名字不能随便更改 */
#include "debug.h"


extern ARM_DRIVER_USART Driver_USART3;

static const struct RS485Type rs485Conf[]=
{
    {
        .rt_pUartDrv=&Driver_USART3,
        .rt_pPin    =&PIN_INDEX_113,
        .rt_mode    =RS485_CON_LSHR,
    }
};

void RS485_2_Init(void)
{
    struct RS485Type* pRs485=(struct RS485Type*)rs485Conf;
    //control pin init
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = pRs485->rt_pPin->pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(pRs485->rt_pPin->port, &GPIO_InitStruct);
    //usart init
    //USART3_UART_Init();/* 系统启动时已初始化usart3,所以这里不再初始化usart3 */
#if 0//debug    
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif
}


void RS485_2_Send(uint8_t * pBuff, uint16_t dataLength)
{
    struct RS485Type* pRs485=(struct RS485Type*)rs485Conf;
    RS485_Send(pRs485,pBuff,dataLength);
}


int RS485_2_Recv(uint8_t* pBuff, uint16_t buffLen)
/* 参数表示传入buffer的头指针及总长度,返回值表示接收到的字节数. */
{
    uint16_t recvedLen=0;
    struct RS485Type* pRs485=(struct RS485Type*)rs485Conf;
    RS485_Recv(pRs485,pBuff,buffLen,&recvedLen);
    return recvedLen;
}

#define BUFFER_LENGTH (500)
static uint8_t buffer[BUFFER_LENGTH];


void RS485_2_Thread(const void* pPara)
/* modbus 从机实例 */
{
    int length=0;

    uint8_t* pBuff=buffer;
    RS485_2_Init();
    while(1)
    {
        #if 0//for debug
        osDelay(5000);
        /* 打包主机发送请求包 */
        RS485_2_XXCMDPackage(pBuff,&length);
        RS485_2_Send(pBuff,length);
        DEBUG(5,"RS485_2主机已发送\n");
        #endif
        
        #if 1
        if(length=RS485_2_Recv(pBuff,BUFFER_LENGTH))
        {
            DEBUG_A(5,"[SLAVE]从站接收到%d字节的数据:",length);
            for(int index=0;index<length;index++)
            {
                DEBUG_A(5,"%02x ",pBuff[index]);
            }
            DEBUG_A(5,"\n");
            ModbusSP_SlaveHandler(pBuff,&length,XXModbus_Slave2Handler);
            if(length)
            {
                RS485_2_Send(pBuff,length);
            }
        }
        #endif

        osThreadYield();
    }
}
