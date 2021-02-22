#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "time_lib.h"
#include "cmd.h"
#include "systime.h"

static char *week_day_name[7] =
{
    "日", "一", "二", "三", "四", "五", "六"
};

int do_gettime(struct tty *ptty)
{
    time_t timer;
    int ms;
    struct tm cur_time;
    char *daybuf;

    timer = time(NULL);
    ms = getms();

    localtime_r(&timer, &cur_time);

    if(cur_time.tm_wday < 7)
    {
        daybuf = week_day_name[cur_time.tm_wday];
    }
    else
    {
        daybuf = "";
    }

    ptty->printf("%d年%d月%d日, 周%s, %d点%d分%d秒%d毫秒\n",
                 cur_time.tm_year + 1900,
                 cur_time.tm_mon + 1,
                 cur_time.tm_mday,
                 daybuf,
                 cur_time.tm_hour,
                 cur_time.tm_min,
                 cur_time.tm_sec,
                 ms);

    return 0;
}

struct time_set
{
    char *name;
    int min;
    int max;
};

struct time_set ts[6] =
{
    {"年",   1970, 9999},
    {"月",   1,    12},
    {"日",   1,    31},
    {"小时", 0,    23},
    {"分",   0,    59},
    {"秒",   0,    59},
};

struct stime_value
{
    int year;
    int month;
    int date;
    int hour;
    int min;
    int sec;
};

union uvalue
{
    int value[6];
    struct stime_value svalue;
};

static int do_set_onetime(struct tty *ptty,
                          struct time_set *ts,
                          int *value)
{
    int v;
    char timebuf[8];

    ptty->printf("请输入%s\n", ts->name);

    if(tty_getstring(ptty, timebuf, sizeof(timebuf)))
    {
        return 1;
    }

    v = atoi(timebuf);

    if(v < ts->min)
    {
        ptty->printf("输入的值不能小于%d\n", ts->min);
        return 1;
    }

    if(v > ts->max)
    {
        ptty->printf("输入的值不能大于%d\n", ts->max);
        return 1;
    }

    *value = v;

    ptty->printf("您输入的是:%d\n", v);

    return 0;
}

int do_settime(struct tty *ptty)
{
    struct tm rtc_time, *ptime;
    union uvalue tv;
    int i, valid;

    ptime = &rtc_time;

    for(i = 0; i < 6; i++)
    {
        if(do_set_onetime(ptty, &ts[i], &tv.value[i]))
            return 1;
    }

    valid = is_time_valid(tv.svalue.year,
                          tv.svalue.month,
                          tv.svalue.date,
                          tv.svalue.hour,
                          tv.svalue.min,
                          tv.svalue.sec);

    if(!valid)
    {
        ptty->printf("检查到时间设置不正确\n");
        return 1;
    }

    ptty->printf("开始设置...\n");

    ptime->tm_year = tv.svalue.year - 1900;
    ptime->tm_mon = tv.svalue.month - 1;
    ptime->tm_mday = tv.svalue.date;
    ptime->tm_hour = tv.svalue.hour;
    ptime->tm_min = tv.svalue.min;
    ptime->tm_sec = tv.svalue.sec;

    set_time(ptime);

    ptty->printf("设置完成\n");

    return 0;
}

cmdt cmd_gettime = {"gettime", do_gettime, "显示时间",
                    "gettime\n"
                    "显示当前时间\n"
                   };

cmdt cmd_settime = {"settime", do_settime, "设置时间",
                    "settime\n"
                    "按提示设置当前时间(只支持24小时制)\n"
                   };

