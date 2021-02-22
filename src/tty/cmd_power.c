#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "para.h"
#include "cmd.h"

int do_powerclear(struct tty *ptty)
{
    power_clear();
    return 0;
}

int do_timeclear(struct tty *ptty)
{
    time_clear();
    return 0;
}

int do_showrunstate(struct tty *ptty)
{
	ptty->printf("�豸������ʱ��[%.1fh]\n�豸��������ʱ��[%.1fh]\n����������ʱ��[%.1fh]\n������������ʱ��[%.1fh]\n����λ����[%.0f]\n",
	(CountData.run_time_total/10.0f),(CountData.run_time_now/10.0f),(CountData.on_time_total/10.0f),(CountData.on_time_now/10.0f),CountData.restart_tick);
	return 0;	
}

cmdt cmd_powerclear = {"powerclear", do_powerclear, "���������Ϣ",
                       "powerclear\n"
                       "���������Ϣ\n"
                      };

cmdt cmd_timeclear = {"timeclear", do_timeclear, "���ʱ����Ϣ",
					   "timeclear\n"
					   "���ʱ����Ϣ\n"
					 };

cmdt cmd_showrunstate = {"showrunstate", do_showrunstate, "��ʾ������Ϣ",
					   "showrunstate\n"
					   "��ʾ������Ϣ\n"
					 };
