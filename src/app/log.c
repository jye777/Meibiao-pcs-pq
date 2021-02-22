#define LOG_C

#include <stdio.h>
#include <string.h>

#include "cmsis_os.h"
#include "systime.h"
#include "log.h"
#include "MBserver.h"
#include "crc.h"
#include "nand_file.h"

#define log_dbg(...)

static _log loga_buf;
static _log logb_buf;

static const char loga_name[] = "log.dat";
static const char logb_name[] = "log.dat";
static const char *log_name;

_log *plog, *plog_temp;
volatile uint32_t log_head, log_tail, log_cnt, log_modified;
static uint32_t log_flag;

osSemaphoreId log_sem;
osSemaphoreDef(log_sem);

osThreadId log_update_task_id;
static void log_update_task(const void *pdata);
osThreadDef(log_update_task, osPriorityNormal, 1, 0);

static void log_head_update(void)
{
    uint16_t crc;

    plog->head.head = log_head;
    plog->head.tail = log_tail;
    plog->head.cnt = log_cnt;
    plog->head.flag = log_flag;

    crc = crc16((unsigned char*)&plog->head, sizeof(_log_head));
    plog->crc = crc;
}

static int read_log(const char *name, _log *buf)
{
    FILE_STRUCT *f;
    int err = 0;
    uint16_t crc;

    f = getfileaddr((char *)name);

    if(f == NULL)
    {
        err = 1;
        return err;
    }
		
		if(f->funused == 1)
		{
				return err;
		}
		else if(f->funused == 0xaa)
		{
//				tty_printf("log.dat文件分配存储区全部损坏\n");
				err = 1;
				return 1;
		}
		
    f->fsize = sizeof(_log);
		
    read_file(f,(uint8_t *)buf,0,sizeof(_log));

    crc = crc16((unsigned char*)&buf->head, sizeof(_log_head));

    if(crc != buf->crc)
    {
        return 1;
    }

    return 0;
}

static _log *find_last_buf(void)
{
    unsigned int flag_a, flag_b;

    flag_a = loga_buf.head.flag;
    flag_b = logb_buf.head.flag;

    if(flag_a + 1 == flag_b)
    {
        return &logb_buf;
    }

    return &loga_buf;
}

void log_init(void)
{
    int err1, err2;

    log_sem = osSemaphoreCreate(osSemaphore(log_sem), 1);

    err1 = read_log(loga_name, &loga_buf);
    err2 = read_log(logb_name, &logb_buf);

    if(err1 && err2)
    {
        plog = &loga_buf;
        log_clear();
    }
    else if(err1)
    {
        plog = &logb_buf;
    }
    else if(err2)
    {
        plog = &loga_buf;
    }
    else
    {
        plog = find_last_buf();
    }

    if(plog == &loga_buf)
    {
        log_name = logb_name;
        plog_temp = &logb_buf;
    }
    else
    {
        log_name = loga_name;
        plog_temp = &loga_buf;
    }

    log_update_task_id = osThreadCreate(osThread(log_update_task), NULL);

    log_dbg("log_init : crc 校验正确\n");

    log_head = plog->head.head;
    log_tail = plog->head.tail;
    log_cnt = plog->head.cnt;
    log_flag = plog->head.flag;

    log_dbg("初始化\n");
    log_dbg("log_tail is %d\n", log_tail);
    log_dbg("log_head is %d\n", log_head);
    log_dbg("log_flag is %d\n", log_flag);
    log_dbg("LOG_MAXNUM is %d\n", LOG_MAXNUM);
    log_dbg("log_cnt is %d\n", log_cnt);
}

void log_add(unsigned char type, unsigned short stype)
{
    struct tm tm;
    int i, ms;
    _log_data* loginf;

    loginf = &plog->logdata[0];

    osSemaphoreWait(log_sem, osWaitForever);

    //如果队列已满，删除队列头
    if(log_cnt == LOG_MAXNUM)
    {
        log_head = (log_head + 1) % LOG_MAXNUM;
        log_cnt--;
    }

    //入队
    //记录时间
    get_time(&tm, &ms);

    loginf[log_tail].tm_hour = BCD(tm.tm_hour);
    loginf[log_tail].tm_min = BCD(tm.tm_min);
    loginf[log_tail].tm_sec = BCD(tm.tm_sec);
    loginf[log_tail].tm_year = BCD((tm.tm_year + 1900) % 100);
    loginf[log_tail].tm_mon = BCD(tm.tm_mon + 1);
    loginf[log_tail].tm_date = BCD(tm.tm_mday);

    loginf[log_tail].tm_ms1 = BCD(ms / 100);
    loginf[log_tail].tm_ms2 = BCD(ms % 100);
    //记录事件、相关设备
    loginf[log_tail].type = type;
    loginf[log_tail].stype = stype & 0xff;

    log_tail = (log_tail + 1) % LOG_MAXNUM;
    log_cnt++;
    log_modified = 1;

    for(i = 0; i < MAX_CONN_NUM; i++)
    {
        if(mbser_inf[i].sendlog_inf.num == LOG_MAXNUM)
        {
            mbser_inf[i].sendlog_inf.tail++;

            if(mbser_inf[i].sendlog_inf.tail >= LOG_MAXNUM)
            {
                mbser_inf[i].sendlog_inf.tail = 0;
            }
        }
        else
        {
            mbser_inf[i].sendlog_inf.num++;
        }
    }

    osSemaphoreRelease(log_sem);
}

int log_clear(void)
{
    int i;

    osSemaphoreWait(log_sem, osWaitForever);

    memset(&plog->head, 0, sizeof(_log_head));

    log_tail = 0;
    log_head = 0;
    log_cnt = 0;
    log_flag = 0;

    for(i = 0; i < MAX_CONN_NUM; i++)
    {
        mbser_inf[i].sendlog_inf.tail = 0;
        mbser_inf[i].sendlog_inf.num = 0;
    }

    log_modified = 1;

    osSemaphoreRelease(log_sem);

    return 0;
}

static void log_update_flash(void)
{
    FILE_STRUCT *f;

    osSemaphoreWait(log_sem, osWaitForever);

    if(!log_modified)
    {
        osSemaphoreRelease(log_sem);
        return;
    }

    log_flag++;
    log_head_update();
    memcpy(plog_temp, plog, sizeof(_log));
    log_modified = 0;

    osSemaphoreRelease(log_sem);


    f = getfileaddr((char *)log_name);

    if(f == NULL)
    {
        return ;
    }
		
    f->fsize = sizeof(_log);
		
    write_file(f,(uint8_t *)plog_temp,0,sizeof(_log));

    if(log_name == loga_name)
    {
        log_name = logb_name;
    }
    else
    {
        log_name = loga_name;
    }
}

static void log_update_task(const void *pdata)
{
    while(1)
    {
        log_update_flash();
        osDelay(10);
    }
}
