#ifndef MODBUS_COMM_H_
#define MODBUS_COMM_H_

#ifdef MODBUS_COMM_C
#define MB_EXTERN
#else
#define MB_EXTERN     extern
#endif

#define MB_DATA_MAXLEN      250	
#define MB_RECVBUF_SIZE     256		/* ���ջ�������С */
#define MB_SENDBUF_SIZE     256		/* ���ͻ�������С */

#define MB_TIMEOUT_MAX     	100
#define MB_INV_MAX         	1
#define MB_RECV_TIMEOUT		80		/* ���ڽ��ճ�ʱ�� */

#define MB_RECV_CHECKADDR	1		/* �Ƿ����ַ */
//#define MB_MONITOR_ADDR		0x02	/* ��������ַ */	//��ini_data_read(MP_INI_MONITOR_ADD) ����


/* MODBUS���ݰ������Ϣ */
typedef struct _mb_information
{
    unsigned char plen;
	unsigned int crc_err;
}modbus_information;

//huangwh ������������(ϵͳ����������)
typedef struct _modbus_data
{
    short ubtra_ever;
    short ubtrb_ever;
    short ibtra_ever;
    short ibtrb_ever;
    short udcbtr_ever;
    unsigned short igbt_tempa0_btr_ever;
    unsigned short igbt_tempa1_btr_ever;
    unsigned short igbt_tempb0_btr_ever;
    unsigned short igbt_tempb1_btr_ever;
    unsigned short uina_rms;
    unsigned short uinb_rms;
    unsigned short uinc_rms;
    unsigned short iina_rms;
    unsigned short iinb_rms;
    unsigned short iinc_rms;
    short udc_ever;
	short Uina_Ever;
	short Uinb_Ever;
	short Uinc_Ever;
	short Iina_Ever;
	short Iinb_Ever;
	short Iinc_Ever;
    unsigned short igbt_tempa0_ever;
    unsigned short igbt_tempa1_ever;
    unsigned short igbt_tempb0_ever;
    unsigned short igbt_tempb1_ever;
    unsigned short igbt_tempc0_ever;
    unsigned short igbt_tempc1_ever;
	unsigned short igbt_tempdc1_ever;
    unsigned short igbt_tempdc2_ever;
	
	unsigned short Iina_CT_Ever;
	unsigned short Iinb_CT_Ever;
	unsigned short Iinc_CT_Ever;
	
	unsigned short Iina_CT_RMS;
	unsigned short Iinb_CT_RMS;
	unsigned short Iinc_CT_RMS;
    unsigned short state0;
    unsigned short state1;
    unsigned short state2;
    unsigned short state3;
    unsigned short state4;
    unsigned short state5;
    unsigned short state6;
    unsigned short state7;
    unsigned short state8;
    unsigned short state9;
    unsigned short state10;
    unsigned short state11;
    unsigned short state12;
    unsigned short state13;
    unsigned short state14;
    unsigned short state15;
    unsigned short err_lck_state0;
    unsigned short err_lck_state1;
    unsigned short err_lck_state2;
    unsigned short err_lck_state3;
    unsigned short err_lck_state4;
    unsigned short err_lck_state5;
    unsigned short err_lck_state6;
    unsigned short err_lck_state7;
    unsigned short err_lck_state8;
    unsigned short err_lck_state9;
    unsigned short err_lck_state10;
    unsigned short err_lck_state11;
    unsigned short err_lck_state12;
    unsigned short err_lck_state13;
    unsigned short err_lck_state14;
    unsigned short err_lck_state15;
    unsigned short ac_state0;
    unsigned short ac_state1;
    unsigned short ac_state2;
    unsigned short ac_state3;
    unsigned short ac_state4;
    unsigned short ac_state5;
    unsigned short ac_state6;
    unsigned short ac_state7;
    unsigned short err_lck_ac_state0;
    unsigned short err_lck_ac_state1;
    unsigned short err_lck_ac_state2;
    unsigned short err_lck_ac_state3;
    unsigned short err_lck_ac_state4;
    unsigned short err_lck_ac_state5;
    unsigned short err_lck_ac_state6;
    unsigned short err_lck_ac_state7;
    int power_local;
	unsigned short power_local_h;
	unsigned short power_local_l;
	unsigned char power_local_flag;
    int power_as;
	short power_as_h;
	short power_as_l;
	unsigned char power_as_flag;
    int co2_dec;
	short co2_dec_h;
	short co2_dec_l;
	unsigned char co2_dec_flag;
    int c_dec;
	short c_dec_h;
    short c_dec_l;
	unsigned char c_dec_flag;
    short power_rea;
    short power;
    short power_rea_set;
    short power_set;
	short pbtr_sola;
    unsigned short startstop_cmd_btr;
	unsigned short auto_start;
    unsigned short branchen_cmd;
	unsigned short Freq;
	short run_mode;
	unsigned short SettingParameter10;
	unsigned short SettingParameter11;
	unsigned short SettingParameter12;
	unsigned short SettingParameter13;
	unsigned short SettingParameter14;
	unsigned short SettingParameter15;
	unsigned short SettingParameter17;
	unsigned short SettingParameter18;
	unsigned short SettingParameter19;
	unsigned short SettingParameter20;
	unsigned short SettingParameter46;
	unsigned short MPP_Pbw;
	unsigned short settingcmd1;
	unsigned short vol_1_abov;
	unsigned short vol_1_below;
	unsigned short vol_2_abov;
	unsigned short vol_2_below;
	short p_ct;
	short q_ct;
	unsigned short SettingParameter50;
	unsigned short SettingParameter49;
	unsigned short IbtrLimit;
	short Udc_max;
	short Udc_min;
	short SettingParameter21;
	short p_factor;
	short q_ref;
	short p_limt;
	unsigned short Battery_c;
	unsigned short vol_set;
	unsigned short vol_setmin;
	unsigned int Ins_res;//��Ե��ֵ
	float pfactor; //��������
	float efficiency;//����Ч��
	/*******************����BMSЭ����������**************/
	/*Parameters*/
	unsigned int Charge_Volume; //�ɳ����ֵ
	unsigned short Charge_Volume_h;
	unsigned short Charge_Volume_l;
	unsigned int Discharge_Volume;//�ɷŵ���ֵ
	unsigned short Discharge_Volume_h;
	unsigned short Dischaarge_Volume_l;
	unsigned int Soc;
	unsigned short Soc_h;
	unsigned short Soc_l;
	unsigned int Soh;
	unsigned short Soh_h;
	unsigned short Soh_l;
	short Backfeed_Voltage; //�¶ȷ�����ѹֵ
	/*status*/
	unsigned short Battery_Full;  //��س���
	unsigned short Battery_Empty;//��طſ�
	unsigned short Tem_Alarm;    //�¶ȸ澯
	unsigned short Current_Alarm;//�����澯
	unsigned short Contactor_off;//�Ӵ����Ͽ�
	unsigned short err_state;//����ָ��
	unsigned short Voltage_Balance;//��ѹ״̬
	unsigned short Battery_Maintain;//���ά��ģʽ
	unsigned char ip[4];
	unsigned short cur_soc;	//��ص�ǰSOC,�Ŵ���10��
	unsigned short battery_num;
	unsigned short last_onoff_mark; //��һ�ο��ػ���¼: 1-arm�ػ�(����������) 2-arm���� 4-upper�ػ� 8-upper���� 16-hmi�ػ� 32-hmi���� 64-ems�ػ� 128-ems���� 256-bms�ػ� 512-bms����
	unsigned short micro_state;//΢����ǰ״̬: 0-���� 1-���� 2-��· 3-����
	unsigned short micro_sw_sta;//΢������״̬
	unsigned short u_grid_ever;//΢������ǰ��ѹƽ��ֵ
	unsigned short life; //����
}modbus_data;

typedef struct _modbus_cmd
{
    unsigned short power_rea_set;
	unsigned char power_rea_set_flag;
    unsigned short power_set;
	unsigned char power_set_flag;
    unsigned short startstop_cmd_btr;
    unsigned short branchen_cmd;

}modbus_cmd;

typedef struct _modbus_config
{
	short baud;
	short parity;
	short stop;
	short data_bit;
	short addr;
	short set_flag;
}modbus_config;

typedef struct
{
    unsigned int Year;
    unsigned int Month;
    unsigned int Date;
    unsigned int Hours;
    unsigned int Minutes;
    unsigned int Seconds;
}BuildDateTime;


#define C_LIFE_Set_l	(207)
#define C_LIFE_Set_h	(208)

#define PCS_CTRL_MODE	(300)
#define PCS_RUN_MODE	(301)
#define PCS_SYS_SATE	(302)
#define ARM_TIME_YEAR	(304)
#define ARM_TIME_MONTH	(305)
#define ARM_TIME_DATE	(306)
#define ARM_TIME_AP		(307)
#define ARM_TIME_WEEK	(308)
#define ARM_TIME_HOUR	(309)
#define ARM_TIME_MINUTE	(310)
#define ARM_TIME_SECOND	(311)
#define ARM_RESIS_VALUE (314)
#define ARM_IP0			(315)
#define ARM_IP1			(316)
#define ARM_IP2			(317)
#define ARM_IP3			(318)
#define FACTOR 			(319)
#define UART_BAUD		(320)
#define UART_PARITY		(321)
#define UART_STOP		(322)
#define UART_DATABIT	(323)
#define UART_ADDR		(324)
#define UART_SET		(325)
 
#define Charge_Start_Time 	(326) // ÿ�춨ʱ�����ʼʱ��	���ֽڣ�Сʱ�����ֽڣ���
#define Charge_Stop_Time	(327)// ÿ�춨ʱ���ֹͣʱ��	���ֽڣ�Сʱ�����ֽڣ���
#define Disch_Start_Time	(328)// ÿ�춨ʱ�����ʼʱ��	���ֽڣ�Сʱ�����ֽڣ���
#define Disch_Stop_Time		(329)// ÿ�춨ʱ���ֹͣʱ��	���ֽڣ�Сʱ�����ֽڣ���

/*����BMSЭ�鶨��Ĵ����б�*/
#define CHARGE_VOLUME_L  	(0x1200)	//�ɳ������λ
#define CHARGE_VOLUME_H  	(0x1201)	//�ɳ������λ
#define DISCHARGE_VOLUME_L  (0x1202)	//�ɷŵ�����λ
#define DISCHARGE_VOLUME_H  (0x1203)	//�ɷŵ�����λ
#define SOC_L               (0x1204)	//SOC��λ
#define SOC_H               (0x1205)	//SOC��λ
#define SOH_L               (0x1206)	//SOH��λ
#define SOH_H               (0x1207)	//SOH��λ
#define BACKFEED_VOLTAGE    (0x1208)	//�¶Ȳ�����ѹ
#define BATTERY_FULL        (0x14e0)	//��س���
#define BATTERY_EMPTY       (0x14e1)	//��طſ�
#define TEM_ALARM           (0x14e2)	//�¶ȸ澯
#define CURRENT_ALARM       (0x14e3)	//�����澯
#define CONTANCTOR_OFF      (0x14e4)	//�Ӵ����Ͽ�
#define ERR_STATE           (0x14e5)	//����
#define VOLTAGE_BANLANCE    (0x14e6)	//��ѹά��ģʽ
#define BATTERY_MAINTAIN    (0x14e7)	//���ά��ģʽ

#define MB_ERR_NONE				0	/* �޴��� */
#define MB_ERR_RECVFAILED		1	/* ���մ��� */
#define MB_ERR_FORMATERR		2	/* �������ݸ�ʽ����ȷ */

#define PF_K		10000.0

extern modbus_information mb_inf;
extern modbus_information mb_inf_u0;

extern modbus_data mb_data;

void modbus_service_task(const void *pdata);

MB_EXTERN int do_mbtest(void);

extern modbus_config mod_config;

#endif /*MODBUS_COMM_H_*/
