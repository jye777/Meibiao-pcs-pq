#include <stdio.h>
#include <time.h>
#include "cmsis_os.h"
#include "time_lib.h"
#include "bsp.h"
#include "systime.h"
#include "stm32f4xx_hal.h"

systime timeset;
static volatile int get_time_ms = 0;

static void do_timeset(const void *pdata);
osTimerDef(TimeSetDelay, do_timeset);

static unsigned char set_compelete = 0; //时间设置完成

int getms(void)
{
    return get_time_ms;
}

int get_time(struct tm *ptm, int *ms)
{
    RTC_TimeTypeDef Time;
    RTC_DateTypeDef Date;

    HAL_RTC_GetTime(&rtc, &Time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&rtc, &Date, RTC_FORMAT_BIN);

    if(Time.SecondFraction < Time.SubSeconds)
    {
        *ms = 0;
    }
    else
    {
        *ms = 1000 * (Time.SecondFraction - Time.SubSeconds) / (Time.SecondFraction + 1);
    }

    get_time_ms = *ms;

    ptm->tm_sec = Time.Seconds;
    ptm->tm_min = Time.Minutes;
    ptm->tm_hour = Time.Hours;

    /* tm_year set from 1900 */
    ptm->tm_year = 100 + Date.Year;
    /* tm_mon set from 0 */
    ptm->tm_mon = Date.Month - 1;
    ptm->tm_mday = Date.Date;
    ptm->tm_wday = Date.WeekDay - 1;

    return 0;
}

int set_time(struct tm *ptm)
{
    RTC_TimeTypeDef Time;
    RTC_DateTypeDef Date;

#if 0
    printf("set_time %d-%d-%d, %d:%d:%d\n", ptm->tm_year + 1900,
           ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min,
           ptm->tm_sec);
#endif

    get_complete_tm(ptm);

    Time.Hours = ptm->tm_hour;
    Time.Minutes = ptm->tm_min;
    Time.Seconds = ptm->tm_sec;
    Time.SubSeconds = 0;
    Time.TimeFormat = 0;
    Time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    Time.StoreOperation = RTC_STOREOPERATION_RESET;
    HAL_RTC_SetTime(&rtc, &Time, FORMAT_BIN);

    Date.WeekDay = ptm->tm_wday + 1;
    Date.Month = ptm->tm_mon + 1;
    Date.Date = ptm->tm_mday;
    Date.Year = ptm->tm_year - 100;
    HAL_RTC_SetDate(&rtc, &Date, FORMAT_BIN);

    return 0;
}

static void do_timeset(const void *pdata)
{
    struct tm tm;
    int year, month, date, hour, min, sec, ms;
    int valid;

    get_time(&tm, &ms);

    year = tm.tm_year + 1900;
    month = tm.tm_mon + 1;
    date = tm.tm_mday;
    hour = tm.tm_hour;
    min = tm.tm_min;
    sec = tm.tm_sec;

    if(timeset.years == 1)
    {
        year = timeset.year;
        timeset.years = 0;
    }

    if(timeset.mons == 1)
    {
        month = timeset.mon;
        timeset.mons = 0;
    }

    if(timeset.dates == 1)
    {
        date = timeset.date;
        timeset.dates = 0;
    }

    if(timeset.hours == 1)
    {
        hour = timeset.hour;
        timeset.hours = 0;
    }

    if(timeset.mins == 1)
    {
        min = timeset.min;
        timeset.mins = 0;
    }

    if(timeset.secs == 1)
    {
        sec = timeset.sec;
        timeset.secs = 0;
    }

    valid = is_time_valid(year,
                          month,
                          date,
                          hour,
                          min,
                          sec);
    //tty_printf("do_timeset\n");
    if(!valid)
    {
        printf("检查到时间设置不正确\n");
        goto timeset_task_exit;
    }

    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = date;

    tm.tm_hour = hour;
    tm.tm_min = min;
    tm.tm_sec = sec;

    set_time(&tm);

timeset_task_exit:
    set_compelete = 0;
    return;
}

void timeset_immediately(void)
{
    do_timeset(NULL);
}

int timeset_delay(uint32_t millisec)
{
    osStatus status;
    static osTimerId id = NULL;

    if(id == NULL)
    {
        id  = osTimerCreate(osTimer(TimeSetDelay), osTimerOnce, NULL);
    }

    if(id == NULL)
    {
        return 1;
    }

    if(set_compelete == 0) {
        status = osTimerStart(id, millisec);
        set_compelete  = 1;
    }
    if(status != osOK)
    {
        printf("timeset_delay fail: %d\n", status);
        set_compelete  = 0;
        return 1;
    }

    return 0;
}

void time_printf(void)
{
    struct tm cur_time;
    int ms;

    get_time(&cur_time, &ms);

    printf("%d-%d-%d, %d:%d:%d, %dms\n",
           cur_time.tm_year + 1900,
           cur_time.tm_mon + 1,
           cur_time.tm_mday,
           cur_time.tm_hour,
           cur_time.tm_min,
           cur_time.tm_sec,
           ms);
}

