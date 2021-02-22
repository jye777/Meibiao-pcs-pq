#ifndef TIME_LIB_H
#define TIME_LIB_H

typedef long int __time_t;
typedef int __suseconds_t;

#ifndef timeval

struct timeval
{
	__time_t tv_sec;		/* Seconds.  */
	__suseconds_t tv_usec;	/* Microseconds.  */
};

#endif

struct timezone
{
	int tz_minuteswest;	/* Minutes west of GMT.  */
	int tz_dsttime;		/* Nonzero if DST is ever in effect.  */
};

#define is_year_invalid(year)       ((year) < 1970)

#define is_month_invalid(month)     ((month) > 12 || (month) < 1)

#define is_date_invalid(year, month, date)  \
            _is_date_invalid(year, month, date)
                
#define is_hour_invalid(hour)       ((unsigned int)(hour) >= 24)

#define is_min_invalid(min)         ((unsigned int)(min) >= 60)

#define is_sec_invalid(sec)         ((unsigned int)(sec) >= 60)

/* linux only, so must be declared */
int gettimeofday(struct timeval *tv, struct timezone *tz);

int _is_date_invalid(int year, int month, int date);
int is_time_valid(int year, int month, int date, 
                int hour, int min, int sec);

void get_complete_tm(struct tm *simple_tm);
extern time_t compute_sec(struct tm *timer);

#endif

