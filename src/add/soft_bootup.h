#ifndef __SOFT_BOOTUP_H
#define __SOFT_BOOTUP_H
#include <stdint.h>
#include <cmsis_os.h>


typedef struct
{
	uint16_t sbEnble; //����ʹ��
	uint16_t sbTime; //����ʱ��(0-1000s) 0.1s
	int16_t sbGoalPwr; //Ŀ�깦�� 0.1kW
	//uint16_t cpltFlag; //��ɱ�־
} soft_bootup_t;

extern soft_bootup_t softBootup;
extern void soft_bootup_task(const void *pdata);

#endif

