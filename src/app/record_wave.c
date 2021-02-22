/**
 * @file record_wave.c
 *
 * @brief Real-time wave and Fault record module
 *
 * @date 2012/4/13
 *
 */
#define RECORD_WAVE_C

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "cmsis_os.h"
#include "stm32f4xx_hal.h"
#include "log.h"
#include "fpga.h"
#include "record_wave.h"
#include "sys.h"
#include "MBSERVER.h"
#include "para.h"
#include "systime.h"
#include "tty.h"
#include "tty_net.h"
#include "telnet.h"
#include "wave_file.h"

osSemaphoreId savefinf_sem = NULL;
osSemaphoreDef(savefinf_sem);

//实时波形数据缓冲区
short rt_cv[CHANNEL_MAX][VOLT_FREQ*RT_BUF_SEC];//电压电流
extern void thread_delete_selfmb(void);

volatile unsigned int rw_int = 0;
volatile unsigned int rtwq_tail, rtwq_head, ftwq_tail, ftwq_head;
volatile unsigned char wavedata_clear = 0;
volatile unsigned char waveinf = 0; //for dbg
WAVEFILE wave;
osThreadId tid_record_wave;
osThreadDef(record_wave_task, osPriorityNormal, 1, 0);

static void memcpy_1(short* sbuf, short* channelbuf, int len);

void (* wave_fun[CHANNEL_MAX])(short*, short*, int) = {
    memcpy_1,
    memcpy_1,
    memcpy_1,
    memcpy_1,
    memcpy_1,
    memcpy_1,
    memcpy_1,
    memcpy_1,
    memcpy_1
};

//void savefsdata_task(void* pdata);
osThreadId	savefsdata_task_id;
void savefsdata_task(const void *pdada);
osThreadDef(savefsdata_task, osPriorityNormal, 1, 0);


extern int mbrs_transcurwave(_mbser_inf* msrinf, unsigned char* data, \
                             unsigned char* resp, unsigned short len);
extern unsigned long crc32_get(unsigned long crc, unsigned char* buf, unsigned int len);
extern volatile unsigned short *fpga;

static void record_wave(void)
{
    uint32_t i, ch, tail;
    volatile uint16_t temp;

    rw_int++;

    if(wavedata_clear == 1)
    {
        for(i = 0; i < 256; i++)
        {
            temp = fpga_read(Addr_Param503);
			temp = fpga_read(Addr_Param504);
            temp = fpga_read(Addr_Param505);
            temp = fpga_read(Addr_Param506);
            temp = fpga_read(Addr_Param507);
            temp = fpga_read(Addr_Param508);
            temp = fpga_read(Addr_Param509);
            temp = fpga_read(Addr_Param510);
            temp = fpga_read(Addr_Param511);
        }

        wavedata_clear = 0;
        return;
    }

    for(i = 0, tail = rtwq_tail; i < 128; i++)
    {
        for(ch = 0; ch < 9; ch++)
        {
					rt_cv[ch][tail]=fpga[Addr_Param503 + ch];
//test
//					rt_cv[ch][tail] = 1200;
//					rt_cv[ch][tail] = 4*i + 500;
//					if(i < 64)
//					{
//							rt_cv[ch][tail] = 500;
//					}
//					else
//					{
//							rt_cv[ch][tail] = 1200;
//					}		
//		osThreadYield();
//		osThreadYield();
//		osThreadYield();
//		osThreadYield();
//		osThreadYield();


        }
        tail = tail + 1 >= RTWQ_MAX_SIZE ? 0 : tail + 1;
				if((i + 1)%20 == 0)
				{
					osDelay(1);
				}
    }

    rtwq_tail = (rtwq_tail + 128) >= RTWQ_MAX_SIZE ? (rtwq_tail + 128 - RTWQ_MAX_SIZE) : (rtwq_tail + 128);
}

void record_wave_task(const void *pdata)
{
    while(1)
    {
        /* 读128个数据(第一次读256个以清空FIFO) */
        if(HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_13) == GPIO_PIN_RESET)
        {
            record_wave();
						if(HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_13) == GPIO_PIN_RESET)
						{
								record_wave();
						}
				}

        /* 如果FIFO里面还有128个数据则再读一次(打断点后FIFO溢出) */

				osDelay(1);
				//        osSignalWait(1, osWaitForever);
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_13) == GPIO_PIN_RESET)
    {
        osSignalSet(tid_record_wave, 1);
    }
}


void disable_fpga_wave(void)
{
    uint16_t cmd1;

    cmd1=fpga_read(Addr_Param111);
    cmd1 = RESETBIT(cmd1,15);
    fpga_write(Addr_Param111,cmd1);
}

static void enable_fpga_wave(void)
{
    uint16_t value;

    osDelay(10);
    value = fpga_read(Addr_Param111);
    fpga_write(Addr_Param111, value & 0x7fff);
}

void record_wave_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* 实时波形初始化 */
    rtwq_tail = 0;
    rtwq_head = 0;
    memset(rt_cv, 0, sizeof(rt_cv));

#ifndef BOARD_ARMFLY
    /* Configure FPGA_INT_Pin:PG13 */
    GPIO_InitStruct.Pin = GPIO_PIN_13;
//    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
#endif

    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

    wavedata_clear = 1;

    /* FIFO读取任务初始化 */
    wave_file_int();
    tid_record_wave = osThreadCreate(osThread(record_wave_task), NULL);

//    enable_fpga_wave();

#if 0
    fault_save_init();
#endif
}

//原始数据
static void memcpy_1(short* sbuf, short* channelbuf, int len)
{
    int i;
    unsigned char* temp;
    short value;
#if MPRDBG_EN
    short max, min;
    unsigned int tol = 0;
    max = min = channelbuf[0];
#endif

    if(len <= 0)
    {
        return;
    }

    for(i = 0; i < len; i++)
    {
        value = (channelbuf[i]);

#if MPRDBG_EN
        if(waveinf)
        {
            tol += value;
            if(value > max)
            {
                max = value;
            }

            if(value < min)
            {
                min = value;
            }
        }
#endif
        temp = (unsigned char*)&sbuf[i];
        *temp = (unsigned short)value >> 8;
        *(temp + 1) = (unsigned short)value & 0xff;
    }

#if MPRDBG_EN

    if(waveinf)
    {
        tty_printf("max: %d, min: %d, tol: %d, len: %d\n", max, min, tol, len);
    }

#endif
}

unsigned int copyrtwave(short* sbuf, unsigned short channel, unsigned int len, int tail)
{
    unsigned int saddr;
    unsigned int qtail;
    short* channelbuf;
    void (* memcpy_f)(short*, short*, int);

    if(tail >= 0)
    {
        qtail = tail;
    }
    else
    {
        qtail = rtwq_tail;
    }

    switch(channel)
    {
    case 0:
        channelbuf = rt_cv[0];
        memcpy_f = wave_fun[0];
        break;
    case 1:
        channelbuf = rt_cv[1];
        memcpy_f = wave_fun[1];
        break;
    case 2:
        channelbuf = rt_cv[2];
        memcpy_f = wave_fun[2];
        break;
    case 3:
        channelbuf = rt_cv[3];
        memcpy_f = wave_fun[3];
        break;
    case 4:
        channelbuf = rt_cv[4];
        memcpy_f = wave_fun[4];
        break;
    case 5:
        channelbuf = rt_cv[5];
        memcpy_f = wave_fun[5];
        break;
    case 6:
        channelbuf = rt_cv[6];
        memcpy_f = wave_fun[6];
        break;
    case 7:
        channelbuf = rt_cv[7];
        memcpy_f = wave_fun[7];
        break;
    case 8:
        channelbuf = rt_cv[8];
        memcpy_f = wave_fun[8];
        break;
    default:
        tty_printf("通道选择错误.\n");
        return 0;
    }

    //tty_printf("tail:%d,",rtwq_tail);
    if(len > RTWQ_MAX_SIZE)
    {
        len = RTWQ_MAX_SIZE;
    }

    if(qtail >= len)
    {
        saddr = qtail - len;
        memcpy_f(sbuf, &channelbuf[saddr], len);
    }
    else
    {
        saddr = RTWQ_MAX_SIZE - len + qtail;
        memcpy_f(sbuf, &channelbuf[saddr], (len - qtail));
        memcpy_f(sbuf + (len - qtail), &channelbuf[0], qtail);
    }

    return len << 1;
}

int fsdata_head_update(void)
{
    unsigned char* crc_buf;
    unsigned int crc_len;

    /* 添加头和CRC信息 */
    crc_buf = (unsigned char*)((unsigned int)&wave.fault.brief + 12);
    crc_len = sizeof(wave.fault.brief) - 12;
    wave.fault.brief.head = FS_BRIEF_HEAD;
    wave.fault.brief.len = crc_len;
    wave.fault.brief.crc = crc32_get(0, crc_buf, crc_len);

    crc_buf = (unsigned char*)((unsigned int)&wave.fault.para + 12);
    crc_len = sizeof(wave.fault.para) - 12;
    wave.fault.para.head = FS_PARA_HEAD;
    wave.fault.para.len = crc_len;
    wave.fault.para.crc = crc32_get(0, crc_buf, crc_len);

    crc_buf = (unsigned char*)((unsigned int)&wave.fault.wave + 12);
    crc_len = sizeof(wave.fault.wave) - 12;
    wave.fault.wave.head = FS_WAVE_HEAD;
    wave.fault.wave.len = crc_len;
    wave.fault.wave.crc = crc32_get(0, crc_buf, crc_len);

    return 0;
}

int savefinf(int failtype, unsigned char* event)
{
    unsigned int tail;
    struct tm tm;
    int ms;

    if(savefinf_sem == NULL)
    {
        savefinf_sem = osSemaphoreCreate(osSemaphore(savefinf_sem), 1);
    }
    osSemaphoreWait(savefinf_sem, osWaitForever);
//    if(faultinf.enable == 0)
//    {
//				osSemaphoreRelease(savefinf_sem);
//        record_dbg("save fault: record has been disabled.\n");
//        return 1;
//    }

    /* 如果故障未被清除则不重新记录 */
    if((faultinf.event != NULL) && (*faultinf.event == 1))
    {
        osSemaphoreRelease(savefinf_sem);
        record_dbg("save fault: fault has not been cleared.\n");
        return 1;
    }

    /* 如果正在写入FLASH，不进行记录 */
    if(faultinf.record_doing == 1)
    {
        osSemaphoreRelease(savefinf_sem);
        record_dbg("save fault: flash is busy.\n");
        return 1;
    }

    faultinf.record_doing = 1;

    faultinf.flag = 0;

    tail = rtwq_tail;

    get_time(&tm, &ms);

    faultinf.event = event;

    wave.fault.brief.did = FS_DEV_ID;

    wave.fault.brief.fid = failtype;
    wave.fault.brief.year = (tm.tm_year + 1900) % 100;
    wave.fault.brief.mon = tm.tm_mon + 1;
    wave.fault.brief.date = tm.tm_mday;
    wave.fault.brief.hour = tm.tm_hour;
    wave.fault.brief.min = tm.tm_min;
    wave.fault.brief.sec = tm.tm_sec;
    wave.fault.brief.ms = ms;

    memcpy(wave.fault.para.para, para_list, paranum << 2);
    wave.fault.brief.num = paranum;
    savefsdata_task_id = osThreadCreate(osThread(savefsdata_task), (void *)tail);
    osSemaphoreRelease(savefinf_sem);
    return 0;
}

/**
 * @brief A thread, Called by savefinf, saving the fault data to ram and rom
 *
 * @param pdata useless
 *
 */
void savefsdata_task(const void* pdata)
{
    unsigned int tail, len_b, len_a, _len, _tail, maxlen;
    short samp;

    tail = (unsigned int)pdata;
    readpara(Addr_Param100, &samp);

    maxlen = samp * FT_BUF_SEC;

    if(maxlen > FTWQ_MAX_SIZE)
    {
        maxlen = FTWQ_MAX_SIZE;
    }

    len_b = maxlen * FT_SBF / FT_BUF_SEC;

    wave.fault.brief.wnum = maxlen;
    wave.fault.brief.widx = len_b;
    wave.fault.brief.cnum = CHANNEL_MAX;
    wave.fault.brief.cid[0] = fpga_read(Addr_Param101);
    wave.fault.brief.cid[1] = fpga_read(Addr_Param102);
    wave.fault.brief.cid[2] = fpga_read(Addr_Param103);
    wave.fault.brief.cid[3] = fpga_read(Addr_Param104);
    wave.fault.brief.cid[4] = fpga_read(Addr_Param105);
    wave.fault.brief.cid[5] = fpga_read(Addr_Param106);
    wave.fault.brief.cid[6] = fpga_read(Addr_Param107);
    wave.fault.brief.cid[7] = fpga_read(Addr_Param108);
    wave.fault.brief.cid[8] = fpga_read(Addr_Param109);
    //存储故障发生前的故障波形
    copyrtwave(wave.fault.wave.ft_cv[0], 0, len_b, tail);
    copyrtwave(wave.fault.wave.ft_cv[1], 1, len_b, tail);
    copyrtwave(wave.fault.wave.ft_cv[2], 2, len_b, tail);
    copyrtwave(wave.fault.wave.ft_cv[3], 3, len_b, tail);
    copyrtwave(wave.fault.wave.ft_cv[4], 4, len_b, tail);
    copyrtwave(wave.fault.wave.ft_cv[5], 5, len_b, tail);
    copyrtwave(wave.fault.wave.ft_cv[6], 6, len_b, tail);
    copyrtwave(wave.fault.wave.ft_cv[7], 7, len_b, tail);
    copyrtwave(wave.fault.wave.ft_cv[8], 8, len_b, tail);

    len_a = maxlen - len_b; //计算故障发生后要存储的波形点数

    _len = 0;

    while(len_a > _len)			//等待波形接收，当故障发生后的波形点数大于len_a时，存储故障发生后的波形
    {
        _tail = rtwq_tail;
        _len = (_tail >= tail) ? (_tail - tail) : (RTWQ_MAX_SIZE + _tail - tail);
        osDelay(1);
    }
    //存储故障发生后的故障波形
    _tail = (tail + len_a) % RTWQ_MAX_SIZE;
    copyrtwave(&wave.fault.wave.ft_cv[0][len_b], 0, len_a, _tail);
    copyrtwave(&wave.fault.wave.ft_cv[1][len_b], 1, len_a, _tail);
    copyrtwave(&wave.fault.wave.ft_cv[2][len_b], 2, len_a, _tail);
    copyrtwave(&wave.fault.wave.ft_cv[3][len_b], 3, len_a, _tail);
    copyrtwave(&wave.fault.wave.ft_cv[4][len_b], 4, len_a, _tail);
    copyrtwave(&wave.fault.wave.ft_cv[5][len_b], 5, len_a, _tail);
    copyrtwave(&wave.fault.wave.ft_cv[6][len_b], 6, len_a, _tail);
    copyrtwave(&wave.fault.wave.ft_cv[7][len_b], 7, len_a, _tail);
    copyrtwave(&wave.fault.wave.ft_cv[8][len_b], 8, len_a, _tail);

    faultinf.flag = 1;

//		fault_write(&wave.fault);
		fault_write();
    faultinf.record_doing = 0;
    osThreadTerminate(savefsdata_task_id);
//    thread_delete_selfmb();
}

void clearftwave(unsigned char* event)
{
    if((faultinf.event == event) && (*event == 0))
    {
        faultinf.event = NULL;
    }
}

#if 0
void savefinf_init(void)
{
    savefinf_sem = OSSemCreate(1);
    memset(&faultinf, 0, sizeof(_fault_inf));
    faultinf.event = NULL;
}

void savefinf_enable(void)
{
    INT8U err;

    osSemaphoreWait(savefinf_sem, 0, &err);
    faultinf.enable = 1;
    osSemaphoreRelease(savefinf_sem, osWaitForever);
}

void savefinf_disable(void)
{
    INT8U err;

    osSemaphoreWait(savefinf_sem, 0, &err);
    faultinf.enable = 0;
    osSemaphoreRelease(savefinf_sem, osWaitForever);
}

/**
 * @brief Save the fault data to ram and rom
 *
 * @param failtype the type of the fail
 *
 * @event a pointer to the fault flag
 *
 */


void clearftwave_any(void)
{
    if((faultinf.event != NULL) && (*(faultinf.event) == 0))
    {
        faultinf.event = NULL;
    }
}

int fs_fifo_valid(unsigned char sec)
{
    unsigned int baseaddr;
    unsigned char* crc_buf;
    unsigned int crc_len;
    unsigned int crc;
    _fault_save_fifo* pfifo;

    baseaddr = flash_getaddr(sec) * 2 + (unsigned int)FLASH_BASE_ADD;
    pfifo = (_fault_save_fifo*)baseaddr;

    crc_buf = (unsigned char*)(baseaddr + 12);
    crc_len = pfifo->len;

    if(pfifo->head != FS_FIFO_HEAD)
    {
        tty_printf("head: %x\n",pfifo->head);
        return 0;
    }

    crc = crc32_get(0, crc_buf, crc_len);

    if(crc != pfifo->crc)
    {
        tty_printf("crc: %d fifo_crc: %d\n", crc, pfifo->crc);
        return 0;
    }

    if(fault_save.data_num != pfifo->size)
    {
        return 0;
    }

    if(fault_save.onedata_size != pfifo->onedata_size)
    {
        return 0;
    }

    return 1;
}

int fs_fifo_equal(void)
{
    unsigned int baseaddr;
    _fault_save_fifo* pfifo;
    unsigned int crc, crcbak;

    baseaddr = flash_getaddr(fault_save.fifoinf_sec) * 2 + (unsigned int)FLASH_BASE_ADD;
    pfifo = (_fault_save_fifo*)baseaddr;
    crc = pfifo->crc;

    baseaddr = flash_getaddr(fault_save.fifoinf_bak_sec) * 2 + (unsigned int)FLASH_BASE_ADD;
    pfifo = (_fault_save_fifo*)baseaddr;
    crcbak = pfifo->crc;

    if(crc == crcbak)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int fs_fifo_copy(int sec, _fault_save_fifo* dst)
{
    unsigned int baseaddr;
    _fault_save_fifo* pfifo;

    if(dst == NULL)
    {
        return 1;
    }

    baseaddr = flash_getaddr(sec) * 2 + (unsigned int)FLASH_BASE_ADD;
    pfifo = (_fault_save_fifo*)baseaddr;

    memcpy(dst, pfifo, sizeof(_fault_save_fifo));

    return 0;
}

void fs_fifo_crcupdate(_fault_save_fifo* pfifo)
{
    unsigned int baseaddr;
    unsigned char* crc_buf;
    unsigned int crc_len;

    baseaddr = (unsigned int)pfifo;

    crc_buf = (unsigned char*)(baseaddr + 12);
    crc_len = pfifo->len;

    pfifo->crc = crc32_get(0, crc_buf, crc_len);
}

void fs_fifo_update(int sec, _fault_save_fifo* pfifo)
{
    flash_erase(sec);
    flash_write(sec, (char*)pfifo, sizeof(_fault_save_fifo));
}

int fsdata_write(_fault_save_fifo *pfifo)
{
    unsigned int tail;
    unsigned int sec;
    unsigned int total;

    tail = pfifo->fifo_tail;
    sec = fault_save.data_sec[tail];
    total = sizeof(wave.fault);
    flash_write(sec, (char*)&wave.fault, total);

    return 0;
}

int fsdata_insert(_fault_save_fifo *pfifo)
{
    /* 入队 */
    pfifo->fifo_tail = (pfifo->fifo_tail + 1) % pfifo->size;
    pfifo->fifo_cnt += 1;
    fs_fifo_crcupdate(pfifo);
    /* 更新队列信息 */
    fs_fifo_update(fault_save.fifoinf_sec, pfifo);
    /* 更新队列信息备份 */
    fs_fifo_update(fault_save.fifoinf_bak_sec, pfifo);

    return 0;
}

/**
 * @brief Copy fault wave from ram to buffer
 *
 * @param sbuf output buffer
 *
 * @param channel wave channel
 *
 * @param num the size of wave data to copy, in byte
 *
 * @return the number of bytes copied
 */
unsigned int copyftwave(short* sbuf, unsigned short channel, unsigned int num, unsigned int saddr)
{
    unsigned int len;
    short* channelbuf;

    switch(channel)
    {
    case 0:
        channelbuf = wave.fault.wave.ft_cv[0];
        break;
    case 1:
        channelbuf = wave.fault.wave.ft_cv[1];
        break;
    case 2:
        channelbuf = wave.fault.wave.ft_cv[2];
        break;
    case 3:
        channelbuf = wave.fault.wave.ft_cv[3];
        break;
    case 4:
        channelbuf = wave.fault.wave.ft_cv[4];
        break;
    case 5:
        channelbuf = wave.fault.wave.ft_cv[5];
        break;
    case 6:
        channelbuf = wave.fault.wave.ft_cv[6];
        break;
    case 7:
        channelbuf = wave.fault.wave.ft_cv[7];
        break;
    case 8:
        channelbuf = wave.fault.wave.ft_cv[8];
        break;
    default:
        record_dbg("通道选择错误.\n");
        return 0;
    }

    memcpy(sbuf, &channelbuf[saddr], num << 1);

    len = num << 1;
    return len;
}

/**
 * @brief Copy the fifo information from ROM
 *
 * @param sec the sector num of fifo area
 *
 * @param dst pointer to the storage location for fifo infromation
 *
 * @return 0 if success, 1 if failed
 */
int fs_fifoinf_copy(int sec, _fault_save_fifoinf* dst)
{
    unsigned int baseaddr;
    _fault_save_fifo* pfifo;

    if(dst == NULL)
    {
        return 1;
    }

    baseaddr = flash_getaddr(sec) * 2 + (unsigned int)FLASH_BASE_ADD;
    pfifo = (_fault_save_fifo*)baseaddr;

    dst->len = pfifo->len;
    dst->size = pfifo->size;
    dst->onedata_size = pfifo->size;
    dst->fifo_head = pfifo->fifo_head;
    dst->fifo_tail = pfifo->fifo_tail;
    dst->fifo_cnt = pfifo->fifo_cnt;

    return 0;
}

/**
 * @brief Read fifo infromation from ROM
 *
 * @param pfifo pointer to the storage location for fifo infromation
 *
 * @return 0 if success, 1 if failed
 */
int fs_fifoinf_read(_fault_save_fifoinf* pfifo)
{
    if(fs_fifo_valid(fault_save.fifoinf_sec))
    {
        fs_fifoinf_copy(fault_save.fifoinf_sec, pfifo);
        return 0;
    }

    if(fs_fifo_valid(fault_save.fifoinf_bak_sec))
    {
        fs_fifoinf_copy(fault_save.fifoinf_bak_sec, pfifo);
        return 0;
    }

    return 1;
}

/**
 * @brief Check the brief data if valid
 *
 * @param idx the index of the fault data
 *
 * @return 1 if valid, 0 if invalid
 */
int fs_brief_valid(int idx)
{
    unsigned int baseaddr;
    unsigned char* crc_buf;
    unsigned int crc_len;
    unsigned int crc;
    _fault_save_brief* pbrief;
    unsigned char sec;

    if(idx >= fault_save.data_num)
    {
        return 0;
    }

    sec = fault_save.data_sec[idx];
    baseaddr = flash_getaddr(sec) * 2 + (unsigned int)FLASH_BASE_ADD;

    pbrief = (_fault_save_brief*)baseaddr;

    crc_buf = (unsigned char*)(baseaddr + 12);
    crc_len = pbrief->len;

    if(pbrief->head != FS_BRIEF_HEAD)
    {
        tty_printf("head: %x\n",pbrief->head);
        return 0;
    }

    crc = crc32_get(0, crc_buf, crc_len);

    if(crc != pbrief->crc)
    {
        tty_printf("crc: %d fifo_crc: %d\n", crc, pbrief->crc);
        return 0;
    }

    if(FS_DEV_ID != pbrief->did)
    {
        tty_printf("dev id is invalid\n");
        return 0;
    }

    return 1;
}

/**
 * @brief Copy the brief information from ROM
 *
 * @param idx the index of the fault data
 *
 * @param dst pointer to the storage location for brief infromation
 *
 * @return 0 if success, 1 if failed
 */
int fs_briefinf_copy(int idx, _fault_save_briefinf* dst)
{
    int i;
    unsigned int baseaddr;
    _fault_save_brief* pbrief;
    unsigned char sec;

    if(idx >= fault_save.data_num)
    {
        return 1;
    }

    if(dst == NULL)
    {
        return 1;
    }

    sec = fault_save.data_sec[idx];
    baseaddr = flash_getaddr(sec) * 2 + (unsigned int)FLASH_BASE_ADD;

    pbrief = (_fault_save_brief*)baseaddr;

    dst->len = pbrief->len;
    dst->did = pbrief->did;
    dst->fid = pbrief->fid;
    dst->year = pbrief->year;
    dst->mon = pbrief->mon;
    dst->date = pbrief->date;
    dst->hour = pbrief->hour;
    dst->min = pbrief->min;
    dst->sec = pbrief->sec;
    dst->ms = pbrief->ms;
    dst->num = pbrief->num;
    dst->wnum = pbrief->wnum;
    dst->widx = pbrief->widx;
    dst->cnum = pbrief->cnum;

    for(i = 0; i < CHANNEL_MAX; i++)
    {
        dst->cid[i] = pbrief->cid[i];
    }

    return 0;
}

/**
 * @brief Read brief information from ROM
 *
 * @param idx the index of the fault data
 *
 * @param pbrief pointer to the storage location for fifo infromation
 *
 * @return 0 if success, 1 if failed
 */
int fs_briefinf_read(int idx, _fault_save_briefinf* pbrief)
{
    if(fs_brief_valid(idx))
    {
        fs_briefinf_copy(idx, pbrief);
        return 0;
    }

    return 1;
}

/**
 * @brief Check the parameter data if valid
 *
 * @param idx the index of the fault data
 *
 * @return 1 if valid, 0 if invalid
 */
int fs_para_valid(int idx)
{
    unsigned int baseaddr;
    unsigned char* crc_buf;
    unsigned int crc_len;
    unsigned int crc;
    _fault_save_brief* pbrief;
    _fault_save_para* ppara;
    unsigned char sec;

    if(idx >= fault_save.data_num)
    {
        return 0;
    }

    if(fs_brief_valid(idx) == 0)
    {
        return 0;
    }

    sec = fault_save.data_sec[idx];

    baseaddr = flash_getaddr(sec) * 2 + (unsigned int)FLASH_BASE_ADD;

    pbrief = (_fault_save_brief*)baseaddr;

    baseaddr += pbrief->len + 12;

    ppara = (_fault_save_para*)baseaddr;

    crc_buf = (unsigned char*)(baseaddr + 12);

    crc_len = ppara->len;

    if(ppara->head != FS_PARA_HEAD)
    {
        tty_printf("head: %x\n",ppara->head);
        return 0;
    }

    crc = crc32_get(0, crc_buf, crc_len);

    if(crc != ppara->crc)
    {
        tty_printf("crc: %d fifo_crc: %d\n", crc, ppara->crc);
        return 0;
    }

    return 1;
}

/**
 * @brief Copy the parameter data from ROM
 *
 * @param dst a pointer to the data destination buffer
 *
 * @param idx the index of the fault data
 *
 * @param num the number of the parameters
 *
 * @return 0 if success, 1 if failed
 */
int fs_para_copy(short* dst, int idx, int num)
{
    unsigned int baseaddr;
    _fault_save_brief* pbrief;
    _fault_save_para* ppara;
    unsigned char sec;
    short* src;

    if(idx >= fault_save.data_num)
    {
        return 1;
    }

    if(dst == NULL)
    {
        return 1;
    }

    sec = fault_save.data_sec[idx];

    baseaddr = flash_getaddr(sec) * 2 + (unsigned int)FLASH_BASE_ADD;

    pbrief = (_fault_save_brief*)baseaddr;

    baseaddr += pbrief->len + 12;

    ppara = (_fault_save_para*)baseaddr;

    src = (short*)(&ppara->para[0]);

    memcpy(dst, src, num << 2);

    return 0;
}

/**
 * @brief Copy the parameter information from ROM
 *
 * @param idx the index of the fault data
 *
 * @param dst a pointer to the storage location for parameter infromation
 *
 * @return 0 if success, 1 if failed
 */
int fs_parainf_copy(int idx, _fault_save_parainf* dst)
{
    unsigned int baseaddr;
    _fault_save_brief* pbrief;
    _fault_save_para* ppara;
    unsigned char sec;

    if(idx >= fault_save.data_num)
    {
        return 1;
    }

    if(dst == NULL)
    {
        return 1;
    }

    sec = fault_save.data_sec[idx];

    baseaddr = flash_getaddr(sec) * 2 + (unsigned int)FLASH_BASE_ADD;

    pbrief = (_fault_save_brief*)baseaddr;

    baseaddr += pbrief->len + 12;

    ppara = (_fault_save_para*)baseaddr;

    dst->len = ppara->len;
    dst->para = (unsigned short*)(baseaddr + 12);

    return 0;
}

int fs_parainf_read(int idx, _fault_save_parainf* ppara)
{
    if(fs_para_valid(idx))
    {
        fs_parainf_copy(idx, ppara);
        return 0;
    }

    return 1;
}

int fs_wave_valid(int idx)
{
    unsigned int baseaddr;
    unsigned char* crc_buf;
    unsigned int crc_len;
    unsigned int crc;
    _fault_save_brief* pbrief;
    _fault_save_para* ppara;
    _fault_save_wave* pwave;
    unsigned char sec;

    if(idx >= fault_save.data_num)
    {
        return 0;
    }

    if(fs_para_valid(idx) == 0)
    {
        return 0;
    }

    sec = fault_save.data_sec[idx];

    baseaddr = flash_getaddr(sec) * 2 + (unsigned int)FLASH_BASE_ADD;

    pbrief = (_fault_save_brief*)baseaddr;

    baseaddr += pbrief->len + 12;

    ppara = (_fault_save_para*)baseaddr;

    baseaddr += ppara->len + 12;

    pwave = (_fault_save_wave*)baseaddr;

    crc_buf = (unsigned char*)(baseaddr + 12);

    crc_len = pwave->len;

    if(pwave->head != FS_WAVE_HEAD)
    {
        tty_printf("head: %x\n",pwave->head);
        return 0;
    }

    crc = crc32_get(0, crc_buf, crc_len);

    if(crc != pwave->crc)
    {
        tty_printf("crc: %d fifo_crc: %d\n", crc, pwave->crc);
        return 0;
    }

    return 1;
}

int fs_wave_copy(short* dst, int idx, int cidx,
                 int widx, int wnum)
{
    unsigned char sec;
    unsigned int baseaddr;
    _fault_save_brief* pbrief;
    _fault_save_para* ppara;
    _fault_save_wave* pwave;
    int offset;
    short* src;

    if(idx >= fault_save.data_num)
    {
        return 1;
    }

    sec = fault_save.data_sec[idx];

    baseaddr = flash_getaddr(sec) * 2 + (unsigned int)FLASH_BASE_ADD;

    pbrief = (_fault_save_brief*)baseaddr;

    baseaddr += pbrief->len + 12;

    ppara = (_fault_save_para*)baseaddr;

    baseaddr += ppara->len + 12;

    pwave = (_fault_save_wave*)baseaddr;

    src = &pwave->ft_cv[0][0];

    offset = cidx * pbrief->wnum + widx;

    memcpy(dst, &src[offset], wnum << 1);

    return 0;
}

int fs_delete(void)
{
    flash_erase(fault_save.fifoinf_sec);
    flash_erase(fault_save.fifoinf_bak_sec);

    return 0;
}
#endif

