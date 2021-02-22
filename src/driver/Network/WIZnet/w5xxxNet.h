#ifndef __W5XXXNET_H__
#define __W5XXXNET_H__
#include "wizchip_conf.h"

#define W55XX_SOCKET_QTY  8

#define SOCKET_0  0
#define SOCKET_1  1
#define SOCKET_2  2
#define SOCKET_3  3
#define SOCKET_4  4
#define SOCKET_5  5
#define SOCKET_6  6
#define SOCKET_7  7

void W5xxxNet_NetworkInit(uint32_t chipId,wiz_NetInfo* pNetInfor);
void W5xxxNet_GetNetworkInfor(uint32_t chipId,wiz_NetInfo* pNetInfor);

int32_t W5xxxNet_TcpClose(uint32_t chipId,uint8_t socket);
int32_t W5xxxNet_TcpSocket(uint32_t chipId,uint8_t socket, uint16_t port);
int32_t W5xxxNet_TcpListen(uint32_t chipId,uint8_t socket);
int32_t W5xxxNet_TcpAccept(uint32_t chipId,uint8_t socket);
int32_t W5xxxNet_TcpConnect(uint32_t chipId,uint8_t socket,uint16_t localPort, uint8_t * pIpAddr, uint16_t port);
int32_t W5xxxNet_TcpSend(uint32_t chipId,uint8_t socket, uint8_t * pBuffer, uint16_t length);
int32_t W5xxxNet_TcpRecv(uint32_t chipId,uint8_t socket, uint8_t * pBuffer, uint16_t length);



#endif
