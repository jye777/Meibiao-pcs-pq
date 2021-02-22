/*****************************************************************
 * ÃÀ±ê£º¹ÊÕÏ»Ö¸´Ä£¿é
*****************************************************************/

#include "fault_rec.h"
#include "tty.h"
#include "fpga.h"
#include "comm_offline.h"
#include "freq_prot.h"
#include "para.h"


#if(TEST_MODE == 0)        
#define FR_TASK_PERIOD 20
#else
#define FR_TASK_PERIOD 100
#endif


fault_rec_t faultRec;

void do_fault_rec(const void *pdata);
osTimerDef(doFaultRec, do_fault_rec);
static void fault_rec_bootup_task(const void *pdata);
osThreadDef(fault_rec_bootup_task, osPriorityNormal, 1, 0);


static void do_fault_rec(const void *pdata)
{
	fault_rec_t *pFr = (fault_rec_t *)pdata;
    freq_prot_t *pFp = &freqProt;
    uint8_t i;
    uint32_t stateTot = 0;
    uint16_t freqRt;

	pFr->startFlag = 0;
    
#if(TEST_MODE == 0)	    
    printf_debug9("******* time out, now shutdown and bootup!\n\n");
    if(pFp->freqProtFault == 1) {
        freqRt = (uint16_t)(fpga_read(Addr_Param271) * 100.0f / 343.595f);	
        if(freqRt > pFp->fpi[FREQ_PRPT_TYPE_OF1].thr || freqRt > pFp->fpi[FREQ_PRPT_TYPE_OF2].thr || 
           freqRt < pFp->fpi[FREQ_PRPT_TYPE_UF1].thr || freqRt < pFp->fpi[FREQ_PRPT_TYPE_UF2].thr) {
            pFr->handleFlag = 0;
			return;
        }
    }
    for(i = 0, stateTot = 0; i < 7; i++) {
        stateTot += fpga_read(STATE0_ADD + i);
    }
    if(stateTot != 0) {
        pFr->handleFlag = 0;
        return;
    }
    if((fpga_read(STATE8_ADD) & 0xF) == FSM_FAULT) {
		set_pcs_shutdown();
	}
    osSignalSet(pFr->bootupTaskId, 0x1);
#else
    printf_debug9("******* time out\n");
    pFr->handleFlag = 0;
    pFp->freqProtFault = 0;
#endif   
}


static void fault_rec_timer_start(fault_rec_t *pFaultRec)
{
	fault_rec_t *pFr = pFaultRec;
    int32_t timeMs;

	if(pFr->startFlag == 0) {
		pFr->startFlag = 1;
        timeMs = (pFr->frWaitTime - pFr->bootupTime) * 1000 / 10;
        timeMs = timeMs < 10 ? 10 : timeMs;
		osTimerStart(pFr->timerId, timeMs);
		printf_debug9("timer start, %dms\n", timeMs);
	}
}


static void fault_rec_timer_stop(fault_rec_t *pFaultRec)
{
	fault_rec_t *pFr = pFaultRec;
	
	if(pFr->startFlag == 1) {
		pFr->startFlag = 0;
		osTimerStop(pFr->timerId);
		printf_debug9("timer stop\n");
	}
}


static void fault_rec_bootup_task(const void *pdata)
{
    fault_rec_t *pFr = (fault_rec_t *)pdata;
    freq_prot_t *pFp = &freqProt;
    
    while(1)
    {
        osSignalWait(0x01, osWaitForever);
        set_pcs_bootup();
        pFr->handleFlag = 0;
        pFp->freqProtFault = 0;
    }
}


void fault_rec_task(const void *pdata)
{
    fault_rec_t *pFr = (fault_rec_t *)pdata;
    comm_offline_t *pCo = &offline;
    freq_prot_t *pFp = &freqProt;
    uint8_t i, commFlag[COMM_OFFLINE_DEV_MAX] = {0};
    uint32_t stateTot = 0;
    uint16_t freqRt;

    pFr->timerId = osTimerCreate(osTimer(doFaultRec), osTimerOnce, (void *)pFr);
    pFr->bootupTaskId = osThreadCreate(osThread(fault_rec_bootup_task), (void *)pFr);
    
    while(1)
    {
        if(pFr->frEnble == 0) {
            pFr->handleFlag = 0;
            pFp->freqProtFault = 0;
            fault_rec_timer_stop(pFr);
            osDelay(50);
			continue;
        }
        
#if(TEST_MODE == 0)	    
        if(pFp->freqProtFault == 0) {
            if((fpga_read(STATE8_ADD) & 0xF) != FSM_FAULT) {
                pFr->handleFlag = 0;
    			osDelay(50);
    			continue;
    		}

            if((fpga_read(ERR_LCK_STATE1_ADD) & 0xFFFC) || (fpga_read(ERR_LCK_STATE2_ADD) & 0xC7FF) || 
               (fpga_read(ERR_LCK_STATE3_ADD) & 0xFFFF) || (fpga_read(ERR_LCK_STATE4_ADD) & 0xFE07) ||
               (fpga_read(ERR_LCK_STATE5_ADD) & 0xFFFF) || (fpga_read(ERR_LCK_STATE6_ADD) & 0xFFFF)) {
                printf_debug9("lock state can't recovery!\n");
                pFr->handleFlag = 0;
    			osDelay(500);
    			continue;
            }
        } else {
            if((fpga_read(STATE8_ADD) & 0xF) != FSM_IDLE &&
               (fpga_read(STATE8_ADD) & 0xF) != FSM_STOP &&
               (fpga_read(STATE8_ADD) & 0xF) != FSM_FAULT) {
                pFr->handleFlag = 0;
    			osDelay(50);
    			continue;
    		}

            freqRt = (uint16_t)(fpga_read(Addr_Param271) * 100.0f / 343.595f);	
            if(freqRt > pFp->fpi[FREQ_PRPT_TYPE_OF1].thr || freqRt > pFp->fpi[FREQ_PRPT_TYPE_OF2].thr || 
               freqRt < pFp->fpi[FREQ_PRPT_TYPE_UF1].thr || freqRt < pFp->fpi[FREQ_PRPT_TYPE_UF2].thr) {
                pFr->handleFlag = 0;
    			osDelay(50);
    			continue;
            }
        }

        for(i = 0, stateTot = 0; i < 7; i++) {
            stateTot += fpga_read(STATE0_ADD + i);
        }
        if(stateTot != 0) {
            printf_debug9("state dirty can't recovery!\n");
            pFr->handleFlag = 0;
			osDelay(50);
			continue;
        }
#endif

        if(GETBIT(pCo->commEn, COMM_OFFLINE_DEV_EMU) == 1) {
            commFlag[COMM_OFFLINE_DEV_EMU] = pCo->pCoi[COMM_OFFLINE_DEV_EMU]->disconnFlag;
        } else {
            commFlag[COMM_OFFLINE_DEV_EMU] = 0;
        }
        if(GETBIT(pCo->commEn, COMM_OFFLINE_DEV_HMI) == 1) {
            commFlag[COMM_OFFLINE_DEV_HMI] = pCo->pCoi[COMM_OFFLINE_DEV_HMI]->disconnFlag;
        } else {
            commFlag[COMM_OFFLINE_DEV_HMI] = 0;
        }
        if(commFlag[COMM_OFFLINE_DEV_EMU] != 0 || commFlag[COMM_OFFLINE_DEV_HMI] != 0) {
            pFr->handleFlag = 0;
            fault_rec_timer_stop(pFr);
			osDelay(50);
			continue;
        }

        if(pFr->handleFlag == 0) {
            pFr->handleFlag = 1;
            fault_rec_timer_start(pFr);
        }
              
        osDelay(FR_TASK_PERIOD);
    }
}

