#include "stdio.h"
#include <cmsis_os.h>
#include "cpu.h"
#include "network.h"
#include "stm32f4xx_hal.h"
//#include "filesystem.h"
#include "bsp.h"
#include "env.h"
#include "shell_com.h"
#include "modbus_comm.h"
#include "modbus_master.h"
#include "telnet.h"
#include "tty.h"
#include "log.h"
#include "MBServer.h"
#include "storedata.h"
#include "sys.h"
#include "fpga.h"
#include "para.h"
#include "record_wave.h"
#include "micro.h"
#include "net_ems.h"
#include "wave_file.h"
#include "custom.h"
#include "nand_file.h"
#include "flashdata.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "rl_net.h"
#include "pcs.h"
#include "sdata_alloc.h"
#include "systime.h"
#include "screen.h"
#include "can.h"
#include "w5xxxeth_app.h"
#include "initw5xxx.h"
#include "rs485_1_app.h"
#include "rs485_2_app.h"
#include "debug.h"
#include "freq_prot.h"
#include "soft_bootup.h"
#include "fault_rec.h"
#include "comm_offline.h"
#include "ul_mode_control.h"


#define PVM4_VERSION      "1.0"
#define CUSTOM

//#define FUNCTION 1

system_control sys_ctl;
modbus_information mb_inf;
modbus_information mb_inf_u0;
modbus_data mb_data;
volatile micro_set miro_write;

extern _CountData CountData;
extern _power_data power_data;
extern volatile float *nvrom_pw_info;
extern volatile unsigned int *nvrom_pw_time_info;

osThreadId tid_shell_com_task;
osThreadDef(shell_com_task, osPriorityNormal, 1, 0);
osThreadId tid_telnet_task;
osThreadDef(telnet_task, osPriorityNormal, 1, 0);
osThreadId tid_MBsever_task;
osThreadDef(MBsever_task, osPriorityNormal, 1, 0);
osThreadId tid_mbresp_spec_task;
osThreadDef(mbresp_spec_task, osPriorityNormal, 1, 0);
osThreadId tid_fpga_cal_task;
osThreadDef(fpga_cal_task,osPriorityNormal,1,0);
osThreadId tid_fpga_data_refresh_task;
osThreadDef(fpga_data_refresh_task,osPriorityNormal, 1, 0);

osThreadId tid_pcs_handle_task;
osThreadDef(pcs_handle_task, osPriorityNormal, 1, 0);

//osThreadId tid_power_ctl_task;
//osThreadDef(power_ctl_task, osPriorityNormal, 1, 0);

osThreadId tid_modbus_screen;
osThreadDef(modbus_screen_task, osPriorityNormal, 1, 0);

//osThreadId tid_net_ems_task;
//osThreadDef(net_ems_task, osPriorityNormal, 1, 0);

extern void watch_dog_task(const void *pdata);
osThreadId tid_watch_dog_task;
//osThreadDef(watch_dog_task,osPriorityBelowNormal, 1, 0);
osThreadDef(watch_dog_task,osPriorityNormal, 1, 0);

#if 1
osThreadId tid_modbus_master;
osThreadDef(modbus_master_task,osPriorityAboveNormal, 1, 0);
#else
osThreadId tid_modbus_service;
osThreadDef(modbus_service_task,osPriorityNormal, 1, 0);
#endif

osThreadId tid_can_task;
osThreadDef(CAN_Thread, osPriorityNormal, 1, 0);

osThreadId tid_overload_task;
osThreadDef(OverLoadShutdown_task, osPriorityNormal, 1, 0);

osThreadId tid_freq_task;
osThreadDef(Frequency_task, osPriorityAboveNormal, 1, 0);


extern void bms_ems_task(const void *pdata);
osThreadId tid_bms_task;
osThreadDef(bms_ems_task, osPriorityNormal, 1, 0);

//#ifdef CUSTOM
//osThreadId SVGModeEn_task_id;
//osThreadDef(SVGModeEn_task, osPriorityNormal, 1, 0);
//osThreadDef(ChrgDiscSwitch_task, osPriorityNormal, 1, 0);
//#endif

osThreadId tid_nand;
extern void nand_task(const void* pdata);
osThreadDef(nand_task, osPriorityNormal, 1, 0);

osThreadId tid_w5500_net0server0;
osThreadDef(W5xxxEth_Net0_Server0, osPriorityNormal, 1, 0);

osThreadId tid_w5500_net0client0;
osThreadDef(W5xxxEth_Net0_Client0, osPriorityNormal, 1, 0);

osThreadId tid_w5500_net1server0;
osThreadDef(W5xxxEth_Net1_Server0, osPriorityNormal, 1, 0);

osThreadId tid_w5500_net1client0;
osThreadDef(W5xxxEth_Net1_Client0, osPriorityNormal, 1, 0);

#if 1
//osThreadId tid_rs485_1;
//osThreadDef(RS485_1_Thread, osPriorityNormal, 1, 0);
osThreadId tid_rs485_2;
osThreadDef(RS485_2_Thread, osPriorityNormal, 1, 0);
#endif

osThreadId tid_freq_prot_task;
osThreadDef(freq_prot_task, osPriorityNormal, 1, 0);
osThreadId tid_soft_bootup_task;
osThreadDef(soft_bootup_task, osPriorityNormal, 1, 0);
osThreadId tid_fault_rec_task;
osThreadDef(fault_rec_task, osPriorityNormal, 1, 0);
osThreadId tid_comm_offline_task;
osThreadDef(comm_offline_task, osPriorityNormal, 1, 0);
osThreadId tid_ul_mode_task;
osThreadDef(ul_mode_task, osPriorityNormal, 1, 0);


osSemaphoreId tty_send_sem;
osSemaphoreDef(tty_send_sem);


void print_clear(void)
{
    printf("\033[2J\033[0;0H");
}

void hmi_slave_id_get()
{
	char *str;
    
    str = getenv("slave_id");
    if(str == NULL) {
		miro_write.slave_id = 1;
    } else {
        miro_write.slave_id = atoi(str);
    }
}


void software_init(void)
{
	hmi_slave_id_get();
	
    if(arm_config_data_read(INI_MONITOR_ADDR_ADD) == 0) 
	{
        arm_config_data_write(INI_MONITOR_ADDR_ADD, 0x05);
    }
	miro_write.modbus_addr = arm_config_data_read(INI_MONITOR_ADDR_ADD);
	miro_write.communication_en = arm_config_data_read(COMMUICATION_PRO_EN); 
	miro_write.strategy_en = arm_config_data_read(STRATEGY_EN); 
	
    CountData.Power_Con         		= count_cal[POWER_CON_ADD];
    CountData.power_ac_con       		= count_cal[POWER_AC_CON_ADD];
    CountData.power_ac_day   			= count_cal[POWER_AC_DAY_ADD];
    CountData.power_ac_yesterday 		= count_cal[POWER_AC_Y_ADD];
    CountData.power_ac_month 			= count_cal[POWER_AC_MONTH_ADD];
    CountData.power_ac_year 			= count_cal[POWER_AC_YEAR_ADD];
    CountData.power_ac_c_yesterday		= count_cal[POWER_AC_C_Y_ADD];
    CountData.power_ac_c_month			= count_cal[POWER_AC_C_MONTH_ADD];
    CountData.power_ac_c_year			= count_cal[POWER_AC_C_YEAR_ADD];
    CountData.power_ac_c_day    		= count_cal[POWER_AC_C_DAY_ADD];
    CountData.power_ac_c_con    		= count_cal[POWER_AC_C_CON_ADD];
    CountData.power_dc1_day     		= count_cal[POWER_DC1_DAY_ADD];
    CountData.power_dc1_con     		= count_cal[POWER_DC1_CON_ADD];
    CountData.power_dc1_c_day   		= count_cal[POWER_DC1_C_DAY_ADD];
    CountData.power_dc1_c_con   		= count_cal[POWER_DC1_C_CON_ADD];
    CountData.power_dc2_day     		= count_cal[POWER_DC2_DAY_ADD];
    CountData.power_dc2_con     		= count_cal[POWER_DC2_CON_ADD];
    CountData.power_dc2_yesterday 		= count_cal[POWER_DC2_Y_ADD];
    CountData.power_dc2_month		 	= count_cal[POWER_DC2_MON_ADD];
    CountData.co2_dec					= count_cal[CO2_DEC_ADD];
    CountData.C_dec						= count_cal[C_DEC_ADD];
	
    CountData.run_time_total = count_cal[RUN_TIME];
    CountData.run_time_old	= count_cal[RUN_TIME];
	CountData.run_time_now = 0;
	
    CountData.on_time_total = count_cal[ON_TIME];
    CountData.on_time_old = count_cal[ON_TIME];
	CountData.on_time_now = 0;	
	
    CountData.fre_adjust_count = count_cal[FREADJUST_COUNT];
    CountData.fre_adjust_count_old = count_cal[FREADJUST_COUNT];
	CountData.fre_adjust_count_now = 0;	
    CountData.fre_adjust_count_today= count_cal[FREADJUST_COUNT_TODAY];

    CountData.fre_lock_count = count_cal[FRELOCK_COUNT];
    CountData.fre_lock_count_old = count_cal[FRELOCK_COUNT];
	CountData.fre_lock_count_now = 0;	
    CountData.fre_lock_count_today= count_cal[FRELOCK_COUNT_TODAY];


    count_cal_write(POWER_DC2_C_DAY_ADD, 0);
    count_cal_write(POWER_DC2_C_CON_ADD, 0);

    CountData.power_ac_day_cache 		= CountData.power_ac_day;
    CountData.power_ac_c_day_cache 		= CountData.power_ac_c_day;
    CountData.power_dc2_day_cache 		= CountData.power_dc2_day;

    setpara(FD_UINX_ADD_H,		(short)((unsigned int)arm_config_data_read32(FD_UINX_ADD) >> 16));
    setpara(FD_UINX_ADD_L,		(short)((unsigned int)arm_config_data_read32(FD_UINX_ADD) & 0xffff));
    setpara(FD_IINX_ADD_H,		(short)((unsigned int)arm_config_data_read32(FD_IINX_ADD) >> 16));
    setpara(FD_IINX_ADD_L,		(short)((unsigned int)arm_config_data_read32(FD_IINX_ADD) & 0xffff));
    setpara(FD_UDC_ADD_H,		(short)((unsigned int)arm_config_data_read32(FD_UDC_ADD) >> 16));
    setpara(FD_UDC_ADD_L,		(short)((unsigned int)arm_config_data_read32(FD_UDC_ADD) & 0xffff));
    setpara(FD_IDC_ADD_H,		(short)((unsigned int)arm_config_data_read32(FD_IDC_ADD) >> 16));
    setpara(FD_IDC_ADD_L,		(short)((unsigned int)arm_config_data_read32(FD_IDC_ADD) & 0xffff));
    setpara(FD_UBTR_ADD_H,		(short)((unsigned int)arm_config_data_read32(FD_UBTR_ADD) >> 16));
    setpara(FD_UBTR_ADD_L,		(short)((unsigned int)arm_config_data_read32(FD_UBTR_ADD) & 0xffff));
    setpara(FD_UPOS_GND_ADD_H,	(short)((unsigned int)arm_config_data_read32(FD_UPOS_GND_ADD) >> 16));
    setpara(FD_UPOS_GND_ADD_L,	(short)((unsigned int)arm_config_data_read32(FD_UPOS_GND_ADD) & 0xffff));
    setpara(FD_UNEG_GND_ADD_H,	(short)((unsigned int)arm_config_data_read32(FD_UNEG_GND_ADD) >> 16));
    setpara(FD_UNEG_GND_ADD_L,	(short)((unsigned int)arm_config_data_read32(FD_UNEG_GND_ADD) & 0xffff));
    setpara(FD_PAC_ADD_H,		(short)((unsigned int)arm_config_data_read32(FD_PAC_ADD) >> 16));
    setpara(FD_PAC_ADD_L,		(short)((unsigned int)arm_config_data_read32(FD_PAC_ADD) & 0xffff));
    setpara(FD_PDC_ADD_H,		(short)((unsigned int)arm_config_data_read32(FD_PDC_ADD) >> 16));
    setpara(FD_PDC_ADD_L,		(short)((unsigned int)arm_config_data_read32(FD_PDC_ADD) & 0xffff));
    setpara(FD_IAC_CT_ADD_H,	(short)((unsigned int)arm_config_data_read32(FD_IAC_CT_ADD) >> 16));
    setpara(FD_IAC_CT_ADD_L,	(short)((unsigned int)arm_config_data_read32(FD_IAC_CT_ADD) & 0xffff));
    setpara(FD_PAC_CT_ADD_H,	(short)((unsigned int)arm_config_data_read32(FD_PAC_CT_ADD) >> 16));
    setpara(FD_PAC_CT_ADD_L,	(short)((unsigned int)arm_config_data_read32(FD_PAC_CT_ADD) & 0xffff));
    setpara(FD_IAC_LKG_H,		(short)((unsigned int)arm_config_data_read32(FD_IAC_LKG_ADD) >> 16));
    setpara(FD_IAC_LKG_L,		(short)((unsigned int)arm_config_data_read32(FD_IAC_LKG_ADDH) & 0xffff));	

    setpara(MP_INI_RESIS_FD,        arm_config_nand_data[INI_RESIS_FD_ADD]);
    setpara(MP_INI_RESIS_REF,       arm_config_nand_data[INI_RESIS_REF_ADD]);
    setpara(MP_INI_MONITOR_ADDR,    arm_config_nand_data[INI_MONITOR_ADDR_ADD]);
    setpara(MP_MAX_P_ADD,   		arm_config_nand_data[MAX_P_ADD]);
    setpara(MP_MIN_P_ADD,   		arm_config_nand_data[MIN_P_ADD]);
    setpara(MP_MAX_BA_C_ADD,    	arm_config_nand_data[MAX_BA_C_ADD]);
    setpara(MP_MIN_BA_C_ADD,    	arm_config_nand_data[MIN_BA_C_ADD]);
    setpara(MP_MAX_BA_U_ADD,    	arm_config_nand_data[MAX_BA_U_ADD]);
    setpara(MP_MIN_BA_U_ADD,    	arm_config_nand_data[MIN_BA_U_ADD]);
    setpara(MP_MAX_Q_ADD,   		arm_config_nand_data[MAX_Q_ADD]);
    setpara(MP_MIN_Q_ADD,   		arm_config_nand_data[MIN_Q_ADD]);
    setpara(MP_LOACAL_ASK,  		arm_config_nand_data[ASK_LOCAL]);
    setpara(MP_CHRG_VOL_LIMIT,  	arm_config_nand_data[CHRG_VOL_LIMIT_ADD]);
    setpara(MP_DISC_VOL_LIMIT,  	arm_config_nand_data[DISC_VOL_LIMIT_ADD]);
    setpara(MP_SVG_EN,  			arm_config_nand_data[SVG_EN_ADD]);
    setpara(MP_ISO_ISLAND,  		arm_config_nand_data[ISO_ISLAND_ADD]);
	
	setpara(AMMETER1_A0, arm_config_nand_data[ammeter1_A0]);
	setpara(AMMETER1_A1, arm_config_nand_data[ammeter1_A1]);			
	setpara(AMMETER1_A2, arm_config_nand_data[ammeter1_A2]);
	setpara(AMMETER1_A3, arm_config_nand_data[ammeter1_A3]);
	setpara(AMMETER1_A4, arm_config_nand_data[ammeter1_A4]);
	setpara(AMMETER1_A5, arm_config_nand_data[ammeter1_A5]);
	setpara(AMMETER1_VRC, arm_config_nand_data[ammeter1_vrc]);
	
	setpara(AMMETER2_A0, arm_config_nand_data[ammeter2_A0]);
	setpara(AMMETER2_A1, arm_config_nand_data[ammeter2_A1]);			
	setpara(AMMETER2_A2, arm_config_nand_data[ammeter2_A2]);
	setpara(AMMETER2_A3, arm_config_nand_data[ammeter2_A3]);
	setpara(AMMETER2_A4, arm_config_nand_data[ammeter2_A4]);
	setpara(AMMETER2_A5, arm_config_nand_data[ammeter2_A5]);
	setpara(AMMETER2_VRC, arm_config_nand_data[ammeter2_vrc]);
	setpara(DIS_MIN_POWER, arm_config_nand_data[dis_min_power]);
	setpara(COS_SETA_PC_SET, arm_config_nand_data[cos_seta]);
	
	setpara(Ctrl_Mode_SET, arm_config_nand_data[Prio_Set_ADD]);
	
	pcs_manage.meter1_a0 = (uint8_t)((arm_config_data_read(ammeter1_A0) /10 << 4) + (arm_config_data_read(ammeter1_A0)%10));	
	pcs_manage.meter1_a1 = (uint8_t)((arm_config_data_read(ammeter1_A1) /10 << 4) + (arm_config_data_read(ammeter1_A1)%10));
	pcs_manage.meter1_a2 = (uint8_t)((arm_config_data_read(ammeter1_A2) /10 << 4) + (arm_config_data_read(ammeter1_A2)%10));
	pcs_manage.meter1_a3 = (uint8_t)((arm_config_data_read(ammeter1_A3) /10 << 4) + (arm_config_data_read(ammeter1_A3)%10));
	pcs_manage.meter1_a4 = (uint8_t)((arm_config_data_read(ammeter1_A4) /10 << 4) + (arm_config_data_read(ammeter1_A4)%10));
	pcs_manage.meter1_a5 = (uint8_t)((arm_config_data_read(ammeter1_A5) /10 << 4) + (arm_config_data_read(ammeter1_A5)%10));
	
	pcs_manage.meter2_a0 = (uint8_t)((arm_config_data_read(ammeter2_A0) /10 << 4) + (arm_config_data_read(ammeter2_A0)%10));	
	pcs_manage.meter2_a1 = (uint8_t)((arm_config_data_read(ammeter2_A1) /10 << 4) + (arm_config_data_read(ammeter2_A1)%10));	
	pcs_manage.meter2_a2 = (uint8_t)((arm_config_data_read(ammeter2_A2) /10 << 4) + (arm_config_data_read(ammeter2_A2)%10));	
	pcs_manage.meter2_a3 = (uint8_t)((arm_config_data_read(ammeter2_A3) /10 << 4) + (arm_config_data_read(ammeter2_A3)%10));	
	pcs_manage.meter2_a4 = (uint8_t)((arm_config_data_read(ammeter2_A4) /10 << 4) + (arm_config_data_read(ammeter2_A4)%10));	
	pcs_manage.meter2_a5 = (uint8_t)((arm_config_data_read(ammeter2_A5) /10 << 4) + (arm_config_data_read(ammeter2_A5)%10));	

	pcs_manage.meter1_vrc = (uint16_t)(arm_config_data_read(ammeter1_vrc));
	pcs_manage.meter2_vrc = (uint16_t)(arm_config_data_read(ammeter2_vrc));
	pcs_manage.grid_min_power = (uint16_t)(arm_config_data_read(dis_min_power));	
	
	miro_write.bootup_flag = 1;
	log_add(ET_OPSTATE,EST_SYSBOOT);
	
	CountData.restart_tick = count_cal[RESTART_COUNT];
	CountData.restart_tick++;
	count_cal_write(RESTART_COUNT,CountData.restart_tick);
	miro_write.pfm_recod_flag = 1;	
	
	//CRC效验统计
	CountData.crc_err_count	= count_cal[CRC_ERR_COUNT];

	//程序密钥
	#if 1
	miro_write.device_year = arm_config_data_read(DEVICE_CODE_YEAR);
	miro_write.device_md = arm_config_data_read(DEVICE_CODE_MD);
	miro_write.device_type = arm_config_data_read(DEVICE_CODE_TYPE);
	miro_write.device_num = arm_config_data_read(DEVICE_CODE_NUM);
	miro_write.code_remain_date = arm_config_data_read(CODE_REMAIN_DATE);

	//生成出厂密钥
	miro_write.code_release = miro_write.device_year - miro_write.device_md + miro_write.device_type - miro_write.device_num;	
	arm_config_data_write(CODE_RELEASE,miro_write.code_release);

	//生成调试密钥
	miro_write.code_debug= miro_write.device_year + miro_write.device_md*10 + miro_write.device_type - miro_write.device_num;	
	arm_config_data_write(CODE_DEBUG,miro_write.code_debug);	

	//生成临时密钥1
	miro_write.code_temporary1 = miro_write.device_year + miro_write.device_md + miro_write.device_type + miro_write.device_num;
	miro_write.code_temporary1 = (short)((miro_write.code_temporary1*miro_write.code_temporary1)%100000);
	arm_config_data_write(CODE_TEMPORARY1,miro_write.code_temporary1);	

	//生成临时密钥2
	miro_write.code_temporary2 = (short)(sqrt((miro_write.device_year*10.0f + miro_write.device_md + miro_write.device_type + miro_write.device_num)*miro_write.device_num*100.0f));
	miro_write.code_temporary2 = (short)(miro_write.code_temporary2%100000);
	arm_config_data_write(CODE_TEMPORARY2,miro_write.code_temporary2);		

	//生成临时密钥3
	miro_write.code_temporary3 = (short)(sqrt((miro_write.device_year*100.0f + miro_write.device_md + miro_write.device_type + miro_write.device_num)*miro_write.device_num));
	miro_write.code_temporary3 = (short)(miro_write.code_temporary3%100000);
	arm_config_data_write(CODE_TEMPORARY3,miro_write.code_temporary3);		

	//生成永久密钥
	miro_write.code_permanent= (short)(sqrt((miro_write.device_year + miro_write.device_md + miro_write.device_type + miro_write.device_num)*miro_write.device_year*miro_write.device_num));
	miro_write.code_permanent = (short)(miro_write.code_permanent%100000);
	arm_config_data_write(CODE_PERMANENT,miro_write.code_permanent);
	#endif

	miro_write.UL_strtg_en = arm_config_data_read(ARM_UL_strtg_en);
	miro_write.PF_x100 = arm_config_data_read(ARM_PF_x100);
	miro_write.VQ_V1 = arm_config_data_read(ARM_VQ_V1);
	miro_write.VQ_V2 = arm_config_data_read(ARM_VQ_V2);
	miro_write.VQ_V3 = arm_config_data_read(ARM_VQ_V3);
	miro_write.VQ_V4 = arm_config_data_read(ARM_VQ_V4);
	miro_write.VQ_Q1 = arm_config_data_read(ARM_VQ_Q1);
	miro_write.VQ_Q2 = arm_config_data_read(ARM_VQ_Q2);
	miro_write.VQ_Q3 = arm_config_data_read(ARM_VQ_Q3);
	miro_write.VQ_Q4 = arm_config_data_read(ARM_VQ_Q4);
	miro_write.PQ_n_P3 = arm_config_data_read(ARM_PQ_n_P3);
	miro_write.PQ_n_P2 = arm_config_data_read(ARM_PQ_n_P2);
	miro_write.PQ_n_P1 = arm_config_data_read(ARM_PQ_n_P1);
	miro_write.PQ_p_P1 = arm_config_data_read(ARM_PQ_p_P1);
	miro_write.PQ_p_P2 = arm_config_data_read(ARM_PQ_p_P2);
	miro_write.PQ_p_P3 = arm_config_data_read(ARM_PQ_p_P3);
	miro_write.PQ_n_Q3 = arm_config_data_read(ARM_PQ_n_Q3);
	miro_write.PQ_n_Q2 = arm_config_data_read(ARM_PQ_n_Q2);
	miro_write.PQ_n_Q1 = arm_config_data_read(ARM_PQ_n_Q1);
	miro_write.PQ_p_Q1 = arm_config_data_read(ARM_PQ_p_Q1);
	miro_write.PQ_p_Q2 = arm_config_data_read(ARM_PQ_p_Q2);
	miro_write.PQ_p_Q3 = arm_config_data_read(ARM_PQ_p_Q3);
	miro_write.VP_V1 = arm_config_data_read(ARM_VP_V1);
	miro_write.VP_V2 = arm_config_data_read(ARM_VP_V2);
	miro_write.VP_P1 = arm_config_data_read(ARM_VP_P1);
	miro_write.VP_P2 = arm_config_data_read(ARM_VP_P2);
	miro_write.Uac_rate = arm_config_data_read(ARM_Uac_rate);
	miro_write.P_rate = arm_config_data_read(ARM_P_rate);
	miro_write.P_max = arm_config_data_read(ARM_P_max);
	miro_write.PF_min_x100 = arm_config_data_read(ARM_PF_min_x100);

	freqProt.fpi[0].thr = arm_config_data_read(AD_OF_Thr_1);
	freqProt.fpi[1].thr = arm_config_data_read(AD_OF_Thr_2);
	freqProt.fpi[2].thr = arm_config_data_read(AD_UF_Thr_1);
	freqProt.fpi[3].thr = arm_config_data_read(AD_UF_Thr_2);
	freqProt.fpi[0].time = arm_config_data_read(AD_OF_Time_1);
	freqProt.fpi[1].time = arm_config_data_read(AD_OF_Time_2);
	freqProt.fpi[2].time = arm_config_data_read(AD_UF_Time_1);
	freqProt.fpi[3].time = arm_config_data_read(AD_UF_Time_2);
	
	softBootup.sbEnble = arm_config_data_read(AD_SB_EN);
	softBootup.sbTime= arm_config_data_read(AD_SB_TIME);
	softBootup.sbGoalPwr = arm_config_data_read(AD_SB_GOPWR);
	faultRec.frEnble= arm_config_data_read(AD_FR_EN);
	faultRec.frWaitTime = arm_config_data_read(AD_FR_WTIME);
    offline.commEn = arm_config_data_read(AD_CO_EN);
    offline.commJdgTime = arm_config_data_read(AD_CO_JTIME);    
    faultRec.bootupTime = arm_config_data_read(AD_BOOTUP_TIME);
	

	/* 打印发送信号量 */
	tty_send_sem = osSemaphoreCreate(osSemaphore(tty_send_sem), 1);
}


int main(void)
{
    bsp_Init();
    nand_file_int();
    //flashdata_init();
    sdata_init(); 
    sdata_alloc_all();
    env_init();
    //network_init();
    W5xxxNet0_Init();
    W5xxxNet1_Init();
    log_init();
    para_init();
    fpga_init();
    record_wave_init();
    software_init();
	network_init();
    osDelay(100);
    Debug_Init();
    printf("\n");
    print_clear();
//	watch_dog_init();

    printf("pvm4 version " PVM4_VERSION ", "__DATE__ " - " __TIME__"\n\n");
	tid_watch_dog_task = osThreadCreate(osThread(watch_dog_task), NULL);	
	tid_nand = osThreadCreate(osThread(nand_task), NULL);
    tid_shell_com_task = osThreadCreate(osThread(shell_com_task), NULL);
    tid_telnet_task = osThreadCreate(osThread(telnet_task), NULL);
	
    tid_fpga_data_refresh_task = osThreadCreate(osThread(fpga_data_refresh_task), NULL);
    tid_fpga_cal_task = osThreadCreate(osThread(fpga_cal_task), NULL);
    tid_pcs_handle_task = osThreadCreate(osThread(pcs_handle_task), NULL);
	tid_rs485_2 = osThreadCreate(osThread(RS485_2_Thread),NULL); //HMI
	tid_can_task = osThreadCreate(osThread(CAN_Thread), NULL); //EMU
	tid_modbus_master = osThreadCreate(osThread(modbus_master_task), NULL);	//ISO&FAN
	
    tid_MBsever_task = osThreadCreate(osThread(MBsever_task), NULL); //Upper
    tid_mbresp_spec_task = osThreadCreate(osThread(mbresp_spec_task), NULL); //Upper	
    
	//tid_net_ems_task = osThreadCreate(osThread(net_ems_task), NULL);
	//tid_modbus_screen = osThreadCreate(osThread(modbus_screen_task), NULL);
	//tid_overload_task = osThreadCreate(osThread(OverLoadShutdown_task), NULL);
	//tid_freq_task = osThreadCreate(osThread(Frequency_task), NULL);
	//tid_bms_task = osThreadCreate(osThread(bms_ems_task), NULL);
	#if 1 /* w5500的实例用法 */
	//tid_w5500_net0server0 = osThreadCreate(osThread(W5xxxEth_Net0_Server0), NULL);
	//tid_w5500_net0client0 = osThreadCreate(osThread(W5xxxEth_Net0_Client0), NULL);
	//tid_w5500_net1server0 = osThreadCreate(osThread(W5xxxEth_Net1_Server0), NULL);
	//tid_w5500_net1client0 = osThreadCreate(osThread(W5xxxEth_Net1_Client0), NULL);
	#endif 
    #if 1 //rs485
    //tid_rs485_1 = osThreadCreate(osThread(RS485_1_Thread),NULL);
    #endif
	#if 0
	osThreadCreate(osThread(ChrgDiscSwitch_task), NULL);
	SVGModeEn_task_id = osThreadCreate(osThread(SVGModeEn_task), NULL);
	#endif

    tid_freq_prot_task = osThreadCreate(osThread(freq_prot_task), (void *)&freqProt);
	tid_soft_bootup_task = osThreadCreate(osThread(soft_bootup_task), (void *)&softBootup);
	tid_fault_rec_task = osThreadCreate(osThread(fault_rec_task), (void *)&faultRec);
    tid_comm_offline_task = osThreadCreate(osThread(comm_offline_task), (void *)&offline);
    tid_ul_mode_task = osThreadCreate(osThread(ul_mode_task), (void *)&umo);
    
	while(1)
	{
        osThreadYield();
	}
}

void watch_dog_task(const void *pdata)
{
    uint32_t t = 0;	
		while(1)
    {
		watch_dog_feed();
        if(t++ > 20)
        {
            t = 0;
            LED_Flicker(0);
        }
        osDelay(50);
    }
}

void firmware_check_delthreah(void)
{
    osThreadTerminate(tid_modbus_master);
	osThreadTerminate(tid_fpga_data_refresh_task);
    osThreadTerminate(tid_fpga_cal_task);
    osThreadTerminate(tid_pcs_handle_task);
	osThreadTerminate(tid_rs485_2);
	osThreadTerminate(tid_can_task);
	osThreadTerminate(tid_modbus_master);
}

void firmware_check_createthreah(void)
{
    tid_fpga_data_refresh_task = osThreadCreate(osThread(fpga_data_refresh_task), NULL);
    tid_fpga_cal_task = osThreadCreate(osThread(fpga_cal_task), NULL);
    tid_pcs_handle_task = osThreadCreate(osThread(pcs_handle_task), NULL);
	tid_rs485_2 = osThreadCreate(osThread(RS485_2_Thread),NULL); //HMI
	tid_can_task = osThreadCreate(osThread(CAN_Thread), NULL); //EMU
	tid_modbus_master = osThreadCreate(osThread(modbus_master_task), NULL);	//ISO&FAN
};
