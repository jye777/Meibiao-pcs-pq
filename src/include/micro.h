
#ifndef _MICRO_H_
#define _MICRO_H_
#define Ubattery_MAX   750		//ֱ����ѹ���ֵ
#define Ubattery_MIN   550      //ֱ����ѹ��Сֵ
#define Ibattery_MAX   1010		//ֱ���������ֵ
#define Ibattery_MIN   -1010    //ֱ��������Сֵ
#define EMS_JUWEI_EN   0
#define EMS_CATL_EN    0
#define JUWEI_QIANSUAN 0
#define JUWEI_LIDIAN   0
#define GUANGFU_EN     1
typedef struct _micro_set_buffer
{
unsigned short Connect_State;
unsigned short Prio_Set;		//0:EMS 1:BMS 2:PCS_Local
unsigned short Prio_Set_Bef;
unsigned short Run_Mode;		//0:PQ	1:VF	 2:IV
unsigned short Run_Mode_bef;
unsigned short Charge_DisC;
short Power_Set_AC;
short Active_Set_AC;

short Power_Set;
short Active_Set;
unsigned short Ubattery_SetC;
unsigned short Ibattery_SetC;
unsigned short Ubattery_SetD;
unsigned short Ibattery_SetD;
unsigned short Vol_Base_Set;
unsigned short Freq_Base_Set;
unsigned short DCS_Sel;
unsigned char Connected;
unsigned short Gps_Y;
unsigned short Gps_M;
unsigned short Gps_D;
unsigned short Gps_W;
unsigned short Gps_Mode;
unsigned short Gps_H;
unsigned short Gps_Min;
unsigned short Gps_S;
unsigned short Battery_c;
unsigned char Soc_mode_flag;     //SOC�궨״̬
unsigned char Vol_banlance_flag;//��ѹά��״̬
unsigned char Battery_maintain; //���ά��ģʽ
unsigned short soc_step;
unsigned short ba_step;
unsigned short vol_step;
short tem_vol;
unsigned char soc_err_flag;
unsigned char vol_err_flag;
unsigned char ba_err_flag;

short bootup_flag;
short pwm_off_tick;
unsigned char auto_recover_flag;

unsigned short communication_en;
unsigned short strategy_en;
unsigned char CU_mode_first_set;
unsigned char SOC_mode_first_set;
unsigned char PRE_mode_first_set;

short soc_charge_tick;
unsigned char soc_charge_ok_flag;
unsigned char soc_mode_end_flag;

short cu_mode_tick;
unsigned char enter_cu_mode_flag;
unsigned char pre_mode_end_flag;

short over_load1_value;
short over_load1_time;
short over_load2_value;
short over_load2_time;
short over_load3_value;

unsigned char recover_flag;
unsigned char pcs_derat_flag;

short power_zero_flag;
short pwm_off_flag;
short pwm_shutdown_flag;

short life_count;
short life_count_old;
short life_count_now;

unsigned char bms_err_flag;
unsigned char pcs_htemp_flag;


short Freq_Adjust_Able;
short Freq_Dead_Zone;
short Freq_Dead_Zone_write;


short Power_Lim_Pro;
short Diversity_Factor;
short Diversity_Factor_set;

short Run_Time_Set;
short Run_Time_set1;

short modbus_addr;
short pf;
short pfm_able;
short dead_zone;

short diversity_factor;
short Diversity_Factor1;

short pfm_lock;
short lock_zone;
short AGC_Lock_Zone;
	
short fre_slop;
short Fre_Slop1;
short inertia_able;
short inertia_zone;
short Inertia_Zone1;
unsigned char pcs_htemp_flag_old;
unsigned short AGC_State;
unsigned short AGC_Chang_flag;
unsigned short AGC_Chang_tick;

unsigned char power_Change_flag;

unsigned char pfm_flag;  //һ�ε�Ƶ��������־λ
unsigned char pfm_power_flag;  //һ�ε�ƵAGCָ��������±�־
short Power_Set_AC_get_old;

unsigned char pfm_recod_flag;
unsigned char pfm_recod_read_flag;

float charge_power_lim_set;

short para_vision;

//��Կ���
short device_year;
short device_md;
short device_type;
short device_num;

short code_release; //������Կ
short code_debug;	//������Կ
short code_temporary1; //��ʱһ��
short code_temporary2; //��ʱһ����
short code_temporary3; //��ʱ������
short code_permanent;  //������Կ
short code_input;  //������Կ

short code_type; //��Կ����
short code_remain_date; //��Կʣ��ʱ��
int   code_remain_tick; //��Կʣ��ʱ��
unsigned short   code_err_tick; //��Կʣ��ʱ��

short lecense_key;//������ȷ��ɺ�����޸ĳ�����
short speed_value;

	short slave_id; //HMI�ӻ���ַ
	
	short UL_strtg_en;
	short PF_x100;
	short VQ_V1;
	short VQ_V2;
	short VQ_V3;
	short VQ_V4;
	short VQ_Q1;
	short VQ_Q2;
	short VQ_Q3;
	short VQ_Q4;
	short PQ_n_P3;
	short PQ_n_P2;
	short PQ_n_P1;
	short PQ_p_P1;
	short PQ_p_P2;
	short PQ_p_P3;
	short PQ_n_Q3;
	short PQ_n_Q2;
	short PQ_n_Q1;
	short PQ_p_Q1;
	short PQ_p_Q2;
	short PQ_p_Q3;
	short VP_V1;
	short VP_V2;
	short VP_P1;
	short VP_P2;
	short Uac_rate;
	short P_rate;
	short P_max;
	short PF_min_x100;

    short manShutdownFlag; //�ֶ��ػ���־
}micro_set;

extern volatile micro_set miro_write;
extern void mirco_task(void * pdata);
extern int find_V_max(void);
extern int find_V_min(void);
extern int BMS_v_Min(void);
//extern struct cfc_send_data cfc_start_data[CFC_NUM]; 
extern int do_micro_reg_w(void);
extern int do_micro_inf(void);
void miro_init(void);


#endif
