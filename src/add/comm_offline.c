/*****************************************************************
 * 美标：通讯掉线模块
*****************************************************************/

#include "comm_offline.h"
#include "tty.h"
#include "fpga.h"


#if (TEST_MODE == 0) 
#define CO_TASK_PERIOD	50
#else
#define CO_TASK_PERIOD	500
#endif

comm_offline_iterm_t emuOffline, hmiOffline;
comm_offline_t offline;


static void comm_offline_check(comm_offline_t *pCommOffline)
{
    comm_offline_t *pCo = pCommOffline;
    comm_offline_iterm_t *pEco = pCo->pCoi[COMM_OFFLINE_DEV_EMU];
    comm_offline_iterm_t *pHco = pCo->pCoi[COMM_OFFLINE_DEV_HMI];
    uint16_t jdgCnt = (uint16_t)(pCo->commJdgTime * 1000 / 10.0f / CO_TASK_PERIOD);
		
	pCo->enterCnt++;
	if(pCo->enterCnt & 0x1) {
		pEco->heartBeat1 = pEco->hbCur;
		pHco->heartBeat1 = pHco->hbCur;
	} else {
		pEco->heartBeat2 = pEco->hbCur;
		pHco->heartBeat2 = pHco->hbCur;
	}
	/* EMU */
    if(GETBIT(pCo->commEn, COMM_OFFLINE_DEV_EMU) == 1) {
    	if(pEco->heartBeat1 == pEco->heartBeat2) {
    		if(pEco->errCnt++ > jdgCnt) {
    			pEco->disconnFlag = 1;
    		}
    	} else {
    		pEco->errCnt = 0;
    		pEco->disconnFlag = 0;
    	}
    } else {
        pEco->errCnt = 0;
    	pEco->disconnFlag = 0;
    }
    /* HMI */
    if(GETBIT(pCo->commEn, COMM_OFFLINE_DEV_HMI) == 1) {
    	if(pHco->heartBeat1 == pHco->heartBeat2) {
    		if(pHco->errCnt++ > jdgCnt) {
    			pHco->disconnFlag = 1;
    		}
    	} else {
    		pHco->errCnt = 0;
    		pHco->disconnFlag = 0;
    	}
    } else {
        pHco->errCnt = 0;
    	pHco->disconnFlag = 0;
    }
#if(TEST_MODE != 0)	
    if(GETBIT(pCo->commEn, COMM_OFFLINE_DEV_EMU) == 1) {
	    printf_debug4("EMU: hb1[%d] hb2[%d] errCnt[%d] disconnFlag[%d]\n", pEco->heartBeat1, pEco->heartBeat2, pEco->
errCnt, pEco->disconnFlag);
    }
    if(GETBIT(pCo->commEn, COMM_OFFLINE_DEV_HMI) == 1) {
	    printf_debug4("HMI: hb1[%d] hb2[%d] errCnt[%d] disconnFlag[%d]\n", pHco->heartBeat1, pHco->heartBeat2, pHco->
errCnt, pHco->disconnFlag);
    }
#endif    
}

static void comm_offline_handle(comm_offline_t *pCommOffline)
{
    comm_offline_t *pCo = pCommOffline;
    comm_offline_iterm_t *pEco = pCo->pCoi[COMM_OFFLINE_DEV_EMU];
    comm_offline_iterm_t *pHco = pCo->pCoi[COMM_OFFLINE_DEV_HMI];

    printf_debug4("enterFlag[%d]\n\n", pCo->enterFlag);
    if(pEco->disconnFlag == 1 || pHco->disconnFlag == 1) {
        if(pCo->enterFlag == 0) {
#if(TEST_MODE == 0)            
            if((fpga_read(STATE8_ADD) & 0xF) == FSM_WORKING) {
                set_pcs_shutdown();
                pCo->enterFlag = 1;
            }
#else
            pCo->enterFlag = 1;
#endif
        }
    }
    if(pEco->disconnFlag == 0 && pHco->disconnFlag == 0) {
        if(pCo->enterFlag == 1) {
#if(TEST_MODE == 0)             
            if((fpga_read(STATE8_ADD) & 0xF) == FSM_STOP) {
                set_pcs_bootup();
                pCo->enterFlag = 0;
            }
#else
            pCo->enterFlag = 0;
#endif 
        }
    }
}


void comm_offline_task(const void *pdata)
{
    comm_offline_t *pO = (comm_offline_t *)pdata;
    pO->pCoi[COMM_OFFLINE_DEV_EMU] = &emuOffline;
    pO->pCoi[COMM_OFFLINE_DEV_HMI] = &hmiOffline;
    
    while(1)
    {
        comm_offline_check(pO);
        comm_offline_handle(pO);

        osDelay(CO_TASK_PERIOD);
    }
}

