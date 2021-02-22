#include "stdint.h"
#include "cmsis_os.h"
#include "stm32f4xx.h"
#include "driver_spi.h"
#include "wizchip_conf.h"
#include "network.h"
#include "env.h"
#include "rl_net.h"
#include "w5xxxnet.h"

//#define MODULE_NAME INITW5XXX /* 这个宏定义必须放在#include"debug.h"之前.且名字不能随便更改 */
//#include "debug.h"
#define DEBUG(...)

/* 此文件用于初始化w5500芯片及网络配置. */



/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CHIP0ID W5XX_CHIP_INDEX_0
#define CHIP1ID W5XX_CHIP_INDEX_1
/* Private macro -------------------------------------------------------------*/

#if 1
// Default Network Configuration
wiz_NetInfo netInfor[W5XX_CHIP_QTY] = 
{
    {
    //.mac = {0x00, 0x08, 0xdc,0x00, 0xab, 0xcd},
    //.ip = {192, 168, 0, 111},
    //.sn = {255,255,255,0},
    //.gw = {192, 168, 0, 1},
    //.dns = {114,114,114,114},
    .dhcp = NETINFO_STATIC 
    },
    {
    //.mac = {0x00, 0x08, 0xdc,0x00, 0xab, 0xcd},
    //.ip = {192, 168, 0, 111},
    //.sn = {255,255,255,0},
    //.gw = {192, 168, 0, 1},
    //.dns = {114,114,114,114},
    .dhcp = NETINFO_STATIC 
    }

};

static const struct networkInfo netInf[W5XX_CHIP_QTY]=
{
    {
    .ni_pMac="00-08-dc-00-ab-cd",
    .ni_pIp="192.168.0.111",
    .ni_pMask="255.255.255.0",
    .ni_pGateway="192.168.0.1",
    .ni_pDns="114.114.114.114",
    },
    {
    .ni_pMac="00-08-dc-00-ab-cf",
    .ni_pIp="192.168.0.112",
    .ni_pMask="255.255.255.0",
    .ni_pGateway="192.168.0.1",
    .ni_pDns="114.114.114.114",
    }
};/* 保存扩展网卡的默认网络配置 */



#endif

#if 0
/* 模拟SPI通讯 */
/* Private variables ---------------------------------------------------------*/
struct spiPinType spi_2=
{
.spi_mosi ={GPIOB,GPIO_PIN_15},
.spi_miso ={GPIOB,GPIO_PIN_14},
.spi_cs   ={GPIOB,GPIO_PIN_12},
.spi_sclk ={GPIOB,GPIO_PIN_13},
.spi_mode = SPI_MODE_0,
};


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


void W5500_0_ChipSelect(void)
{
    Spi_ChipSelect(&spi_2);
}

void W5500_0_ChipDeselect(void)
{
    Spi_ChipDeselect(&spi_2);
}

uint8_t W5500_1_ReadByte(void)
{
    uint8_t data=0;
    Spi_TransferBytes(&spi_2,NULL,0,&data,1);
    return data;
}
void W5500_1_WriteByte(uint8_t wb)
{
    Spi_TransferBytes(&spi_2,&wb,1,NULL,0);
}
void W5500_0_ReadBurst(uint8_t* pBuf, uint16_t len)
{
    Spi_TransferBytes(&spi_2,NULL,0,pBuf,len);
}
void W5500_0_WriteBurst(uint8_t* pBuf, uint16_t len)
{
    Spi_TransferBytes(&spi_2,pBuf,len,NULL,0);
}
#endif


#if 1
/* 硬件SPI通信 */
extern ARM_DRIVER_SPI Driver_SPI2;

void W5500_0_ChipSelect(void)
{
    ARM_DRIVER_SPI *spi_drv = &Driver_SPI2;
    
    spi_drv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_ACTIVE);
    //Spi_ChipSelect(&spi_2);
}

void W5500_0_ChipDeselect(void)
{
    ARM_DRIVER_SPI *spi_drv = &Driver_SPI2;
    
    spi_drv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
    //Spi_ChipDeselect(&spi_2);
}

void W5500_0_ReadBurst(uint8_t* pBuf, uint16_t len)
{
    ARM_DRIVER_SPI *spi_drv = &Driver_SPI2;
    
    //while(spi_drv->GetStatus().busy);/* 操作前判断状态(这种操作方式会有未知问题,不要使用) */
    spi_drv->Receive(pBuf, len);
    while(spi_drv->GetDataCount() != len);/* 操作后判断状态 */
}
void W5500_0_WriteBurst(uint8_t* pBuf, uint16_t len)
{
    ARM_DRIVER_SPI *spi_drv = &Driver_SPI2;
    //while(spi_drv->GetStatus().busy);/* 操作前判断状态(这种操作方式会有未知问题,不要使用) */
    spi_drv->Send(pBuf, len);
    while(spi_drv->GetDataCount() != len);/* 操作后判断状态 */
}

#endif

#if 1
/* 硬件SPI通信 */
extern ARM_DRIVER_SPI Driver_SPI1;

void W5500_1_ChipSelect(void)
{
    ARM_DRIVER_SPI *spi_drv = &Driver_SPI1;
    
    spi_drv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_ACTIVE);
    //Spi_ChipSelect(&spi_2);
}

void W5500_1_ChipDeselect(void)
{
    ARM_DRIVER_SPI *spi_drv = &Driver_SPI1;
    
    spi_drv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
    //Spi_ChipDeselect(&spi_2);
}

void W5500_1_ReadBurst(uint8_t* pBuf, uint16_t len)
{
    ARM_DRIVER_SPI *spi_drv = &Driver_SPI1;
    
    //while(spi_drv->GetStatus().busy);/* 操作前判断状态(这种操作方式会有未知问题,不要使用) */
    spi_drv->Receive(pBuf, len);
    while(spi_drv->GetDataCount() != len);/* 操作后判断状态 */
}
void W5500_1_WriteBurst(uint8_t* pBuf, uint16_t len)
{
    ARM_DRIVER_SPI *spi_drv = &Driver_SPI1;
    //while(spi_drv->GetStatus().busy);/* 操作前判断状态(这种操作方式会有未知问题,不要使用) */
    spi_drv->Send(pBuf, len);
    while(spi_drv->GetDataCount() != len);/* 操作后判断状态 */
}

#endif





#if 1

void W5xxxNet0_CommunicationPortInit(void)
/* step1:初始化spi通信接口 */
{
    #if 0/* gpio simulate spi */
    GpioSpi_Init3(&spi_2);
    #endif

    #if 1
    /* spi init is in bsp.c */
    #endif
}

void W5xxxNet0_ChipInitialise(void)
/* step2:对芯片进行初始化,并注册spi通信函数 */
{
    uint32_t chipId=CHIP0ID;
    uint8_t tmp;
    int32_t ret = 0;
    uint8_t memsize[2][8] = {{2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};  

    //W5500_1_CommunicationPortInit();

    reg_wizchip_iomode_init(chipId);
    //Host dependent peripheral initialized
    // First of all, Should register SPI callback functions implemented by user for accessing WIZCHIP 
    /* Critical section callback */
    reg_wizchip_cris_cbfunc(chipId,NULL, NULL); //注册临界区函数
    /* Chip selection call back */
#if   _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_VDM_
    reg_wizchip_cs_cbfunc(chipId,W5500_0_ChipSelect, W5500_0_ChipDeselect);//注册SPI片选信号函数
#elif _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_FDM_
    reg_wizchip_cs_cbfunc(chipId,W5500_0_ChipSelect, W5500_0_ChipSelect);  // CS must be tried with LOW.
#else
    reg_wizchip_cs_cbfunc(chipId,NULL, NULL);
#endif
    /* SPI Read & Write callback function */
    //reg_wizchip_spi_cbfunc(chipId,W5500_1_ReadByte, W5500_1_WriteByte);   //注册读写函数
    reg_wizchip_spiburst_cbfunc(chipId,W5500_0_ReadBurst,W5500_0_WriteBurst);

#if 0
    while(0)
    //(for debug spi reading and writing.)
    {
        /* Network initialization */
        W5xxxNet_NetworkInit(chipId);
        osDelay(3000);
    }
#endif

    /* WIZCHIP SOCKET Buffer initialize */
    if(ctlwizchip(chipId,CW_INIT_WIZCHIP,(void*)memsize) == -1)
    {
        DEBUG(3,"WIZCHIP Initialized fail.\r\n");
        while(1);
    }
#if 0
    /* PHY link status check */
    do{
        if(ctlwizchip(chipId,CW_GET_PHYLINK, (void*)&tmp) == -1)
        {
            DEBUG(3,"Unknown PHY Link stauts.\r\n");
        }
        if(tmp == PHY_LINK_OFF)
        {
            DEBUG(3,"PHY Link is OFF.\r\n");
        }
        else
        {
            DEBUG(3,"PHY Link ON.\r\n");
        }
        osDelay(2000);
    }while(tmp == PHY_LINK_OFF);
#endif
    /*******************************/
    /* WIZnet W5500 Code Examples  */
    /* TCPS/UDPS Loopback test     */
    /*******************************/
    /* Main loop */
}



wiz_NetInfo* W5xxxNet0_GetConfig(void)
/* 将网卡配置信息读出来 */
{
    uint32_t chipId=CHIP0ID;
    wiz_NetInfo* pNetInfor = netInfor+chipId;
    return pNetInfor;
}

void W5xxxNet0_LoadConfig(void)
/* 从环境变量中读出网卡配置信息保存起来,以便后续设置到网卡中 */
{
    uint32_t chipId=CHIP0ID;
    wiz_NetInfo* pNetInfor = netInfor+chipId;
    char *str;
    struct networkInfo* pNetInf= (struct networkInfo*)(netInf+chipId);

#if 1
    str = getenv(MAC0);
    if(str == NULL)
    {
        netMAC_aton(pNetInf->ni_pMac, pNetInfor->mac);
    }
    else
    {
        netMAC_aton(str, pNetInfor->mac);
    }

    str = getenv(IP0);
    if(str == NULL)
    {
        netIP_aton(pNetInf->ni_pIp, NET_ADDR_IP4, pNetInfor->ip);
    }
    else
    {
        netIP_aton(str, NET_ADDR_IP4, pNetInfor->ip);
    }

    str = getenv(NETMASK0);
    if(str == NULL)
    {
        netIP_aton(pNetInf->ni_pMask, NET_ADDR_IP4, pNetInfor->sn);
    }
    else
    {
        netIP_aton(str, NET_ADDR_IP4, pNetInfor->sn);
    }

    str = getenv(GATEWAY0);
    if(str == NULL)
    {
        netIP_aton(pNetInf->ni_pGateway, NET_ADDR_IP4, pNetInfor->gw);
    }
    else
    {
        netIP_aton(str, NET_ADDR_IP4, pNetInfor->gw);
    }
    
    str = getenv(DNS0);
    if(str == NULL)
    {
        netIP_aton(pNetInf->ni_pDns, NET_ADDR_IP4, pNetInfor->dns);
    }
    else
    {
        netIP_aton(str, NET_ADDR_IP4, pNetInfor->dns);
    }
#else
    netMAC_aton(pNetInf->ni_pMac, pNetInf->mac);
    netIP_aton(pNetInf->ni_pIp, NET_ADDR_IP4, pNetInfor->ip);
    netIP_aton(pNetInf->ni_pMask, NET_ADDR_IP4, pNetInfor->sn);
    netIP_aton(pNetInf->ni_pGateway, NET_ADDR_IP4, pNetInfor->gw);
    netIP_aton(pNetInf->ni_pDns, NET_ADDR_IP4, pNetInfor->dns);
#endif
}


void W5xxxNet0_NetworkInit(void)
/* step3:进行本地网络配置 */
{
    uint32_t chipId=CHIP0ID;

    /* Network initialization */
    W5xxxNet_NetworkInit(chipId,netInfor+chipId);
}

void W5xxxNet0_Init(void)
{
    W5xxxNet0_CommunicationPortInit();
    W5xxxNet0_ChipInitialise();
    W5xxxNet0_LoadConfig();
    W5xxxNet0_NetworkInit();
}

#endif

#if 1

void W5xxxNet1_CommunicationPortInit(void)
/* step1:初始化spi通信接口 */
{
    #if 0/* gpio simulate spi */
    GpioSpi_Init3(&spi_2);
    #endif

    #if 1
    /* spi init is in bsp.c */
    #endif
}

void W5xxxNet1_ChipInitialise(void)
/* step2:对芯片进行初始化,并注册spi通信函数 */
{
    uint32_t chipId=CHIP1ID;
    uint8_t tmp;
    int32_t ret = 0;
    uint8_t memsize[2][8] = {{2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};  

    //W5500_1_CommunicationPortInit();

    reg_wizchip_iomode_init(chipId);
    //Host dependent peripheral initialized
    // First of all, Should register SPI callback functions implemented by user for accessing WIZCHIP 
    /* Critical section callback */
    reg_wizchip_cris_cbfunc(chipId,NULL, NULL); //注册临界区函数
    /* Chip selection call back */
#if   _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_VDM_
    reg_wizchip_cs_cbfunc(chipId,W5500_1_ChipSelect, W5500_1_ChipDeselect);//注册SPI片选信号函数
#elif _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_FDM_
    reg_wizchip_cs_cbfunc(chipId,W5500_1_ChipSelect, W5500_1_ChipSelect);  // CS must be tried with LOW.
#else
    reg_wizchip_cs_cbfunc(chipId,NULL, NULL);
#endif
    /* SPI Read & Write callback function */
    //reg_wizchip_spi_cbfunc(chipId,W5500_1_ReadByte, W5500_1_WriteByte);   //注册读写函数
    reg_wizchip_spiburst_cbfunc(chipId,W5500_1_ReadBurst,W5500_1_WriteBurst);

#if 0
    while(0)
    //(for debug spi reading and writing.)
    {
        /* Network initialization */
        W5xxxNet_NetworkInit(chipId);
        osDelay(3000);
    }
#endif

    /* WIZCHIP SOCKET Buffer initialize */
    if(ctlwizchip(chipId,CW_INIT_WIZCHIP,(void*)memsize) == -1)
    {
        DEBUG(3,"WIZCHIP Initialized fail.\r\n");
        while(1);
    }
#if 0
    /* PHY link status check */
    do{
        if(ctlwizchip(chipId,CW_GET_PHYLINK, (void*)&tmp) == -1)
        {
            DEBUG(3,"Unknown PHY Link stauts.\r\n");
        }
        if(tmp == PHY_LINK_OFF)
        {
            DEBUG(3,"PHY Link is OFF.\r\n");
        }
        else
        {
            DEBUG(3,"PHY Link ON.\r\n");
        }
        osDelay(2000);
    }while(tmp == PHY_LINK_OFF);
#endif
    /*******************************/
    /* WIZnet W5500 Code Examples  */
    /* TCPS/UDPS Loopback test     */
    /*******************************/
    /* Main loop */
}


wiz_NetInfo* W5xxxNet1_GetConfig(void)
/* 将网卡配置信息读出来 */
{
    uint32_t chipId=CHIP1ID;
    wiz_NetInfo* pNetInfor = netInfor+chipId;
    return pNetInfor;
}

void W5xxxNet1_LoadConfig(void)
/* 从环境变量中读出网卡配置信息保存起来,以便后续设置到网卡中 */
{
    uint32_t chipId=CHIP1ID;
    wiz_NetInfo* pNetInfor = netInfor+chipId;
    char *str;
    struct networkInfo* pNetInf= (struct networkInfo*)(netInf+chipId);

#if 1
    str = getenv(MAC1);
    if(str == NULL)
    {
        netMAC_aton(pNetInf->ni_pMac, pNetInfor->mac);
    }
    else
    {
        netMAC_aton(str, pNetInfor->mac);
    }

    str = getenv(IP1);
    if(str == NULL)
    {
        netIP_aton(pNetInf->ni_pIp, NET_ADDR_IP4, pNetInfor->ip);
    }
    else
    {
        netIP_aton(str, NET_ADDR_IP4, pNetInfor->ip);
    }

    str = getenv(NETMASK1);
    if(str == NULL)
    {
        netIP_aton(pNetInf->ni_pMask, NET_ADDR_IP4, pNetInfor->sn);
    }
    else
    {
        netIP_aton(str, NET_ADDR_IP4, pNetInfor->sn);
    }

    str = getenv(GATEWAY1);
    if(str == NULL)
    {
        netIP_aton(pNetInf->ni_pGateway, NET_ADDR_IP4, pNetInfor->gw);
    }
    else
    {
        netIP_aton(str, NET_ADDR_IP4, pNetInfor->gw);
    }
    
    str = getenv(DNS1);
    if(str == NULL)
    {
        netIP_aton(pNetInf->ni_pDns, NET_ADDR_IP4, pNetInfor->dns);
    }
    else
    {
        netIP_aton(str, NET_ADDR_IP4, pNetInfor->dns);
    }
#else
    netMAC_aton(pNetInf->ni_pMac, pNetInf->mac);
    netIP_aton(pNetInf->ni_pIp, NET_ADDR_IP4, pNetInfor->ip);
    netIP_aton(pNetInf->ni_pMask, NET_ADDR_IP4, pNetInfor->sn);
    netIP_aton(pNetInf->ni_pGateway, NET_ADDR_IP4, pNetInfor->gw);
    netIP_aton(pNetInf->ni_pDns, NET_ADDR_IP4, pNetInfor->dns);
#endif
}


void W5xxxNet1_NetworkInit(void)
/* step3:进行本地网络配置 */
{
    uint32_t chipId=CHIP1ID;

    /* Network initialization */
    W5xxxNet_NetworkInit(chipId,netInfor+chipId);
}

void W5xxxNet1_Init(void)
{
    W5xxxNet1_CommunicationPortInit();
    W5xxxNet1_ChipInitialise();
    W5xxxNet1_LoadConfig();
    W5xxxNet1_NetworkInit();
}

#endif


