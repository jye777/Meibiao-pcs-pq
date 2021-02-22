/**
 * @file			systime.h
 * @brief			
 * @author			wangpeng
 * @date			2014/8/29
 * @version			Initial Draft
 * @par				Copyright (C),  2013-2023, SCNEE
 * @par History:
 * 1.Date: 			2014/8/29
 *   Author: 			wangpeng
 *   Modification: 		Created file
*/

#ifndef SYSTIME_H
#define SYSTIME_H

#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

typedef struct _systime{
	uint8_t years;
	uint16_t year;
	uint8_t mons;
	uint8_t mon;
	uint8_t dates;
	uint8_t date;
	uint8_t hours;
	uint8_t hour;
	uint8_t mins;
	uint8_t min;
	uint8_t secs;
	uint8_t sec;
}systime;

extern systime timeset;

int get_time(struct tm *ptm,int * ms);
int set_time(struct tm *ptm);
int getms(void);
void timeset_immediately(void);
int timeset_delay(uint32_t millisec);
void time_printf(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYSTIME_H */

