#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "time_lib.h"
#include "cmd.h"
#include "systime.h"

static char *week_day_name[7] =
{
    "��", "һ", "��", "��", "��", "��", "��"
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

    ptty->printf("%d��%d��%d��, ��%s, %d��%d��%d��%d����\n",
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
    {"��",   1970, 9999},
    {"��",   1,    12},
    {"��",   1,    31},
    {"Сʱ", 0,    23},
    {"��",   0,    59},
    {"��",   0,    59},
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

    ptty->printf("������%s\n", ts->name);

    if(tty_getstring(ptty, timebuf, sizeof(timebuf)))
    {
        return 1;
    }

    v = atoi(timebuf);

    if(v < ts->min)
    {
        ptty->printf("�����ֵ����С��%d\n", ts->min);
        return 1;
    }

    if(v > ts->max)
    {
        ptty->printf("�����ֵ���ܴ���%d\n", ts->max);
        return 1;
    }

    *value = v;

    ptty->printf("���������:%d\n", v);

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
        ptty->printf("��鵽ʱ�����ò���ȷ\n");
        return 1;
    }

    ptty->printf("��ʼ����...\n");

    ptime->tm_year = tv.svalue.year - 1900;
    ptime->tm_mon = tv.svalue.month - 1;
    ptime->tm_mday = tv.svalue.date;
    ptime->tm_hour = tv.svalue.hour;
    ptime->tm_min = tv.svalue.min;
    ptime->tm_sec = tv.svalue.sec;

    set_time(ptime);

    ptty->printf("�������\n");

    return 0;
}

cmdt cmd_gettime = {"gettime", do_gettime, "��ʾʱ��",
                    "gettime\n"
                    "��ʾ��ǰʱ��\n"
                   };

cmdt cmd_settime = {"settime", do_settime, "����ʱ��",
                    "settime\n"
                    "����ʾ���õ�ǰʱ��(ֻ֧��24Сʱ��)\n"
                   };

