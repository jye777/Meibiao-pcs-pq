#ifndef __FREQ_PROT_H
#define __FREQ_PROT_H
#include <stdint.h>
#include <cmsis_os.h>

#define FREQ_PROT_NUM	4

enum
{
	FREQ_PRPT_TYPE_OF1 = 0, //һ����Ƶ
	FREQ_PRPT_TYPE_OF2 = 1, //������Ƶ
	FREQ_PRPT_TYPE_UF1 = 2, //һ��ǷƵ
	FREQ_PRPT_TYPE_UF2 = 3, //����ǷƵ
	FREQ_PRPT_TYPE_MAX
};

typedef struct
{
	uint16_t thr; //�������� x100
	uint16_t time; //����ʱ�� 1=20ms
	osTimerId timerId; //��ʱ�����
	uint8_t startFlag; //������־
} freq_prot_iterm_t;

typedef struct
{
	freq_prot_iterm_t fpi[FREQ_PROT_NUM]; //0-of1 1-of2 2-uf1 3-uf2
	uint16_t freqProtFault;
} freq_prot_t;


extern freq_prot_t freqProt;
extern void freq_prot_task(const void *pdata);


#endif

