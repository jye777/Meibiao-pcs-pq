#ifndef __COMM_OFFLINE_H
#define __COMM_OFFLINE_H
#include <stdint.h>
#include <cmsis_os.h>


enum
{
    COMM_OFFLINE_DEV_EMU = 0,
    COMM_OFFLINE_DEV_HMI = 1,
    COMM_OFFLINE_DEV_MAX
};

typedef struct
{
	uint16_t errCnt; 
	uint16_t heartBeat1;
	uint16_t heartBeat2;
	uint16_t disconnFlag; //���߱�־
    uint16_t hbCur; //��ǰ����
} comm_offline_iterm_t;

typedef struct
{
    uint16_t commEn; //ʹ��
    uint16_t commJdgTime; //�ж�ʱ�� 0.1s
	comm_offline_iterm_t *pCoi[COMM_OFFLINE_DEV_MAX];
    uint16_t enterCnt;
    uint16_t enterFlag;
} comm_offline_t;

extern comm_offline_iterm_t emuOffline, hmiOffline;
extern comm_offline_t offline;
extern void comm_offline_task(const void *pdata);

#endif
