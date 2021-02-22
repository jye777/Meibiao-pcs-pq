#include "stm32f4xx_hal.h"
#include "nand.h"
#include <cmsis_os.h>
//#include "NAND_MemBus_Config.h"
#include "Driver_NAND.h"
#include <string.h>
#include "tty.h"

#define _Driver_NAND_(n)                    Driver_NAND##n
#define  Driver_NAND_(n)                   _Driver_NAND_(n)

static uint8_t block_table[256];
extern ARM_DRIVER_NAND Driver_NAND_(NAND_DRIVER);
HAL_StatusTypeDef HAL_NAND_DeInit(NAND_HandleTypeDef *hnand);

#define FSMC_Bank2_3_reg_base   (uint32_t)0xa0000060

void nand_task(const void *pdata);
extern osThreadId tid_nand;

extern FSMC_NAND_InitTypeDef       NandInit;

NAND_InfoTypeDef nand_info = {2048,16384,64,2048,1024};
NAND_HandleTypeDef nand;

HAL_StatusTypeDef NAND_DeInit(NAND_HandleTypeDef *hnand)
{
    /* Initialize the low level hardware (MSP) */
    HAL_NAND_MspDeInit(hnand);

    /* Reset the NAND controller state */
    hnand->State = HAL_NAND_STATE_RESET;

    return HAL_OK;
}

void nand_int(void)
{

    nand.Instance = (FMC_NAND_TypeDef *)FSMC_Bank2_3_reg_base;
    nand.Init     = NandInit;
    nand.Lock     = HAL_LOCKED;
    nand.State    = HAL_NAND_STATE_RESET;
    nand.Info     = nand_info;

    NAND_DeInit(&nand);
    block_table_update(&nand);
}

/****  NAND访问地址转换为连续地址  ***/
uint32_t CycleAddress_to_SerialAddress(NAND_Address *pAddress,NAND_HandleTypeDef *hnand)
{
    uint32_t addr = 0;

    addr  = (pAddress->Zone) * (hnand->Info.ZoneSize) * (hnand->Info.BlockSize) * (hnand->Info.PageSize);
    addr += (pAddress->Block) * (hnand->Info.BlockSize) * (hnand->Info.PageSize);
    addr += (pAddress->Page) * (hnand->Info.PageSize);
    addr +=  pAddress->column;

    return addr;
}

/****  连续地址转换为NAND访问地址 ***/
NAND_Address SerialAddress_to_CycleAddress(uint32_t addr,NAND_HandleTypeDef *hnand)
{
    NAND_Address Address;

    uint32_t blocksize;
    uint32_t pagesize;

    pagesize  = addr / (hnand->Info.PageSize);
    blocksize = pagesize / (hnand->Info.BlockSize);

    Address.column = addr % (hnand->Info.PageSize);
    Address.Page   = pagesize % (hnand->Info.BlockSize);
    Address.Block  = blocksize % (hnand->Info.ZoneSize);
    Address.Zone   = blocksize / (hnand->Info.ZoneSize);

    return Address;
}

uint32_t NAND_Address_Inc(NAND_HandleTypeDef *hnand, NAND_Address *pAddress)
{
    uint32_t status = NAND_VALID_ADDRESS;

    /* Increment page address */
    pAddress->Page++;

    /* Check NAND address is valid */
    if(pAddress->Page == hnand->Info.BlockSize)
    {
        pAddress->Page = 0U;
        pAddress->Block++;

        if(pAddress->Block == hnand->Info.ZoneSize)
        {
            pAddress->Block = 0U;
            pAddress->Zone++;

            if(pAddress->Zone == (hnand->Info.BlockNbr/hnand->Info.ZoneSize))
            {
                status = NAND_INVALID_ADDRESS;
            }
        }
    }

    return (status);
}

/***  N读取AND状态指令  ****/
uint8_t NAND_Read_Status(NAND_HandleTypeDef *hnand)
{
    uint8_t data = 0U;
    uint32_t deviceaddress = 0U;

    /* Identify the device address */
    if(hnand->Init.NandBank == FMC_NAND_BANK2)
    {
        deviceaddress = NAND_DEVICE1;
    }
    else
    {
        deviceaddress = NAND_DEVICE2;
    }

    /* Send Read status operation command */
    *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_STATUS;

    /* Read status register data */
    data = *(__IO uint8_t *)deviceaddress;

    /* Return the status */
    return data;
}

HAL_StatusTypeDef NAND_Erase_Block(NAND_HandleTypeDef *hnand, NAND_Address *pAddress)
{
    uint32_t deviceaddress = 0U;
    uint32_t tickstart = 0U;
    uint32_t rowaddress = 0U;

    /* Check the NAND controller state */
    if(hnand->State == HAL_NAND_STATE_BUSY)
    {
        return HAL_BUSY;
    }

    /* Identify the device address */
    if(hnand->Init.NandBank == FMC_NAND_BANK2)
    {
        deviceaddress = NAND_DEVICE1;
    }
    else
    {
        deviceaddress = NAND_DEVICE2;
    }

    /* Update the NAND controller state */
    hnand->State = HAL_NAND_STATE_BUSY;

    rowaddress = ROWADDR(pAddress);

    /* Send Erase block command sequence */
    *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_ERASE0;

    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_1ST_CYCLE(rowaddress);
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_2ND_CYCLE(rowaddress);
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_3RD_CYCLE(rowaddress);

    *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_ERASE1;

    /* Update the NAND controller state */
    hnand->State = HAL_NAND_STATE_READY;

    /* Get tick */
    tickstart = HAL_GetTick();

    /* Read status until NAND is ready */
    while(HAL_NAND_Read_Status(hnand) != NAND_READY)
    {
        if((HAL_GetTick() - tickstart ) > NAND_TIMEOUT)
        {
            return HAL_TIMEOUT;
        }
				osDelay(1);
    }

    return HAL_OK;
}

/***  以页为单位读  ****/
HAL_StatusTypeDef NAND_Read_Page(NAND_HandleTypeDef *hnand, NAND_Address *pAddress, uint8_t *pBuffer, uint32_t NumPageToRead)
{
    __IO uint32_t index  = 0U;
    uint32_t deviceaddress = 0U, size = 0U, numpagesread = 0U, addressstatus = NAND_VALID_ADDRESS;
    NAND_Address nandaddress;
    uint32_t rowaddress = 0U;

    /* Check the NAND controller state */
    if(hnand->State == HAL_NAND_STATE_BUSY)
    {
        return HAL_BUSY;
    }

    /* Identify the device address */
    if(hnand->Init.NandBank == FMC_NAND_BANK2)
    {
        deviceaddress = NAND_DEVICE1;
    }
    else
    {
        deviceaddress = NAND_DEVICE2;
    }

    /* Update the NAND controller state */
    hnand->State = HAL_NAND_STATE_BUSY;

    /* Save the content of pAddress as it will be modified */
    nandaddress.Block     = pAddress->Block;
    nandaddress.Page      = pAddress->Page;
    nandaddress.Zone      = pAddress->Zone;

    /* Page(s) read loop */
    while((NumPageToRead != 0U) && (addressstatus == NAND_VALID_ADDRESS))
    {
        /* update the buffer size */
        size = hnand->Info.PageSize + ((hnand->Info.PageSize) * numpagesread);

        /* Get the row address */
        rowaddress = ROWADDR(&nandaddress);
//    rowaddress = ARRAY_ADDRESS(&nandaddress, hnand);
        /* Send read page command sequence */
        *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_AREA_A;
        *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = 0x00;
        *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = 0x00;
        *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_1ST_CYCLE(rowaddress);
        *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_2ND_CYCLE(rowaddress);
        *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_3RD_CYCLE(rowaddress);
        *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA))  = NAND_CMD_AREA_TRUE1;


        while( HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_6))
        {
            size = size;
        }

        while( HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_6) == 0)
        {
            size = size;
        }
//		for (uint8_t i = 0; i < 20; i++);
        /* Get Data into Buffer */
        for(; index < size; index++)
        {
            *(uint8_t *)pBuffer++ = *(uint8_t *)deviceaddress;
        }

        /* Increment read pages number */
        numpagesread++;

        /* Decrement pages to read */
        NumPageToRead--;

        /* Increment the NAND address */
        addressstatus = NAND_Address_Inc(hnand, &nandaddress);
    }

    /* Update the NAND controller state */
    hnand->State = HAL_NAND_STATE_READY;

    return HAL_OK;
}

/***  NAND 读  ****/
NAND_StatusTypeDef NAND_Read_Data(NAND_HandleTypeDef *hnand, NAND_Address *StartAddress, NAND_Address *EndAddress, uint8_t *pBuffer, uint32_t NumByteToRead)
{
    __IO uint32_t index   = 0U;
    uint32_t deviceaddress = 0U, size = 0U, addressstatus = NAND_VALID_ADDRESS;
    NAND_Address nandaddress;
    uint32_t rowaddress = 0U;
    uint16_t NumPageToRead = 0;
    uint16_t NumPageWillRead = 0;
    uint16_t NumPageHaveRead = 0;

    /**  检查NAND控制器状态  **/
    if(hnand->State == HAL_NAND_STATE_BUSY)
    {
        return NAND_IS_OK;
    }

    /**  初始化NAND设备访问地址（FSMC地址）  **/
    if(hnand->Init.NandBank == FMC_NAND_BANK2)
    {
        deviceaddress = NAND_DEVICE1;
    }
    else
    {
        deviceaddress = NAND_DEVICE2;
    }

    /**  更新NAND控制器状态  **/
    hnand->State = HAL_NAND_STATE_BUSY;

    /**  坏块查询  **/
    if(isbadblock(StartAddress->Block + (StartAddress->Zone) * hnand->Info.ZoneSize))
    {
        StartAddress->Block++;
    }

    /**  地址初始化  **/
    nandaddress.Block     = StartAddress->Block;
    nandaddress.Page      = StartAddress->Page;
    nandaddress.Zone      = StartAddress->Zone;
    nandaddress.column    = StartAddress->column;

    /**  需要编程页数计算  **/
    if(NumByteToRead == 0)
    {
        NumPageToRead = 0;
    }
    else if(NumByteToRead <= hnand->Info.PageSize - StartAddress->column)
    {
        NumPageToRead = 1;
    }
    else if(NumByteToRead - (hnand->Info.PageSize - StartAddress->column) <= hnand->Info.PageSize )
    {
        NumPageToRead = 2;
    }
    else
    {
        NumPageToRead = (NumByteToRead + StartAddress->column - hnand->Info.PageSize -1) / hnand->Info.PageSize + 2;
    }
    NumPageWillRead	= NumPageToRead;

    /**  页写循环  **/
    while((NumPageToRead != 0U) && (addressstatus == NAND_VALID_ADDRESS))
    {
        /**  更新写数据大小  **/
        if(NumPageHaveRead == 0)
        {
            if(NumByteToRead <= hnand->Info.PageSize - StartAddress->column)
            {
                size = NumByteToRead;
            }
            else
            {
                size = hnand->Info.PageSize - StartAddress->column;
            }
        }
        else if(NumPageHaveRead + 1 == NumPageWillRead)
        {
            size = NumByteToRead + StartAddress->column - hnand->Info.PageSize - (hnand->Info.PageSize * (NumPageWillRead - 2));
        }
        else
        {
            size = hnand->Info.PageSize;
        }

        /**  计算 row address  **/
        rowaddress = ROWADDR(&nandaddress);

        /**  写命令时序  **/
        *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_AREA_A;
        *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandaddress.column);
        *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandaddress.column);
        *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_1ST_CYCLE(rowaddress);
        *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_2ND_CYCLE(rowaddress);
        *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_3RD_CYCLE(rowaddress);
        *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA))  = NAND_CMD_AREA_TRUE1;

        /**  等待RB信号  **/
        while( HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_6))
        {
            size = size;
        }

        while( HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_6) == 0)
        {
            size = size;
						osDelay(1);
        }

        /**  从NAND FLASH获取数据  **/
        for(; index < size; index++)
        {
            *(uint8_t *)pBuffer++ = *(uint8_t *)deviceaddress;
						if(size >= 512 && (index%512 == 0))
						{
							 osDelay(1); 
						}
        }
				
        /**  更新地址  **/
        nandaddress.column += size;				
        if(nandaddress.column >= hnand->Info.PageSize)
        {
            nandaddress.Page++;
            nandaddress.column = 0;
            if(nandaddress.Page >= hnand->Info.BlockSize)
            {
                nandaddress.Block++;
								nandaddress.Page = 0;
								nandaddress.column = 0;
                while(isbadblock(nandaddress.Block + (nandaddress.Zone) * hnand->Info.ZoneSize))
                {
                    nandaddress.Block++;
                }

                if(nandaddress.Block >= hnand->Info.ZoneSize)
                {
                    nandaddress.Zone++;
										nandaddress.Block = 0;
										nandaddress.Page = 0;
										nandaddress.column = 0;
                    if(nandaddress.Zone >= 2)
                    {
                        addressstatus = NAND_INVALID_ADDRESS;

                        /**  更新NAND控制器状态  **/
                        hnand->State = HAL_NAND_STATE_READY;

                        return  NAND_ADDRESS_IS_INVALID;
                    }
                }
            }
        }

        /** 已写页数自加  **/
        NumPageHaveRead++;

        /** 剩余需要写的页数自减  **/
        NumPageToRead--;
    }

    /**  获取写结束地址  **/
    EndAddress->column = nandaddress.column;
    EndAddress->Block = nandaddress.Block;
    EndAddress->Page = nandaddress.Page;
    EndAddress->Zone = nandaddress.Zone;

    /**  更新NAND控制器状态  **/
    hnand->State = HAL_NAND_STATE_READY;

    return NAND_IS_OK;
}


/***  以页为单位写  ****/
NAND_StatusTypeDef NAND_Write_Data(NAND_HandleTypeDef *hnand, NAND_Address *StartAddress, NAND_Address *EndAddress, uint8_t *pBuffer, uint32_t NumByteToWrite)
{
    __IO uint32_t index   = 0U;
    uint32_t tickstart = 0U;
    uint32_t deviceaddress = 0U , size = 0U, numpageswritten = 0U, addressstatus = NAND_VALID_ADDRESS;
    NAND_Address nandaddress;
    uint32_t rowaddress = 0U;
    uint16_t NumPageToWrite = 0;
    uint16_t NumPageWillWrite = 0;
    uint8_t blockupdate = 0,wtitestatus = NAND_CACHE_PROGRAM_BUSY ;
    NAND_StatusTypeDef hal_state = NAND_IS_OK;

    /**  检查NAND控制器状态  **/
    if(hnand->State == HAL_NAND_STATE_BUSY)
    {
        return NAND_IS_OK;
    }

    /**  初始化NAND设备访问地址（FSMC地址）  **/
    if(hnand->Init.NandBank == FMC_NAND_BANK2)
    {
        deviceaddress = NAND_DEVICE1;
    }
    else
    {
        deviceaddress = NAND_DEVICE2;
    }

    /**  更新NAND控制器状态  **/
    hnand->State = HAL_NAND_STATE_BUSY;

    /**  坏块查询  **/
    if(isbadblock(StartAddress->Block + (StartAddress->Zone) * hnand->Info.ZoneSize))
    {
        StartAddress->Block++;
    }

    /**  地址初始化  **/
    nandaddress.Block     = StartAddress->Block;
    nandaddress.Page      = StartAddress->Page;
    nandaddress.Zone      = StartAddress->Zone;
    nandaddress.column    = StartAddress->column;

    /**  需要编程页数计算  **/
    if(NumByteToWrite == 0)
    {
        NumPageToWrite = 0;
    }
    else if(NumByteToWrite <= hnand->Info.PageSize - StartAddress->column)
    {
        NumPageToWrite = 1;
    }
    else if(NumByteToWrite - (hnand->Info.PageSize - StartAddress->column) <= hnand->Info.PageSize )
    {
        NumPageToWrite = 2;
    }
    else
    {
        NumPageToWrite = (NumByteToWrite + StartAddress->column - hnand->Info.PageSize -1) / hnand->Info.PageSize + 2;
    }
    NumPageWillWrite	= NumPageToWrite;

    /**  页写循环  **/
    while((NumPageToWrite != 0U) && (addressstatus == NAND_VALID_ADDRESS))
    {
        /**  更新写数据大小  **/
        if(numpageswritten == 0)
        {
            if(NumByteToWrite <= hnand->Info.PageSize - StartAddress->column)
            {
                size = NumByteToWrite;
            }
            else
            {
                size = hnand->Info.PageSize - StartAddress->column;
            }
        }
        else if(numpageswritten + 1 == NumPageWillWrite)
        {
            size = NumByteToWrite + StartAddress->column - hnand->Info.PageSize - (hnand->Info.PageSize * (NumPageWillWrite - 2));
        }
        else
        {
            size = hnand->Info.PageSize;
        }

        /**  计算 row address  **/
        rowaddress = ROWADDR(&nandaddress);

        /**  写命令时序  **/
        *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_WRITE0;
        *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandaddress.column);
        *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandaddress.column);
        *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_1ST_CYCLE(rowaddress);
        *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_2ND_CYCLE(rowaddress);
        *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_3RD_CYCLE(rowaddress);

        /**  写数据到 NAND FLASH  **/
        for(; index < size; index++)
        {
            *(__IO uint8_t *)deviceaddress = *(uint8_t *)pBuffer++;
 						if(size >= 512 && (index%512 == 0))
						{
							 osDelay(1); 
						}
       }

        /** 写数据结束指令或者一个BLOCK写结束 **/
        if((numpageswritten + 1 == NumPageWillWrite) || (blockupdate == 1))
        {
            blockupdate = 0;
            *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_WRITE_TRUE1;
        }
        else
        {
            *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NANDCACHE_CMD_WRITE_TRUE1;
        }

        /**  等待RB信号  **/
        while( HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_6))
        {
            size = size;
        }

        while( HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_6) == 0)
        {
            size = size;
        }

        /**  读取 tick  **/
        tickstart = HAL_GetTick();

        /**  写读取NAND状态指令  **/
        *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_STATUS;

        /**  等待NAND READY  **/
        for(wtitestatus = *(__IO uint8_t *)deviceaddress; (wtitestatus & NAND_CACHE_PROGRAM_READY) == 0; )
        {
            if((HAL_GetTick() - tickstart ) > NAND_TIMEOUT)
            {
                hnand->State = HAL_NAND_STATE_READY;
                hal_state = NAND_IS_TIMEOUT;
								break;
            }
            if((wtitestatus & NAND_CACHE_PROGRAM_N_0_ERR) && (wtitestatus & NAND_CACHE_PROGRAM_COMPLETE))
            {
                hnand->State = HAL_NAND_STATE_READY;

                hal_state = NAND_BLOCK_IS_ERROR; 			  /**  BLOCK出现坏块  **/
								break;
            }
						osDelay(1);
        }

        if((wtitestatus & NAND_CACHE_PROGRAM_N_1_ERR) && (wtitestatus & NAND_CACHE_PROGRAM_READY))
        {
            hnand->State = HAL_NAND_STATE_READY;
            hal_state = NAND_BLOCK_IS_ERROR; 			  /**  BLOCK出现坏块  **/
        }

        /**  错误退出  **/
        if(hal_state != HAL_OK)
        {
            /**  更新NAND控制器状态  **/
            hnand->State = HAL_NAND_STATE_READY;

            return hal_state;
        }

        /** 已写页数自加  **/
        numpageswritten++;

        /** 剩余需要写的页数自减  **/
        NumPageToWrite--;
				
        /**  更新地址  **/
				nandaddress.column += size;			
        if(nandaddress.column >= hnand->Info.PageSize)
        {
            nandaddress.Page++;
            nandaddress.column = 0;
            if(nandaddress.Page >= hnand->Info.BlockSize)
            {
                nandaddress.Block++;
								nandaddress.Page = 0;
								nandaddress.column = 0;
                while(isbadblock(nandaddress.Block + (nandaddress.Zone) * hnand->Info.ZoneSize))
                {
                    nandaddress.Block++;
                }

                NAND_Erase_Block(hnand,&nandaddress);

                if(nandaddress.Block >= hnand->Info.ZoneSize)
                {
                    nandaddress.Zone++;
										nandaddress.Block = 0;
										nandaddress.Page = 0;
										nandaddress.column = 0;
                    if(nandaddress.Zone >= 2)
                    {
                        addressstatus = NAND_INVALID_ADDRESS;

                        /**  更新NAND控制器状态  **/
                        hnand->State = HAL_NAND_STATE_READY;

                        return  NAND_ADDRESS_IS_INVALID;
                    }
                }
            }
        }
    }

    /**  获取写结束地址  **/
    EndAddress->column = nandaddress.column;
    EndAddress->Block = nandaddress.Block;
    EndAddress->Page = nandaddress.Page;
    EndAddress->Zone = nandaddress.Zone;

    /**  更新NAND控制器状态  **/
    hnand->State = HAL_NAND_STATE_READY;

    return NAND_IS_OK;
}


/***  以字节为单位写  ****/
HAL_StatusTypeDef NAND_Write(NAND_HandleTypeDef *hnand, NAND_Address *pAddress, uint8_t *pBuffer, uint32_t NumByteToWrite)
{
    __IO uint32_t index   = 0U;
    uint32_t tickstart = 0U;
    uint32_t deviceaddress = 0U , size = 0U, numpageswritten = 0U, addressstatus = NAND_VALID_ADDRESS;
    NAND_Address nandaddress;
    uint32_t rowaddress = 0U;
    uint16_t NumPageToWrite = 0;
    uint16_t NumPageWillWrite = 0;
    uint8_t blockupdate = 0,wtitestatus = NAND_CACHE_PROGRAM_BUSY ;
    HAL_StatusTypeDef hal_state = HAL_OK;

    /**  检查NAND控制器状态  **/
    if(hnand->State == HAL_NAND_STATE_BUSY)
    {
        return HAL_BUSY;
    }

    /**  初始化NAND设备访问地址（FSMC地址）  **/
    if(hnand->Init.NandBank == FMC_NAND_BANK2)
    {
        deviceaddress = NAND_DEVICE1;
    }
    else
    {
        deviceaddress = NAND_DEVICE2;
    }

    /**  更新NAND控制器状态  **/
    hnand->State = HAL_NAND_STATE_BUSY;

    /**  地址初始化  **/
    nandaddress.Block     = pAddress->Block;
    nandaddress.Page      = pAddress->Page;
    nandaddress.Zone      = pAddress->Zone;
    nandaddress.column    = pAddress->column;

    /**  需要编程页数计算  **/
    if(NumByteToWrite == 0)
    {
        NumPageToWrite = 0;
    }
    else if(NumByteToWrite <= hnand->Info.PageSize - pAddress->column)
    {
        NumPageToWrite = 1;
    }
    else if(NumByteToWrite - (hnand->Info.PageSize - pAddress->column) <= hnand->Info.PageSize )
    {
        NumPageToWrite = 2;
    }
    else
    {
        NumPageToWrite = (NumByteToWrite + pAddress->column - hnand->Info.PageSize -1) / hnand->Info.PageSize + 2;
    }
    NumPageWillWrite	= NumPageToWrite;

    /**  页写循环  **/
    while((NumPageToWrite != 0U) && (addressstatus == NAND_VALID_ADDRESS))
    {
        /**  更新写数据大小  **/
        if(numpageswritten == 0)
        {
            if(NumByteToWrite <= hnand->Info.PageSize - pAddress->column)
            {
                size = NumByteToWrite;
            }
            else
            {
                size = hnand->Info.PageSize - pAddress->column;
            }
        }
        else if(numpageswritten + 1 == NumPageWillWrite)
        {
            size = NumByteToWrite + pAddress->column - hnand->Info.PageSize - (hnand->Info.PageSize * NumPageWillWrite - 2);
        }
        else
        {
            size = hnand->Info.PageSize;
        }

        /**  计算 row address  **/
        rowaddress = ROWADDR(&nandaddress);

        /**  写命令时序  **/
        *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_WRITE0;
        *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandaddress.column);
        *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandaddress.column);
        *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_1ST_CYCLE(rowaddress);
        *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_2ND_CYCLE(rowaddress);
        *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_3RD_CYCLE(rowaddress);

        /**  写数据到 NAND FLASH  **/
        for(; index < size; index++)
        {
            *(__IO uint8_t *)deviceaddress = *(uint8_t *)pBuffer++;
        }

        /** 写数据结束指令或者一个BLOCK写结束 **/
        if((numpageswritten + 1 == NumPageWillWrite) || (blockupdate == 1))
        {
            blockupdate = 0;
            *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_WRITE_TRUE1;
        }
        else
        {
            *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NANDCACHE_CMD_WRITE_TRUE1;
        }

        /**  等待RB信号  **/
        while( HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_6))
        {
            size = size;
        }

        while( HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_6) == 0)
        {
            size = size;
        }

        /**  读取 tick  **/
        tickstart = HAL_GetTick();

        /**  写读取NAND状态指令  **/
        *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_STATUS;

        /**  等待NAND READY  **/
        for(wtitestatus = *(__IO uint8_t *)deviceaddress; (wtitestatus & NAND_CACHE_PROGRAM_READY) == 0; )
        {
            if((HAL_GetTick() - tickstart ) > NAND_TIMEOUT)
            {
                hnand->State = HAL_NAND_STATE_READY;
                hal_state = HAL_TIMEOUT;
								break;
            }
            if((wtitestatus & NAND_CACHE_PROGRAM_N_0_ERR) && (wtitestatus & NAND_CACHE_PROGRAM_COMPLETE))
            {
                hnand->State = HAL_NAND_STATE_READY;

                hal_state = HAL_TIMEOUT; 			  /**  BLOCK出现坏块  **/
								break;
            }
						osDelay(1);
        }

        if((wtitestatus & NAND_CACHE_PROGRAM_N_1_ERR) && (wtitestatus & NAND_CACHE_PROGRAM_READY))
        {
            hnand->State = HAL_NAND_STATE_READY;
            hal_state = HAL_TIMEOUT; 			  /**  BLOCK出现坏块  **/
        }

        /**  错误推出  **/
        if(hal_state != HAL_OK)
        {
            /**  更新NAND控制器状态  **/
            hnand->State = HAL_NAND_STATE_READY;

            return hal_state;
        }

        /** 已写页数自加  **/
        numpageswritten++;

        /** 剩余需要写的页数自减  **/
        NumPageToWrite--;

        /**  更新地址  **/
        nandaddress.Page++;
        nandaddress.column = 0;
        if(nandaddress.Page >= hnand->Info.BlockSize)
        {
            nandaddress.Block++;
            blockupdate = 1;                  /**  BLOCK更新**/
            if(nandaddress.Block >= hnand->Info.ZoneSize)
            {
                nandaddress.Zone++;
                if(nandaddress.Zone >= 2)
                {
                    addressstatus = NAND_INVALID_ADDRESS;

                    /**  更新NAND控制器状态  **/
                    hnand->State = HAL_NAND_STATE_READY;

                    return  HAL_ERROR;
                }
            }
        }
    }

    /**  更新NAND控制器状态  **/
    hnand->State = HAL_NAND_STATE_READY;

    return HAL_OK;
}

HAL_StatusTypeDef NAND_Write_Byte(NAND_HandleTypeDef *hnand, NAND_Address *pAddress, uint8_t *pBuffer, uint32_t NumByteToWrite)
{
    __IO uint32_t index   = 0U;
    uint32_t tickstart = 0U;
    uint32_t deviceaddress = 0U , numpageswritten = 0U;
    NAND_Address nandaddress;
    uint32_t rowaddress = 0U;

    /* Check the NAND controller state */
    if(hnand->State == HAL_NAND_STATE_BUSY)
    {
        return HAL_BUSY;
    }

    /* Identify the device address */
    if(hnand->Init.NandBank == FMC_NAND_BANK2)
    {
        deviceaddress = NAND_DEVICE1;
    }
    else
    {
        deviceaddress = NAND_DEVICE2;
    }

    /* Update the NAND controller state */
    hnand->State = HAL_NAND_STATE_BUSY;

    /* Save the content of pAddress as it will be modified */
    nandaddress.Block     = pAddress->Block;
    nandaddress.Page      = pAddress->Page;
    nandaddress.Zone      = pAddress->Zone;
    nandaddress.column    = pAddress->column;

    /* Get the row address */
    rowaddress = ROWADDR(&nandaddress);

    /* Send write page command sequence */
    *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_WRITE0;

    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandaddress.column);
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandaddress.column);
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_1ST_CYCLE(rowaddress);
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_2ND_CYCLE(rowaddress);
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_3RD_CYCLE(rowaddress);

    /* Write data to memory */
    for(; index < NumByteToWrite; index++)
    {
        *(__IO uint8_t *)deviceaddress = *(uint8_t *)pBuffer++;
    }

    *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_WRITE_TRUE1;

    /* Get tick */
    tickstart = HAL_GetTick();

    /* Read status until NAND is ready */
    while(HAL_NAND_Read_Status(hnand) != NAND_READY)
    {
        if((HAL_GetTick() - tickstart ) > NAND_TIMEOUT)
        {
            /* Update the NAND controller state */
            hnand->State = HAL_NAND_STATE_READY;

            return HAL_TIMEOUT;
        }
				osDelay(1);
    }

    /* Increment written pages number */
    numpageswritten++;

    /* Increment the NAND address */
    NAND_Address_Inc(hnand, &nandaddress);

    /* Update the NAND controller state */
    hnand->State = HAL_NAND_STATE_READY;

    return HAL_OK;
}


/***********************************************************************************************************/
/*************           坏块管理               ************************************************************/
/***********************************************************************************************************/

/****  读取坏块标识  *********************/
uint16_t NAND_ReadBadBlockFlag(NAND_HandleTypeDef *hnand ,uint16_t _ulBlockNo ,uint8_t planeNo)
{
    __IO uint32_t index  = 0U;
    uint32_t deviceaddress = 0U, size = 0U;
    NAND_Address nandaddress;
    uint32_t rowaddress = 0U;
    uint16_t temp ;
    uint8_t count = 0;

    /* Check the NAND controller state */
    if(hnand->State == HAL_NAND_STATE_BUSY)
    {
        return HAL_BUSY;
    }

    /* Identify the device address */
    if(hnand->Init.NandBank == FMC_NAND_BANK2)
    {
        deviceaddress = NAND_DEVICE1;
    }
    else
    {
        deviceaddress = NAND_DEVICE2;
    }

    /* Update the NAND controller state */
    hnand->State = HAL_NAND_STATE_BUSY;

    /* Save the content of pAddress as it will be modified */
    while(count < 2)
    {
        nandaddress.Block     = _ulBlockNo;
        nandaddress.Page      = count;
        nandaddress.Zone      = planeNo;

        /* Get the row address */
        rowaddress = ROWADDR(&nandaddress);
        /* Send read page command sequence */
        *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_AREA_A;
        *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = 0x00;
        *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = 0x08;
        *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_1ST_CYCLE(rowaddress);
        *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_2ND_CYCLE(rowaddress);
        *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_3RD_CYCLE(rowaddress);
        *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA))  = NAND_CMD_AREA_TRUE1;


        while( HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_6))
        {
            size = size;
        }

        while( HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_6) == 0)
        {
            size = size;
        }
        /* Get Data into Buffer */
        if(count == 0)
        {
            temp = *(uint8_t *)deviceaddress;
        }
        else if(count == 1)
        {
            temp |= ((*(uint8_t *)deviceaddress)<<8);
        }
        else
        {
            break;
        }
        count++;
    }

    /* Update the NAND controller state */
    hnand->State = HAL_NAND_STATE_READY;

    return temp;
}

/****  获取坏块标识表  ***********************/
static void block_flag_table_update(NAND_HandleTypeDef *hnand,uint16_t *nand_block_bad)
{
    uint16_t blockno = 0;

    for(blockno = 0; blockno < hnand->Info.BlockNbr; blockno++)
    {
        *(nand_block_bad + blockno) = NAND_ReadBadBlockFlag(hnand,blockno%1024,blockno/1024);
    }
}

/****  坏块表初始化  ***********************/
static uint8_t block_table[256];
static void block_table_update(NAND_HandleTypeDef *hnand)
{
    uint16_t blockno = 0;

    for(blockno = 0; blockno < hnand->Info.BlockNbr; blockno++)
    {
        if(NAND_ReadBadBlockFlag(hnand,blockno%1024,blockno/1024) != 0xffff)
        {
            BITSET(block_table[blockno/8],blockno%8);
        }
        else
        {
            BITCLEAR(block_table[blockno/8],blockno%8);
        }
    }
}

/****  坏块查询  ***********************/
uint8_t isbadblock(uint16_t blockno)
{
    return BITGET(block_table[blockno/8],blockno%8);
}

/****  更新坏块标志 ***********************/
static void badblockupdate(uint16_t blockno ,uint8_t *badblockflag)
{
    NAND_Address	nandaddr;

    nandaddr.Block  = blockno % 1024;
    nandaddr.Page   = 0;
    nandaddr.Zone   = blockno / 1024;
    nandaddr.column = 2048;

    NAND_Write_Byte(&nand,&nandaddr,badblockflag,1);

    nandaddr.Page   = 1;
    NAND_Write_Byte(&nand,&nandaddr,badblockflag+1,1);
	
    BITSET(block_table[blockno/8],blockno%8);
}

/****  页检测  ***********************/

/**  执行该函数前必须保证该页被擦除过及页数据全为0xff  **/
static uint8_t pagecheck(NAND_Address *pAddress,uint8_t data)
{
    HAL_StatusTypeDef status;
    static uint8_t nand_wbuf[2048];
    static uint8_t nand_rbuf[2048];
    uint16_t i = 0;

    memset(nand_wbuf,data,2048);
    pAddress->column = 0;
    status = NAND_Write(&nand,pAddress,nand_wbuf,2048);

    if(status == HAL_ERROR)
    {
        return 1;
    }
    osDelay(1);

    NAND_Read_Page(&nand,pAddress,nand_rbuf,1);

    for(i = 0; i < 2048; i++)
    {
        if(nand_rbuf[i] != data)
        {
            return 1;
        }
    }
    osDelay(1);

    return 0;
}

/****  块检测  ***********************/
static uint8_t blockcheck(NAND_Address *pAddress)
{
    uint8_t i = 0;
    uint16_t badblockflag = 0;
    uint16_t badblockflagclou = 0;

    badblockflagclou = 'c' << 8 | 'l';

    badblockflag = NAND_ReadBadBlockFlag(&nand,pAddress->Block,pAddress->Zone);

    NAND_Erase_Block(&nand,pAddress);
    pAddress->Page = 0;
    for(i = 0; i < 64 ; i++)
    {
        if(pAddress->Page != i)
        {
            i = i;
        }
        if(pagecheck(pAddress,0x55))
        {
            if(badblockflag == 0xffff)
            {
                badblockflag = badblockflagclou;
            }
            badblockupdate(pAddress->Block + pAddress->Zone * 1024,(uint8_t *)&badblockflag);

            return 1;
        }
        pAddress->Page++;
    }

    NAND_Erase_Block(&nand,pAddress);
    pAddress->Page = 0;
    for(i = 0; i < 64 ; i++)
    {
        if(pAddress->Page != i)
        {
            i = i;
        }
        if(pagecheck(pAddress,0xaa))
        {
            if(badblockflag == 0xffff)
            {
                badblockflag = badblockflagclou;
            }
            badblockupdate(pAddress->Block + pAddress->Zone * 1024,(uint8_t *)&badblockflag);

            return 1;
        }
        pAddress->Page++;
    }

    return 0;
}

extern osSemaphoreId nand_sem;
/****  格式化NAND ***********************/
void format(void)
{
    uint16_t blockno = 0;
    float temp;
    NAND_Address Address = {0,0,0,0};
    osSemaphoreWait(nand_sem, osWaitForever);

    for(blockno = 0; blockno < nand.Info.BlockNbr ; blockno++)
    {
        if(blockno == 0x036f)
        {
            temp = temp;
        }
        Address.Block = blockno % 1024;
        Address.Zone = blockno / 1024;
        if(isbadblock(blockno) == 0)
				{
            NAND_Erase_Block(&nand,&Address);
        }
        osDelay(1);
    }
 		osSemaphoreRelease(nand_sem);
		tty_printf("\n");
}

void format_all(void)
{
    uint16_t blockno = 0;
    float temp;
    NAND_Address Address = {0,0,0,0};
    osSemaphoreWait(nand_sem, osWaitForever);

    for(blockno = 0; blockno < nand.Info.BlockNbr ; blockno++)
    {
        if(blockno == 0x036f)
        {
            temp = temp;
        }
        Address.Block = blockno % 1024;
        Address.Zone = blockno / 1024;
        NAND_Erase_Block(&nand,&Address);
        osDelay(1);
    }
 		osSemaphoreRelease(nand_sem);
}


/****  BAD BLOCK reint ******************/
void badblockreint(void)
{
    NAND_Address	nandaddr = {0,0,0,0};
    uint16_t i = 0;

    osSemaphoreWait(nand_sem, osWaitForever);
    for(i = 0 ; i <nand.Info.BlockNbr; i++)
    {
        nandaddr.Block = i % nand.Info.ZoneSize;
        nandaddr.Zone  = i / nand.Info.ZoneSize;
        blockcheck(&nandaddr);
    }

    block_table_update(&nand);
 		osSemaphoreRelease(nand_sem);
}

/**  文件信息区坏块管理  **/
uint32_t get_filedat_block(uint32_t addr)
{
    uint8_t  i = 0;
    NAND_Address	nandaddr;

    nandaddr = SerialAddress_to_CycleAddress(addr,&nand);

    for(i = 0;i < 30;i++)
		{
				if(isbadblock(nandaddr.Block + nandaddr.Zone * nand.Info.ZoneSize))
				{
						nandaddr.Block++;
				}
				else
				{
						addr = CycleAddress_to_SerialAddress(&nandaddr,&nand);
						break;
				}
		}
		if( i == 30)
		{
				printf("nand文件地址信息区域全部损坏\n");
				addr = 0xffffffff;
		}
		return addr;
}

bool skip_bad_block(FILE_STRUCT *f,NAND_Address *pnandaddr,NAND_HandleTypeDef *hnand)
{	
		NAND_Address addrstart;
		uint32_t addr;
	
		addrstart = SerialAddress_to_CycleAddress(f->fstartaddr,hnand);
		addr = CycleAddress_to_SerialAddress(pnandaddr,hnand);
	
		while(isbadblock(pnandaddr->Block + (pnandaddr->Zone) * nand.Info.ZoneSize))
		{								
				addr += 0x20000;
				if(addr >= f->faddrmax)																				//该文件分配存储空间最后一块block
				{
						addr = f->faddrmin;
				}
				*pnandaddr = SerialAddress_to_CycleAddress(addr,hnand);
			
				if((pnandaddr->Block == addrstart.Block) && (pnandaddr->Zone == addrstart.Zone))
				{
						f->funused = 0xaa;
						return false;																															//该文件分配存储空间全部损坏
				}
		}
						
		return true;
}

void get_newstartaddr(FILE_STRUCT *f , NAND_HandleTypeDef *hnand)
{
	
		NAND_Address startaddrc,startaddrc_temp;
		uint32_t startaddrs;
		uint16_t badblocknum = 0,nomalblocknum = 0,file_bolcksize = 0;
	
		startaddrs = f->fstartaddr;
		startaddrc = SerialAddress_to_CycleAddress(startaddrs,hnand);
		startaddrc_temp	= startaddrc;
		file_bolcksize = (f->fsize - 1) / ((float)0x20000) + 1;
	
		while(1)
		{
				if(isbadblock(startaddrc.Block + (startaddrc.Zone) * nand.Info.ZoneSize))
				{
						badblocknum++;
				}
				else
				{
						nomalblocknum++;
						if(nomalblocknum >= file_bolcksize)
						{
								break;
						}
				}
				
				startaddrs += 0x20000;				
				if(startaddrs >= f->faddrmax)																				//该文件分配存储空间最后一块block
				{
						startaddrs = f->faddrmin;
				}
				
				startaddrc = SerialAddress_to_CycleAddress(startaddrs,hnand);
		
				if((startaddrc.Block == startaddrc_temp.Block) && (startaddrc.Zone == startaddrc_temp.Zone))
				{
						f->funused = 0xaa;
						return;																													//该文件分配存储空间全部损坏
				}
		}	
		
		f->fnewstartaddr = f->fstartaddr + badblocknum * 0x20000 + f->fsizemax;
		
		if(f->fnewstartaddr >= f->faddrmax)
		{
				f->fnewstartaddr = f->faddrmin + (f->fnewstartaddr - f->faddrmax);
		}
}

bool set_readnum(FILE_STRUCT *f , uint32_t readnum)
{
	
		NAND_Address curaddrc,curaddrc_temp;
		uint32_t curaddrs;
		uint16_t badblocknum = 0, nomalblocknum = 0,readnum_bolcksize = 0;
	
	
		if((readnum >= f->fsize) || (readnum + f->fstartaddr > f->faddrmax))
		{
				return false;
		}
		
		f->freadnum = readnum;
		
		if(readnum == 0)
		{
				f->fcuraddr = f->fstartaddr;
				return true;
		}
		
		readnum_bolcksize = (readnum - 1) / ((float)0x20000) + 1;
		f->fcuraddr = f->fstartaddr;
		curaddrs = f->fcuraddr;
		curaddrc = SerialAddress_to_CycleAddress(curaddrs,&nand);
		curaddrc_temp = curaddrc;
		
		while(1)
		{
				if(isbadblock(curaddrc.Block + (curaddrc.Zone) * nand.Info.ZoneSize))
				{
						badblocknum++;
				}
				else
				{
						nomalblocknum++;
						if(nomalblocknum >= readnum_bolcksize)
						{
								break;
						}
				}
				
				curaddrs += 0x20000;
				if(curaddrs >= f->faddrmax)																										//该文件分配存储空间最后一块block
				{
						curaddrs = f->faddrmin;
				}
				curaddrc = SerialAddress_to_CycleAddress(curaddrs,&nand);
			
				if((curaddrc.Block == curaddrc_temp.Block) && (curaddrc.Zone == curaddrc_temp.Zone))
				{
						f->funused = 0xaa;
						return false;																															//该文件分配存储空间全部损坏
				}
		}
				
		f->fcuraddr = f->fstartaddr + badblocknum * 0x20000 + readnum;
		
		if(f->fcuraddr >= f->faddrmax)
		{
				f->fcuraddr = f->faddrmin + (f->fcuraddr - f->faddrmax);
		}
		
		return true;
}

bool set_writenum(FILE_STRUCT *f , uint32_t writenum)
{
	
		NAND_Address curaddrc,curaddrc_temp;
		uint32_t curaddrs;
		uint16_t badblocknum = 0, nomalblocknum = 0,readnum_bolcksize = 0;
	
	
		if((writenum >= f->fsize) || (writenum + f->fstartaddr > f->faddrmax))
		{
				return false;
		}
		
		f->fwritennum = writenum;
		
		if(writenum == 0)
		{
				f->fcuraddr = f->fstartaddr;
				return true;
		}
		
		readnum_bolcksize = (writenum - 1) / ((float)0x20000) + 1;
		f->fcuraddr = f->fstartaddr;
		curaddrs = f->fcuraddr;
		curaddrc = SerialAddress_to_CycleAddress(curaddrs,&nand);
		curaddrc_temp = curaddrc;
		
		while(1)
		{
				if(isbadblock(curaddrc.Block + (curaddrc.Zone) * nand.Info.ZoneSize))
				{
						badblocknum++;
				}
				else
				{
						nomalblocknum++;
						if(nomalblocknum >= readnum_bolcksize)
						{
								break;
						}
				}
				
				curaddrs += 0x20000;
				if(curaddrs >= f->faddrmax)																										//该文件分配存储空间最后一块block
				{
						curaddrs = f->faddrmin;
				}
				curaddrc = SerialAddress_to_CycleAddress(curaddrs,&nand);
			
				if((curaddrc.Block == curaddrc_temp.Block) && (curaddrc.Zone == curaddrc_temp.Zone))
				{
						f->funused = 0xaa;
						return false;																															//该文件分配存储空间全部损坏
				}
		}
				
		f->fcuraddr = f->fstartaddr + badblocknum * 0x20000 + writenum;
		
		if(f->fcuraddr >= f->faddrmax)
		{
				f->fcuraddr = f->faddrmin + (f->fcuraddr - f->faddrmax);
		}
		
		return true;
}

/**  获取文件信息首地址  **/
uint32_t get_filedat_addr(uint32_t addr,uint8_t *isnandused)
{
    static uint8_t  rbuf[63];
    uint8_t  i = 0;
    NAND_Address	nandaddr;
    NAND_Address	endnandaddr;

    nandaddr = SerialAddress_to_CycleAddress(addr,&nand);
    NAND_Read_Data(&nand,&nandaddr,&endnandaddr,rbuf,63);

    for(i = 0; i < 63; i++)
    {
        if(rbuf[i] == 0xff)
        {
            break;
        }
    }
    if(i == 0)
    {
        addr +=  nand.Info.PageSize;
				*isnandused = 0;
    }
    else
    {
        addr += i * nand.Info.PageSize;
				*isnandused = 1;
    }

    return addr;
}

/**  记录文件信息地址的信息  **/
void set_filedat_addr(uint32_t addr)
{
    uint16_t badblockflagclou = 0;
    static uint8_t  wbuf[63];
    uint8_t  i = 0;
    NAND_Address	nandaddr;
    NAND_Address	endnandaddr;
    NAND_StatusTypeDef status;

    badblockflagclou = 'c' << 8 | 'l';

    nandaddr = SerialAddress_to_CycleAddress(addr,&nand);
    NAND_Read_Data(&nand,&nandaddr,&endnandaddr,wbuf,63);

    for(i = 0; i < 63; i++)
    {
        if(wbuf[i] == 0xff)
        {
            break;
        }
    }

    if(i >=63)                      /**  block完成一次erese-program循环  **/
    {
        NAND_Erase_Block(&nand,&nandaddr);
    }
    else
    {
        addr += i	;
    }

    wbuf[0] = 0;
    nandaddr = SerialAddress_to_CycleAddress(addr,&nand);
    status = NAND_Write_Data(&nand,&nandaddr,&endnandaddr,wbuf,1);

    if(status == NAND_BLOCK_IS_ERROR)
    {
        badblockupdate(nandaddr.Block + nandaddr.Zone * nand.Info.ZoneSize,(uint8_t *)&badblockflagclou);
    }
}


extern uint16_t NAND_ReadBadBlockFlag(NAND_HandleTypeDef *hnand ,uint16_t _ulBlockNo ,uint8_t planeNo);
uint8_t file_update = 0;

uint8_t get_file_update(void)
{
    return file_update;
}

void clear_file_update(void)
{
    file_update = 0;
}

NAND_StatusTypeDef fwrite_w(FILE_STRUCT *file,uint8_t *buf,uint32_t len)
{
    NAND_Address	stnandaddr;
    NAND_Address	endnandaddr;

    NAND_StatusTypeDef status;
    FILE_STRUCT *f;
    uint16_t badblockflagclou = 0;
		
		badblockflagclou = 'c' << 8 | 'l';

    f = file;

    /**  文件写开始时，偏移地址为零  **/
		if(f->fwritennum == 0)
		{
				stnandaddr = SerialAddress_to_CycleAddress(f->fnewstartaddr,&nand);
		}
		else
		{
				if(f->fcuraddr >= f->faddrmax)
				{
						f->fcuraddr = f->faddrmin;
				}
				stnandaddr = SerialAddress_to_CycleAddress(f->fcuraddr,&nand);
		}

    /**  坏块处理  **/
    if(stnandaddr.Page == 0 && stnandaddr.column == 0)
    {
        if(skip_bad_block(f,&stnandaddr,&nand) == false)
				{
						return NAND_BLOCK_IS_ERROR;																												//存储区域损坏
				}
				NAND_Erase_Block(&nand,&stnandaddr);
    }

    /**  写文件  **/
    status = NAND_Write_Data(&nand,&stnandaddr,&endnandaddr,buf,len);

    if(status == NAND_BLOCK_IS_ERROR)
    {
        badblockupdate(stnandaddr.Block + stnandaddr.Zone * nand.Info.ZoneSize,(uint8_t *)&badblockflagclou);
    }

    f->fcuraddr = CycleAddress_to_SerialAddress(&endnandaddr,&nand);

    /**  写文件全部结束  **/
    f->fwritennum += len;
    if(f->fwritennum >= f->fsize)
    {
				f->flaststartaddr= f->fstartaddr;
				f->fendaddr = f->fcuraddr;
        f->fstartaddr = f->fnewstartaddr;
        f->fcuraddr = f->fstartaddr;
        f->fwritennum = 0;
        f->freadnum = 0;
				get_newstartaddr(f,&nand);
			
        if(strcmp(f->fname,"file.dat") != 0)
        {
            file_update = 1;
						f->funused = 0;
        }
    }

    return status;
}

NAND_StatusTypeDef fread_w(FILE_STRUCT *file,uint8_t *buf,uint32_t len)
{
    NAND_Address	stnandaddr;
    NAND_Address	endnandaddr;

    NAND_StatusTypeDef status;
    FILE_STRUCT *f;

    f = file;

    /**  文件写开始时，偏移地址为零  **/
		if(f->freadnum == 0)
		{
				stnandaddr = SerialAddress_to_CycleAddress(f->fstartaddr,&nand);
		}
		else
		{
				if(f->fcuraddr >= f->faddrmax)
				{
						f->fcuraddr = f->faddrmin;
				}
				stnandaddr = SerialAddress_to_CycleAddress(f->fcuraddr,&nand);
		}
		
    /**  坏块处理  **/
    if(stnandaddr.Page == 0 && stnandaddr.column == 0)
    {
        if(skip_bad_block(f,&stnandaddr,&nand) == false)
				{
						return NAND_BLOCK_IS_ERROR;																												//存储区域损坏
				}
    }
	
    /**  写文件  **/
    status = NAND_Read_Data(&nand,&stnandaddr,&endnandaddr,buf,len);

    f->fcuraddr = CycleAddress_to_SerialAddress(&endnandaddr,&nand);

    /**  写文件全部结束  **/
    f->freadnum += len;
    if(f->freadnum >= f->fsize)
    {
        f->freadnum = 0;
        f->fendaddr = f->fcuraddr;
        f->fcuraddr = f->fstartaddr;
    }
    osDelay(1);

    return status;
}
