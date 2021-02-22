#include "stdint.h"
#include "stdio.h"
#include "wizchip_conf.h"
#include "stm32f4xx_hal_conf.h"
//#include "spi_gpio.h"
#include "Driver_SPI.h"
#include "cmsis_os.h"
#include "loopback.h"
//#include "debug.h"

#define W5500_DEBUG_ENABLE 0

#if W5500_DEBUG_ENABLE
#define W5500_DEBUG printf
#else
#define W5500_DEBUG(...)
#endif

#if 1 /* spi1 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define PORT1_SOCK_TCPS        0
#define PORT1_SOCK_UDPS        1
#define PORT1_DATA_BUF_SIZE   2048
/* Private macro -------------------------------------------------------------*/
uint8_t gPORT1_DATABUF[DATA_BUF_SIZE];

// Default Network Configuration
wiz_NetInfo gPORT1_WIZNETINFO = { .mac = {0x00, 0x08, 0xdc,0x00, 0xab, 0xcd},
                            .ip = {192, 168, 1, 10},
                            .sn = {255,255,255,0},
                            .gw = {192, 168, 1, 1},
                            .dns = {0,0,0,0},
                            .dhcp = NETINFO_STATIC };

wiz_NetInfo PORT1_NetInforReadOut;

/* Private variables ---------------------------------------------------------*/
#if 0
struct spiPinType spi_2=
{
.spi_mosi ={GPIOB,GPIO_PIN_15},
.spi_miso ={GPIOB,GPIO_PIN_14},
.spi_cs   ={GPIOB,GPIO_PIN_12},
.spi_sclk ={GPIOB,GPIO_PIN_13},
.spi_mode = SPI_MODE_0,
};
#endif
extern ARM_DRIVER_SPI Driver_SPI1;

ARM_DRIVER_SPI* pDriverSpi1 = &Driver_SPI1;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


void W5500_1_ChipSelect(void)
{
    //Spi_ChipSelect(&spi_2);
    ARM_DRIVER_SPI *spi_drv = pDriverSpi1;
    spi_drv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_ACTIVE);
}

void W5500_1_ChipDeselect(void)
{
    //Spi_ChipDeselect(&spi_2);
    ARM_DRIVER_SPI *spi_drv = pDriverSpi1;
    spi_drv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
}



void W5500_1_ReadBurst(uint8_t* pBuf, uint16_t len)
{
    //Spi_TransferBytes(&spi_2,NULL,0,pBuf,len);
    ARM_DRIVER_SPI *spi_drv = pDriverSpi1;
    spi_drv->Receive(pBuf,len);
    while(spi_drv->GetDataCount() != len);
}
void W5500_1_WriteBurst(uint8_t* pBuf, uint16_t len)
{
    //Spi_TransferBytes(&spi_2,pBuf,len,NULL,0);
    ARM_DRIVER_SPI *spi_drv = pDriverSpi1;
    spi_drv->Send(pBuf,len);
    while(spi_drv->GetDataCount() != len);
}


void W5500_1_CommunicationPortInit(void)
{
    #if 0/* gpio simulate spi */
    GpioSpi_Init3(&spi_2);
    #endif

    #if 1
    /* spi init is in bsp.c */
    #endif
}


/**
  * @brief  Intialize the network information to be used in WIZCHIP
  * @retval None
  */
void W5500_1_Network_Init(uint32_t chipId)
{
   uint8_t tmpstr[6];
	ctlnetwork(chipId,CN_SET_NETINFO, (void*)&gPORT1_WIZNETINFO);
	ctlnetwork(chipId,CN_GET_NETINFO, (void*)&PORT1_NetInforReadOut);

	// Display Network Information
	ctlwizchip(chipId,CW_GET_ID,(void*)tmpstr);
	W5500_DEBUG("\r\n=== %s NET CONF ===\r\n",(char*)tmpstr);
	W5500_DEBUG("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",PORT1_NetInforReadOut.mac[0],PORT1_NetInforReadOut.mac[1],PORT1_NetInforReadOut.mac[2],
		  PORT1_NetInforReadOut.mac[3],PORT1_NetInforReadOut.mac[4],PORT1_NetInforReadOut.mac[5]);
	W5500_DEBUG("SIP: %d.%d.%d.%d\r\n", PORT1_NetInforReadOut.ip[0],PORT1_NetInforReadOut.ip[1],PORT1_NetInforReadOut.ip[2],PORT1_NetInforReadOut.ip[3]);
	W5500_DEBUG("GAR: %d.%d.%d.%d\r\n", PORT1_NetInforReadOut.gw[0],PORT1_NetInforReadOut.gw[1],PORT1_NetInforReadOut.gw[2],PORT1_NetInforReadOut.gw[3]);
	W5500_DEBUG("SUB: %d.%d.%d.%d\r\n", PORT1_NetInforReadOut.sn[0],PORT1_NetInforReadOut.sn[1],PORT1_NetInforReadOut.sn[2],PORT1_NetInforReadOut.sn[3]);
	W5500_DEBUG("DNS: %d.%d.%d.%d\r\n", PORT1_NetInforReadOut.dns[0],PORT1_NetInforReadOut.dns[1],PORT1_NetInforReadOut.dns[2],PORT1_NetInforReadOut.dns[3]);
	W5500_DEBUG("======================\r\n");
}


void W5500_1_Thread(const void* pPara)
{
    uint32_t chipId=W5XX_CHIP_INDEX_0;
	uint8_t tmp;
	int32_t ret = 0;
	uint8_t memsize[2][8] = {{2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};	

    W5500_1_CommunicationPortInit();
    
	reg_wizchip_iomode_init(chipId);
	//Host dependent peripheral initialized
	// First of all, Should register SPI callback functions implemented by user for accessing WIZCHIP 
	/* Critical section callback */
	reg_wizchip_cris_cbfunc(chipId,NULL, NULL);	//注册临界区函数
	/* Chip selection call back */
#if   _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_VDM_
	reg_wizchip_cs_cbfunc(chipId,W5500_1_ChipSelect, W5500_1_ChipDeselect);//注册SPI片选信号函数
#elif _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_FDM_
	reg_wizchip_cs_cbfunc(chipId,W5500_1_ChipSelect, W5500_1_ChipSelect);  // CS must be tried with LOW.
#else
    reg_wizchip_cs_cbfunc(chipId,NULL, NULL);
#endif
	/* SPI Read & Write callback function */
	//reg_wizchip_spi_cbfunc(chipId,W5500_1_ReadByte, W5500_1_WriteByte);	//注册读写函数
    reg_wizchip_spiburst_cbfunc(chipId,W5500_1_ReadBurst,W5500_1_WriteBurst);



	/* WIZCHIP SOCKET Buffer initialize */
	if(ctlwizchip(chipId,CW_INIT_WIZCHIP,(void*)memsize) == -1){
		 W5500_DEBUG("WIZCHIP Initialized fail.\r\n");
		 while(1);
	}

	/* PHY link status check */
	do{
		 if(ctlwizchip(chipId,CW_GET_PHYLINK, (void*)&tmp) == -1){
				W5500_DEBUG("Unknown PHY Link stauts.\r\n");
		 }
		 if(tmp == PHY_LINK_OFF)
		 {
             W5500_DEBUG("PHY Link is OFF.\r\n");
		 }
		 else
		 {
             W5500_DEBUG("PHY Link ON.\r\n");
		 }
		 osDelay(2000);
	}while(tmp == PHY_LINK_OFF);

	/* Network initialization */
	W5500_1_Network_Init(chipId);

	/*******************************/
	/* WIZnet W5500 Code Examples  */
	/* TCPS/UDPS Loopback test     */
	/*******************************/
  /* Main loop */
	while(1)
	{
		/* Loopback Test */
		// TCP server loopback test
		if( (ret = loopback_tcps(chipId,PORT1_SOCK_TCPS, gPORT1_DATABUF, 5000)) < 0) {
			W5500_DEBUG("SOCKET ERROR : %ld\r\n", ret);
		}
    // UDP server loopback test
		if( (ret = loopback_udps(chipId,PORT1_SOCK_UDPS, gPORT1_DATABUF, 3000)) < 0) {
			W5500_DEBUG("SOCKET ERROR : %ld\r\n", ret);
		}
		#if 0 /* 用于检测网线拔出,当检测到网线被拔出时断开已建立的tcp连接。 */
        do{
            if(ctlwizchip(chipId,CW_GET_PHYLINK, (void*)&tmp) == -1){
                W5500_DEBUG("Unknown PHY Link stauts.\r\n");
            }
            if(tmp == PHY_LINK_OFF)
            {
                W5500_DEBUG("PHY Link is OFF.\r\n");
                if(getSn_SR(chipId,PORT1_SOCK_TCPS) == SOCK_ESTABLISHED)
                {
                    w5xx_disconnect(chipId,PORT1_SOCK_TCPS);
                }
            }
            else
            {
                W5500_DEBUG("PHY Link ON.\r\n");
            }
            osDelay(2000);
        }while(tmp == PHY_LINK_OFF);
        #endif
        //osDelay(10);
	} // end of Main loop
}

#endif


#if 1 /* spi2 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define PORT2_SOCK_TCPS        0
#define PORT2_SOCK_UDPS        1
#define PORT2_DATA_BUF_SIZE   2048
/* Private macro -------------------------------------------------------------*/
uint8_t gPORT2_DATABUF[PORT2_DATA_BUF_SIZE];

// Default Network Configuration
wiz_NetInfo gPORT2_WIZNETINFO = { .mac = {0x00, 0x08, 0xdc,0x00, 0xab, 0xcd},
                            .ip = {192, 168, 1, 10},
                            .sn = {255,255,255,0},
                            .gw = {192, 168, 1, 1},
                            .dns = {0,0,0,0},
                            .dhcp = NETINFO_STATIC };

wiz_NetInfo PORT2_NetInforReadOut;


extern ARM_DRIVER_SPI Driver_SPI2;

ARM_DRIVER_SPI* pDriverSpi2 = &Driver_SPI2;


void W5500_2_ChipSelect(void)
{
    //Spi_ChipSelect(&spi_2);
    ARM_DRIVER_SPI *spi_drv = pDriverSpi2;
    spi_drv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_ACTIVE);
}

void W5500_2_ChipDeselect(void)
{
    //Spi_ChipDeselect(&spi_2);
    ARM_DRIVER_SPI *spi_drv = pDriverSpi2;
    spi_drv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
}


void W5500_2_ReadBurst(uint8_t* pBuf, uint16_t len)
{
    //Spi_TransferBytes(&spi_2,NULL,0,pBuf,len);
    ARM_DRIVER_SPI *spi_drv = pDriverSpi2;
    spi_drv->Receive(pBuf,len);
    while(spi_drv->GetDataCount() != len);
}
void W5500_2_WriteBurst(uint8_t* pBuf, uint16_t len)
{
    //Spi_TransferBytes(&spi_2,pBuf,len,NULL,0);
    ARM_DRIVER_SPI *spi_drv = pDriverSpi2;
    spi_drv->Send(pBuf,len);
    while(spi_drv->GetDataCount() != len);
}


void W5500_2_CommunicationPortInit(void)
{
    #if 0/* gpio simulate spi */
    GpioSpi_Init3(&spi_2);
    #endif

    #if 1
    /* spi init is in bsp.c */
    #endif
}


/**
  * @brief  Intialize the network information to be used in WIZCHIP
  * @retval None
  */
void W5500_2_Network_Init(uint32_t chipId)
{
   uint8_t tmpstr[6];
	ctlnetwork(chipId,CN_SET_NETINFO, (void*)&gPORT2_WIZNETINFO);
	ctlnetwork(chipId,CN_GET_NETINFO, (void*)&PORT2_NetInforReadOut);

	// Display Network Information
	ctlwizchip(chipId,CW_GET_ID,(void*)tmpstr);
	W5500_DEBUG("\r\n=== %s NET CONF ===\r\n",(char*)tmpstr);
	W5500_DEBUG("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",PORT2_NetInforReadOut.mac[0],PORT2_NetInforReadOut.mac[1],PORT2_NetInforReadOut.mac[2],
		  PORT2_NetInforReadOut.mac[3],PORT2_NetInforReadOut.mac[4],PORT2_NetInforReadOut.mac[5]);
	W5500_DEBUG("SIP: %d.%d.%d.%d\r\n", PORT2_NetInforReadOut.ip[0],PORT2_NetInforReadOut.ip[1],PORT2_NetInforReadOut.ip[2],PORT2_NetInforReadOut.ip[3]);
	W5500_DEBUG("GAR: %d.%d.%d.%d\r\n", PORT2_NetInforReadOut.gw[0],PORT2_NetInforReadOut.gw[1],PORT2_NetInforReadOut.gw[2],PORT2_NetInforReadOut.gw[3]);
	W5500_DEBUG("SUB: %d.%d.%d.%d\r\n", PORT2_NetInforReadOut.sn[0],PORT2_NetInforReadOut.sn[1],PORT2_NetInforReadOut.sn[2],PORT2_NetInforReadOut.sn[3]);
	W5500_DEBUG("DNS: %d.%d.%d.%d\r\n", PORT2_NetInforReadOut.dns[0],PORT2_NetInforReadOut.dns[1],PORT2_NetInforReadOut.dns[2],PORT2_NetInforReadOut.dns[3]);
	W5500_DEBUG("======================\r\n");
}


void W5500_2_Thread(const void* pPara)
{
    uint32_t chipId=W5XX_CHIP_INDEX_1;
	uint8_t tmp;
	int32_t ret = 0;
	uint8_t memsize[2][8] = {{2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};	

    W5500_2_CommunicationPortInit();
    
	reg_wizchip_iomode_init(chipId);
	//Host dependent peripheral initialized
	// First of all, Should register SPI callback functions implemented by user for accessing WIZCHIP 
	/* Critical section callback */
	reg_wizchip_cris_cbfunc(chipId,NULL, NULL);	//注册临界区函数
	/* Chip selection call back */
#if   _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_VDM_
	reg_wizchip_cs_cbfunc(chipId,W5500_2_ChipSelect, W5500_2_ChipDeselect);//注册SPI片选信号函数
#elif _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_FDM_
	reg_wizchip_cs_cbfunc(chipId,W5500_2_ChipSelect, W5500_2_ChipSelect);  // CS must be tried with LOW.
#else
    reg_wizchip_cs_cbfunc(chipId,NULL, NULL);
#endif
	/* SPI Read & Write callback function */
	//reg_wizchip_spi_cbfunc(chipId,W5500_1_ReadByte, W5500_1_WriteByte);	//注册读写函数
    reg_wizchip_spiburst_cbfunc(chipId,W5500_2_ReadBurst,W5500_2_WriteBurst);


	/* WIZCHIP SOCKET Buffer initialize */
	if(ctlwizchip(chipId,CW_INIT_WIZCHIP,(void*)memsize) == -1){
		 W5500_DEBUG("WIZCHIP Initialized fail.\r\n");
		 while(1);
	}

	/* PHY link status check */
	do{
		 if(ctlwizchip(chipId,CW_GET_PHYLINK, (void*)&tmp) == -1){
				W5500_DEBUG("Unknown PHY Link stauts.\r\n");
		 }
		 if(tmp == PHY_LINK_OFF)
		 {
             W5500_DEBUG("PHY Link is OFF.\r\n");
		 }
		 else
		 {
             W5500_DEBUG("PHY Link ON.\r\n");
		 }
		 osDelay(2000);
	}while(tmp == PHY_LINK_OFF);

	/* Network initialization */
	W5500_2_Network_Init(chipId);

	/*******************************/
	/* WIZnet W5500 Code Examples  */
	/* TCPS/UDPS Loopback test     */
	/*******************************/
  /* Main loop */
	while(1)
	{
		/* Loopback Test */
		// TCP server loopback test
		if( (ret = loopback_tcps(chipId,PORT2_SOCK_TCPS, gPORT2_DATABUF, 5000)) < 0) {
			W5500_DEBUG("SOCKET ERROR : %ld\r\n", ret);
		}
    	// UDP server loopback test
		if( (ret = loopback_udps(chipId,PORT2_SOCK_UDPS, gPORT2_DATABUF, 3000)) < 0) {
			W5500_DEBUG("SOCKET ERROR : %ld\r\n", ret);
		}
		#if 0 /* 用于检测网线拔出,当检测到网线被拔出时断开已建立的tcp连接。 */
        do{
            if(ctlwizchip(chipId,CW_GET_PHYLINK, (void*)&tmp) == -1){
                W5500_DEBUG("Unknown PHY Link stauts.\r\n");
            }
            if(tmp == PHY_LINK_OFF)
            {
                W5500_DEBUG("PHY Link is OFF.\r\n");
                if(getSn_SR(chipId,PORT2_SOCK_TCPS) == SOCK_ESTABLISHED)
                {
                    w5xx_disconnect(chipId,PORT1_SOCK_TCPS);
                }
            }
            else
            {
                W5500_DEBUG("PHY Link ON.\r\n");
            }
            osDelay(2000);
        }while(tmp == PHY_LINK_OFF);
		#endif
		//osDelay(10);
	} // end of Main loop
}

#endif

