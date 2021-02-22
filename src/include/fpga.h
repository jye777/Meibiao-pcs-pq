#ifndef FPGA_H
#define FPGA_H
#include <cmsis_os.h>
#include "micro.h"
//#include "feedback_control.h"

#define DEV_TYPE				(1)		//1-PCS 2-PVICS 3-PV
#define PCS_POWER_LEVEL			(1600)	//功率等级kW
#define PCS_VOLTAGE_LEVEL		(630)	//电压登记V
#define MODULUS					(10)
#define SOFT_VER_V				(3)
#define SOFT_VER_C				(1)
#define SOFT_VER_B				(30)   
#define SOFT_VER				((SOFT_VER_V<<12) + (SOFT_VER_C<<8) + SOFT_VER_B)
#define DEVICE_NO				((DEV_TYPE<<12) + (PCS_POWER_LEVEL))
#define SOFT_VER1				((SOFT_VER_V*1000) + (SOFT_VER_C*100) + SOFT_VER_B)

#if 0
/* 状态机 */
enum {
	FSM_IDLE		= 0,
	FSM_ISLCHECK	= 1,
	FSM_CHARGE		= 2,
	FSM_BTRIN		= 3, //接通电池状态	
	FSM_EXCITE		= 4,
	FSM_PRE_WORK	= 5,
	FSM_WORKING		= 6,
	FSM_FAULT		= 7,
	FSM_STOP        = 8,
	FSM_MAX
};
#else
/* 状态机 */
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

/* 运行模式 */
enum {
	VF_MODE = 1, //V/F模式
	PQ_MODE	= 2, //P/Q模式
	IV_MODE = 4, //I/V模式
	CP_MODE	= 8, //恒P模式
	CI_MODE = 16, //恒I模式
	CU_MODE	= 32, //恒U模式
	VSG_MODE = 64,//VSG模式
	CR_MODE = 128,//恒电阻模式	
	RUN_MODE_MAX
};

/* 控制模式 */
enum {
	EMS_CTRL = 1, //EMS正在控制
	BMS_CTRL = 2, //BMS正在控制
	HMI_CTRL = 4, //HMI正在控制
	CTRL_MODE_MAX
};


enum {
	STATE0_ADD = 275, //系统状态0
	STATE1_ADD, //系统状态1
	STATE2_ADD, //系统状态2
	STATE3_ADD, //系统状态3
	STATE4_ADD, //系统状态4
	STATE5_ADD, //系统状态5
	STATE6_ADD, //系统状态6
	STATE7_ADD, //系统状态7
	STATE8_ADD, //系统状态8
	STATE9_ADD, //系统状态9
	STATE10_ADD, //系统状态10
	STATE11_ADD, //系统状态11
	STATE12_ADD, //系统状态12
	STATE13_ADD, //系统状态13
	STATE14_ADD, //系统状态14
	STATE15_ADD, //系统状态15
	ERR_LCK_STATE0_ADD = 291, //系统状态故障锁存0
	ERR_LCK_STATE1_ADD, //系统状态故障锁存1
	ERR_LCK_STATE2_ADD, //系统状态故障锁存2
	ERR_LCK_STATE3_ADD, //系统状态故障锁存3
	ERR_LCK_STATE4_ADD, //系统状态故障锁存4
	ERR_LCK_STATE5_ADD, //系统状态故障锁存5
	ERR_LCK_STATE6_ADD, //系统状态故障锁存6
	ERR_LCK_STATE7_ADD, //系统状态故障锁存7
	ERR_LCK_STATE8_ADD, //系统状态故障锁存8
	ERR_LCK_STATE9_ADD, //系统状态故障锁存9
	ERR_LCK_STATE10_ADD, //系统状态故障锁存10
	ERR_LCK_STATE11_ADD, //系统状态故障锁存11
	ERR_LCK_STATE12_ADD, //系统状态故障锁存12
	ERR_LCK_STATE13_ADD, //系统状态故障锁存13
	ERR_LCK_STATE14_ADD, //系统状态故障锁存14
	ERR_LCK_STATE15_ADD //系统状态故障锁存15
};


/* 电量和变比 - 用于count_cal[]地址*/
enum {	
	POWER_CON_ADD = 0, //AC累积发电量
	POWER_AC_CON_ADD, //AC累计发电量
	POWER_AC_DAY_ADD, //AC当日发电量
	POWER_AC_Y_ADD, //AC昨日发电量
	POWER_AC_MONTH_ADD, //AC月发电量
	POWER_AC_YEAR_ADD, //AC年发电量
	POWER_DC2_Y_ADD, //DC2支路昨日输出电量
	POWER_DC2_MON_ADD, //DC2支路月发电量
	POWER_AC_C_DAY_ADD, //AC支路日输入充电量
	POWER_AC_C_CON_ADD, //AC支路总输入充电量
	POWER_DC1_DAY_ADD, //DC1支路日输出电量
	POWER_DC1_CON_ADD, //DC1支路总输出电量
	POWER_DC1_C_DAY_ADD, //DC1支路日输入电量
	POWER_DC1_C_CON_ADD, //DC1支路总输入电量
	POWER_DC2_DAY_ADD, //DC2支路日输出电量
	POWER_DC2_CON_ADD, //DC2支路总输出电量
	POWER_DC2_C_DAY_ADD, //DC2支路日输入电量(预留)
	POWER_DC2_C_CON_ADD, //DC2支路总输入电量(预留)
	COUNT_SIZE, //所保存的发电量信息数量值
	
	RUN_TIME,   	//总运行时间	
	//RUN_TIME_NOW,	//连续运行时长
	ON_TIME,		//程序运行时长
	//ON_TIME_NOW,	//程序连续运行时长
	RESTART_COUNT,	//程序复位次数统计

	
	CO2_DEC_ADD, //CO2减排量
	C_DEC_ADD, //C减排量
	
	C_life, //电容寿命预测值
	POWER_AC_C_Y_ADD, //AC昨日充电量
	POWER_AC_C_MONTH_ADD, //AC月充电量
	POWER_AC_C_YEAR_ADD, //AC年充电量
	
	FREADJUST_COUNT, //一次调频动作统计
	FREADJUST_COUNT_TODAY, //一次调频动作当天统计

	FRELOCK_COUNT, //一次调频AGC闭锁动作统计
	FRELOCK_COUNT_TODAY, //一次调频AGC闭锁动作当天统计

	CRC_ERR_COUNT,
	CODE_DAY_COUNT,
	
	COUNT_CAL_MAX
};


/* ARM程序配置 - 用于arm_config_data[]地址 */
enum {	

	//变比系数
	FD_UINX_ADD = 0, //交流输入电压变比系数
	FD_UINX_ADDH,
	FD_IINX_ADD, //交流输入电流变比系数
	FD_IINX_ADDH,
	FD_UDC_ADD, //直流电压变比系数
	FD_UDC_ADDH,
	FD_IDC_ADD, //直流电流变比系数
	FD_IDC_ADDH,
	FD_UBTR_ADD, //电池电压变比系数
	FD_UBTR_ADDH,
	FD_UPOS_GND_ADD, //正极对地电压变比系数
	FD_UPOS_GND_ADDH,
	FD_UNEG_GND_ADD, //负极对地电压变比系数
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
	
	//电能表参数设定(削封填谷)
	ammeter1_A0, //电表1的地址A0
	ammeter1_A1, //电表1的地址A1
	ammeter1_A2, //电表1的地址A2
	ammeter1_A3, //电表1的地址A3
	ammeter1_A4, //电表1的地址A4
	ammeter1_A5, //电表1的地址A5
	ammeter2_A0, //电表2的地址A0
	ammeter2_A1, //电表2的地址A1
	ammeter2_A2, //电表2的地址A2
	ammeter2_A3, //电表2的地址A3
	ammeter2_A4, //电表2的地址A4
	ammeter2_A5, //电表2的地址A5
	ammeter1_vrc, //电表1变比系数
	ammeter2_vrc, //电表2变比系数
	dis_min_power,//电网最小供电功率限制值
	cos_seta,	//电网功率因素	
	
	Prio_Set_ADD, //控制模式设置
	Run_Mode_ADD, 
	COMMUICATION_PRO_EN, //通讯保护 
	STRATEGY_EN,	//策略使能

	INI_RESIS_FD_ADD , //绝缘监测系数
	INI_RESIS_REF_ADD, //绝缘判定门限
	INI_MONITOR_ADDR_ADD, //监控机地址
	MAX_P_ADD, //最大交流有功功率值
	MIN_P_ADD, //最小交流有功功率值	
	MAX_BA_C_ADD, //最大放电电流值
	MIN_BA_C_ADD, //最小充电电流
	MAX_BA_U_ADD, //最大直流电压
	MIN_BA_U_ADD, //最小直流电压
	MAX_Q_ADD, //最大交流无功功率值
	MIN_Q_ADD, //最小交流无功功率值
	ASK_LOCAL, //调试请求命令  0无请求 1有请求
		
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
	
	POWER_STEP,				//功率给定变化率
	HIGHT_TEMP_DERAT, 		//降额温度设定值
	MAX_IAC_ADD, 			//最大放交流电流值


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
	CODE_REMAIN_DATE, //密钥剩余时间
	CODE_TPYE, //密钥类型
	
	CODE_RELEASE_FLAG,
	CODE_DEBUG_FLAG,
	CODE_TEMPORARY1_FLAG,
	CODE_TEMPORARY2_FLAG,
	CODE_TEMPORARY3_FLAG,
	CODE_PERMANENT_FLAG,
	
	AGC_EN = 180,	// 调频参数	
	OVER_LOAD_L1, 	//一级过载保护值
	OVER_LOAD_L1_TIME,	//一贾刀过载时间设定
	OVER_LOAD_L2, 	//二级过载保护值
	OVER_LOAD_L2_TIME,	//二级过载时间设定
	OVER_LOAD_L3, 	//三级过载保护值(瞬时过载)
	
	//FRE_SET,	//频率设定值
	//VAL_SET,	//电压设定值
	//VAL_TIME,	//电压持续时间
	
	FRE_OVER,//频率适应上限
	FRE_UNDER,//频率适应下限

	//风机和温度保护设定点
	AC_FUN_START_TEMP_REC		= 190,//交流柜风机启动温度
	AC_FUN_STOP_TEMP_REC		= 191,//交流柜风机停止运行温度

	FILTER_CAPAC_ERR_TEMP_REC   = 192,//滤波电容过温点
	TRANSFORMER_ERR_TEMP_REC    = 193,//控制变压器过温点
	BUS_CAPAC_ERR_TEMP_REC		= 194,//母线电容过温点
	ELEC_REACTOR_ERR_TEMP_REC	= 195,//电抗器过温点
	AC_CABINET_ERR_TEMP_REC		= 196,//交流柜过温点	
	
	XFTG_EN = 200, //削封填谷参数
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
	
	AD_OF_Thr_1, //一级过频门限
	AD_OF_Thr_2, //二级过频门限
	AD_UF_Thr_1, //一级欠频门限
	AD_UF_Thr_2, //二级欠频门限
	AD_OF_Time_1, //一级过频时间
	AD_OF_Time_2, //二级过频时间
	AD_UF_Time_1, //一级欠频时间
	AD_UF_Time_2, //二级欠频时间

	AD_SB_EN, //软启使能
	AD_SB_TIME, //软启时间
	AD_SB_GOPWR, //软启目标功率
	AD_FR_EN, //故障恢复使能
	AD_FR_WTIME, //故障恢复等待时间
	AD_CO_EN, //通讯判线停机使能
	AD_CO_JTIME, //通讯掉线判定时间
	AD_BOOTUP_TIME, //启动时间
	
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
	unsigned char timer_flag; //定时器开启标志
	unsigned char recover_cnt; //故障恢复重启计数，超过5次就不再恢复
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
	uint16_t cntComm; //通讯计数
	uint16_t isoComm; //绝缘检测板通讯状态: 0-无通讯 1-正常
	int16_t temp[8]; //温度      		x10
	uint16_t volGridAB; //电网电压AB    x10
	uint16_t volGridBC; //电网电压BC    x10
	uint16_t volGridCA; //电网电压CA    x10
	uint16_t isoResPos; //绝缘阻值+(k)  x10 ＜50k就视为异常
	uint16_t isoResNeg; //绝缘阻值-(k)  x10 ＜50k就视为异常
	int16_t curGridA; //电网电流A       x10
	int16_t curGridB; //电网电流B       x10
	int16_t curGridC; //电网电流C       x10
	int16_t curLeak; //漏电流           x10
	uint16_t isoVolPos; //绝缘电压+     x10
	uint16_t isoVolNeg; //绝缘电压-     x10
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


#define TEST_MODE 0 //0-发行版本 !0-测试版本

#endif
