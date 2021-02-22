#ifndef __SOFT_BOOTUP_H
#define __SOFT_BOOTUP_H
#include <stdint.h>
#include <cmsis_os.h>


typedef struct
{
	uint16_t sbEnble; //软启使能
	uint16_t sbTime; //软启时间(0-1000s) 0.1s
	int16_t sbGoalPwr; //目标功率 0.1kW
	//uint16_t cpltFlag; //完成标志
} soft_bootup_t;

extern soft_bootup_t softBootup;
extern void soft_bootup_task(const void *pdata);

#endif

