#include <stdio.h>
#include <string.h>

#include "cmd.h"

cmdhandle shell_find_cmd(struct tty *ptty, cmdt_list *pcmdt_list, int size)
{
    int i, j;

    for(i = 0; i < size; i++)
    {
        if(pcmdt_list[i].pcmdt == NULL)
        {
            continue;
        }

        for(j = 0; j < pcmdt_list[i].cmdt_size; j++)
        {
            if(pcmdt_list[i].pcmdt[j]->name == NULL)
            {
                continue;
            }

            if(strcmp(pcmdt_list[i].pcmdt[j]->name, ptty->cmd_buf[0]) == 0)
            {
                return pcmdt_list[i].pcmdt[j]->handle;
            }
        }
    }

    return NULL;
}

int shell_run_cmd(struct tty * ptty, cmdhandle handle)
{
    if (handle == NULL)
    {
        ptty->printf("Î´¶¨ÒåÃüÁî\n");
        return -1;
    }
    else
    {
        return handle(ptty);
    }
}

int shell_run(struct tty *ptty)
{
    cmdhandle handle;

    handle = shell_find_cmd(ptty,
                            cmd_main_list(),
                            cmd_main_listlen());

    return shell_run_cmd(ptty, handle);
}

