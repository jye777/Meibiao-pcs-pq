#include "stdint.h"
#include "cmsis_os.h"
#include "stm32f4xx.h"
#include "wizchip_conf.h"
#include "w5xxxNet.h"
#include "initW5xxx.h"
//这个文件用于示范W5500提供的tcp/ip栈该如何使用.

uint8_t pNet0Server0Buff[200];
uint8_t pNet0Client0Buff[200];

void W5xxxEth_Net0_Server0(const void* pPara)
{
    uint8_t socket=0;
    uint16_t port =5000;
    uint32_t chipId=0;
    int32_t recvDataLength=0;
    uint8_t step=0;

    while(1)
    {
        switch(step)
        {
            case 0:
            {
                W5xxxNet_TcpSocket(chipId,socket,port);
                W5xxxNet_TcpListen(chipId,socket);
                W5xxxNet_TcpAccept(chipId,socket);
                step=1;
            }
            break;
            case 1:
            {
                recvDataLength=0;
                memset(pNet0Server0Buff,0,sizeof(pNet0Server0Buff));
                recvDataLength=W5xxxNet_TcpRecv(chipId,socket,pNet0Server0Buff,sizeof(pNet0Server0Buff));
                if(recvDataLength>0&&recvDataLength<sizeof(pNet0Server0Buff))
                {
                    printf("Net_0 收到并发送数据:%s\n",pNet0Server0Buff);
                    W5xxxNet_TcpSend(chipId,socket,pNet0Server0Buff,recvDataLength);
                }
                else if(recvDataLength>0)
                {
                    printf("数据过长,已超出buffer\n");
                }
                else if(recvDataLength<0)
                {
                    step=0;
                }
            }
            break;
        }
    }
}

void W5xxxEth_Net0_Client0(const void* pPara)
{
    uint8_t socket=1;
    uint16_t localPort =50000;
    uint32_t chipId=0;
    int32_t recvDataLength=0;
    uint8_t step=0;
    while(1)
    {
        switch(step)
        {
            case 0:
            {
                W5xxxNet_TcpSocket(chipId,socket,localPort);
                uint8_t ipAddr[]={192,168,0,101};
                W5xxxNet_TcpConnect(chipId,socket,localPort,ipAddr,5000);
                step=1;
            }
            break;
            case 1:
            {
                recvDataLength=0;
                memset(pNet0Client0Buff,0,sizeof(pNet0Client0Buff));
                recvDataLength=W5xxxNet_TcpRecv(chipId,socket,pNet0Client0Buff,sizeof(pNet0Client0Buff));
                if(recvDataLength>0&&recvDataLength<sizeof(pNet0Client0Buff))
                {
                    printf("Net 0 收到数据:%s\n",pNet0Client0Buff);
                    W5xxxNet_TcpSend(chipId,socket,pNet0Client0Buff,recvDataLength);
                }
                else if(recvDataLength>0)
                {
                    printf("数据过长,已超出buffer\n");
                }
                else if(recvDataLength<0)
                {
                    step=0;
                }
            }
            break;
        }
    }
}


uint8_t pNet1Server0Buff[200];
uint8_t pNet1Client0Buff[200];


void W5xxxEth_Net1_Server0(const void* pPara)
{
    uint8_t socket=0;
    uint16_t port =5000;
    uint32_t chipId=1;
    int32_t recvDataLength=0;
    uint8_t step=0;

    while(1)
    {
        switch(step)
        {
            case 0:
            {
                W5xxxNet_TcpSocket(chipId,socket,port);
                W5xxxNet_TcpListen(chipId,socket);
                W5xxxNet_TcpAccept(chipId,socket);
                step=1;
            }
            break;
            case 1:
            {
                recvDataLength=0;
                memset(pNet1Server0Buff,0,sizeof(pNet1Server0Buff));
                recvDataLength=W5xxxNet_TcpRecv(chipId,socket,pNet1Server0Buff,sizeof(pNet1Server0Buff));
                if(recvDataLength>0&&recvDataLength<sizeof(pNet1Server0Buff))
                {
                    printf("Net_1 收到并发送数据:%s\n",pNet1Server0Buff);
                    W5xxxNet_TcpSend(chipId,socket,pNet1Server0Buff,recvDataLength);
                }
                else if(recvDataLength>0)
                {
                    printf("数据过长,已超出buffer\n");
                }
                else if(recvDataLength<0)
                {
                    step=0;
                }
            }
            break;
        }
    }
}

void W5xxxEth_Net1_Client0(const void* pPara)
{
    uint8_t socket=1;
    uint16_t localPort =50000;
    uint32_t chipId=1;
    int32_t recvDataLength=0;
    uint8_t step=0;
    while(1)
    {
        switch(step)
        {
            case 0:
            {
                W5xxxNet_TcpSocket(chipId,socket,localPort);
                uint8_t ipAddr[]={192,168,0,101};
                W5xxxNet_TcpConnect(chipId,socket,localPort,ipAddr,5000);
                step=1;
            }
            break;
            case 1:
            {
                recvDataLength=0;
                memset(pNet1Client0Buff,0,sizeof(pNet1Client0Buff));
                recvDataLength=W5xxxNet_TcpRecv(chipId,socket,pNet1Client0Buff,sizeof(pNet1Client0Buff));
                if(recvDataLength>0&&recvDataLength<sizeof(pNet1Client0Buff))
                {
                    printf("Net_1 收到数据:%s\n",pNet1Client0Buff);
                    W5xxxNet_TcpSend(chipId,socket,pNet1Client0Buff,recvDataLength);
                }
                else if(recvDataLength>0)
                {
                    printf("数据过长,已超出buffer\n");
                }
                else if(recvDataLength<0)
                {
                    step=0;
                }
            }
            break;
        }
    }
}


