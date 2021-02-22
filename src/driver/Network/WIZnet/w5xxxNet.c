#include "stdint.h"
#include "cmsis_os.h"
#include "stm32f4xx.h"
#include "wizchip_conf.h"
#include "socket.h"
#include "w5xxxNet.h"

//#define MODULE_NAME W5XXXNET /* ����궨��������#include"debug.h"֮ǰ.�����ֲ��������� */
//#include "debug.h"

#define DEBUG(...)
#define GLOBAL_DEBUG(...)

/* ���ļ����ڽ�һ����װBSD socket API. */


static uint8_t w5xxxSockState[W55XX_SOCKET_QTY]={0};


/**
  * @brief  Intialize the network information to be used in WIZCHIP
  * @retval None
  */
void W5xxxNet_NetworkInit(uint32_t chipId,wiz_NetInfo* pNetInfor)
{
    ctlnetwork(chipId,CN_SET_NETINFO, (void*)pNetInfor);
#if 0
    uint8_t tmpstr[6];
    // Display Network Information
    ctlnetwork(chipId,CN_GET_NETINFO, (void*)&NetInforReadOut);
    ctlwizchip(chipId,CW_GET_ID,(void*)tmpstr);
    W5500_DEBUG("\r\n=== %s NET CONF ===\r\n",(char*)tmpstr);
    W5500_DEBUG("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",NetInforReadOut.mac[0],NetInforReadOut.mac[1],NetInforReadOut.mac[2],
          NetInforReadOut.mac[3],NetInforReadOut.mac[4],NetInforReadOut.mac[5]);
    W5500_DEBUG("SIP: %d.%d.%d.%d\r\n", NetInforReadOut.ip[0],NetInforReadOut.ip[1],NetInforReadOut.ip[2],NetInforReadOut.ip[3]);
    W5500_DEBUG("GAR: %d.%d.%d.%d\r\n", NetInforReadOut.gw[0],NetInforReadOut.gw[1],NetInforReadOut.gw[2],NetInforReadOut.gw[3]);
    W5500_DEBUG("SUB: %d.%d.%d.%d\r\n", NetInforReadOut.sn[0],NetInforReadOut.sn[1],NetInforReadOut.sn[2],NetInforReadOut.sn[3]);
    W5500_DEBUG("DNS: %d.%d.%d.%d\r\n", NetInforReadOut.dns[0],NetInforReadOut.dns[1],NetInforReadOut.dns[2],NetInforReadOut.dns[3]);
    W5500_DEBUG("======================\r\n");
#endif
}

void W5xxxNet_GetNetworkInfor(uint32_t chipId,wiz_NetInfo* pNetInfor)
{
	ctlnetwork(chipId,CN_GET_NETINFO, (void*)&pNetInfor);
}

int32_t W5xxxNet_TcpClose(uint32_t chipId,uint8_t socket)
{
    int32_t ret=SOCK_ERROR; // return value for SOCK_ERRORs
    switch(getSn_SR(chipId,socket))
    {
        case SOCK_ESTABLISHED:
        case SOCK_CLOSE_WAIT :
        {
            if((ret=w5xx_disconnect(chipId,socket)) != SOCK_OK) return ret;
            
            ret=w5xx_close(chipId,socket);
            DEBUG(3,"%d:Socket Closed\n", socket);
            
        }break;
        default:
        {
            DEBUG(3,"W5500 socket:%d��ǰ״̬����,�޷�ִ��close����\n",socket);
        }break;
    }
    return ret;
}


int32_t W5xxxNet_TcpSocket(uint32_t chipId,uint8_t socket, uint16_t port)
{
    int32_t ret=SOCK_ERROR; // return value for SOCK_ERRORs
    //�ж������socket�Ƿ񳬷�Χ(ֻ����socket 0,1,2,3,4,5,6,7���˸�)
    if(socket >= W55XX_SOCKET_QTY)
    {
        ret = SOCKERR_SOCKNUM;
        return ret;
    }
    switch(getSn_SR(chipId,socket))
    {
        case SOCK_CLOSE_WAIT :
        case SOCK_CLOSED:
        {
            w5xx_disconnect(chipId,socket);
            w5xx_close(chipId,socket);
            if((ret=w5xx_socket(chipId,socket, Sn_MR_TCP,port, 0x00)) != socket)
            {
                DEBUG(3,"socket open FAILED.\n");
                return ret; // TCP socket open with 'any_port' port number
            }
            else
            {
                ret=SOCK_OK;
            }
            uint8_t arg=2;/* 2*5s����һ��keep     alive�� */
            w5xx_setsockopt(chipId,socket,SO_KEEPALIVEAUTO,&arg/* 2*5s����һ��keep     alive�� */);
            DEBUG(3,"socket opened\n");
        }break;
        default:
        {
            DEBUG(3,"W5500 socket:%d��ǰ״̬����,�޷�ִ������socket����\n",socket);
        }break;
    }
    return ret;
}

int32_t W5xxxNet_TcpListen(uint32_t chipId,uint8_t socket)
{
    int32_t ret=SOCK_ERROR; // return value for SOCK_ERRORs
    switch(getSn_SR(chipId,socket))
    {

        case SOCK_INIT :
            DEBUG(3,"%d:Listen, TCP server listen\r\n", socket);
            if( (ret = w5xx_listen(chipId,socket)) != SOCK_OK)
            {
                return ret;
            }
            break;
        default:
        {
            DEBUG(3,"��ǰδ����SOCK_INIT״̬,�޷����м���.\n");
        }
        break;
    }
    return ret;
}

int32_t W5xxxNet_TcpAccept(uint32_t chipId,uint8_t socket)
{
    uint8_t srData=0;
    uint32_t counter=0;
    while(1)
    {
        srData = getSn_SR(chipId,socket);
        
        if (srData == SOCK_ESTABLISHED)
        {
            if(getSn_IR(chipId,socket) & Sn_IR_CON)    // Socket n interrupt register mask; TCP CON interrupt = connection with peer is successful
            {
#ifdef MODULE_DEBUG_ENABLE(MODULE_NAME)
                uint8_t destip[4];
                uint16_t destport;

                getSn_DIPR(chipId,socket, destip);
                destport = getSn_DPORT(chipId,socket);

                DEBUG(3,"%d:Connected - %d.%d.%d.%d : %d\r\n",socket, destip[0], destip[1], destip[2], destip[3], destport);
#endif
                setSn_IR(chipId,socket, Sn_IR_CON);  // this interrupt should be write the bit cleared to '1'
                break;
            }
        }
        else
        {
            if(counter++%2000==0)
            {
                DEBUG(3,"W55XX�ĵ�ǰ״̬��%d.\n",srData);
            }
        }
    }
    return 0;
}


int32_t W5xxxNet_TcpConnect(uint32_t chipId,uint8_t socket,uint16_t localPort, uint8_t * pIpAddr, uint16_t port)
{
    uint32_t counter=0;
    while(1)
    {
        switch(getSn_SR(chipId,socket))
        {
            case SOCK_INIT :
            {
                if(counter++%2000==0)
                {
                    DEBUG(3,"%d:Try to connect to the %d.%d.%d.%d : %d\r\n", socket, pIpAddr[0], pIpAddr[1], pIpAddr[2], pIpAddr[3], port);
                }
                w5xx_connect(chipId,socket, pIpAddr, port); 
                
            }break;
    
            case SOCK_ESTABLISHED :
            {
                if(getSn_IR(chipId,socket) & Sn_IR_CON)    // Socket n interrupt register mask; TCP CON interrupt = connection with peer is successful
                {
                    DEBUG(3,"%d:Connected to - %d.%d.%d.%d : %d\r\n",socket, pIpAddr[0], pIpAddr[1], pIpAddr[2], pIpAddr[3], port);
                    setSn_IR(chipId,socket, Sn_IR_CON);  // this interrupt should be write the bit cleared to '1'
                }
                return 0;    
            }break;
            
            case SOCK_CLOSED:
            {
                w5xx_close(chipId,socket);
                if(w5xx_socket(chipId,socket, Sn_MR_TCP, localPort, 0x00) != socket)
                {
                    DEBUG(3,"��ȡsocketʧ��\n");
                }
                
            }break;
            
            default:
            {
                if(counter++%2000==0)
                {
                    DEBUG(3,"connectʱ��δ�����״̬\n");
                }
            }break;
        }

    }

    return SOCK_OK;
}

int32_t W5xxxNet_TcpSend(uint32_t chipId,uint8_t socket, uint8_t * pBuffer, uint16_t length)
{
    int32_t ret=0; // return value for SOCK_ERRORs
    uint32_t toBeSentByteQty=0;
    uint32_t haveSentByteQty=0;

    toBeSentByteQty=length;
    switch(getSn_SR(chipId,socket))
    {
        case SOCK_ESTABLISHED :
        {
            if(getSn_IR(chipId,socket) & Sn_IR_CON)    // Socket n interrupt register mask; TCP CON interrupt = connection with peer is successful
            {
                DEBUG(3,"%d:Connected �ɹ���\n",socket);
                setSn_IR(chipId,socket, Sn_IR_CON);  // this interrupt should be write the bit cleared to '1'
            }
            
			// Data sentsize control
			while(haveSentByteQty != toBeSentByteQty)
			{
				ret = w5xx_send(chipId,socket, pBuffer+haveSentByteQty, toBeSentByteQty-haveSentByteQty); // Data send process (User's buffer -> Destination through H/W Tx socket buffer)
				if(ret < 0) // Send Error occurred (sent data length < 0)
				{
					w5xx_close(chipId,socket); // socket close
					return ret;
				}
				haveSentByteQty += ret; // Don't care SOCKERR_BUSY, because it is zero.
			}
        }break;
        default:
        {
            ret=SOCKERR_SOCKSTATUS;
            GLOBAL_DEBUG("W5500 socket:%d��ǰ������ESTABLISHED״̬,�޷�ִ�з��Ͳ���\n",socket);
        }break;
    }
    return SOCK_OK;
}

int32_t W5xxxNet_TcpRecv(uint32_t chipId,uint8_t socket, uint8_t * pBuffer, uint16_t length)
/* ���ض�ȡ�����ֽ������ߴ����� */
{
    int32_t ret=0; // return value for SOCK_ERRORs
    uint32_t maxAllowedRecvByteQty=0;
    uint32_t haveReadByteQty=0;
    uint32_t haveRecvedByteQty;

    maxAllowedRecvByteQty=length;
    switch(getSn_SR(chipId,socket))
    {
        case SOCK_ESTABLISHED:
        {
            if(getSn_IR(chipId,socket) & Sn_IR_CON)    // Socket n interrupt register mask; TCP CON interrupt = connection with peer is successful
            {
#ifdef W5XXXNET_DEBUG_ENABLE
                uint8_t destip[4];
                uint16_t destport;

                getSn_DIPR(chipId,socket, destip);
                destport = getSn_DPORT(chipId,socket);
    
                DEBUG(3,"%d:Connected - %d.%d.%d.%d : %d\r\n",socket, destip[0], destip[1], destip[2], destip[3], destport);
#endif
                //DEBUG(DEBUG_GRADE_3,"%d:Connected �ɹ���\n",socket);
                setSn_IR(chipId,socket, Sn_IR_CON);  // this interrupt should be write the bit cleared to '1'
            }
            
            if((haveRecvedByteQty = getSn_RX_RSR(chipId,socket)) > 0) // Sn_RX_RSR: Socket n Received Size Register, Receiving data length
            {
                if(haveRecvedByteQty > maxAllowedRecvByteQty)
                {
                    haveRecvedByteQty = maxAllowedRecvByteQty; // DATA_BUF_SIZE means user defined buffer size (array)
                    DEBUG(3,"�������ݸ��������û���buff����,���������Ĳ���\n");
                }
                ret = w5xx_recv(chipId,socket, pBuffer, haveRecvedByteQty); // Data Receive process (H/W Rx socket buffer -> User's buffer)

                if(ret <= 0) 
                {
                    return ret; // If the received data length <= 0, receive failed and process end
                }
                DEBUG(3,"SOCKET:%d���յ�%d�ֽ�\n",socket,ret);
            }
        }break;
        default:
        {
            ret=SOCKERR_SOCKSTATUS;
            DEBUG(3,"W5500 socket:%d��ǰ������ESTABLISHED״̬,�޷�ִ�н��ղ���\n",socket);
        }break;
    }
    return ret;
}

