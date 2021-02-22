#include "nand.h"
#include "nand_file.h"
#include "string.h"
#include <cmsis_os.h>
#include "tty.h"

FILE_STRUCT *pfsdata,*pfmain,*pfmaindef,*pffpga,*pffpgadef,*pfwave;
FILE_STRUCT *pfile[10];
uint8_t file_data[2048];
#define NAND_BUF_SIZE        2048
uint8_t nand_buf[NAND_BUF_SIZE];
osSemaphoreId nand_sem;
osSemaphoreDef(nand_sem);

FILE_STRUCT *creat_file(uint8_t *name)
{
    FILE_STRUCT *f;
    uint8_t i = 0;

    f = (FILE_STRUCT *)&file_data[0];

    for(i = 0; i < 30; i++)
    {
        if(*(name + i) != ' ')
        {
            f->fname[i] = *(name + i);
        }
        else
        {
            break;
        }
    }

    f->fcuraddr = f->fstartaddr;
    return f;
}

FILE_STRUCT *creat_file_sdata(void)
{
    FILE_STRUCT *f;

    f = (FILE_STRUCT *)(&file_data[0]);

    if(strcmp(f->fname,"sdata.dat") == 0)
    {
        return f;
    }

    strcpy(f->fname,"sdata.dat");

    f->faddrmin      = 0x00020000;
    f->faddrmax      = 0x00c60000;

    f->fnewstartaddr = f->faddrmin;
    f->fcuraddr 	   = f->faddrmin;
    f->fstartaddr    = f->faddrmin;
    f->fsize         = 0x10000;   /**  8Kbyte  **/ //2000
    f->fsizemax      = 0x10000;
    f->fendaddr      = f->fstartaddr + f->fsize;
    f->fwritennum    = 0;
    f->freadnum      = 0;
		f->flaststartaddr= f->faddrmin;
		f->flastsize  = f->fsizemax;
		f->funused			 = 1;
    return f;
}

FILE_STRUCT *creat_file_main(void)
{
    FILE_STRUCT *f;

    f = (FILE_STRUCT *)(&file_data[0] + 1*sizeof(FILE_STRUCT));

    if(strcmp(f->fname,"main_cur.bin") == 0)
    {
        return f;
    }

    strcpy(f->fname,"main_cur.bin");

    f->faddrmin      = 0x01300000;
    f->faddrmax      = 0x01660000;

    f->fnewstartaddr = f->faddrmin;
    f->fcuraddr 	   = f->faddrmin;
    f->fstartaddr    = f->faddrmin;
    f->fsize         = 0x40000;   /**  256Kbyte  **/
    f->fsizemax      = 0x40000;
    f->fendaddr      = f->fstartaddr + f->fsize;
    f->fwritennum    = 0;
    f->freadnum      = 0;
		f->flaststartaddr= f->faddrmin;
		f->flastsize  = f->fsizemax;
		f->funused			 = 1;

    return f;
}

FILE_STRUCT *creat_file_main_last(void)
{
    FILE_STRUCT *f;

    f = (FILE_STRUCT *)(&file_data[0] + 8*sizeof(FILE_STRUCT));

    if(strcmp(f->fname,"main_last.bin") == 0)
    {
        return f;
    }

    strcpy(f->fname,"main_last.bin");

    f->faddrmin      = 0x01300000;
    f->faddrmax      = 0x01660000;

    f->fnewstartaddr = f->faddrmin;
    f->fcuraddr 	   = f->faddrmin;
    f->fstartaddr    = f->faddrmin;
    f->fsize         = 0x40000;   /**  256Kbyte  **/
    f->fsizemax      = 0x40000;
    f->fendaddr      = f->fstartaddr + f->fsize;
    f->fwritennum    = 0;
    f->freadnum      = 0;
		f->flaststartaddr= f->faddrmin;
		f->flastsize  = f->fsizemax;
		f->funused			 = 1;

    return f;
}

FILE_STRUCT *creat_file_main_def(void)
{
    FILE_STRUCT *f;

    f = (FILE_STRUCT *)(&file_data[0] + 2*sizeof(FILE_STRUCT));

    if(strcmp(f->fname,"main_def.bin") == 0)
    {
        return f;
    }

    strcpy(f->fname,"main_def.bin");

    f->faddrmin      = 0x01660000;
    f->faddrmax      = 0x01b60000;

    f->fnewstartaddr = f->faddrmin;
    f->fcuraddr 	   = f->faddrmin;
    f->fstartaddr    = f->faddrmin;
    f->fsize         = 0x40000;   /**  256Kbyte  **/
    f->fsizemax      = 0x40000;
    f->fendaddr      = f->fstartaddr + f->fsize;
    f->fwritennum    = 0;
    f->freadnum      = 0;
		f->flaststartaddr= f->faddrmin;
		f->flastsize  = f->fsizemax;
		f->funused			 = 1;

    return f;
}

FILE_STRUCT *creat_file_fpga(void)
{
    FILE_STRUCT *f;

    f = (FILE_STRUCT *)(&file_data[0] + 3*sizeof(FILE_STRUCT));

    if(strcmp(f->fname,"fpga_cur.pof") == 0)
    {
        return f;
    }

    strcpy(f->fname,"fpga_cur.pof");

    f->faddrmin      = 0x01b60000;
    f->faddrmax      = 0x04360000;

    f->fnewstartaddr = f->faddrmin;
    f->fcuraddr 	   = f->faddrmin;
    f->fstartaddr    = f->faddrmin;
    f->fsize         = 0x100000;   /**  1Mbyte  **/
    f->fsizemax      = 0x220000;
    f->fendaddr      = f->fstartaddr + f->fsize;
    f->fwritennum    = 0;
    f->freadnum      = 0;
		f->flaststartaddr= f->faddrmin;
		f->flastsize  = f->fsizemax;
		f->funused			 = 1;

    return f;
}

FILE_STRUCT *creat_file_fpga_def(void)
{
    FILE_STRUCT *f;

    f = (FILE_STRUCT *)(&file_data[0] + 4*sizeof(FILE_STRUCT));

    if(strcmp(f->fname,"fpga_def.pof") == 0)
    {
        return f;
    }

    strcpy(f->fname,"fpga_def.pof");

    f->faddrmin      = 0x04360000;
    f->faddrmax      = 0x05760000;

    f->fnewstartaddr = f->faddrmin;
    f->fcuraddr 	   = f->faddrmin;
    f->fstartaddr    = f->faddrmin;
    f->fsize         = 0x100000;   /**  1Mbyte  **/
    f->fsizemax      = 0x220000;
    f->fendaddr      = f->fstartaddr + f->fsize;
    f->fwritennum    = 0;
    f->freadnum      = 0;
		f->flaststartaddr= f->faddrmin;
		f->flastsize  = f->fsizemax;
		f->funused			 = 1;

    return f;
}

FILE_STRUCT *creat_file_log(void)
{
    FILE_STRUCT *f;

    f = (FILE_STRUCT *)(&file_data[0] + 5*sizeof(FILE_STRUCT));

    if(strcmp(f->fname,"log.dat") == 0)
    {
        return f;
    }

    strcpy(f->fname,"log.dat");

    f->faddrmin      = 0x05760000;
    f->faddrmax      = 0x06de0000;

    f->fnewstartaddr = f->faddrmin;
    f->fcuraddr 	   = f->faddrmin;
    f->fstartaddr    = f->faddrmin;
    f->fsize         = 0xc812;   /**  1Mbyte + 128Kbyte  **/
    f->fsizemax      = 0x20000;
    f->fendaddr      = f->fstartaddr + f->fsize;
    f->fwritennum    = 0;
    f->freadnum      = 0;
		f->flaststartaddr= f->faddrmin;
		f->flastsize  = f->fsizemax;
		f->funused			 = 1;

    return f;
}

FILE_STRUCT *creat_file_wave(void)
{
    FILE_STRUCT *f;

    f = (FILE_STRUCT *)(&file_data[0] + 7*sizeof(FILE_STRUCT));

    if(strcmp(f->fname,"wave.dat") == 0)
    {
        return f;
    }

    strcpy(f->fname,"wave.dat");

    f->faddrmin      = 0x071a0000;
    f->faddrmax      = 0x10000000;

    f->fnewstartaddr = f->faddrmin;
    f->fcuraddr 	   = f->faddrmin;
    f->fstartaddr    = f->faddrmin;
    f->fsize         = 0x120000;   /**  1Mbyte + 128Kbyte  **/
    f->fsizemax      = 0x120000;
    f->fendaddr      = f->fstartaddr + f->fsize;
    f->fwritennum    = 0;
    f->freadnum      = 0;
		f->flaststartaddr= f->faddrmin;
		f->flastsize  = f->fsizemax;
		f->funused			 = 1;

    return f;
}

bool file_file_init(void)
{
		FILE_STRUCT *f;
		uint32_t addrblock = 0x06de0000;
	
		f = (FILE_STRUCT *)(&file_data[0] + 6*sizeof(FILE_STRUCT));
		pfile[6] = f;
		get_file_file(&addrblock,f);
		if(addrblock == 0xffffffff)
		{
				return false;
		}
	
		f = (FILE_STRUCT *)(&file_data[0] + 9*sizeof(FILE_STRUCT));
		pfile[9] = f;
		get_file_file(&addrblock,f);
		
		return true;
}

void *get_file_file(uint32_t *addrblock,FILE_STRUCT *f)
{
		uint8_t flag = 1;
		uint32_t addr;
	
    strcpy(f->fname,"file.dat");

		*addrblock = get_filedat_block(*addrblock);
		f->faddrmin   	 = *addrblock + 0x800;
		f->faddrmax   	 = *addrblock + 0x20000;
		f->fsize         = 0x800;   /**  2Kbyte  **/
		f->fsizemax      = 0x800;
		addr = get_filedat_addr(*addrblock,&flag);
	
		if(addr != f->fstartaddr)
		{
				tty_printf("更正或者初始化地址信息\n");
				if(!flag)
				{
						f->fnewstartaddr = addr;
						f->flastsize		 = addr;
						f->funused			 = 0xaaaaaaaa;
				}
				else
				{
						f->fnewstartaddr = addr + f->fsizemax;
						if(f->fnewstartaddr >= f->faddrmax)
						{
								f->fnewstartaddr = f->faddrmin;
						}
						
						f->flaststartaddr = addr - f->fsizemax;
						if(f->flaststartaddr < f->faddrmin)
						{
								f->flaststartaddr = f->faddrmax - f->fsizemax;
						}
				}	
				
				f->fcuraddr 	   = addr;
				f->fstartaddr    = addr;
				f->fendaddr      = addr + f->fsize;
				f->fwritennum    = 0;
				f->freadnum      = 0;
				f->flastsize		 = f->fsizemax;
//				f->funused			 = 1;
				*addrblock += 0x20000;
		}		
		return 0;		
}

FILE_STRUCT *get_file_file_addr(FILE_STRUCT *fa,FILE_STRUCT *fb)
{
		if((fb->funused == 0xffffffff)&&(fa->funused <= 62))
		{
				/*块内页循环到page1时，执行完set_filedat_addr（会执行块擦除，写page0完成），
				未执行fwrite_w时，MCU复位或者掉电，会出现*/
				return fa;				
		}
		else if((fa->funused == 0xffffffff)&&(fb->funused <= 62))
		{
				/*块内页循环到page1时，执行完set_filedat_addr（会执行块擦除，写page0完成），
				未执行fwrite_w时，MCU复位或者掉电，会出现*/
				return fb;
		}
		else if(fa->funused + 1 == fb->funused)
		{
				/*最新地址数为A区数据*/
				return fb;
		}
		else if(fb->funused + 1 == fa->funused)
		{
				/*最新地址数为B区数据*/
				return fa;
		}
		else if((fb->funused == 62)&&(fa->funused == 0))
		{
				/*A区标识循环为0******/
				return fa;
		}
		else if((fa->funused == 62)&&(fb->funused == 0))
		{
				/*B区标识循环为0******/
				return fb;
		}
		else if((fa->funused == 0xaaaaaaaa) && (fb->funused < 62))
		{
				/*原a区块损坏*********/
				return fb;
		}
		else if((fb->funused == 0xaaaaaaaa) && (fa->funused < 62))
		{
				/*原b区块损坏*********/
				return fa;
		}
		else if((fa->funused == 0xaaaaaaaa) && (fb->funused == 0xaaaaaaaa))
		{
				/*nand格式化后，或者第一次使用*/
				return fa;
		}
		else
		{		/*相等时，区域只剩一块BLOCK可使用*/
				if((fa->funused >= fb->funused) && (fa->funused <= 62))				
				{
				/*存储区标识混乱******/
				return fa;
				}			
				else if((fb->funused >= fa->funused) && (fb->funused <= 62))
				{
				/*存储区标识混乱******/
				return fb;
				}
		}
		return fa;
}

void fileint(void)
{
    FILE_STRUCT *f,fa,fb;
	
		osSemaphoreWait(nand_sem,osWaitForever);
		file_file_init();
    fa = *pfile[6];
    fb = *pfile[9];
			
		if(fa.funused != 0xaaaaaaaa)
		{
				fread_w(&fa,file_data,2048);
				fa.funused = pfile[6]->funused;
		}
	
		if(fb.funused != 0xaaaaaaaa)
		{
				fread_w(&fb,file_data,2048);
				fb.funused = pfile[9]->funused;
		}
	
		f = get_file_file_addr(&fa,&fb);
		if(f != NULL)
		{
				fread_w(f,file_data,2048);
		}
		else
		{
				printf("地址存储区全部损坏");
		}
	
    pfile[0] = creat_file_sdata();
    pfile[1] = creat_file_main();
    pfile[2] = creat_file_main_def();
    pfile[3] = creat_file_fpga();
    pfile[4] = creat_file_fpga_def();
    pfile[5] = creat_file_wave();
    pfile[7] = creat_file_log();
    pfile[8] = creat_file_main_last();
		*pfile[6] = fa;
		*pfile[9] = fb;
		osSemaphoreRelease(nand_sem);
}

FILE_STRUCT *getfileaddr(char *name)
{
    for(uint8_t i = 0; i < 10; i++)
    {
        if(strcmp(pfile[i]->fname,name) == 0)
        {
            return pfile[i];
        }
    }
    return 0;
}


uint8_t check_fileware_def(char *name, FILE_STRUCT *f)
{
		uint8_t i = 0;
	
		for(i = 0; i < 10; i++)
    {
        if(strcmp(pfile[i]->fname,name) == 0)
        {
            break;
        }
    }
		if(i == 10)
		{
				return 1;
		}
		
		f = pfile[i];
		
		if(f->fstartaddr == f->fnewstartaddr)
		{
				return 2;
		}
				
		return 0;
}

uint8_t get_lastfile(char *name, FILE_STRUCT *f)
{
		uint8_t i = 0;
	
		for(i = 0; i < 10; i++)
    {
        if(strcmp(pfile[i]->fname,name) == 0)
        {
            break;
        }
    }
		if(i == 10)
		{
				return 1;
		}
		
		*f = *pfile[i];
		
		f->fstartaddr    = f->flaststartaddr;
		f->fsize         = f->flastsize;  
		f->fnewstartaddr = f->fstartaddr + f->fsizemax;
		f->fcuraddr 	    = f->fstartaddr;
		f->fendaddr      = f->fstartaddr + f->fsize;
		f->fwritennum    = 0;
		f->freadnum      = 0;
		
		return 0;
}

void nand_file_int(void)
{
		nand_sem = osSemaphoreCreate(osSemaphore(nand_sem), 1);
    nand_int();
    fileint();
}

void read_file(FILE_STRUCT *f,uint8_t *buf,uint32_t offset,uint32_t len)
{
    uint32_t size;

    size = NAND_BUF_SIZE;

		if(set_readnum(f,offset) == false)
		{
				return;
		}
		
		osSemaphoreWait(nand_sem,osWaitForever);

    while(1)
    {
        if(f->freadnum + size >= len)
        {
            size = len - (f->freadnum - offset);
        }
				else if(f->freadnum == offset)
				{
						size = NAND_BUF_SIZE - offset;
				}
				else
				{
						size = NAND_BUF_SIZE;
				}
				
        fread_w(f, buf + f->freadnum - offset, size);				
        if(f->freadnum - offset == len || f->freadnum == 0)
        {
            break;
        }
				osDelay(1);
    }
		osSemaphoreRelease(nand_sem);
}

void write_file(FILE_STRUCT *f,uint8_t *buf,uint32_t offset,uint32_t len)
{
    uint32_t size;

    size = NAND_BUF_SIZE;

		if(set_writenum(f,offset) == false)
		{
				return;
		}
		
    osSemaphoreWait(nand_sem, osWaitForever);

    while(1)
    {
        if(f->fwritennum + size >= len)
        {
            size = len - (f->fwritennum - offset);
        }
				else if(f->fwritennum == offset)
				{
						size = NAND_BUF_SIZE - offset;
				}
				else
				{
						size = NAND_BUF_SIZE;
				}
				
        fwrite_w(f, buf + f->fwritennum - offset, size);				
        if(f->fwritennum - offset== len || f->fwritennum == 0)
        {
            break;
        }
				osDelay(1);
    }
		osSemaphoreRelease(nand_sem);
}

uint8_t copyfile(char *name1,char *name2)
{
		FILE_STRUCT *f1,*f2;
		uint32_t size = NAND_BUF_SIZE;
	
		f1 = getfileaddr(name1);
		f2 = getfileaddr(name2);
	
		osSemaphoreWait(nand_sem,osWaitForever);
		f1->fsize = f2->fsize;
		if(set_readnum(f2,0) == false)
		{
				return 1;
		}
		
		if(set_writenum(f1,0) == false)
		{
				return 1;
		}
	
    while(1)
    {
        if(f2->freadnum + size >= f2->fsize)
        {
            size = f2->fsize - f2->freadnum;
        }
				else
				{
						size = NAND_BUF_SIZE;
				}
				
        fread_w(f2, nand_buf, size);				
        fwrite_w(f1, nand_buf, size);				
        if(f2->freadnum == 0)
        {
            break;
        }
				osDelay(1);
    }
		osSemaphoreRelease(nand_sem);
		return 0;
}

extern NAND_HandleTypeDef nand;
void nand_task(const void *pdata)
{
    FILE_STRUCT *f,*ftemp;
		uint32_t addrblock;
    NAND_Address	addrblockc;		
	
		while(1)
    {
        if(get_file_update())
        {
						osSemaphoreWait(nand_sem, osWaitForever);
            clear_file_update();
					
						f = get_file_file_addr(pfile[6],pfile[9]);
						if(pfile[6] == f)
						{
								ftemp = pfile[9];
						}
						else if(pfile[9] == f)
						{
								ftemp = pfile[6];
						}
						else
						{
								osSemaphoreRelease(nand_sem);
								break;
						}
						
						addrblockc = SerialAddress_to_CycleAddress(ftemp->fnewstartaddr,&nand);
						addrblockc.column = 0;
						addrblockc.Page = 0;
						addrblockc.Zone = 0;
						addrblock = CycleAddress_to_SerialAddress(&addrblockc,&nand);
						
						get_file_file(&addrblock,ftemp);
						
            set_filedat_addr(ftemp->faddrmin - 0x800); //和下一行代码不可改变顺序
						if(f->funused == 0xaaaaaaaa)
						{
								ftemp->funused = 0;
						}
						else
						{
								ftemp->funused = f->funused + 1;
						}
						if(ftemp->funused >= 63)
						{
								ftemp->funused = 0;
						}
						fwrite_w(ftemp,file_data,2048);
						osSemaphoreRelease(nand_sem);
        }

        osDelay(100);
    }
}
