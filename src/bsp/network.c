#include <stdio.h>
#include <string.h>
#include "cmsis_os.h"
#include "rl_net.h"
#include "env.h"
#include "sdata_alloc.h"
#include "fpga.h"
#include "micro.h"

static const char def_mac_addr1[] = "00-01-02-32-3c-11";
static const char def_ip_addr1[]  = "192.168.1.11";
static const char def_mac_addr2[] = "00-01-02-32-3c-12";
static const char def_ip_addr2[]  = "192.168.1.12";

static const char def_gw[]       = "192.168.1.1";
static const char def_net_mask[] = "255.255.255.0";


uint8_t mac_addr[NET_ADDR_ETH_LEN];
uint8_t ip4_addr[NET_ADDR_IP4_LEN];
static uint8_t ip4_subnetmask[NET_ADDR_IP4_LEN];
static uint8_t ip4_defgateway[NET_ADDR_IP4_LEN];


void net_load_config(void)
{
    char *str;

    str = getenv("ethaddr");
    if(str == NULL)
    {
    	if(miro_write.slave_id == 1) {
        	netMAC_aton(def_mac_addr1, mac_addr);
		} else {
        	netMAC_aton(def_mac_addr2, mac_addr);
		}
    }
    else
    {
        netMAC_aton(str, mac_addr);
    }

    str = getenv("ipaddr");
    if(str == NULL)
    {
    	if(miro_write.slave_id == 1) {
        	netIP_aton(def_ip_addr1, NET_ADDR_IP4, ip4_addr);
		} else {
			netIP_aton(def_ip_addr2, NET_ADDR_IP4, ip4_addr);
		}
    }
    else
    {
			netIP_aton(str, NET_ADDR_IP4, ip4_addr);
    }

    str = getenv("netmask");
    if(str == NULL)
    {
        netIP_aton(def_net_mask, NET_ADDR_IP4, ip4_subnetmask);
    }
    else
    {
        netIP_aton(str, NET_ADDR_IP4, ip4_subnetmask);
    }

    str = getenv("gatewayip");
    if(str == NULL)
    {
        netIP_aton(def_gw, NET_ADDR_IP4, ip4_defgateway);
    }
    else
    {
        netIP_aton(str, NET_ADDR_IP4, ip4_defgateway);
    }
}

void network_init(void)
{
    net_load_config();
    netInitialize();
    /* Change MAC address */
    netIF_SetOption (NET_IF_CLASS_ETH | 0, netIF_OptionMAC_Address, mac_addr, NET_ADDR_ETH_LEN);
    /* Change IP address */
    netIF_SetOption (NET_IF_CLASS_ETH | 0, netIF_OptionIP4_Address, ip4_addr, NET_ADDR_IP4_LEN);
    /* Change Network mask */
    netIF_SetOption (NET_IF_CLASS_ETH | 0, netIF_OptionIP4_SubnetMask, ip4_subnetmask, NET_ADDR_IP4_LEN);
    /* Change Default Gateway address */
    netIF_SetOption (NET_IF_CLASS_ETH | 0, netIF_OptionIP4_DefaultGateway, ip4_defgateway, NET_ADDR_IP4_LEN);
}
