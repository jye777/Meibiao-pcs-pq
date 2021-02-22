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
	ptty->printf("设备总运行时间[%.1fh]\n设备连续运行时间[%.1fh]\n程序总运行时间[%.1fh]\n程序连续运行时间[%.1fh]\n程序复位次数[%.0f]\n",
	(CountData.run_time_total/10.0f),(CountData.run_time_now/10.0f),(CountData.on_time_total/10.0f),(CountData.on_time_now/10.0f),CountData.restart_tick);
	return 0;	
}

cmdt cmd_powerclear = {"powerclear", do_powerclear, "清除电量信息",
                       "powerclear\n"
                       "清除电量信息\n"
                      };

cmdt cmd_timeclear = {"timeclear", do_timeclear, "清除时间信息",
					   "timeclear\n"
					   "清除时间信息\n"
					 };

cmdt cmd_showrunstate = {"showrunstate", do_showrunstate, "显示运行信息",
					   "showrunstate\n"
					   "显示运行信息\n"
					 };
