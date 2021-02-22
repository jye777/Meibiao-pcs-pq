#include <stdio.h>
#include <string.h>

#include "cmsis_os.h"
#include "storedata.h"
#include "nand_file.h"
#include "tty.h"
#include "c_md5.h"

#define STORE_BUF_SIZE      0x10000

__align(8) uint8_t sdata_buf[STORE_BUF_SIZE];
uint8_t *psdata;

static const char sdata_name[] = "sdata.dat";

osThreadId sdata_update_task_id;
static void sdata_update_task(const void *pdata);
osThreadDef(sdata_update_task, osPriorityAboveNormal, 1, 0);

static int read_sdata(const char *name, uint8_t *buf)
{
    FILE_STRUCT *f;
    int err = 0;

    f = getfileaddr((char *)name);
    if(f == NULL)
    {
        err = 1;
    }
		if(f->funused == 1)
		{
				return err;
		}
		else if(f->funused == 0xaa)
		{
				tty_printf("sdata.dat文件分配存储区全部损坏\n");
				err = 1;
				return 1;
		}
    f->fsize = STORE_BUF_SIZE;
//		osSemaphoreWait(nand_sem,osWaitForever);
		read_file(f,buf,0,STORE_BUF_SIZE);
//		osSemaphoreRelease(nand_sem);

    return err;
}

void sdata_init(void)
{
    int err;

    err = read_sdata(sdata_name, sdata_buf);

    if(err)
		{
			tty_printf("sdata.dat文件丢失\n");
		}
    psdata = sdata_buf;

    sdata_update_task_id = osThreadCreate(osThread(sdata_update_task), NULL);
}

uint32_t get_sdata_addr(uint32_t offset)
{
    return (uint32_t)&psdata[offset];
}

static void sdata_update_flash(void)
{
    FILE_STRUCT *f;
    static uint32_t cnt = 0;
    cnt++;

    f = getfileaddr((char *)sdata_name);

    if(f == NULL)
    {
        return;
    }
		write_file(f,psdata,0,STORE_BUF_SIZE);
		tty_printf("sdata.dat已储存\n");		
}

static void get_sdatadatamd5(uint8_t *md5)
{
    MD5_CTX context;
		uint8_t i = 0;
		uint32_t clcy ;
    MD5Init(&context);

    clcy = (STORE_BUF_SIZE - FPGA_ZERO_BASE)/512 +1;
		for(; i<clcy; i++)
		{
			if(i != clcy -1)
			{
				MD5Update(&context, psdata + FPGA_ZERO_BASE + 512*i,512);
			}
			else
			{
				MD5Update(&context, psdata + FPGA_ZERO_BASE + 512 *i,512 - FPGA_ZERO_BASE);
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

void save_sdata(void)
{
    uint8_t *md5_last;
    uint8_t md5_cur[16];
		
		md5_last = psdata;
    get_sdatadatamd5(md5_cur);
    memcpy(md5_last,md5_cur,16);
		sdata_update_flash();
}

static void sdata_update_task(const void *pdata)
{
    uint8_t *md5_last;
    uint8_t md5_cur[16];
		
		md5_last = psdata;

    while(1)
    {
        get_sdatadatamd5(md5_cur);

        if(cmp_md5(md5_cur,md5_last) == false)
        {
            memcpy(md5_last,md5_cur,16);
						sdata_update_flash();
        }
        osDelay(3000);
    }
}
