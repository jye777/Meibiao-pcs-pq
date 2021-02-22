#include <stdio.h>
#include <string.h>

#include "cmd.h"
#include "env.h"


int do_printenv(struct tty *ptty)
{
    char *v = (char *)&env_cache->data[0];

    while(*v != '\0' || *(v+1) != '\0')
    {
        if(*v != '\0')
        {
            ptty->printf("%c", *v);
        }
        else
        {
            ptty->printf("\n");
        }

        v++;
    }

    ptty->printf("\n");

    return 0;
}

int do_setenv(struct tty *ptty)
{
    int err = 0;

    char *envname, *value;

    if(ptty->cmd_num < 2)
    {
        ptty->printf("�����������\n");
    }

    envname = ptty->cmd_buf[1];

    if(ptty->cmd_num > 2)
    {
        value = ptty->cmd_buf[2];
    }
    else
    {
        value = "";
    }

    err = setenv(envname, value);

    if(err)
    {
        ptty->printf("���û�������ʧ��\n");
    }
    else
    {
        ptty->printf("�������������õ�������\n");
    }

    return 0;
}

int do_saveenv(struct tty *ptty)
{
    int err = 0;

    err = saveenv();

    if(err)
    {
        ptty->printf("������������ʧ��\n");
    }
    else
    {
        ptty->printf("���������ѱ���\n");
    }

    return 0;
}

int do_clearenv(struct tty *ptty)
{
    memset(env_cache->data,0,ENV_MAXSIZE);
    env_crc_update();
    saveenv();
    return 0;
}

cmdt cmd_printenv = {"printenv", do_printenv, "��ʾ��������",
                     "printenv\n"
                     "��ʾ��������\n"
                    };

cmdt cmd_setenv = {"setenv", do_setenv, "���û�������",
                   "setenv\n"
                   "���û�������\n"
                  };

cmdt cmd_saveenv = {"saveenv", do_saveenv, "���滷������",
                    "saveenv\n"
                    "���滷������\n"
                   };
cmdt cmd_clearenv = {"clearenv", do_clearenv, "��ջ�������",
                     "clearenv\n"
                     "��ջ�������\n"
                    };