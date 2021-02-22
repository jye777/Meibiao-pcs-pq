#include <time.h>
#include "time_lib.h"
#include "systime.h"

static const unsigned char days_in_month[] = 
{
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static int is_leap_year(unsigned int year)
{
	return (!(year % 4) && (year % 100)) || !(year % 400);
}

/*
    ����linux mktimeԴ���޸�,��Ӳ���ע��

    daysΪ1970��1��1���������������1970��1��1�����������
     = ((days * 24 + hour) * 60 + min) * 60 + sec

    ��˱������ĺ����ǻ�ȡdays
    leapdaysΪ��Ԫ0�굽����֮ǰ(����������)��������
    ydaysΪ����1��1�����������
    ��: ��Ԫ0�굽1970��1��1�յ�����Ϊ719162��
    days = leapdays + (year0 - 1) * 365 + ydays - 719162    (1)

    �������year, mon, magic
    ��mon0 > 2 ʱ, mon = mon0 - 2, year = year0             (2a)
    ��mon0 <= 2 ʱ, mon = mon0 + 10, year = year0 - 1       (2b)
    magic = 367 * mon / 12

    ���Է���magic����������:
    ��mon0 > 2 ʱ, ydays = magic + day + 28                 (3a)
    ��mon0 <= 2 ʱ, ydays = magic + day - 365 + 28          (3b)

    ����, ����leapdays = year/4 - year/100 + year/400       (4)

    ���(1)(2a)(3a)(4), ��mon0 > 2 ʱ
    days = year/4 - year/100 + year/400 + (year - 1) * 365
        + 367 * mon / 12 + day + 28 - 719162
    ���(1)(2b)(3b)(4), ��mon0 <= 2 ʱ
    days = year/4 - year/100 + year/400 + year * 365
        + 367 * mon / 12 + day - 365 + 28 - 719162

    �õ���ͬ�ĵ�ʽ:
    days = (year/4 - year/100 + year/400 + 367*mon/12 + day)
          + year*365 - 719499

                                        --- wangpeng 2014.03.11
 */
static unsigned long
_mktime(const unsigned int year0, const unsigned int mon0,
        const unsigned int day, const unsigned int hour,
        const unsigned int min, const unsigned int sec)
{
    unsigned int mon = mon0, year = year0;

    /* 1..12 -> 11,12,1..10 */
    if(0 >= (int)(mon -= 2))
    {
        mon += 12;  /* Puts Feb last since it has leap day */
        year -= 1;
    }

    return ((((unsigned long)
              (year / 4 - year / 100 + year / 400 + 367 * mon / 12 + day) +
              year * 365 - 719499
             ) * 24 + hour /* now have hours */
            ) * 60 + min /* now have minutes */
           ) * 60 + sec; /* finally seconds */
}

time_t compute_sec(struct tm *timer)
{
    time_t t;

    t = _mktime(timer->tm_year + 1900,
                timer->tm_mon + 1,
                timer->tm_mday,
                timer->tm_hour,
                timer->tm_min,
                timer->tm_sec);

    return t;
}

/**
 *@brief:       get_time
 *@details:     external function, used to get the time
 *@param[in]    rtc_tm year, month, date, hour, minute, sec
                ms million second
 *@retval:      0 if sucess, 1 if failed
 */
extern int get_time(struct tm *rtc_tm, int *ms);

time_t time(time_t *timer)
{
    time_t t;
    struct tm tm;
    int ms;

    get_time(&tm, &ms);

    t = compute_sec(&tm);

    if(timer != NULL)
    {
        *timer = t;
    }

    return t;
}

/**
 *@brief:       gettimeofday
 *@details:     implement for gettimeofday
 *@param[out]   tv the current time of day
                tz timezone information (not implemented)
 *@retval:      0 on success, -1 on errors
 */
int gettimeofday(struct timeval *tv, struct timezone *tz)
{
    time_t t;
    struct tm rtc_tm;
    int ms;
    int err;

    err = get_time(&rtc_tm, &ms);

    if(err != 0)
    {
        return -1;
    }

    t = compute_sec(&rtc_tm);

    tv->tv_sec = t;
    tv->tv_usec = ms * 1000;

    if(tz != NULL)
    {
        tz->tz_minuteswest = 0;
        tz->tz_minuteswest = 0;
    }

    return 0;
}

/**
 * @brief:      get_complete_tm
 * @details:    get complete tm, include wday, yday
 * @param[in]   simple_tm  simple tm just has year, mon, mday, hour, min, sec
 * @retval:     
 */
void get_complete_tm(struct tm *simple_tm)
{
    time_t t;
    
    t = compute_sec(simple_tm);
    localtime_r(&t, simple_tm);
}

int month_days(unsigned int month, unsigned int year)
{
	return days_in_month[month - 1] + (is_leap_year(year) && month == 2);
}

int is_time_valid(int year, int month, int date, 
                int hour, int min, int sec)
{
    if (is_year_invalid(year)
		|| is_month_invalid(month)
		|| is_date_invalid(year,month,date)
		|| is_hour_invalid(hour)
		|| is_min_invalid(min)
		|| is_sec_invalid(sec))
    {
		return 0;
    }

	return 1;
}

int _is_date_invalid(int year,int month,int date)
{
    if(date < 1 || 
        date > month_days(month, year))
    {
        return 1;
    }

    return 0;
}

