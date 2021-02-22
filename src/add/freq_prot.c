/*****************************************************************
 * ���꣺Ƶ�ʱ���ģ��
*****************************************************************/

#include "freq_prot.h"
#include "tty.h"
#include "fpga.h"
#include "para.h"


#if(TEST_MODE == 0)	
#define FP_TASK_PERIOD 10
#else
#define FP_TASK_PERIOD 1000
#endif


freq_prot_t freqProt;

static void do_freq_prot(const void *pdata);
osTimerDef(doFreqProtOF1, do_freq_prot);
osTimerDef(doFreqProtOF2, do_freq_prot);
osTimerDef(doFreqProtUF1, do_freq_prot);
osTimerDef(doFreqProtUF2, do_freq_prot);


static void freq_prot_timer_start(freq_prot_t *pFreqProt, int8_t No)
{
	freq_prot_t *pFp = pFreqProt;
    int32_t timeMs;
	
	if(pFp->fpi[No].startFlag == 0) {
		pFp->fpi[No].startFlag = 1;
        timeMs = pFp->fpi[No].time * 20;
        timeMs = timeMs < 10 ? 10 : timeMs;
		osTimerStart(pFp->fpi[No].timerId, timeMs);
		printf_debug9("%d timer start, %dms\n", No, timeMs);
	}
}


static void freq_prot_timer_stop(freq_prot_t *pFreqProt, int8_t No)
{
	freq_prot_t *pFp = pFreqProt;
	
	if(pFp->fpi[No].startFlag == 1) {
		pFp->fpi[No].startFlag = 0;
		osTimerStop(pFp->fpi[No].timerId);
		printf_debug9("%d timer stop\n", No);
	}
}


static void do_freq_prot(const void *pdata)
{
	freq_prot_t *pFp = (freq_prot_t *)pdata;
	uint8_t i;

	printf_debug9("******* time out, now shutdown!\n\n");

    pFp->freqProtFault = 1;
	for(i = 0; i < FREQ_PROT_NUM; i++) {
		freq_prot_timer_stop(pFp, i);
	}
#if(TEST_MODE == 0)		
	set_pcs_shutdown();
#endif
}


void freq_prot_task(const void *pdata)
{
	freq_prot_t *pFp = (freq_prot_t *)pdata;
	uint16_t freqRt, freqNormalUp, freqNormalDown; //x100
	uint8_t normalUpNo, normalDownNo;
	uint8_t enterUpFlag, enterDownFlag, i;

	pFp->fpi[FREQ_PRPT_TYPE_OF1].timerId = osTimerCreate(osTimer(doFreqProtOF1), osTimerOnce, (void *)pFp);
	pFp->fpi[FREQ_PRPT_TYPE_OF2].timerId = osTimerCreate(osTimer(doFreqProtOF2), osTimerOnce, (void *)pFp);
	pFp->fpi[FREQ_PRPT_TYPE_UF1].timerId = osTimerCreate(osTimer(doFreqProtUF1), osTimerOnce, (void *)pFp);
	pFp->fpi[FREQ_PRPT_TYPE_UF2].timerId = osTimerCreate(osTimer(doFreqProtUF2), osTimerOnce, (void *)pFp);

	while(1)
	{
#if(TEST_MODE == 0)	
		if((fpga_read(STATE8_ADD) & 0xF) != FSM_WORKING) {
			osDelay(1000);
			continue;
		}
		//Ƶ�ʱ���
		freqRt = (uint16_t)(fpga_read(Addr_Param271) * 100.0f / 343.595f);	
#else		
		freqRt = arm_config_data_read(SET_POWER_0); //ͨ��720����д��Ƶ��ģ��ֵ
#endif
		if(pFp->fpi[FREQ_PRPT_TYPE_OF1].thr > pFp->fpi[FREQ_PRPT_TYPE_OF2].thr) {
			freqNormalUp = pFp->fpi[FREQ_PRPT_TYPE_OF2].thr;
			normalUpNo = FREQ_PRPT_TYPE_OF2;
		} else {
			freqNormalUp = pFp->fpi[FREQ_PRPT_TYPE_OF1].thr;
			normalUpNo = FREQ_PRPT_TYPE_OF1;
		}
		if(pFp->fpi[FREQ_PRPT_TYPE_UF1].thr < pFp->fpi[FREQ_PRPT_TYPE_UF2].thr) {
			freqNormalDown= pFp->fpi[FREQ_PRPT_TYPE_UF2].thr;
			normalDownNo = FREQ_PRPT_TYPE_UF2;
		} else {
			freqNormalDown = pFp->fpi[FREQ_PRPT_TYPE_UF1].thr;
			normalDownNo = FREQ_PRPT_TYPE_UF1;
		}
#if(TEST_MODE != 0)
		printf_debug9("Freq: rt[%d] OF1[%d] OF2[%d] UF1[%d] UF2[%d]\n", freqRt, pFp->fpi[FREQ_PRPT_TYPE_OF1].thr, pFp->fpi[FREQ_PRPT_TYPE_OF2].thr, pFp->fpi[FREQ_PRPT_TYPE_UF1].thr, pFp->fpi[FREQ_PRPT_TYPE_UF2].thr);
		printf_debug9("normalUpNo[%d] normalDownNo[%d]\n", normalUpNo, normalDownNo);
#endif
		if(freqRt >= freqNormalDown && freqRt <= freqNormalUp) {
			/* �ָ�������Χ, ȡ�����ж�ʱ�� */
			for(i = 0; i < FREQ_PRPT_TYPE_MAX; i++) {
				freq_prot_timer_stop(pFp, i);
			}
		} else {
			/* ��Ƶ */
			if(freqRt > freqNormalUp) { 
				/* һ����Ƶ���޵��ڶ�����Ƶ���� */
				if(normalUpNo == FREQ_PRPT_TYPE_OF1) {
					/* ʼ�տ�һ����Ƶ��ʱ�� */
					freq_prot_timer_start(pFp, FREQ_PRPT_TYPE_OF1);
					/* ������������, ��������Ƶ��ʱ�� */
					if(freqRt > pFp->fpi[FREQ_PRPT_TYPE_OF2].thr) {
						enterUpFlag = 1;
						freq_prot_timer_start(pFp, FREQ_PRPT_TYPE_OF2);
					}
					/* �����ָ�һ��, �ض�����Ƶ��ʱ�� */
					if(freqRt > pFp->fpi[FREQ_PRPT_TYPE_OF1].thr && freqRt <= pFp->fpi[FREQ_PRPT_TYPE_OF2].thr) {
						if(enterUpFlag == 1) {
							enterUpFlag = 0;
							freq_prot_timer_stop(pFp, FREQ_PRPT_TYPE_OF2);
						}
					}
				}
				/* һ����Ƶ���޸��ڶ�����Ƶ���� */
				if(normalUpNo == FREQ_PRPT_TYPE_OF2) {
					/* ʼ�տ�������Ƶ��ʱ�� */
					freq_prot_timer_start(pFp, FREQ_PRPT_TYPE_OF2);
					/* ����һ������, ��һ����Ƶ��ʱ�� */
					if(freqRt > pFp->fpi[FREQ_PRPT_TYPE_OF1].thr) {
						enterUpFlag = 1;
						freq_prot_timer_start(pFp, FREQ_PRPT_TYPE_OF1);
					} 
					/* һ���ָ�����, ��һ����Ƶ��ʱ�� */
					if(freqRt > pFp->fpi[FREQ_PRPT_TYPE_OF2].thr && freqRt <= pFp->fpi[FREQ_PRPT_TYPE_OF1].thr) {
						if(enterUpFlag == 1) {
							enterUpFlag = 0;
							freq_prot_timer_stop(pFp, FREQ_PRPT_TYPE_OF1);
						}
					}
				}
			}
			/* ǷƵ */
			if(freqRt < freqNormalDown) {
				/* һ��ǷƵ���޸��ڶ���ǷƵ���� */
				if(normalDownNo == FREQ_PRPT_TYPE_UF1) {
					/* ʼ�տ�һ��ǷƵ��ʱ�� */
					freq_prot_timer_start(pFp, FREQ_PRPT_TYPE_UF1);
					/* ������������, ������ǷƵ��ʱ�� */
					if(freqRt < pFp->fpi[FREQ_PRPT_TYPE_UF2].thr) {
						enterDownFlag = 1;
						freq_prot_timer_start(pFp, FREQ_PRPT_TYPE_UF2);
					} 
					/* �����ָ�һ��, �ض���ǷƵ��ʱ�� */
					if(freqRt < pFp->fpi[FREQ_PRPT_TYPE_UF1].thr && freqRt >= pFp->fpi[FREQ_PRPT_TYPE_UF2].thr) {
						if(enterDownFlag == 1) {
							enterDownFlag = 0;
							freq_prot_timer_stop(pFp, FREQ_PRPT_TYPE_UF2);
						}
					}
				}
				/* һ��ǷƵ���޵��ڶ���ǷƵ���� */
				if(normalDownNo == FREQ_PRPT_TYPE_UF2) {
					/* ʼ�տ�����ǷƵ��ʱ�� */
					freq_prot_timer_start(pFp, FREQ_PRPT_TYPE_UF2);
					/* ����һ������, ��һ��ǷƵ��ʱ�� */
					if(freqRt < pFp->fpi[FREQ_PRPT_TYPE_UF1].thr) {
						enterDownFlag = 1;
						freq_prot_timer_start(pFp, FREQ_PRPT_TYPE_UF1);
					} 
					/* һ���ָ�����, ��һ��ǷƵ��ʱ�� */
					if(freqRt < pFp->fpi[FREQ_PRPT_TYPE_UF2].thr && freqRt >= pFp->fpi[FREQ_PRPT_TYPE_UF1].thr) {
						if(enterDownFlag == 1) {
							enterDownFlag = 0;
							freq_prot_timer_stop(pFp, FREQ_PRPT_TYPE_UF1);
						}
					}
				}
			}
		}

		osDelay(FP_TASK_PERIOD);
	}
}







