#include <stdio.h>
#include <string.h>

#include "cmsis_os.h"
#include "stm32f4xx.h"
#include "env.h"
#include "nand_file.h"
#include "cmd.h"

static int check_user_confirm(struct tty *ptty)
{
    char buf[5];

    tty_getstring(ptty, buf, sizeof(buf));

    if(strcmp(buf, "y") && strcmp(buf, "Y"))
    {
        return 0;
    }

    return 1;
}

int do_format(struct tty *ptty)
{
    int i, sn;
    char opt[20] = "/W /LL";

    ptty->printf("确定格式化文件系统?(y/N)\n");

    if(!check_user_confirm(ptty))
    {
        ptty->printf("操作被取消\n");
        return 0;
    }

    ptty->printf("正在格式化...\n");

    if(ptty->cmd_num > 1)
    {
        memset(opt, 0, sizeof(opt));
    }

    sn = 0;

    for(i = 1; i < ptty->cmd_num; i++)
    {
        if(sn + 2 + strlen(ptty->cmd_buf[i]) > sizeof(opt))
        {
            break;
        }

        strcat(opt, " ");
        strcat(opt, ptty->cmd_buf[i]);
    }

		format();
		fileint();
    ptty->printf("格式化完成\n");

    ptty->printf("\n");

    return 0;
}

int do_formatall(struct tty *ptty)
{
    int i, sn;
    char opt[20] = "/W /LL";

    ptty->printf("确定擦除NAND(包括坏块标识)?(y/N)\n");

    if(!check_user_confirm(ptty))
    {
        ptty->printf("操作被取消\n");
        return 0;
    }

    ptty->printf("正在擦除..\n");

    if(ptty->cmd_num > 1)
    {
        memset(opt, 0, sizeof(opt));
    }

    sn = 0;

    for(i = 1; i < ptty->cmd_num; i++)
    {
        if(sn + 2 + strlen(ptty->cmd_buf[i]) > sizeof(opt))
        {
            break;
        }

        strcat(opt, " ");
        strcat(opt, ptty->cmd_buf[i]);
    }

		format_all();
		fileint();
    ptty->printf("nand擦除完成\n");

    ptty->printf("\n");

    return 0;
}
int do_badblockcheck(struct tty *ptty)
{
		uint16_t i = 0,j = 0;	
	
		ptty->printf("该过程会擦除nand所有数据，确定执行操作?(y/N)\n");

    if(!check_user_confirm(ptty))
    {
        ptty->printf("操作被取消\n");
        return 0;
    }
    ptty->printf("坏块检测中(大约需要9分钟)......\n");		
		badblockreint();
    ptty->printf("坏块检测完成\n");		
    ptty->printf("nand格式化中......\n");		
		format();
    ptty->printf("格式化完成\n坏块如下:\n");		
		for(i = 0,j = 0;i < 2048 ; i++)
		{
				if(isbadblock(i))
				{
						ptty->printf("块%d\n",i);		
						j++;
				}
		}
		ptty->printf("共%d个坏块\n",j);		
		
		fileint();

    ptty->printf("\n");

    return 0;
}

extern FILE_STRUCT *pfile[10];
int do_printfileinf(struct tty *ptty)
{
		uint16_t i = 0, j = 0;
		for(i = 0,j = 0;i < 2048 ; i++)
		{
				if(isbadblock(i))
				{
						ptty->printf("块%d\n",i);		
						j++;
				}
		}
		ptty->printf("共%d个坏块\n",j);		
		
		for(i = 0;i < 10 ;i++)
		{
				ptty->printf(pfile[i]->fname);
				ptty->printf("\n");
				ptty->printf("fmd5:");
				for(j = 0;j < 16 ; j++)
				{
						ptty->printf("%x",pfile[i]->fmd5[j]);
				}
				ptty->printf("\n");
				ptty->printf("fsize         :0x%x\n",pfile[i]->fsize);
				ptty->printf("flastsize     :0x%x\n",pfile[i]->flastsize);
				ptty->printf("fsizemax      :0x%x\n",pfile[i]->fsizemax);
				ptty->printf("fwritennum    :0x%x\n",pfile[i]->fwritennum);
				ptty->printf("freadnum      :0x%x\n",pfile[i]->freadnum);
				ptty->printf("fnewstartaddr :0x%x\n",pfile[i]->fnewstartaddr);
				ptty->printf("flaststartaddr:0x%x\n",pfile[i]->flaststartaddr);
				ptty->printf("fstartaddr    :0x%x\n",pfile[i]->fstartaddr);
				ptty->printf("fcuraddr      :0x%x\n",pfile[i]->fcuraddr);
				ptty->printf("fendaddr      :0x%x\n",pfile[i]->fendaddr);
				ptty->printf("faddrmin      :0x%x\n",pfile[i]->faddrmin);
				ptty->printf("faddrmax      :0x%x\n",pfile[i]->faddrmax);
				ptty->printf("funused       :0x%x\n",pfile[i]->funused);
		}
    return 0;
}

cmdt cmd_printfileinf = {"printfileinf", do_printfileinf, "显示文件信息",
                   "显示文件信息\n"
                  };

cmdt cmd_badblockcheck = {"badblockcheck", do_badblockcheck, "nand坏块检测",
                   "badblockcheck\nnand坏块检测\n"
                  };

cmdt cmd_format = {"format", do_format, "格式文件系统",
                   "format\n格式化文件系统\n"
                  };
cmdt cmd_formatall = {"formatall", do_formatall, "擦除nand",
                   "format 擦除nand\n"
                  };

