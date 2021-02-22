#include <cmsis_os.h>
#include "stmflash.h"
#include "stm32f4xx_hal.h"

FLASH_EraseInitTypeDef pEraseInit = {FLASH_TYPEERASE_SECTORS,
                                     FLASH_BANK_1,FLASH_SECTOR_11,1,VoltageRange_3
                                    };

HAL_StatusTypeDef Erase_Sector(FLASH_EraseInitTypeDef *pEraseInit);

static FunctionalState flashdatacache = ENABLE;

void FLASH_DataCacheCmd(FunctionalState state)
{
    flashdatacache = state;
}

FunctionalState FLASH_DataCacheRead(void)
{
    return flashdatacache;
}

uint32_t STMFLASH_ReadWord(uint32_t faddr)
{
    return *((volatile uint32_t *)faddr);
}

uint32_t STMFLASH_ReadByt(uint32_t faddr)
{
    return *((volatile uint8_t *)faddr);
}

//获取某个地址所在的flash扇区
//addr:flash地址
//返回值:0~11,即addr所在的扇区
uint32_t STMFLASH_GetFlashSectorAddr(uint32_t sector)
{
    if(sector == FLASH_Sector_0)return ADDR_FLASH_SECTOR_0;
    else if(sector == FLASH_Sector_1)return ADDR_FLASH_SECTOR_1;
    else if(sector == FLASH_Sector_2)return ADDR_FLASH_SECTOR_2;
    else if(sector == FLASH_Sector_3)return ADDR_FLASH_SECTOR_3;
    else if(sector == FLASH_Sector_4)return ADDR_FLASH_SECTOR_4;
    else if(sector == FLASH_Sector_5)return ADDR_FLASH_SECTOR_5;
    else if(sector == FLASH_Sector_6)return ADDR_FLASH_SECTOR_6;
    else if(sector == FLASH_Sector_7)return ADDR_FLASH_SECTOR_7;
    else if(sector == FLASH_Sector_8)return ADDR_FLASH_SECTOR_8;
    else if(sector == FLASH_Sector_9)return ADDR_FLASH_SECTOR_9;
    else if(sector == FLASH_Sector_10)return ADDR_FLASH_SECTOR_10;
    return ADDR_FLASH_SECTOR_11;
}
//从指定地址开始写入指定长度的数据
//WriteAddr:起始地址(此地址必须为4的倍数!!)
//pBuffer:数据指针
//NumToWrite:字(32位)数(就是要写入的32位数据的个数.)
uint8_t STMFLASH_Write(uint16_t sector,uint8_t *pBuffer,uint32_t NumToWrite)
{
    HAL_StatusTypeDef status = HAL_OK;
    uint32_t endaddr=0;
    uint32_t WriteAddr;
    uint32_t *SectorError;

    pEraseInit.Sector = sector;
    WriteAddr = STMFLASH_GetFlashSectorAddr(sector);
    if(HAL_FLASH_Unlock() != HAL_OK)	//解锁
    {
        return 1;
    }
    FLASH_DataCacheCmd(DISABLE);//FLASH擦除期间,必须禁止数据缓存

    endaddr=WriteAddr+NumToWrite*4;	//写入的结束地址
    status = HAL_FLASHEx_Erase(&pEraseInit,SectorError);

    status = HAL_TIMEOUT;
    for(status = FLASH_WaitForLastOperation(0); status == HAL_TIMEOUT;)
    {
        osDelay(1);
    }

    if(status==HAL_OK)
    {
        while(WriteAddr<endaddr)//写数据
        {
            status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,WriteAddr,*pBuffer);
            if(status !=HAL_OK)//写入数据
            {
                continue ;	//写入异常
            }
            WriteAddr++;
            pBuffer++;
            if(WriteAddr % 12 == 0)
            {
                osDelay(1);
            }
        }
    }

    FLASH_DataCacheCmd(ENABLE);	//FLASH擦除结束,开启数据缓存
    if(HAL_FLASH_Lock() != HAL_OK)	//上锁
    {
        return 1;
    }
    if(status == HAL_OK)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToRead:字(4位)数
void STMFLASH_Read(uint32_t ReadAddr,uint32_t *pBuffer,uint32_t NumToRead)
{
    uint32_t i;
    for(i=0; i<NumToRead; i++)
    {
        pBuffer[i]=STMFLASH_ReadWord(ReadAddr);//读取4个字节.
        ReadAddr+=4;//偏移4个字节.
    }
}







