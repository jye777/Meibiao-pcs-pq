#include <stdio.h>
#include <string.h>

#include "cmd.h"

int do_help(struct tty *ptty, cmdt_list *pcmdt_list, int list_size)
{
    int all;
    char *cmdname = NULL;
    cmdt **pcmdt;
    int array_size;
    int i, j;

    if(ptty->cmd_num >= 2)
    {
        cmdname = ptty->cmd_buf[1];
        all = 0;
    }
    else
    {
        all = 1;
    }

    for(i = 0; i < list_size; i++)
    {
        array_size = pcmdt_list[i].cmdt_size;
        pcmdt = pcmdt_list[i].pcmdt;

        for(j = 0; j < array_size; j++)
        {
            if(all)
            {
                ptty->printf("%s - %s\n", pcmdt[j]->name, pcmdt[j]->help_t);
            }
            else
            {
                if(strcmp(cmdname, pcmdt[j]->name) == 0)
                {
                    ptty->printf("\n%s\n", pcmdt[j]->help_f);
                    return 0;
                }
            }
        }
    }

    ptty->printf("\n");

    return 0;
}

int do_main_help(struct tty *ptty)
{
    cmdt_list *pcmdt_list;
    int list_size;
    int rt;

    pcmdt_list = cmd_main_list();
    list_size = cmd_main_listlen();

    rt = do_help(ptty, pcmdt_list, list_size);

    return rt;
}

int do_clear(struct tty *ptty)
{
    ptty->printf("\033[2J\033[0;0H");
    return 0;
}

cmdt cmd_main_help = {"help", do_main_help, "显示命令帮助信息",
                      "help\n"
                      "显示当前所有命令及其简介\n"
                      "help 命令名\n"
                      "显示指定命令的详细帮助信息\n"
                     };

cmdt cmd_clear = {"clear", do_clear, "清屏",
                  "clear\n"
                  "清屏(部分终端不支持)\n"
                 };

