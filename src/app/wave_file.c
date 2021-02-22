#include <stdio.h>
#include <string.h>
#include "cmsis_os.h"
#include "nand_file.h"
#include "crc.h"
#include "wave_file.h"
#include "record_wave.h"

const char wave_filename[] = "wave.dat";

uint16_t fault_num = 0;
uint16_t fault_nun_rd = 0;

extern int tty_printf(const char * format, ...);

//------------------------------------------------------------------------------------------
//故障文件初始化****************************************************************************
//------------------------------------------------------------------------------------------
uint8_t wave_file_int(void)
{
    FILE_STRUCT *f;
    uint8_t i = 0;

    f  = getfileaddr((char *)wave_filename);

    if(f == NULL)
    {
        return 1;
    }
		
		if(f->funused == 1)
		{
				return 1;
		}
		else if(f->funused == 0xaa)
		{
				tty_printf("wave.dat文件分配存储区全部损坏\n");
				return 1;
		}

		if(set_readnum(f,0) == false)
		{
				return 1;
		}
		
    fread_w(f,(uint8_t *)&wave.curnum,FAULT_MAX * 4 + 4);

    for(i = 0; i < FAULT_MAX; i++)
    {
        if(wave.addr[i] <= f->faddrmax && wave.addr[i] >= f->faddrmin)
        {
            fault_num++;
        }
    }

    return 0;
}

//------------------------------------------------------------------------------------------
//存储一条新故障****************************************************************************
//------------------------------------------------------------------------------------------
void fault_write(void)
{
    FILE_STRUCT *f;

    f  = getfileaddr((char *)wave_filename);

    if(fault_num < 5)
    {
        fault_num++;																										//记录总条数
    }

    wave.curnum++;

    if(wave.curnum >= 5)
    {
        wave.curnum = 0;
    }

    wave.addr[wave.curnum] = f->fnewstartaddr;

    f->fsize = FAULT_MAX * 4 + 4 + sizeof(_fault_save_data);
		write_file(f,(uint8_t *)&wave,0,f->fsize);		
}

//------------------------------------------------------------------------------------------
//读取第rd_num条故障状态信息****************************************************************
//------------------------------------------------------------------------------------------
uint8_t fault_read_brief(uint16_t rd_num)
{
    FILE_STRUCT *f;

    uint16_t firstnum ;

    uint32_t addr;

    firstnum =  (wave.curnum + 1 + FAULT_MAX - fault_num) % FAULT_MAX;

    if(firstnum + rd_num -1 >= FAULT_MAX)
    {
        addr = wave.addr[firstnum + rd_num -1 - FAULT_MAX];
    }
    else
    {
        addr = wave.addr[firstnum + rd_num -1];
    }

    f  = getfileaddr((char *)wave_filename);

    f->fstartaddr = addr;

		if(set_readnum(f,FAULT_MAX * 4 + 4) == false)
		{
				return 1;
		}

		read_file(f,(uint8_t *)&wave + FAULT_MAX * 4 + 4,FAULT_MAX * 4 + 4,sizeof(_fault_save_brief));

    f->fstartaddr = wave.addr[wave.curnum];
		if(set_readnum(f,0) == false)
		{
				return 1;
		}
    f->fcuraddr = f->fstartaddr;

    return 0;
}

//------------------------------------------------------------------------------------------
//读取第rd_num条故障的数据******************************************************************
//------------------------------------------------------------------------------------------
uint8_t fault_read(uint16_t rd_num)
{
    FILE_STRUCT *f;

    uint16_t firstnum ;

    uint32_t addr;

    firstnum =  (wave.curnum + 1 + FAULT_MAX - fault_num) % FAULT_MAX;

    if(firstnum + rd_num -1 >= FAULT_MAX)
    {
        addr = wave.addr[firstnum + rd_num -1 - FAULT_MAX];
    }
    else
    {
        addr = wave.addr[firstnum + rd_num -1];
    }

    f  = getfileaddr((char *)wave_filename);

    f->fstartaddr = addr;

		if(set_readnum(f,0) == false)
		{
				return 1;
		}
		
		memset((uint8_t *)&(wave.fault),0,sizeof(_fault_save_data));

		read_file(f,(uint8_t *)&wave + FAULT_MAX * 4 + 4,FAULT_MAX * 4 + 4,f->fsize - FAULT_MAX * 4 + 4);
    f->fstartaddr = wave.addr[wave.curnum];

    return 0;
}

//------------------------------------------------------------------------------------------
//读取故障已存储数目,其他文件使用***********************************************************
//------------------------------------------------------------------------------------------
uint16_t getfaultnum(void)
{
    return fault_num;
}
