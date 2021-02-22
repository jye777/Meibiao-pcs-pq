#include <string.h>
#include <stdlib.h>

#include "log.h"
#include "cmd.h"

int do_logtest(struct tty *ptty)
{
    int type, subtype, num;

    if(ptty->cmd_num < 3)
    {
        ptty->printf("�����������\n");
        return 1;
    }

    type = atoi(ptty->cmd_buf[1]);
    subtype = atoi(ptty->cmd_buf[2]);

    if(ptty->cmd_num > 3)
    {
        num = atoi(ptty->cmd_buf[3]);
    }
    else
    {
        num = 1;
    }

    while(num--)
    {
        log_add(type, subtype);
    }

    ptty->printf("tail:%d, head:%d, cnt:%d\n",
                 log_tail, log_head, log_cnt);

    return 0;
}

int do_logclear(struct tty *ptty)
{
    log_clear();

    return 0;
}

int do_showlog(struct tty *ptty)
{
    uint32_t num, head;
    _log_data *data;

    num = log_cnt;
    head = log_head;
    data = plog->logdata;

    ptty->printf("num: %d\n", num);

    while(num--)
    {
        if(head == LOG_MAXNUM)
        {
            head = 0;
        }

        ptty->printf("type: %d, subtype: %d, %02x/%02x/%02x %02x:%02x:%02x %x%02xms\n",
                     data[head].type,
                     data[head].stype,
                     data[head].tm_year,
                     data[head].tm_mon,
                     data[head].tm_date,
                     data[head].tm_hour,
                     data[head].tm_min,
                     data[head].tm_sec,
                     data[head].tm_ms1,
                     data[head].tm_ms2
                    );

        head++;
    }

    return 0;
}

cmdt cmd_logtest = {"logtest", do_logtest, "������־���ڲ���",
                    "logtest type subtype\n"
                    "�����趨����1����־���ڲ���\n"
                    "logtest type subtype num\n"
                    "�����趨����ָ����������־���ڲ���\n"
                   };

cmdt cmd_logclear = {"logclear", do_logclear, "���������־",
                     "logclear\n"
                     "���������־\n"
                    };

cmdt cmd_showlog = {"showlog", do_showlog, "��ʾ��־",
                    "showlog\n"
                    "��ʾ��־\n"
                   };