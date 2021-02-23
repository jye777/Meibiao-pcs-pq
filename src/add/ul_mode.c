#include "ul_mode_control.h"
#include "ul_mode.h"
#include "fpga.h"
#include "para.h"
#include "tty.h"


ul_mode_opt_t umo;

#define UM_TASK_PERIOD 5

enum
{
	UL_MODE_CONST_PF = 0,	//恒功率因数
	UL_MODE_VQ		 = 1,	//电压无功
	UL_MODE_PQ		 = 2,	//有功无功
	UL_MODE_CONST_Q	 = 3,	//恒无功
	UL_MODE_VP		 = 4,	//电压有功
	UL_MODE_PFR      = 6,   //一次调频模式
};


static void ul_mode_judge(ul_mode_opt_t *pUmo)
{
    volatile micro_set *pMs = pUmo->pMs;
    ul_ExternalInputs *pul_U = pUmo->pRtu;

    pul_U->ul_mode = pMs->UL_strtg_en;
}

static void ul_input_vars_update(ul_mode_opt_t *pUmo)
{
    volatile micro_set *pMs = pUmo->pMs;
    ul_ExternalInputs *pul_U = pUmo->pRtu;
    
    ul_mode_judge(pUmo);
    
    pul_U->p_cmd = pUmo->pCmd;
    pul_U->q_cmd = pUmo->qCmd;
    pul_U->p_rate = pMs->P_rate;
    pul_U->p_max = pMs->P_max;
    
    if(GETBIT(pul_U->ul_mode, UL_MODE_CONST_PF) == 1) { //UL_MODE_CONST_PF
        pul_U->cosPhi = pMs->PF_x100;
        pul_U->cosMin = pMs->PF_min_x100;
        printf_debug8("---p_cmd[%d] q_cmd[%d]\n", pul_U->p_cmd, pul_U->q_cmd);
        printf_debug8("---p_rate[%d] p_max[%d] cosPhi[%d] cosMin[%d]\n", pul_U->p_rate, pul_U->p_max, pul_U->cosPhi, pul_U->cosMin);
    } 
    if(GETBIT(pul_U->ul_mode, UL_MODE_VQ) == 1) { //UL_MODE_VQ
		pul_U->uac_now = (int16_t)((fpga_pdatabuf_get[Addr_Param256] + fpga_pdatabuf_get[Addr_Param257] + fpga_pdatabuf_get[Addr_Param258]) / 3.0f);
		pul_U->uac_rate = pMs->Uac_rate;
		pul_U->m2V1 = pMs->VQ_V1;
		pul_U->m2V2 = pMs->VQ_V2;
		pul_U->m2V3 = pMs->VQ_V3;
		pul_U->m2V4 = pMs->VQ_V4;
		pul_U->m2Q1 = pMs->VQ_Q1;
		pul_U->m2Q2 = pMs->VQ_Q2;
		pul_U->m2Q3 = pMs->VQ_Q3;
		pul_U->m2Q4 = pMs->VQ_Q4;
        printf_debug8("---p_cmd[%d] q_cmd[%d]\n", pul_U->p_cmd, pul_U->q_cmd);
		printf_debug8("---uac_now[%d] uac_rate[%d] p_rate[%d] p_max[%d]\n", pul_U->uac_now, pul_U->uac_rate, pul_U->p_rate, 
pul_U->p_max);
	    printf_debug8("---v1-v4[%d][%d][%d][%d]\n", pul_U->m2V1, pul_U->m2V2, pul_U->m2V3, pul_U->m2V4);
		printf_debug8("---q1-q4[%d][%d][%d][%d]\n", pul_U->m2Q1, pul_U->m2Q2, pul_U->m2Q3, pul_U->m2Q4);
    } 
    if(GETBIT(pul_U->ul_mode, UL_MODE_PQ) == 1) { //UL_MODE_PQ
		pul_U->P3neg = pMs->PQ_n_P3;
		pul_U->P2neg = pMs->PQ_n_P2;
		pul_U->P2pos = pMs->PQ_p_P2;
		pul_U->P3pos = pMs->PQ_p_P3;
		pul_U->Q3neg = pMs->PQ_n_Q3;
		pul_U->Q2neg = pMs->PQ_n_Q2;
		pul_U->Q2pos = pMs->PQ_p_Q2;
		pul_U->Q3pos = pMs->PQ_p_Q3;
        printf_debug8("---p_cmd[%d] q_cmd[%d]\n", pul_U->p_cmd, pul_U->q_cmd);
		printf_debug8("---p_rate[%d] p_max[%d]\n", pul_U->p_rate, pul_U->p_max);
		printf_debug8("---pn32pp23[%d][%d][%d][%d]\n", pul_U->P3neg, pul_U->P2neg, pul_U->P2pos, pul_U->P3pos);
		printf_debug8("---qn32qp23[%d][%d][%d][%d]\n", pul_U->Q3neg, pul_U->Q2neg, pul_U->Q2pos, pul_U->Q3pos);
    } 
    if(GETBIT(pul_U->ul_mode, UL_MODE_CONST_Q) == 1) { //UL_MODE_CONST_Q
        printf_debug8("---p_cmd[%d] q_cmd[%d]\n", pul_U->p_cmd, pul_U->q_cmd);
    } 
    if(GETBIT(pul_U->ul_mode, UL_MODE_VP) == 1) { //UL_MODE_VP
		pul_U->uac_now = (int16_t)((fpga_pdatabuf_get[Addr_Param256] + fpga_pdatabuf_get[Addr_Param257] + fpga_pdatabuf_get[Addr_Param258]) / 3.0f);
		pul_U->uac_rate = pMs->Uac_rate;
		pul_U->m3V1 = pMs->VP_V1;
		pul_U->m3V2 = pMs->VP_V2;
		pul_U->m3P1 = pMs->VP_P1;
		pul_U->m3P2 = pMs->VP_P2;
        printf_debug8("---p_cmd[%d] q_cmd[%d]\n", pul_U->p_cmd, pul_U->q_cmd);
        printf_debug8("---uac_now[%d] uac_rate[%d] p_rate[%d] p_max[%d]\n", pul_U->uac_now, pul_U->uac_rate, pul_U->p_rate, pul_U->p_max);
		printf_debug8("---v12p12[%d][%d][%d][%d]\n", pul_U->m3V1, pul_U->m3V2, pul_U->m3P1, pul_U->m3P2);
    } 
    if(GETBIT(pul_U->ul_mode, UL_MODE_PFR) == 1) {
        pul_U->f_now = fpga_pdatabuf_get[Addr_Param271];
		pul_U->f_rate = pMs->Freq_rate;
        pul_U->pfr_dbUF = pMs->Pfr_dbUF;
        pul_U->pfr_kUF = pMs->Pfr_kUF;
        pul_U->pfr_dbOF = pMs->Pfr_dbOF;
        pul_U->pfr_kOF = pMs->Pfr_kOF;
        pul_U->pfr_upLmt = 8192;
        pul_U->pfr_lwLmt = -8192;
        pul_U->pfr_Tresp_ms = pMs->Pfr_Tresp_ms;
        pul_U->Ts_ms = UM_TASK_PERIOD;
        printf_debug8("---p_cmd[%d] q_cmd[%d]\n", pul_U->p_cmd, pul_U->q_cmd);
        printf_debug8("---f_now[%d] f_rate[%d]\n", pul_U->f_now, pul_U->f_rate);
        printf_debug8("---pfr_dbUF[%d] pfr_kUF[%d] pfr_dbOF[%d] pfr_kOF[%d]\n", pul_U->pfr_dbUF, pul_U->pfr_kUF, pul_U->pfr_dbOF, pul_U->pfr_kOF);
        printf_debug8("---pfr_upLmt[%d] pfr_lwLmt[%d]\n", pul_U->pfr_upLmt, pul_U->pfr_lwLmt);
        printf_debug8("---pfr_Tresp_ms[%d] Ts_ms[%d]\n", pul_U->pfr_Tresp_ms, pul_U->Ts_ms);
    }   
}


static void ul_mode_handle(ul_mode_opt_t *pUmo)
{
    fpga_write(Addr_Param26, pUmo->pRty->p_cmd_out);
    fpga_write(Addr_Param27, pUmo->pRty->q_cmd_out);
    printf_debug8("------p_out[%d] q_out[%d] freq_lpf[%d]\n\n", pUmo->pRty->p_cmd_out, pUmo->pRty->q_cmd_out, pUmo->pRty->freq_lpf);
}


static void ul_mode_pwr_assign(ul_mode_opt_t *pUmo)
{
    fpga_write(Addr_Param26, pUmo->pCmd);
    fpga_write(Addr_Param27, pUmo->qCmd);
}


void ul_mode_task(const void *pdata)
{
    ul_mode_opt_t *pUmo = (ul_mode_opt_t *)pdata;
    
    pUmo->pRtu = &ul_U;
    pUmo->pRty = &ul_Y;
    pUmo->pMs = &miro_write;
    
    ul_mode_control_initialize();

    //pUmo->pRtu->lpf_times = (uint16_T)(1000 / UM_TASK_PERIOD);
    pUmo->pRtu->lpf_times = pUmo->pMs->Lpf_times;

    while(1)
    {
        ul_input_vars_update(pUmo);

        if((pUmo->pRtu->ul_mode & 0x5F) != 0) {
            ul_mode_control_step();
            ul_mode_handle(pUmo);
        } else {
            ul_mode_pwr_assign(pUmo);
        }
        
        osDelay(UM_TASK_PERIOD);
    }  
}


