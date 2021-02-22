/* ----------------------------------------------------------------------
 * Copyright (C) 2013-2015 ARM Limited. All rights reserved.
 *
 * $Date:        16. October 2015
 * $Revision:    V6.1
 *
 * Driver:       Driver_ETH_PHYn (default: Driver_ETH_PHY0)
 * Project:      Ethernet Physical Layer Transceiver (PHY)
 *               Driver for KSZ8081RNA
 * ----------------------------------------------------------------------
 * Use the following configuration settings in the middleware component
 * to connect to this driver.
 *
 *   Configuration Setting                     Value
 *   ---------------------                     -----
 *   Connect to hardware via Driver_ETH_PHY# = n (default: 0)
 * -------------------------------------------------------------------- */

/* History:
 *  Version 6.1
 *    Added driver flow control flags
 *  Version 6.0
 *    Initial release
 */

#include "PHY_General.h"

#define ARM_ETH_PHY_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(6,1) /* driver version */


#ifndef ETH_PHY_NUM
#define ETH_PHY_NUM     0        /* Default driver number */
#endif

#ifndef ETH_PHY_ADDR
#define ETH_PHY_ADDR    0x00     /* Default device address */
#endif

typedef struct phyid_st
{
    uint16_t id1;
    uint16_t id2;
} phyid_t;

static phyid_t phyid_list[] =
{
    {0x0181U, 0xB8A0U},
    {0x0022U, 0x1513U}
};

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
    ARM_ETH_PHY_API_VERSION,
    ARM_ETH_PHY_DRV_VERSION
};

/* Ethernet PHY control structure */
static PHY_CTRL PHY = { 0 };


/**
  \fn          ARM_DRIVER_VERSION GetVersion (void)
  \brief       Get driver version.
  \return      \ref ARM_DRIVER_VERSION
*/
static ARM_DRIVER_VERSION GetVersion (void) {
    return DriverVersion;
}


/**
  \fn          int32_t Initialize (ARM_ETH_PHY_Read_t  fn_read,
                                   ARM_ETH_PHY_Write_t fn_write)
  \brief       Initialize Ethernet PHY Device.
  \param[in]   fn_read   Pointer to \ref ARM_ETH_MAC_PHY_Read
  \param[in]   fn_write  Pointer to \ref ARM_ETH_MAC_PHY_Write
  \return      \ref execution_status
*/
static int32_t Initialize (ARM_ETH_PHY_Read_t fn_read, ARM_ETH_PHY_Write_t fn_write) {

    if ((fn_read == NULL) || (fn_write == NULL)) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    if ((PHY.flags & PHY_INIT) == 0U) {
        /* Register PHY read/write functions. */
        PHY.reg_rd = fn_read;
        PHY.reg_wr = fn_write;

        PHY.bmcr   = 0U;
        PHY.flags  = PHY_INIT;
    }

    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t Uninitialize (void)
  \brief       De-initialize Ethernet PHY Device.
  \return      \ref execution_status
*/
static int32_t Uninitialize (void) {

    PHY.reg_rd = NULL;
    PHY.reg_wr = NULL;
    PHY.bmcr   = 0U;
    PHY.flags  = 0U;

    return ARM_DRIVER_OK;
}

static int check_phy_id(void)
{
    uint16_t id1, id2;
    int i;

    PHY.reg_rd(ETH_PHY_ADDR, REG_PHYIDR1, &id1);
    PHY.reg_rd(ETH_PHY_ADDR, REG_PHYIDR2, &id2);

    for(i = 0; i < sizeof(phyid_list) / sizeof(phyid_list[0]); i++)
    {
        if(id1 == phyid_list[i].id1 && id2 == phyid_list[i].id2)
        {
            return 0;
        }
    }

    return 1;
}

/**
  \fn          int32_t PowerControl (ARM_POWER_STATE state)
  \brief       Control Ethernet PHY Device Power.
  \param[in]   state  Power state
  \return      \ref execution_status
*/
static int32_t PowerControl (ARM_POWER_STATE state) {

    switch (state) {
    case ARM_POWER_OFF:
        if ((PHY.flags & PHY_INIT) == 0U) {
            /* Initialize must provide register access function pointers */
            return ARM_DRIVER_ERROR;
        }

        PHY.flags &= ~PHY_POWER;
        PHY.bmcr   =  BMCR_POWER_DOWN;

        return (PHY.reg_wr(ETH_PHY_ADDR, REG_BMCR, PHY.bmcr));

    case ARM_POWER_FULL:
        if ((PHY.flags & PHY_INIT) == 0U) {
            return ARM_DRIVER_ERROR;
        }
        if (PHY.flags & PHY_POWER) {
            return ARM_DRIVER_OK;
        }

        /* Check Device Identification. */
        if (check_phy_id()) {
            /* Invalid PHY ID */
            return ARM_DRIVER_ERROR_UNSUPPORTED;
        }

        PHY.bmcr = 0U;

        if (PHY.reg_wr(ETH_PHY_ADDR, REG_BMCR, PHY.bmcr) != ARM_DRIVER_OK) {
            return ARM_DRIVER_ERROR;
        }

        PHY.flags |=  PHY_POWER;

        return ARM_DRIVER_OK;

    case ARM_POWER_LOW:
    default:
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
}

/**
  \fn          int32_t SetInterface (uint32_t interface)
  \brief       Set Ethernet Media Interface.
  \param[in]   interface  Media Interface type
  \return      \ref execution_status
*/
static int32_t SetInterface (uint32_t interface) {
    int32_t status;

    if ((PHY.flags & PHY_POWER) == 0U) {
        return ARM_DRIVER_ERROR;
    }

    /* This device only supports RMII interface */
    switch (interface) {
    case ARM_ETH_INTERFACE_RMII:
        status = ARM_DRIVER_OK;
        break;
    default:
        status = ARM_DRIVER_ERROR_UNSUPPORTED;
        break;
    }

    return (status);
}

/**
  \fn          int32_t SetMode (uint32_t mode)
  \brief       Set Ethernet PHY Device Operation mode.
  \param[in]   mode  Operation Mode
  \return      \ref execution_status
*/
static int32_t SetMode (uint32_t mode) {
    uint16_t val;

    if ((PHY.flags & PHY_POWER) == 0U) {
        return ARM_DRIVER_ERROR;
    }

    val = PHY.bmcr & BMCR_POWER_DOWN;

    switch (mode & ARM_ETH_PHY_SPEED_Msk) {
    case ARM_ETH_PHY_SPEED_10M:
        break;
    case ARM_ETH_PHY_SPEED_100M:
        val |= BMCR_SPEED_SELECT;
        break;
    default:
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    switch (mode & ARM_ETH_PHY_DUPLEX_Msk) {
    case ARM_ETH_PHY_DUPLEX_HALF:
        break;
    case ARM_ETH_PHY_DUPLEX_FULL:
        val |= BMCR_DUPLEX_MODE;
        break;
    default:
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    if (mode & ARM_ETH_PHY_AUTO_NEGOTIATE) {
        val |= BMCR_ANEG_EN;
    }

    if (mode & ARM_ETH_PHY_LOOPBACK) {
        val |= BMCR_LOOPBACK;
    }

    if (mode & ARM_ETH_PHY_ISOLATE) {
        val |= BMCR_ISOLATE;
    }

    PHY.bmcr = val;

    return (PHY.reg_wr(ETH_PHY_ADDR, REG_BMCR, PHY.bmcr));
}

/**
  \fn          ARM_ETH_LINK_STATE GetLinkState (void)
  \brief       Get Ethernet PHY Device Link state.
  \return      current link status \ref ARM_ETH_LINK_STATE
*/
static ARM_ETH_LINK_STATE GetLinkState (void) {
    ARM_ETH_LINK_STATE state;
    uint16_t           val = 0U;

    if (PHY.flags & PHY_POWER) {
        PHY.reg_rd(ETH_PHY_ADDR, REG_BMSR, &val);
    }
    state = (val & BMSR_LINK_STAT) ? ARM_ETH_LINK_UP : ARM_ETH_LINK_DOWN;

    return (state);
}

/**
  \fn          ARM_ETH_LINK_INFO GetLinkInfo (void)
  \brief       Get Ethernet PHY Device Link information.
  \return      current link parameters \ref ARM_ETH_LINK_INFO
*/
static ARM_ETH_LINK_INFO GetLinkInfo (void) {
    ARM_ETH_LINK_INFO info;
    uint16_t          lpa = 0U;
    uint16_t          adv = 0U;

    if (PHY.flags & PHY_POWER) {
        /* Get operation mode indication from PHY Control 1 register */
        PHY.reg_rd(ETH_PHY_ADDR, REG_ANLPAR, &lpa);
        PHY.reg_rd(ETH_PHY_ADDR, REG_ANAR, &adv);
        lpa &= adv;
    }

    /* Link must be up to get valid state */
    info.speed  = (lpa & (ANAR_100B_TX_FD | ANAR_100B_TX_HD)) ? ARM_ETH_SPEED_100M  : ARM_ETH_SPEED_10M;
    info.duplex = (lpa & (ANAR_100B_TX_FD | ANAR_10B_TX_FD)) ? ARM_ETH_DUPLEX_FULL : ARM_ETH_DUPLEX_HALF;

    return (info);
}


/* PHY Driver Control Block */
ARM_DRIVER_ETH_PHY ARM_Driver_ETH_PHY_(ETH_PHY_NUM) = {
    GetVersion,
    Initialize,
    Uninitialize,
    PowerControl,
    SetInterface,
    SetMode,
    GetLinkState,
    GetLinkInfo
};
