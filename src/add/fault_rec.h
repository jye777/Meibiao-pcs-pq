#ifndef __FAULT_REC_H
#define __FAULT_REC_H
#include <stdint.h>
#include <cmsis_os.h>


typedef struct
{
	uint16_t frEnble; //���ϻָ�ʹ��
	uint16_t frWaitTime; //���ϻָ��ȴ�ʱ��(0-600s) 0.1s
	osThreadId bootupTaskId; //�ָ�����������
	osTimerId timerId; //��ʱ�����
	uint8_t startFlag; //������־
	uint8_t handleFlag; //�����־
	uint16_t bootupTime; //����ʱ�� 0.1s
} fault_rec_t;

extern fault_rec_t faultRec;
extern void fault_rec_task(const void *pdata);

#endif

