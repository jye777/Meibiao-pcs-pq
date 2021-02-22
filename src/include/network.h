/**
 * @file			network.h
 * @brief			
 * @author			wangpeng
 * @date			2016/4/18
 * @version			Initial Draft
 * @par				Copyright (C),  2013-2023, SCNEE
 * @par History:
 * 1.Date: 			2016/4/18
 *   Author: 			wangpeng
 *   Modification: 		Created file
*/

#ifndef NETWORK_H
#define NETWORK_H
#include "rl_net.h"
struct networkInfo
{
    /* MAC */
    const char* ni_pMac;
    char ni_Mac[NET_ADDR_ETH_LEN];
    /* IP */
    const char* ni_pIp;
    char ni_Ip[NET_ADDR_IP4_LEN];
    /* MASK */
    const char* ni_pMask;
    char ni_Mask[NET_ADDR_IP4_LEN];
    /* GATEWAY */
    const char* ni_pGateway;
    char ni_Gateway[NET_ADDR_IP4_LEN];
    /* DNS */
    const char* ni_pDns;
    char ni_Dns[NET_ADDR_IP4_LEN];
};


void network_init(void);

#endif /* NETWORK_H */

