#ifndef __FAULT_REC_H
#define __FAULT_REC_H
#include <stdint.h>
#include <cmsis_os.h>


typedef struct
{
	uint16_t frEnble; //故障恢复使能
	uint16_t frWaitTime; //故障恢复等待时间(0-600s) 0.1s
	osThreadId bootupTaskId; //恢复启动任务句柄
	osTimerId timerId; //定时器句柄
	uint8_t startFlag; //开启标志
	uint8_t handleFlag; //处理标志
	uint16_t bootupTime; //启动时间 0.1s
} fault_rec_t;

extern fault_rec_t faultRec;
extern void fault_rec_task(const void *pdata);

#endif

