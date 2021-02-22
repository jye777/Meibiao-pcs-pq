#include <stdio.h>
#include <string.h>

#include "cmsis_os.h"
#include "stm32f4xx.h"
#include "rl_net.h"
#include "env.h"
#include "cmd.h"
#include "nand_file.h"
#include "flashdata.h"
#include "storedata.h"

extern int do_reset(struct tty *ptty);

int do_Specified_def(struct tty *ptty)
{
    char *filename; //fn[15];
    char md5_def_str[33],*str;
    //uint8_t type;

    if(ptty->cmd_num < 2)
    {
        ptty->printf("�����������\n");
        return 0;
    }

    filename = ptty->cmd_buf[1];
    if(strcmp(filename, "main") == 0)
    {
				copyfile("main_def.bin","main_cur.bin");
				str = getenv("main_cur_md5");						
				strcpy(md5_def_str,str);
				setenv("main_def_md5", md5_def_str);
				ptty->printf("mainĬ�ϳ���ָ�����\n");		
		}
    else if(strcmp(filename, "fpga") == 0)
    {
				copyfile("fpga_def.bin","fpga_cur.bin");
				str = getenv("fpga_cur_md5");						
				strcpy(md5_def_str,str);
				setenv("fpga_def_md5", md5_def_str);
				ptty->printf("fpgaĬ�ϳ���ָ�����\n");		
    }
		else
		{
        ptty->printf("�̼����ʹ���\n");
		}
    return 0;
}

int do_fireware(struct tty *ptty)
{
    char *filename,fn[15];
    uint8_t type;

    if(ptty->cmd_num < 2)
    {
        ptty->printf("�����������\n");
        return 0;
    }

    filename = ptty->cmd_buf[1];

    /* Only support the main program now! */
    if(strcmp(filename, "main_cur") == 0)
    {
        strcpy(fn,"main_cur.bin");
        type = 2;
        setenv("main_update", "1");
    }

    else if(strcmp(filename, "main_last") == 0)
    {
        strcpy(fn,"main_last.bin");
        type = 2;
        setenv("main_update", "2");
    }

    else if(strcmp(filename, "main_def") == 0)
    {
        strcpy(fn,"main_def.bin");
        type = 2;
        setenv("main_update", "3");
    }

    else if(strcmp(filename, "fpga_cur") == 0)
    {
        filename = "fpga_cur.bin";
        type = 3;
        ptty->printf("�ݲ�֧�ָ������ļ�\n");
        return 0;
    }

    else if(strcmp(filename, "fpga_last") == 0)
    {
        filename = "fpga_last.bin";
        type = 3;
        ptty->printf("�ݲ�֧�ָ������ļ�\n");
        return 0;
    }

    else if(strcmp(filename, "fpga_def") == 0)
    {
        filename = "fpga_def.bin";
        type = 3;
        ptty->printf("�ݲ�֧�ָ������ļ�\n");
        return 0;
    }
    else
    {
        ptty->printf("�ݲ�֧�ָ������ļ�\n");
        return 0;
    }

    saveenv();
		save_sdata();
//		flashdata_update_flash();
    osDelay(100);
    if(type == 2)
    {
        ptty->printf("�Զ���λ����ɸ���\n");
        do_reset(ptty);
    }
    else if(type == 3)
    {
        ptty->printf("������İ������ϵ磬����ɸ���\n");
        do_reset(ptty);
    }
    return 0;
}

cmdt cmd_fireware = {"run", do_fireware, "ѡ�����汾",
                     "run filename [serverip]\n����XX�汾����\n"
                    };
cmdt cmd_Specified_def = {"sdf", do_Specified_def, "ѡ�����汾",
                     "sdf filewaretype\nָ����ǰ����XX����ΪĬ�ϳ���\n"
                    };

