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

    ptty->printf("ȷ����ʽ���ļ�ϵͳ?(y/N)\n");

    if(!check_user_confirm(ptty))
    {
        ptty->printf("������ȡ��\n");
        return 0;
    }

    ptty->printf("���ڸ�ʽ��...\n");

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
    ptty->printf("��ʽ�����\n");

    ptty->printf("\n");

    return 0;
}

int do_formatall(struct tty *ptty)
{
    int i, sn;
    char opt[20] = "/W /LL";

    ptty->printf("ȷ������NAND(���������ʶ)?(y/N)\n");

    if(!check_user_confirm(ptty))
    {
        ptty->printf("������ȡ��\n");
        return 0;
    }

    ptty->printf("���ڲ���..\n");

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
    ptty->printf("nand�������\n");

    ptty->printf("\n");

    return 0;
}
int do_badblockcheck(struct tty *ptty)
{
		uint16_t i = 0,j = 0;	
	
		ptty->printf("�ù��̻����nand�������ݣ�ȷ��ִ�в���?(y/N)\n");

    if(!check_user_confirm(ptty))
    {
        ptty->printf("������ȡ��\n");
        return 0;
    }
    ptty->printf("��������(��Լ��Ҫ9����)......\n");		
		badblockreint();
    ptty->printf("���������\n");		
    ptty->printf("nand��ʽ����......\n");		
		format();
    ptty->printf("��ʽ�����\n��������:\n");		
		for(i = 0,j = 0;i < 2048 ; i++)
		{
				if(isbadblock(i))
				{
						ptty->printf("��%d\n",i);		
						j++;
				}
		}
		ptty->printf("��%d������\n",j);		
		
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
						ptty->printf("��%d\n",i);		
						j++;
				}
		}
		ptty->printf("��%d������\n",j);		
		
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

cmdt cmd_printfileinf = {"printfileinf", do_printfileinf, "��ʾ�ļ���Ϣ",
                   "��ʾ�ļ���Ϣ\n"
                  };

cmdt cmd_badblockcheck = {"badblockcheck", do_badblockcheck, "nand������",
                   "badblockcheck\nnand������\n"
                  };

cmdt cmd_format = {"format", do_format, "��ʽ�ļ�ϵͳ",
                   "format\n��ʽ���ļ�ϵͳ\n"
                  };
cmdt cmd_formatall = {"formatall", do_formatall, "����nand",
                   "format ����nand\n"
                  };

