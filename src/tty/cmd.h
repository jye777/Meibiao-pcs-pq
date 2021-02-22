#ifndef _CMD_H_
#define _CMD_H_

#include "tty.h"

typedef int (* cmdhandle)(struct tty *);

typedef struct _cmdt
{
    char *name;
    cmdhandle handle;
    char *help_t;
    char *help_f;
} cmdt;

typedef struct _cmdt_list
{
    cmdt **pcmdt;
    int cmdt_size;
} cmdt_list;

cmdt_list *cmd_main_list(void);
int cmd_main_listlen(void);

#endif
