#include <stdio.h>
#include <stdlib.h>
#include "cmd.h"
#include "cmsis_os.h"
#include "modbus_master.h"

int do_meter(struct tty *ptty)
{
    int idx;
    meter_data_t *meter_data;

    if(ptty->cmd_num < 2)
    {
        ptty->printf("����ָ�����ܱ����\n");
        return 0;
    }

    idx = atoi(ptty->cmd_buf[1]);

    switch(idx)
    {
    case 0:
        meter_data = &meter0_data;
        break;
    case 1:
        meter_data = &meter1_data;
        break;
    default:
        ptty->printf("��������\n");
        return 0;
    }

    return 0;
}

cmdt cmd_meter = {"meter", do_meter, "��ʾ���ܱ�����",
                  "meter idx\n"
                  "��ʾָ�����ܱ�����(idx = 0, 1)\n"
                 };

