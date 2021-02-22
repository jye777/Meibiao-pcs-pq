#ifndef __FREQ_PROT_H
#define __FREQ_PROT_H
#include <stdint.h>
#include <cmsis_os.h>

#define FREQ_PROT_NUM	4

enum
{
	FREQ_PRPT_TYPE_OF1 = 0, //一级过频
	FREQ_PRPT_TYPE_OF2 = 1, //二级过频
	FREQ_PRPT_TYPE_UF1 = 2, //一级欠频
	FREQ_PRPT_TYPE_UF2 = 3, //二级欠频
	FREQ_PRPT_TYPE_MAX
};

typedef struct
{
	uint16_t thr; //保护门限 x100
	uint16_t time; //保护时间 1=20ms
	osTimerId timerId; //定时器句柄
	uint8_t startFlag; //开启标志
} freq_prot_iterm_t;

typedef struct
{
	freq_prot_iterm_t fpi[FREQ_PROT_NUM]; //0-of1 1-of2 2-uf1 3-uf2
	uint16_t freqProtFault;
} freq_prot_t;


extern freq_prot_t freqProt;
extern void freq_prot_task(const void *pdata);


#endif

