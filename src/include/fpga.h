#ifndef FPGA_H
#define FPGA_H
#include <cmsis_os.h>
#include "micro.h"
//#include "feedback_control.h"

#define DEV_TYPE				(1)		//1-PCS 2-PVICS 3-PV
#define PCS_POWER_LEVEL			(1600)	//���ʵȼ�kW
#define PCS_VOLTAGE_LEVEL		(630)	//��ѹ�Ǽ�V
#define MODULUS					(10)
#define SOFT_VER_V				(3)
#define SOFT_VER_C				(1)
#define SOFT_VER_B				(30)   
#define SOFT_VER				((SOFT_VER_V<<12) + (SOFT_VER_C<<8) + SOFT_VER_B)
#define DEVICE_NO				((DEV_TYPE<<12) + (PCS_POWER_LEVEL))
#define SOFT_VER1				((SOFT_VER_V*1000) + (SOFT_VER_C*100) + SOFT_VER_B)

#if 0
/* ״̬�� */
enum {
	FSM_IDLE		= 0,
	FSM_ISLCHECK	= 1,
	FSM_CHARGE		= 2,
	FSM_BTRIN		= 3, //��ͨ���״̬	
	FSM_EXCITE		= 4,
	FSM_PRE_WORK	= 5,
	FSM_WORKING		= 6,
	FSM_FAULT		= 7,
	FSM_STOP        = 8,
	FSM_MAX
};
#else
/* ״̬�� */
enum {
	FSM_IDLE		= 0,
	FSM_ISLCHECK	= 1,
	FSM_CHARGE		= 2,
	FSM_PRE_WORK	= 3,
	FSM_WORKING		= 4,
	FSM_FAULT		= 5,
	FSM_STOP        = 6,
	FSM_MAX
};
#endif

/* ����ģʽ */
enum {
	VF_MODE = 1, //V/Fģʽ
	PQ_MODE	= 2, //P/Qģʽ
	IV_MODE = 4, //I/Vģʽ
	CP_MODE	= 8, //��Pģʽ
	CI_MODE = 16, //��Iģʽ
	CU_MODE	= 32, //��Uģʽ
	VSG_MODE = 64,//VSGģʽ
	CR_MODE = 128,//�����ģʽ	
	RUN_MODE_MAX
};

/* ����ģʽ */
enum {
	EMS_CTRL = 1, //EMS���ڿ���
	BMS_CTRL = 2, //BMS���ڿ���
	HMI_CTRL = 4, //HMI���ڿ���
	CTRL_MODE_MAX
};


enum {
	STATE0_ADD = 275, //ϵͳ״̬0
	STATE1_ADD, //ϵͳ״̬1
	STATE2_ADD, //ϵͳ״̬2
	STATE3_ADD, //ϵͳ״̬3
	STATE4_ADD, //ϵͳ״̬4
	STATE5_ADD, //ϵͳ״̬5
	STATE6_ADD, //ϵͳ״̬6
	STATE7_ADD, //ϵͳ״̬7
	STATE8_ADD, //ϵͳ״̬8
	STATE9_ADD, //ϵͳ״̬9
	STATE10_ADD, //ϵͳ״̬10
	STATE11_ADD, //ϵͳ״̬11
	STATE12_ADD, //ϵͳ״̬12
	STATE13_ADD, //ϵͳ״̬13
	STATE14_ADD, //ϵͳ״̬14
	STATE15_ADD, //ϵͳ״̬15
	ERR_LCK_STATE0_ADD = 291, //ϵͳ״̬��������0
	ERR_LCK_STATE1_ADD, //ϵͳ״̬��������1
	ERR_LCK_STATE2_ADD, //ϵͳ״̬��������2
	ERR_LCK_STATE3_ADD, //ϵͳ״̬��������3
	ERR_LCK_STATE4_ADD, //ϵͳ״̬��������4
	ERR_LCK_STATE5_ADD, //ϵͳ״̬��������5
	ERR_LCK_STATE6_ADD, //ϵͳ״̬��������6
	ERR_LCK_STATE7_ADD, //ϵͳ״̬��������7
	ERR_LCK_STATE8_ADD, //ϵͳ״̬��������8
	ERR_LCK_STATE9_ADD, //ϵͳ״̬��������9
	ERR_LCK_STATE10_ADD, //ϵͳ״̬��������10
	ERR_LCK_STATE11_ADD, //ϵͳ״̬��������11
	ERR_LCK_STATE12_ADD, //ϵͳ״̬��������12
	ERR_LCK_STATE13_ADD, //ϵͳ״̬��������13
	ERR_LCK_STATE14_ADD, //ϵͳ״̬��������14
	ERR_LCK_STATE15_ADD //ϵͳ״̬��������15
};


/* �����ͱ�� - ����count_cal[]��ַ*/
enum {	
	POWER_CON_ADD = 0, //AC�ۻ�������
	POWER_AC_CON_ADD, //AC�ۼƷ�����
	POWER_AC_DAY_ADD, //AC���շ�����
	POWER_AC_Y_ADD, //AC���շ�����
	POWER_AC_MONTH_ADD, //AC�·�����
	POWER_AC_YEAR_ADD, //AC�귢����
	POWER_DC2_Y_ADD, //DC2֧·�����������
	POWER_DC2_MON_ADD, //DC2֧·�·�����
	POWER_AC_C_DAY_ADD, //AC֧·����������
	POWER_AC_C_CON_ADD, //AC֧·����������
	POWER_DC1_DAY_ADD, //DC1֧·���������
	POWER_DC1_CON_ADD, //DC1֧·���������
	POWER_DC1_C_DAY_ADD, //DC1֧·���������
	POWER_DC1_C_CON_ADD, //DC1֧·���������
	POWER_DC2_DAY_ADD, //DC2֧·���������
	POWER_DC2_CON_ADD, //DC2֧·���������
	POWER_DC2_C_DAY_ADD, //DC2֧·���������(Ԥ��)
	POWER_DC2_C_CON_ADD, //DC2֧·���������(Ԥ��)
	COUNT_SIZE, //������ķ�������Ϣ����ֵ
	
	RUN_TIME,   	//������ʱ��	
	//RUN_TIME_NOW,	//��������ʱ��
	ON_TIME,		//��������ʱ��
	//ON_TIME_NOW,	//������������ʱ��
	RESTART_COUNT,	//����λ����ͳ��

	
	CO2_DEC_ADD, //CO2������
	C_DEC_ADD, //C������
	
	C_life, //��������Ԥ��ֵ
	POWER_AC_C_Y_ADD, //AC���ճ����
	POWER_AC_C_MONTH_ADD, //AC�³����
	POWER_AC_C_YEAR_ADD, //AC������
	
	FREADJUST_COUNT, //һ�ε�Ƶ����ͳ��
	FREADJUST_COUNT_TODAY, //һ�ε�Ƶ��������ͳ��

	FRELOCK_COUNT, //һ�ε�ƵAGC��������ͳ��
	FRELOCK_COUNT_TODAY, //һ�ε�ƵAGC������������ͳ��

	CRC_ERR_COUNT,
	CODE_DAY_COUNT,
	
	COUNT_CAL_MAX
};


/* ARM�������� - ����arm_config_data[]��ַ */
enum {	

	//���ϵ��
	FD_UINX_ADD = 0, //���������ѹ���ϵ��
	FD_UINX_ADDH,
	FD_IINX_ADD, //��������������ϵ��
	FD_IINX_ADDH,
	FD_UDC_ADD, //ֱ����ѹ���ϵ��
	FD_UDC_ADDH,
	FD_IDC_ADD, //ֱ���������ϵ��
	FD_IDC_ADDH,
	FD_UBTR_ADD, //��ص�ѹ���ϵ��
	FD_UBTR_ADDH,
	FD_UPOS_GND_ADD, //�����Եص�ѹ���ϵ��
	FD_UPOS_GND_ADDH,
	FD_UNEG_GND_ADD, //�����Եص�ѹ���ϵ��
	FD_UNEG_GND_ADDH,
	FD_PAC_ADD, 
	FD_PAC_ADDH,
	FD_PDC_ADD, 
	FD_PDC_ADDH,
	FD_IAC_CT_ADD, 
	FD_IAC_CT_ADDH,
	FD_PAC_CT_ADD, 
	FD_PAC_CT_ADDH,
	FD_IAC_LKG_ADD,	
	FD_IAC_LKG_ADDH,
	
	//���ܱ�����趨(�������)
	ammeter1_A0, //���1�ĵ�ַA0
	ammeter1_A1, //���1�ĵ�ַA1
	ammeter1_A2, //���1�ĵ�ַA2
	ammeter1_A3, //���1�ĵ�ַA3
	ammeter1_A4, //���1�ĵ�ַA4
	ammeter1_A5, //���1�ĵ�ַA5
	ammeter2_A0, //���2�ĵ�ַA0
	ammeter2_A1, //���2�ĵ�ַA1
	ammeter2_A2, //���2�ĵ�ַA2
	ammeter2_A3, //���2�ĵ�ַA3
	ammeter2_A4, //���2�ĵ�ַA4
	ammeter2_A5, //���2�ĵ�ַA5
	ammeter1_vrc, //���1���ϵ��
	ammeter2_vrc, //���2���ϵ��
	dis_min_power,//������С���繦������ֵ
	cos_seta,	//������������	
	
	Prio_Set_ADD, //����ģʽ����
	Run_Mode_ADD, 
	COMMUICATION_PRO_EN, //ͨѶ���� 
	STRATEGY_EN,	//����ʹ��

	INI_RESIS_FD_ADD , //��Ե���ϵ��
	INI_RESIS_REF_ADD, //��Ե�ж�����
	INI_MONITOR_ADDR_ADD, //��ػ���ַ
	MAX_P_ADD, //������й�����ֵ
	MIN_P_ADD, //��С�����й�����ֵ	
	MAX_BA_C_ADD, //���ŵ����ֵ
	MIN_BA_C_ADD, //��С������
	MAX_BA_U_ADD, //���ֱ����ѹ
	MIN_BA_U_ADD, //��Сֱ����ѹ
	MAX_Q_ADD, //������޹�����ֵ
	MIN_Q_ADD, //��С�����޹�����ֵ
	ASK_LOCAL, //������������  0������ 1������
		
	UART_CONFIG_BAUD, 
	UART_CONFIG_STOP, 
	UART_CONFIG_PARITY, 
	UART_CONFIG_DATABIT, 
			
	SCREEN_PROTECT_TIME, 
	SCREEN_PROTECT_ONOFF, 
	SCREEN_PROTECT_ETH, 
	SCREEN_PROTECT_RS485, 
	SCREEN_PROTECT_CAN, 

	CHRG_VOL_LIMIT_ADD, 
	DISC_VOL_LIMIT_ADD, 
	END_SWITCH_EN_ADD,
	SVG_EN_ADD, 
	ISO_ISLAND_ADD, 

	CP_MODE_FLAG_ADD,
	CI_MODE_FLAG_ADD,
	
	POWER_STEP,				//���ʸ����仯��
	HIGHT_TEMP_DERAT, 		//�����¶��趨ֵ
	MAX_IAC_ADD, 			//���Ž�������ֵ


	DEVICE_CODE_YEAR,
	DEVICE_CODE_MD,
	DEVICE_CODE_TYPE,
	DEVICE_CODE_NUM,
	DEVICE_WRITE_CODE,

	CODE_RELEASE,
	CODE_DEBUG,
	CODE_TEMPORARY1,
	CODE_TEMPORARY2,
	CODE_TEMPORARY3,
	CODE_PERMANENT,
	CODE_REMAIN_DATE, //��Կʣ��ʱ��
	CODE_TPYE, //��Կ����
	
	CODE_RELEASE_FLAG,
	CODE_DEBUG_FLAG,
	CODE_TEMPORARY1_FLAG,
	CODE_TEMPORARY2_FLAG,
	CODE_TEMPORARY3_FLAG,
	CODE_PERMANENT_FLAG,
	
	AGC_EN = 180,	// ��Ƶ����	
	OVER_LOAD_L1, 	//һ�����ر���ֵ
	OVER_LOAD_L1_TIME,	//һ�ֵ�����ʱ���趨
	OVER_LOAD_L2, 	//�������ر���ֵ
	OVER_LOAD_L2_TIME,	//��������ʱ���趨
	OVER_LOAD_L3, 	//�������ر���ֵ(˲ʱ����)
	
	//FRE_SET,	//Ƶ���趨ֵ
	//VAL_SET,	//��ѹ�趨ֵ
	//VAL_TIME,	//��ѹ����ʱ��
	
	FRE_OVER,//Ƶ����Ӧ����
	FRE_UNDER,//Ƶ����Ӧ����

	//������¶ȱ����趨��
	AC_FUN_START_TEMP_REC		= 190,//�������������¶�
	AC_FUN_STOP_TEMP_REC		= 191,//��������ֹͣ�����¶�

	FILTER_CAPAC_ERR_TEMP_REC   = 192,//�˲����ݹ��µ�
	TRANSFORMER_ERR_TEMP_REC    = 193,//���Ʊ�ѹ�����µ�
	BUS_CAPAC_ERR_TEMP_REC		= 194,//ĸ�ߵ��ݹ��µ�
	ELEC_REACTOR_ERR_TEMP_REC	= 195,//�翹�����µ�
	AC_CABINET_ERR_TEMP_REC		= 196,//��������µ�	
	
	XFTG_EN = 200, //������Ȳ���
	START_TIME_0, 
	STOP_TIME_0, 
	SET_POWER_0, 
	START_TIME_1, 
	STOP_TIME_1, 
	SET_POWER_1, 
	START_TIME_2, 
	STOP_TIME_2, 
	SET_POWER_2, 
	START_TIME_3, 
	STOP_TIME_3, 
	SET_POWER_3, 
	START_TIME_4, 
	STOP_TIME_4, 
	SET_POWER_4, 
	START_TIME_5, 
	STOP_TIME_5, 
	SET_POWER_5, 
	START_TIME_6, 
	STOP_TIME_6, 
	SET_POWER_6, 
	START_TIME_7, 
	STOP_TIME_7, 
	SET_POWER_7, 
	CAPACITY, 
	CHARGE_POWER_LMT,
	DISCHARGE_POWER_LMT,	

	ARM_UL_strtg_en,
	ARM_PF_x100,
	ARM_VQ_V1,
	ARM_VQ_V2,
	ARM_VQ_V3,
	ARM_VQ_V4,
	ARM_VQ_Q1,
	ARM_VQ_Q2,
	ARM_VQ_Q3,
	ARM_VQ_Q4,
	ARM_PQ_n_P3,
	ARM_PQ_n_P2,
	ARM_PQ_n_P1,
	ARM_PQ_p_P1,
	ARM_PQ_p_P2,
	ARM_PQ_p_P3,
	ARM_PQ_n_Q3,
	ARM_PQ_n_Q2,
	ARM_PQ_n_Q1,
	ARM_PQ_p_Q1,
	ARM_PQ_p_Q2,
	ARM_PQ_p_Q3,
	ARM_VP_V1,
	ARM_VP_V2,
	ARM_VP_P1,
	ARM_VP_P2,
	ARM_Uac_rate,
	ARM_P_rate,
	ARM_P_max,
	ARM_PF_min_x100,
	
	AD_OF_Thr_1, //һ����Ƶ����
	AD_OF_Thr_2, //������Ƶ����
	AD_UF_Thr_1, //һ��ǷƵ����
	AD_UF_Thr_2, //����ǷƵ����
	AD_OF_Time_1, //һ����Ƶʱ��
	AD_OF_Time_2, //������Ƶʱ��
	AD_UF_Time_1, //һ��ǷƵʱ��
	AD_UF_Time_2, //����ǷƵʱ��

	AD_SB_EN, //����ʹ��
	AD_SB_TIME, //����ʱ��
	AD_SB_GOPWR, //����Ŀ�깦��
	AD_FR_EN, //���ϻָ�ʹ��
	AD_FR_WTIME, //���ϻָ��ȴ�ʱ��
	AD_CO_EN, //ͨѶ����ͣ��ʹ��
	AD_CO_JTIME, //ͨѶ�����ж�ʱ��
	AD_BOOTUP_TIME, //����ʱ��
	
	ARM_CFG_MAX,
};


#define ZERO_NUM	(14)

#define CTRL_UPPER	0
#define CTRL_SCREEN	1

#if 0

#define FPGA_WRITE_TYPE_THREE
#if defined FPGA_WRITE_TYPE_ONE
#define fpga_write(a,b)     fpga[a]=(uint16_t)(b);osDelay(1)
#elif defined FPGA_WRITE_TYPE_TWO
#define fpga_write(a,b)     {fpga[a]=(uint16_t)(b);udelay(10);}
#elif defined FPGA_WRITE_TYPE_THREE
#define fpga_write(a,b)     f_fpga_write((a), (uint16_t)(b))
#endif

#define FPGA_READ_TYPE_TWO
#if defined FPGA_READ_TYPE_ONE
#define fpga_read(a)     fpga[(a)]
#elif defined FPGA_READ_TYPE_TWO
#define fpga_read(a)     f_fpga_read((a))
#endif

#else
#include "fpgaRamData.h"
#define fpga_write FpgaRamData_Write
#define fpga_read  FpgaRamData_Read
#endif



#define GETBIT(x,i) ((x>>i)&0x01)
#define SETBIT(x,i)	(x=((x)|(0x1u<<(i))))
#define RESETBIT(x,i)((x)&(~(0x1u<<(i))))
#define GETMIN(a,b)	((a > b)?(b):(a))
#define GETMAX(a,b)	((a > b)?(a):(b))

#define	PW_BCD_TIME(year,mon,date,hour)		(((BCD((int)year))<<24)|((BCD((int)mon))<<16)|((BCD((int)date))<<8)|(BCD((int)hour)) )
#define	PW_BCD_TIME_GET_Y(bcd_time)			BCDGET(0xff&((bcd_time)>>24))
#define	PW_BCD_TIME_GET_M(bcd_time)			BCDGET(0xff&((bcd_time)>>16))
#define	PW_BCD_TIME_GET_D(bcd_time)			BCDGET(0xff&((bcd_time)>>8))
#define	PW_BCD_TIME_GET_H(bcd_time)			BCDGET(0xff&(bcd_time))


typedef struct {
    int16_t temp;
    uint16_t ad;
} temp_ad_t;


typedef struct 
{
	osTimerId timer_id;
	unsigned char timer_flag; //��ʱ��������־
	unsigned char recover_cnt; //���ϻָ���������������5�ξͲ��ٻָ�
} fault_recover_info_t;

typedef struct _fpga_data_buffer
{
	short set[256];
//	short get[256];
} fpga_data_buffer;


typedef struct _fpga_zero
{
	unsigned short zero_check;
	short zero[ZERO_NUM];
} fpga_zero;


typedef struct
{
	short life_time;
	short dead_time;
} dead_time;


typedef struct {
	unsigned char err_flag;
	unsigned char err_flag_old;
	unsigned char err_flag1;
	unsigned char err_flag_old1;
    short err_state_last[7];
    short err_state[7];
	unsigned char fault_err[112];
} fault_record_info_t;


typedef struct {
	short state9_last;
    short state10_last;
    short state11_last;
} switch_record_info_t;

typedef struct {
	short cmd0_last;
    short cmd1_last;
    short cmd2_last;
} cmd_record_info_t;

typedef struct {
	short strategy_last;
    short communication_last;
} strategy_record_info_t;

typedef struct {
	unsigned char flag_open;
    unsigned char flag_close;
    unsigned char flag_open_new;
    unsigned char flag_close_new;
} opsstate_record_info_t;

typedef struct
{
	uint16_t cntComm; //ͨѶ����
	uint16_t isoComm; //��Ե����ͨѶ״̬: 0-��ͨѶ 1-����
	int16_t temp[8]; //�¶�      		x10
	uint16_t volGridAB; //������ѹAB    x10
	uint16_t volGridBC; //������ѹBC    x10
	uint16_t volGridCA; //������ѹCA    x10
	uint16_t isoResPos; //��Ե��ֵ+(k)  x10 ��50k����Ϊ�쳣
	uint16_t isoResNeg; //��Ե��ֵ-(k)  x10 ��50k����Ϊ�쳣
	int16_t curGridA; //��������A       x10
	int16_t curGridB; //��������B       x10
	int16_t curGridC; //��������C       x10
	int16_t curLeak; //©����           x10
	uint16_t isoVolPos; //��Ե��ѹ+     x10
	uint16_t isoVolNeg; //��Ե��ѹ-     x10
} iso_info_t;


extern volatile float *count_cal;
extern volatile short *arm_config_flash_data;
extern volatile short *arm_config_nand_data;
extern fpga_data_buffer *fpga_pdatabuf;
extern short fpga_pdatabuf_get[512];
extern fpga_zero *fpga_pzero;
extern iso_info_t isoInfo;


void fpga_init(void);
void f_fpga_write(uint32_t addr,uint16_t v);
uint16_t f_fpga_read(uint32_t addr);
int zero_adjust(unsigned short cmd);
void fpga_data_refresh_task(const void *pdata);
void mb_data_update(void);
void pcs_handle_task(const void * pdata);
int EDGEPOS(unsigned char pv,unsigned char* buffer);
void  Pcs_Ctrl_Mode_Set(short ctrl);
void  Pcs_Run_Mode_Set(short mode);
short Pcs_Run_Mode_Get(void);
short Pcs_Ctrl_Mode_Get(void);
int EDGENPO(unsigned char pv,unsigned char* buffer) ;

int EDGENPO1(unsigned char pv, unsigned char pv_old);

int edgepos_bit(short state,short* buffer,short j);
int edgeng_bit(short state,short* buffer,short j);
void Lifetask(void *pdata);
void fpga_cal_task(const void *pdata);
void Determine_scope(short min,short max,volatile short* value);
void set_pcs_shutdown(void);
void set_pcs_bootup(void);
void  set_count_Insulation_en(unsigned char enable);
void  Request_Control(short addr,short value);
void ems_catl_process_task(void *pdata);


#define TEST_MODE 0 //0-���а汾 !0-���԰汾

#endif
