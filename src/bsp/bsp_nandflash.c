#include "stm32f4xx_hal.h"
//#include "NAND_MemBus_Config.h"

#if (NAND_DEV0_RB_PIN)

#define BUSY_STATE  GPIO_PIN_RESET
/**
  \fn          int32_t Driver_NANDx_GetDeviceBusy (uint32_t dev_num)
  \brief       NAND Driver GetDeviceBusy callback.
               Needs to be implemented by user.
  \param[in]   dev_num   Device number
  \return      1=busy, 0=not busy, or error
*/
int32_t Driver_NAND0_GetDeviceBusy (uint32_t dev_num)
{
    if(HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_6) == BUSY_STATE)
    {
        return 1;
    }

    return 0;
}

#endif
