#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "cmsis_os.h"
#include "Driver_CAN.h"
#include "RTE_Components.h"

/******�궨��*************************************/
#define CAN_ADDR 				1				//΢�����ƹ�CAN��ַ
#define BROADCAST_ADDR 			0xFF			//�㲥��ַ

#define GET_SRCADDR(msg)		(uint8_t)((msg >> 0) & 0x000000ff)
#define GET_DSTADDR(msg)		(uint8_t)((msg >> 8) & 0x000000ff)
#define GET_MSGFLAG(msg)		(uint8_t)((msg >> 16) & 0x0000ffff)

/******�ⲿ��������*******************************/
extern uint16_t BMS_Frame01[];
extern uint16_t BMS_Frame02[];
extern uint16_t BMS_Frame03[];
extern uint16_t BMS_Frame04[];
extern uint16_t BMS_Frame05[];
extern uint16_t BMS_Frame06[];
extern uint16_t BMS_Frame07[];
extern uint16_t BMS_Frame08[];


/******�ⲿ��������*******************************/
extern bool CAN_Init(void);

/******�������Ͷ���*******************************/

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

/******��������***********************************/
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
	uint16_t volAc[3];		//������ѹ x10
	int16_t curAc[3];		//�������� x10
	uint16_t freq;			//����Ƶ�� x10
	int16_t pAc;			//�й����� x10
	int16_t qAc;			//�޹����� x10
	uint16_t volBus;		//ĸ��ֱ����ѹֵ x10
	int16_t curBus;			//ĸ��ֱ������ֵ x10
	int16_t pwrBus;			//ĸ��ֱ���๦�� x10
	int16_t tempA;			//IGBT�¶�_A��
	int16_t tempB;			//IGBT�¶�_B��
	int16_t tempC;			//IGBT�¶�_C��
	uint16_t state[8];		//ϵͳ״̬�Ĵ���0-7
	uint16_t stateLock[8];	//ϵͳ��������Ĵ���0-7
	uint16_t heartbeat;		//PCS����
	uint16_t runMode;		//����ģʽ: 0x1111-PQģʽ 0x2222-VFģʽ 0x3333-VSGģʽ
	uint16_t pcsFsm;		//PCS״̬��ֵ: 0����ʼ 1-3�������� 4�������� 5������ 6��ͣ��
} get_info_t;

typedef struct
{
	uint16_t runMode;		//����ģʽ����: 0-PQģʽ 1-VFģʽ 2-VSGģʽ
	uint16_t optCmd;		//���ػ�: 0-ͣ�� 1-����
	int16_t pAcSet;			//�й��趨 x10
	int16_t qAcSet;			//�޹��趨 x10

	uint16_t runModeLast;	//����ģʽ����(�ϴε�����ֵ)
	uint16_t optCmdLast;	//���ػ�(�ϴε�����ֵ)
	int16_t pAcSetLast;		//�й��趨(�ϴε�����ֵ)
	int16_t qAcSetLast;		//�޹��趨(�ϴε�����ֵ)
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

