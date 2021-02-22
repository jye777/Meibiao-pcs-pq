#include <stdio.h>
#include <string.h>
#include "cmsis_os.h"
#include "nand_file.h"
#include "crc.h"
#include "wave_file.h"
#include "recordwave.h"
#include "tty.h"

const char frequency_filename[] = "fmlog.dat";

uint16_t fmLogQty = 0;
//uint16_t fault_nun_rd = 0;
extern struct fmFileType fmFile;

//------------------------------------------------------------------------------------------
//故障文件初始化****************************************************************************
//------------------------------------------------------------------------------------------
uint8_t FM_FileInit(void)
{
    FILE_STRUCT *f;
    uint8_t i = 0;

    f  = getfileaddr((char *)frequency_filename);

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
		
    fread_w(f,(uint8_t *)&fmFile.curnum,sizeof(fmFile.curnum) + sizeof(fmFile.addr));

    for(i = 0; i < FMLOG_MAX; i++)
    {
        if(fmFile.addr[i] <= f->faddrmax && fmFile.addr[i] >= f->faddrmin)
        {
            fmLogQty++;
        }
    }

    return 0;
}

//------------------------------------------------------------------------------------------
//存储一条新调频日志****************************************************************************
//------------------------------------------------------------------------------------------
void FM_Write(void)
{
    FILE_STRUCT *f;

    f  = getfileaddr((char *)frequency_filename);

    if(fmLogQty < FMLOG_MAX)
    {
        fmLogQty++;																										//记录总条数
    }

    fmFile.curnum++;

    if(fmFile.curnum >= FMLOG_MAX)
    {
        fmFile.curnum = 0;
    }

    fmFile.addr[fmFile.curnum] = f->fnewstartaddr;

    f->fsize = sizeof(struct fmFileType);
	write_file(f,(uint8_t *)&fmFile,0,f->fsize);
}

//------------------------------------------------------------------------------------------
//读取第rd_num条故障状态信息****************************************************************
//------------------------------------------------------------------------------------------
uint8_t FM_ReadBriefInfor(uint16_t rd_num)
{
    FILE_STRUCT *f;

    uint16_t firstnum ;

    uint32_t addr;

    firstnum =  (fmFile.curnum + 1 + FMLOG_MAX - fmLogQty) % FMLOG_MAX;

    if(firstnum + rd_num -1 >= FMLOG_MAX)
    {
        addr = fmFile.addr[firstnum + rd_num -1 - FMLOG_MAX];
    }
    else
    {
        addr = fmFile.addr[firstnum + rd_num -1];
    }

    f  = getfileaddr((char *)frequency_filename);

    f->fstartaddr = addr;

		if(set_readnum(f,FMLOG_MAX * 4 + 4) == false)
		{
				return 1;
		}

		read_file(f,(uint8_t *)&fmFile + FMLOG_MAX * 4 + 4,FMLOG_MAX * 4 + 4,sizeof(_fault_save_brief));

    f->fstartaddr = fmFile.addr[fmFile.curnum];
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
uint8_t FM_ReadFMLog(uint16_t rd_num)
{
    FILE_STRUCT *f;

    uint16_t firstnum ;

    uint32_t addr;

    firstnum =  (fmFile.curnum + 1 + FMLOG_MAX - fmLogQty) % FMLOG_MAX;

    if(firstnum + rd_num -1 >= FMLOG_MAX)
    {
        addr = fmFile.addr[firstnum + rd_num -1 - FMLOG_MAX];
    }
    else
    {
        addr = fmFile.addr[firstnum + rd_num -1];
    }

    f  = getfileaddr((char *)frequency_filename);

    f->fstartaddr = addr;

    if(set_readnum(f,0) == false)
    {
    		return 1;
    }

    memset((uint8_t *)&(fmFile.fft_infor),0,sizeof(struct fmAllInforType));

    read_file(f,(uint8_t *)&fmFile + FMLOG_MAX * 4 + 4,FMLOG_MAX * 4 + 4,f->fsize - FMLOG_MAX * 4 + 4);
    f->fstartaddr = fmFile.addr[fmFile.curnum];

    return 0;
}

//------------------------------------------------------------------------------------------
//读取故障已存储数目,其他文件使用***********************************************************
//------------------------------------------------------------------------------------------
uint16_t FM_GetFMLogQty(void)
{
    return fmLogQty;
}

