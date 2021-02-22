#include <cmsis_os.h>
#include "stmflash.h"
#include "fpga.h"
#include "flashdata.h"
#include "env.h"
#include <string.h>
#include "c_md5.h"
#include <stdbool.h>
#include "tty.h"

osSemaphoreId flashdata_sem;
osSemaphoreDef(flashdata_sem);

osThreadId flashdata_update_task_id;
static void flashdata_update_task(const void *pdata);
osThreadDef(flashdata_update_task, osPriorityAboveNormal, 1, 0);

uint8_t flash_data0[4096];
//uint8_t flash_data1[4096];

volatile short *arm_config_flash_data;
fpga_data_buffer *fpga_pdatabuf_flash;
short fpga_pdatabuf_get[512];

uint32_t get_flashdata0_addr(uint32_t offset)
{
    return (uint32_t)&flash_data0[offset];
}

void flashdata_alloc_all(void)
{
    arm_config_flash_data = (short *)get_flashdata0_addr(CONFIG_FLASH_BASE);
    fpga_pdatabuf_flash = (fpga_data_buffer *)get_flashdata0_addr(FPGA_FLASH_DATA_BASE);
//    env_cache = (ub_env *)get_flashdata0_addr(ENV_BASE);	
//		env_init();
}

static void read_flashdata(uint8_t *buf,uint32_t len)
{
    uint32_t addr = FLASH_DATA_ADDR;
    uint32_t i = 0;

    for(i = 0 ; i < len ; i++,addr++)
    {
        buf[i] = STMFLASH_ReadByt(addr);
    }
}

void flashdata_init(void)
{
    flashdata_alloc_all();
    flashdata_sem = osSemaphoreCreate(osSemaphore(flashdata_sem), 1);

    read_flashdata(flash_data0,4);

    if(*((uint32_t *)(flash_data0)) != 0x55555555)
    {
        memset(flash_data0,0,4096);
        env_crc_update();
        *((uint32_t *)(flash_data0)) = 0x55555555;
        STMFLASH_Write(FLASH_DATA_SECTOR,flash_data0,4096);
    }
    else
    {
        read_flashdata(flash_data0,4095);
    }
    flashdata_update_task_id = osThreadCreate(osThread(flashdata_update_task), NULL);
}

uint32_t get_flashdata_addr(uint32_t offset)
{
    return (uint32_t)&flash_data0[offset];
}

void flashdata_update_flash(void)
{
    STMFLASH_Write(FLASH_DATA_SECTOR,flash_data0,4096);
}

//static void get_flashdatamd5(uint8_t *md5)
//{
//    MD5_CTX context;

//    MD5Init(&context);

//    MD5Update(&context, flash_data0 + 20, 4076);
//    MD5Final(md5, &context);
//}

static void get_flashdatamd5(uint8_t *md5)
{
    MD5_CTX context;
		uint8_t i = 0;
		uint32_t clcy ;
    MD5Init(&context);

    clcy = 4076/512 +1;
		for(; i<clcy; i++)
		{
			if(i != clcy -1)
			{
				MD5Update(&context, flash_data0 + 20 + 512*i,512);
			}
			else
			{
				MD5Update(&context, flash_data0 + 20 + 512 *i,512 - 20);
			}
			osDelay(1);
		}

    MD5Final(md5, &context);
}

static bool cmp_md5(uint8_t *md51,uint8_t *md52)
{
    uint8_t i = 0;

    for(i = 0 ; i < 16; i++)
    {
        if(*(md51 + i) == *(md52 + i))
        {
            continue;
        }
        else
        {
            break;
        }
    }

    if(i <16)
    {
        return false;
    }
    else
    {
        return true;
    }
}

static void flashdata_update_task(const void *pdata)
{
    uint8_t *md5_last;
    uint8_t md5_cur[16];

		md5_last = flash_data0+4;

    while(1)
    {
        get_flashdatamd5(md5_cur);

        if(cmp_md5(md5_cur,md5_last) == false)
        {
            memcpy(md5_last,md5_cur,16);
            flashdata_update_flash();
						tty_printf("flash data ÒÑ±£´æ\n");
        }

        osDelay(3000);
    }
}


