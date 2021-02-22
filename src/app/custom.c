#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <cmsis_os.h>
#include "cpu.h"
#include "stm32f4xx_hal.h"
#include "rl_net.h"
#include "custom.h"
#include "tty.h"
#include "para.h"
#include "fpga.h"
#include "sdata_alloc.h"
#include "screen.h"
#include "micro.h"
#include "log.h"

#define FILTER_NUM		(3)
#define SVG_P_HIG		(500)
#define SVG_P_LOW		(-500)
#define SVG_Q_HIG		(200)
#define SVG_Q_LOW		(-200)
#define VOL_LIMIT_DIFF1	(3) //充放电限压预留差值(V)
#define VOL_LIMIT_DIFF2	(5) //充放电限压预留差值(V)

svg_info_t svg_info;
chrg_disc_info_t chrg_disc_info;
ol_shutdown_info_t ol_shutdown_info;

//static void UacFreqFilter_task(const void* pdata);
//static void PcsWorkCheck_task(const void* pdata);

osTimerDef(ol_shutdown_timer, (os_ptimer)set_pcs_shutdown);
//osThreadDef(UacFreqFilter_task, osPriorityNormal, 1, 0);
//osThreadDef(PcsWorkCheck_task, osPriorityNormal, 1, 0);

extern osThreadId SVGModeEn_task_id;

#if 0

/* 每隔100ms采样一次三相电压和频率，做FILTER_NUM*100ms的滤波 */
static void UacFreqFilter_task(const void* pdata)
{
    unsigned char i = 0, cnt = 0;
    short U1[FILTER_NUM] = {0}, U1_f = 0;
    short U2[FILTER_NUM] = {0}, U2_f = 0;
    short U3[FILTER_NUM] = {0}, U3_f = 0;
    short Freq[FILTER_NUM] = {0}, Freq_f = 0;

    while(1)
    {
        svg_info.svg_en = arm_config_data_read(SVG_EN_ADD);
        if(svg_info.svg_en == 1) {
            readpara(320, &U1[cnt%FILTER_NUM]);
            readpara(321, &U2[cnt%FILTER_NUM]);
            readpara(322, &U3[cnt%FILTER_NUM]);
            readpara(335, &Freq[cnt%FILTER_NUM]);

            if(cnt%FILTER_NUM == (FILTER_NUM - 1)) {
                for(i = 0; i < FILTER_NUM; i++) {
                    U1_f += U1[i];
                    U2_f += U2[i];
                    U3_f += U3[i];
                    Freq_f += Freq[i];
                }
                U1_f = (unsigned short)(U1_f/FILTER_NUM);
                U2_f = (unsigned short)(U2_f/FILTER_NUM);
                U3_f = (unsigned short)(U3_f/FILTER_NUM);
                svg_info.Uac_aver = (unsigned short)((U1_f + U2_f + U3_f)/3);
                svg_info.Freq_aver = (unsigned short)(Freq_f/FILTER_NUM);
                U1_f = 0;
                U2_f = 0;
                U3_f = 0;
                Freq_f = 0;
                osSignalSet(SVGModeEn_task_id, 1);
            }
            //tty_printf("cnt[%d] u1[%d] u2[%d] u3[%d] freq[%d]\n", cnt, U1[cnt%FILTER_NUM], U2[cnt%FILTER_NUM], U3[cnt%FILTER_NUM], Freq[cnt%FILTER_NUM]);
            //tty_printf("u_aver[%d] f_aver[%d]\n\n", svg_info.Uac_aver, svg_info.Freq_aver);
            cnt++;
            if(cnt == FILTER_NUM) {
                cnt = 0;
            }
        } else {
            cnt = 0;
            U1_f = 0;
            U2_f = 0;
            U3_f = 0;
            Freq_f = 0;
            //memset(U1, 0, FILTER_NUM);
            //memset(U2, 0, FILTER_NUM);
            //memset(U3, 0, FILTER_NUM);
            //memset(Freq, 0, FILTER_NUM);
        }

        osDelay(100);
    }
}



void SVGModeEn_task(const void* pdata)
{
    CPU_SR cpu_sr;
    short Uref = 0, p = 0, q = 0;

    /* 电网采样电压频率滤波任务 */
    osThreadCreate(osThread(UacFreqFilter_task), NULL);

    while(1)
    {
        svg_info.svg_en = arm_config_data_read(SVG_EN_ADD);
        if(svg_info.svg_en == 1) {
            osSignalWait(1, osWaitForever); //等待电压频率滤波完成
            Uref = getting_data_handle(Addr_Param24, fpga_pdatabuf_get[Addr_Param24]);
            p = (short)(PQNonSVG.p/MODULUS);
            q = (short)(PQNonSVG.q/MODULUS);
            //tty_printf("Uref[%d] Uac_aver[%d] Freq_aver[%d] \n", Uref, svg_info.Uac_aver, svg_info.Freq_aver);
            //tty_printf("p[%d] q[%d]\n", p, q);
#if 0
            p += (short)((50*MODULUS-svg_info.Freq_aver)*500/MODULUS);
            if(p > SVG_P_HIG) {
                p = (short)SVG_P_HIG;
            } else if(p < SVG_P_LOW) {
                p = (short)SVG_P_LOW;
            }
#endif

            q += (short)(Uref-svg_info.Uac_aver)*230/40/MODULUS;
            if(q > SVG_Q_HIG) {
                q = (short)SVG_Q_HIG;
            } else if(q < SVG_Q_LOW) {
                q = (short)SVG_Q_LOW;
            }
            //tty_printf("p[%d] q[%d]\n\n", p, q);
            fpga_pdatabuf_get[Addr_Param26]=setting_data_handle(Addr_Param26, p*MODULUS);
            fpga_pdatabuf_get[Addr_Param27]=setting_data_handle(Addr_Param27, q*MODULUS);
            CPU_CRITICAL_ENTER();
            fpga_write(Addr_Param26,fpga_pdatabuf_get[Addr_Param26]);
            //fpga_pdatabuf_write(Addr_Param26,fpga_pdatabuf_get[Addr_Param26]);
            fpga_write(Addr_Param27,fpga_pdatabuf_get[Addr_Param27]);
            //fpga_pdatabuf_write(Addr_Param27,fpga_pdatabuf_get[Addr_Param27]);
            CPU_CRITICAL_EXIT();
        }
        osDelay(1000);
    }
}


#define CURRENT_THREHOLD	(10) //充放电流阈值A
static void PcsWorkCheck_task(const void* pdata)
{
    unsigned short Ibat = 0;
    short value = 0;

    while(1)
    {
        readpara(327, &value);
        Ibat = (unsigned short)(value/MODULUS);
        chrg_disc_info.mode_en = arm_config_data_read(END_SWITCH_EN_ADD);
        //suk_printf("Ibat[%d] mode_en[%d]\n", Ibat, chrg_disc_info.mode_en);

        if(chrg_disc_info.mode_en == 1) {
            if(abs(Ibat) > CURRENT_THREHOLD) {
                if(chrg_disc_info.work_cnt == 5) { //充放电超过5s
                    chrg_disc_info.work_flag = 1;
                } else {
                    chrg_disc_info.work_cnt++;
                }
            } else {
                chrg_disc_info.work_flag = 0;
                chrg_disc_info.work_cnt = 0;
            }
        } else {
            chrg_disc_info.work_flag = 0;
            chrg_disc_info.work_cnt = 0;
        }
        //suk_printf("work_flag[%d]\n\n", chrg_disc_info.work_flag);

        osDelay(1000);
    }
}


void ChrgDiscSwitch_task(const void* pdata)
{
    CPU_SR cpu_sr;
    unsigned short Ubat = 0;
    short value = 0, addr_tmp;

    /* 末期转恒压充放条件检测任务 */
    osThreadCreate(osThread(PcsWorkCheck_task), NULL);

    while(1)
    {
        if(chrg_disc_info.work_flag == 1) {
            readpara(328, &value);
            Ubat = (unsigned short)(value/MODULUS);
            chrg_disc_info.chrg_vol_limit = arm_config_data_read(CHRG_VOL_LIMIT_ADD)/MODULUS;
            chrg_disc_info.disc_vol_limit = arm_config_data_read(DISC_VOL_LIMIT_ADD)/MODULUS;

            //tty_printf("Ubat[%d] chrg_vol_limit[%d] disc_vol_limit[%d]\n", Ubat, chrg_disc_info.chrg_vol_limit, chrg_disc_info.disc_vol_limit);
            //tty_printf("chrg_disc_state[%d]\n\n", chrg_disc_info.chrg_disc_state);

            if((chrg_disc_info.chrg_disc_state == 0) && (Ubat > chrg_disc_info.chrg_vol_limit - VOL_LIMIT_DIFF1)) {
                chrg_disc_info.chrg_disc_state = 1;
                CPU_CRITICAL_ENTER();
                fpga_pdatabuf_get[Addr_Param23]=setting_data_handle(Addr_Param23, chrg_disc_info.chrg_vol_limit*MODULUS);
                setpara(Addr_Param23,fpga_pdatabuf_get[Addr_Param23]);
                fpga_write(Addr_Param23,fpga_pdatabuf_get[Addr_Param23]);
                fpga_pdatabuf_write(Addr_Param23,fpga_pdatabuf_get[Addr_Param23]);
                CPU_CRITICAL_EXIT();
                if(Pcs_Run_Mode_Get() != IV_MODE) {
                    Pcs_Run_Mode_Set(IV_MODE);
                    addr_tmp = Addr_Param31;
                    fpga_pdatabuf_get[addr_tmp]=setting_data_handle(addr_tmp, (-1)*650*MODULUS);
                    setpara(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_ENTER();
                    fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_EXIT();
                    addr_tmp = Addr_Param32;
                    fpga_pdatabuf_get[addr_tmp]=setting_data_handle(addr_tmp, 650*MODULUS);
                    setpara(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_ENTER();
                    fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_EXIT();
                    addr_tmp = Addr_Param33;
                    fpga_pdatabuf_get[addr_tmp]=setting_data_handle(addr_tmp, (-1)*1320*MODULUS);
                    setpara(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_ENTER();
                    fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_EXIT();
                    addr_tmp = Addr_Param34;
                    fpga_pdatabuf_get[addr_tmp]=setting_data_handle(addr_tmp, 1320*MODULUS);
                    setpara(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_ENTER();
                    fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_EXIT();
                }
            } else if((chrg_disc_info.chrg_disc_state == 0) && (Ubat < chrg_disc_info.disc_vol_limit + VOL_LIMIT_DIFF1)) {
                chrg_disc_info.chrg_disc_state = 2;
                CPU_CRITICAL_ENTER();
                fpga_pdatabuf_get[Addr_Param23]=setting_data_handle(Addr_Param23, chrg_disc_info.disc_vol_limit*MODULUS);
                setpara(Addr_Param23,fpga_pdatabuf_get[Addr_Param23]);
                fpga_write(Addr_Param23,fpga_pdatabuf_get[Addr_Param23]);
                fpga_pdatabuf_write(Addr_Param23,fpga_pdatabuf_get[Addr_Param23]);
                CPU_CRITICAL_EXIT();
                if(Pcs_Run_Mode_Get() != IV_MODE) {
                    Pcs_Run_Mode_Set(IV_MODE);
                    addr_tmp = Addr_Param31;
                    fpga_pdatabuf_get[addr_tmp]=setting_data_handle(addr_tmp, (-1)*650*MODULUS);
                    setpara(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_ENTER();
                    fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_EXIT();
                    addr_tmp = Addr_Param32;
                    fpga_pdatabuf_get[addr_tmp]=setting_data_handle(addr_tmp, 650*MODULUS);
                    setpara(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_ENTER();
                    fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_EXIT();
                    addr_tmp = Addr_Param33;
                    fpga_pdatabuf_get[addr_tmp]=setting_data_handle(addr_tmp, (-1)*1320*MODULUS);
                    setpara(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_ENTER();
                    fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_EXIT();
                    addr_tmp = Addr_Param34;
                    fpga_pdatabuf_get[addr_tmp]=setting_data_handle(addr_tmp, 1320*MODULUS);
                    setpara(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_ENTER();
                    fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_EXIT();
                }
            } else if((chrg_disc_info.chrg_disc_state == 1) && (Ubat < chrg_disc_info.chrg_vol_limit - VOL_LIMIT_DIFF2)) {
                chrg_disc_info.chrg_disc_state = 0;
                if(Pcs_Run_Mode_Get() != PQ_MODE) {
                    Pcs_Run_Mode_Set(PQ_MODE);
                }
            } else if((chrg_disc_info.chrg_disc_state == 2) && (Ubat > chrg_disc_info.disc_vol_limit + VOL_LIMIT_DIFF2)) {
                chrg_disc_info.chrg_disc_state = 0;
                if(Pcs_Run_Mode_Get() != PQ_MODE) {
                    Pcs_Run_Mode_Set(PQ_MODE);
                }
            }
        }

        osDelay(1000);
    }
}

#endif


/* 120%过载10min */
void OverLoadShutdown_task(const void* pdata)
{

	short current_power;	
    while(1)
    {
		if((GETBIT(miro_write.strategy_en,9)==1))
		{
			current_power = (short)(getting_data_handle(Addr_Param131, Upper_Param[Addr_Param131])/MODULUS);
			//过载保护值读取
			miro_write.over_load1_value = arm_config_data_read(OVER_LOAD_L1); 	
			miro_write.over_load1_time = arm_config_data_read(OVER_LOAD_L1_TIME); 	
			miro_write.over_load2_value = arm_config_data_read(OVER_LOAD_L2); 	
			miro_write.over_load2_time = arm_config_data_read(OVER_LOAD_L2_TIME); 
			miro_write.over_load3_value = arm_config_data_read(OVER_LOAD_L3); 
			
	        if((current_power >=miro_write.over_load1_value) && (current_power < miro_write.over_load2_value)) 
	        {
				ol_shutdown_info.ol1_tick++;
				ol_shutdown_info.ol2_tick = 0;
				if(ol_shutdown_info.ol1_tick >= 5)
				{
					ol_shutdown_info.ol1_flag = 1;
		            ol_shutdown_info.ol2_flag = 0;
		            ol_shutdown_info.ol3_flag = 0;
					SETBIT(out_state_info.Alarm_HMI,3);
				}
				if(ol_shutdown_info.ol1_tick > miro_write.over_load1_time)
				{				
					//fpga_write(Addr_Param26,0);			
					set_pcs_shutdown();			
					log_add(ET_OPSTATE,EST_OVERLOAD1_ERR);	
					ol_shutdown_info.ol1_tick = 0;
				}
				
	        }

			
			else if((current_power >=miro_write.over_load2_value) && (current_power < miro_write.over_load3_value)) 
			{
				ol_shutdown_info.ol1_tick = 0;
				ol_shutdown_info.ol2_tick++;
				
				if(ol_shutdown_info.ol2_tick >=5)
				{
					ol_shutdown_info.ol1_flag = 0;
		            ol_shutdown_info.ol2_flag = 1;
		            ol_shutdown_info.ol3_flag = 0;
					SETBIT(out_state_info.Alarm_HMI,4);
				}
				if(ol_shutdown_info.ol2_tick >miro_write.over_load2_time)
				{				
					//fpga_write(Addr_Param26,0);					
					set_pcs_shutdown();			
					log_add(ET_OPSTATE,EST_OVERLOAD2_ERR);						
					ol_shutdown_info.ol2_tick = 0;
				}
				
	        }
			else if((current_power >= miro_write.over_load3_value)) 
			{
				ol_shutdown_info.ol1_tick = 0;
				ol_shutdown_info.ol2_tick = 0;
				ol_shutdown_info.ol1_flag = 0;
	            ol_shutdown_info.ol2_flag = 0;
	            ol_shutdown_info.ol3_flag = 1;			
				set_pcs_shutdown();			
				log_add(ET_OPSTATE,EST_OVERLOAD3_ERR);			
	        }
			else
			{
				ol_shutdown_info.ol1_tick = 0;
				ol_shutdown_info.ol2_tick = 0;
				ol_shutdown_info.ol1_flag = 0;
	            ol_shutdown_info.ol2_flag = 0;
	            ol_shutdown_info.ol3_flag = 0;
				out_state_info.Alarm_HMI = RESETBIT(out_state_info.Alarm_HMI,3);
				out_state_info.Alarm_HMI = RESETBIT(out_state_info.Alarm_HMI,4);
			}
		}
        osDelay(1000);
   }
}


void Frequency_task(const void* pdata)
{
	short fre_now;
	short power_get_value;
	short pcs_state;
	short power_set_value;
    while(1)
    {		
		pcs_state = fpga_read(STATE8_ADD) & 0xf;	
		if((GETBIT(miro_write.strategy_en,11)==1))
		{			
			fre_now = getting_data_handle(Addr_Param135, Upper_Param[Addr_Param135]);
			power_get_value = getting_data_handle(Addr_Param26, fpga_pdatabuf_get[Addr_Param26]);
			
			if((fre_now <480) && (power_get_value < 0) &&(pcs_state == FSM_WORKING))
			{
				power_set_value = 0;
				power_set_value = setting_data_handle(Addr_Param26, power_set_value);				
				fpga_write(Addr_Param26,power_set_value);
			}

			else if((fre_now >=480) &&(fre_now <495) && (power_get_value < 0) &&(pcs_state == FSM_WORKING))
			{
				power_set_value = (short)(50);
				power_set_value = setting_data_handle(Addr_Param26, power_set_value);				
				fpga_write(Addr_Param26,power_set_value);
			}
			
			else if((fre_now >502) &&(fre_now <=505) && (power_get_value > 0) &&(pcs_state == FSM_WORKING))
			{
				power_set_value = (short)(-50);
				power_set_value = setting_data_handle(Addr_Param26, power_set_value);				
				fpga_write(Addr_Param26,power_set_value);
			}
			
			else if((fre_now >505) && (power_get_value > 0) &&(pcs_state == FSM_WORKING))
			{
				power_set_value = 0;
				power_set_value = setting_data_handle(Addr_Param26, power_set_value);				
				fpga_write(Addr_Param26,power_set_value);
			}

			
		}
    osDelay(20);
    }
}


