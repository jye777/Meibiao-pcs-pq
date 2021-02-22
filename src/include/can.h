#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "cmsis_os.h"
#include "Driver_CAN.h"
#include "RTE_Components.h"

/******宏定义*************************************/
#define CAN_ADDR 				1				//微网控制柜CAN地址
#define BROADCAST_ADDR 			0xFF			//广播地址

#define GET_SRCADDR(msg)		(uint8_t)((msg >> 0) & 0x000000ff)
#define GET_DSTADDR(msg)		(uint8_t)((msg >> 8) & 0x000000ff)
#define GET_MSGFLAG(msg)		(uint8_t)((msg >> 16) & 0x0000ffff)

/******外部变量声明*******************************/
extern uint16_t BMS_Frame01[];
extern uint16_t BMS_Frame02[];
extern uint16_t BMS_Frame03[];
extern uint16_t BMS_Frame04[];
extern uint16_t BMS_Frame05[];
extern uint16_t BMS_Frame06[];
extern uint16_t BMS_Frame07[];
extern uint16_t BMS_Frame08[];


/******外部函数声明*******************************/
extern bool CAN_Init(void);

/******数据类型定义*******************************/

typedef struct CAN_frame
{
		uint8_t            data[8];
		ARM_CAN_MSG_INFO   msg_info;
}CAN_frame;

typedef struct CAN_rxdata
{
		uint16_t						 soc;
}CAN_rxdata;

typedef struct CAN_txdata
{
		uint8_t						 ats_sta;
}CAN_txdata;

/******函数申明***********************************/
static void CAN1_FFIOenable(void);
static void CAN1_FFIOdsiable(void);
static void Can_transmit(CAN_frame *canframe); 
static void Can_DataSOC_Parser(void);
bool CAN_Initialize (void);
static void can_rx_info(void);
static void CAN_SignalObjectEvent (uint32_t obj_idx, uint32_t event); 
static void CAN_SignalUnitEvent (uint32_t event);
static void Can_Tx_info(void);
static void CAN1_FFIOdisable(void);
void data_parse_task(void const *arg);
void can_send_task(void const *arg);
void CAN_Thread(void const *arg);


typedef struct
{
	uint8_t dataBuf[8];
} frame_data_t;


typedef struct
{
	uint16_t volAc[3];		//交流电压 x10
	int16_t curAc[3];		//交流电流 x10
	uint16_t freq;			//电网频率 x10
	int16_t pAc;			//有功功率 x10
	int16_t qAc;			//无功功率 x10
	uint16_t volBus;		//母线直流电压值 x10
	int16_t curBus;			//母线直流电流值 x10
	int16_t pwrBus;			//母线直流侧功率 x10
	int16_t tempA;			//IGBT温度_A相
	int16_t tempB;			//IGBT温度_B相
	int16_t tempC;			//IGBT温度_C相
	uint16_t state[8];		//系统状态寄存器0-7
	uint16_t stateLock[8];	//系统故障锁存寄存器0-7
	uint16_t heartbeat;		//PCS心跳
	uint16_t runMode;		//运行模式: 0x1111-PQ模式 0x2222-VF模式 0x3333-VSG模式
	uint16_t pcsFsm;		//PCS状态机值: 0：初始 1-3：启动中 4：工作中 5：故障 6：停机
} get_info_t;

typedef struct
{
	uint16_t runMode;		//运行模式设置: 0-PQ模式 1-VF模式 2-VSG模式
	uint16_t optCmd;		//开关机: 0-停机 1-开机
	int16_t pAcSet;			//有功设定 x10
	int16_t qAcSet;			//无功设定 x10

	uint16_t runModeLast;	//运行模式设置(上次的设置值)
	uint16_t optCmdLast;	//开关机(上次的设置值)
	int16_t pAcSetLast;		//有功设定(上次的设置值)
	int16_t qAcSetLast;		//无功设定(上次的设置值)
} set_info_t;


typedef struct
{
	get_info_t getInfo;
	set_info_t setInfo;
} pcs_info_t;


typedef struct
{
	pcs_info_t *pPcs1;
	pcs_info_t *pPcs2;
} pcs_data_t;


extern pcs_data_t pcsData;

