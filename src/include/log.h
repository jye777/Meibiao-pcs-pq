#ifndef LOG_H_
#define LOG_H_

#include <stdint.h>
#include "cmsis_os.h"
#include "log_def.h"

#define LOG_OFFSET      0//(0x1e000)    //nvsrambase+120k
#define LOG_MAXNUM      5120

//log
typedef struct
{
    uint8_t type;
    uint8_t stype;
    uint8_t tm_year;
    uint8_t tm_mon;
    uint8_t tm_date;
    uint8_t tm_hour;
    uint8_t tm_min;
    uint8_t tm_sec;
    uint8_t tm_ms1;
    uint8_t tm_ms2;
} _log_data;

typedef struct
{
    uint32_t tail;
    uint32_t head;
    uint32_t cnt;
    uint32_t flag;
} _log_head;

typedef struct
{
    uint16_t crc;
    _log_head head;
    _log_data logdata[LOG_MAXNUM];
} _log;

#define BCD(c) (((((c)/10)&0xf)<<4) | (((c)%10)&0xf))
#define BCDGET(c)	(((((c)>>4)&0xf)*10) + ((c)&0xf))

extern _log *plog;
extern volatile uint32_t log_head, log_tail, log_cnt;
extern osSemaphoreId log_sem;

void log_init(void);
void log_add(uint8_t type, uint16_t stype);
int log_clear(void);

#endif /*LOG_H_*/
