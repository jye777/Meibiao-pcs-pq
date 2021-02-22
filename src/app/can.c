#include "stdlib.h"
#include "can.h"
#include "RTE_Components.h"
#include "Driver_CAN.h"
#include "stm32f407xx.h"
#include "tty.h"
#include "fpga.h"
#include "para.h"
#include "screen.h"
#include "modbus_comm.h"
#include "sdata_alloc.h"
#include "micro.h"
#include "log.h"
#include "comm_offline.h"

osThreadId can_send_tid;
osThreadDef(can_send_task, osPriorityNormal, 1, 0);

osThreadId data_parse_tid;
osThreadDef(data_parse_task, osPriorityNormal, 1, 0);


// CAN Interface configuration -------------------------------------------------
#define CAN_CONTROLLER			1       // CAN Controller number
#define CAN_LOOPBACK			0       // 0 = no loopback, 1 = external loopback
#define CAN_BITRATE_NOMINAL		250000  // Nominal bitrate (250 kbit/s)
#define _CAN_Driver_(n)			Driver_CAN##n
#define CAN_Driver_(n)			_CAN_Driver_(n)
#define ptrCAN					(&CAN_Driver_(CAN_CONTROLLER))
extern  ARM_DRIVER_CAN			CAN_Driver_(CAN_CONTROLLER);
#define CAN_RX_NUM				20		//一个循坏周期内最大接收帧数量
#define CAN_TX_NUM				1		//一个循环周期内需要发送的帧数量

#define EMU_ADDR				0xF0

#define EMU_SEND_ID(pgn)		(0x1800F050 | (pgn << 16))



enum
{
	PGN_00 = 0,
	PGN_01,
	PGN_02,
	PGN_03,

	PGN_MAX
};

CAN_frame rx_frame[CAN_RX_NUM];
CAN_frame tx_frame;
uint8_t rx_num_count = 0; //周期内接收帧数量
uint8_t rd_num_count = 0; //周期内已读取接收帧数量
uint32_t rx_obj_idx, tx_obj_idx;

pcs_info_t pcs1, pcs2;
pcs_data_t pcsData;
frame_data_t frameData[PGN_MAX];




void CAN_SignalUnitEvent (uint32_t event) {

  switch (event) {
    case ARM_CAN_EVENT_UNIT_ACTIVE:
      break;
    case ARM_CAN_EVENT_UNIT_WARNING:
      break;
    case ARM_CAN_EVENT_UNIT_PASSIVE:
      break;
    case ARM_CAN_EVENT_UNIT_BUS_OFF:
      break;
  }
}


void CAN_SignalObjectEvent (uint32_t obj_idx, uint32_t event) {

  if (event == ARM_CAN_EVENT_RECEIVE) {         // If receive event
    if (obj_idx == rx_obj_idx) {                // If receive object event
      //ptrCAN->MessageRead(rx_obj_idx, &rx_msg_info, rx_data, 8U);
	  //osSignalSet(ui_thread_id, 1U);            // Send signal to user interface thread
		ptrCAN->MessageRead(rx_obj_idx, &rx_frame[rx_num_count].msg_info, rx_frame[rx_num_count].data, 8U);
		rx_num_count++;
		if(rx_num_count >= CAN_RX_NUM) {
			rx_num_count = 0;
		}
    }
  }
  if (event == ARM_CAN_EVENT_SEND_COMPLETE) {   // If send completed event
    if (obj_idx == tx_obj_idx) {                // If transmit object event
      //osSignalSet(ui_thread_id, 1U << 1);       // Send signal to user interface thread
    }
  }
}


//------------------------------------------------------------------------------
//	说明：邮箱筛选器使用匹配模式，筛选器配置为不进行任何标识符匹配，及邮箱可以收
//	所有CAN消息。
//------------------------------------------------------------------------------
bool CAN_Init(void) {
	ARM_CAN_CAPABILITIES     can_cap;
	ARM_CAN_OBJ_CAPABILITIES can_obj_cap;
	int32_t                  status;
	uint32_t                 i, num_objects, clock;

	can_cap = ptrCAN->GetCapabilities ();                                         // Get CAN driver capabilities
	num_objects = can_cap.num_objects;                                            // Number of receive/transmit objects

	status = ptrCAN->Initialize    (CAN_SignalUnitEvent, CAN_SignalObjectEvent);  // Initialize CAN driver
	if (status != ARM_DRIVER_OK) { return false; }

	CAN1_FFIOenable();
	status = ptrCAN->PowerControl  (ARM_POWER_FULL);                              // Power-up CAN controller																													// 使能FIFO消息挂起中断
	if (status != ARM_DRIVER_OK) { return false; }

	status = ptrCAN->SetMode       (ARM_CAN_MODE_INITIALIZATION);                 // Activate initialization mode
	if (status != ARM_DRIVER_OK) { return false; }

	clock = ptrCAN->GetClock();                                                   // Get CAN bas clock
	if ((clock % (8U*CAN_BITRATE_NOMINAL)) == 0U) {                               // If CAN base clock is divisible by 8 * nominal bitrate without remainder
	status = ptrCAN->SetBitrate    (ARM_CAN_BITRATE_NOMINAL,                    // Set nominal bitrate
	                            CAN_BITRATE_NOMINAL,                        // Set nominal bitrate to configured constant value
	                            ARM_CAN_BIT_PROP_SEG  (5U) |                // Set propagation segment to 5 time quanta
	                            ARM_CAN_BIT_PHASE_SEG1(1U) |                // Set phase segment 1 to 1 time quantum (sample point at 87.5% of bit time)
	                            ARM_CAN_BIT_PHASE_SEG2(1U) |                // Set phase segment 2 to 1 time quantum (total bit is 8 time quanta long)
	                            ARM_CAN_BIT_SJW       (1U));                // Resynchronization jump width is same as phase segment 2
	} else if ((clock % (10U*CAN_BITRATE_NOMINAL)) == 0U) {                       // If CAN base clock is divisible by 10 * nominal bitrate without remainder
	status = ptrCAN->SetBitrate    (ARM_CAN_BITRATE_NOMINAL,                    // Set nominal bitrate
	                            CAN_BITRATE_NOMINAL,                        // Set nominal bitrate to configured constant value
	                            ARM_CAN_BIT_PROP_SEG  (7U) |                // Set propagation segment to 7 time quanta
	                            ARM_CAN_BIT_PHASE_SEG1(1U) |                // Set phase segment 1 to 1 time quantum (sample point at 90% of bit time)
	                            ARM_CAN_BIT_PHASE_SEG2(1U) |                // Set phase segment 2 to 1 time quantum (total bit is 10 time quanta long)
	                            ARM_CAN_BIT_SJW       (1U));                // Resynchronization jump width is same as phase segment 2
	} else if ((clock % (12U*CAN_BITRATE_NOMINAL)) == 0U) {                       // If CAN base clock is divisible by 12 * nominal bitrate without remainder
	status = ptrCAN->SetBitrate    (ARM_CAN_BITRATE_NOMINAL,                    // Set nominal bitrate
	                            CAN_BITRATE_NOMINAL,                        // Set nominal bitrate to configured constant value
	                            ARM_CAN_BIT_PROP_SEG  (7U) |                // Set propagation segment to 7 time quanta
	                            ARM_CAN_BIT_PHASE_SEG1(2U) |                // Set phase segment 1 to 2 time quantum (sample point at 83.3% of bit time)
	                            ARM_CAN_BIT_PHASE_SEG2(2U) |                // Set phase segment 2 to 2 time quantum (total bit is 12 time quanta long)
	                            ARM_CAN_BIT_SJW       (2U));                // Resynchronization jump width is same as phase segment 2
	} else {
	return false;
	}
	if (status != ARM_DRIVER_OK) { return false; }

	rx_obj_idx = 0xFFFFFFFFU;
	tx_obj_idx = 0xFFFFFFFFU;
	for (i = 0U; i < num_objects; i++) {                                          // Find first available object for receive and transmit
	can_obj_cap = ptrCAN->ObjectGetCapabilities (i);                            // Get object capabilities
	if      ((rx_obj_idx == 0xFFFFFFFFU) && (can_obj_cap.rx == 1U)) { rx_obj_idx = i;        }
	else if ((tx_obj_idx == 0xFFFFFFFFU) && (can_obj_cap.tx == 1U)) { tx_obj_idx = i; break; }
	}
	if ((rx_obj_idx == 0xFFFFFFFFU) || (tx_obj_idx == 0xFFFFFFFFU)) { return false; }

	// Set filter to receive messages with extended ID 0x12345678
	status = ptrCAN->ObjectSetFilter(rx_obj_idx, ARM_CAN_FILTER_ID_MASKABLE_ADD, ARM_CAN_EXTENDED_ID(0x12345678U), 0U);
	if (status != ARM_DRIVER_OK) { return false; }

	status = ptrCAN->ObjectConfigure(tx_obj_idx, ARM_CAN_OBJ_TX);                 // Configure transmit object
	if (status != ARM_DRIVER_OK) { return false; }

	status = ptrCAN->ObjectConfigure(rx_obj_idx, ARM_CAN_OBJ_RX);                 // Configure receive object
	if (status != ARM_DRIVER_OK) { return false; }

#if (CAN_LOOPBACK == 1)
	if (can_cap.external_loopback != 1U) { return false; }
	status = ptrCAN->SetMode (ARM_CAN_MODE_LOOPBACK_EXTERNAL);                    // Activate loopback external mode
	if (status != ARM_DRIVER_OK) { return false; }
#else
	status = ptrCAN->SetMode (ARM_CAN_MODE_NORMAL);                               // Activate normal operation mode
	if (status != ARM_DRIVER_OK) { return false; }
#endif

	return true;
}


static void CAN1_FFIOenable(void)
{
  CAN_TypeDef *ptr_CAN;
  ptr_CAN = (CAN_TypeDef *)CAN1;
  ptr_CAN->IER |= (CAN_IER_FFIE0 | CAN_IER_FFIE1);       
}


static void Can_transmit(CAN_frame *canframe) 
{
	ptrCAN->MessageSend(tx_obj_idx, &(*canframe).msg_info, (*canframe).data, 8U);
}


static void can_rx_info(void)
{
	uint32_t can_msg = 0;
	uint8_t can_msg_flag = 0, dstAddr, srcAddr;

	//printf_debug2("rd_num[%d] rx_num[%d]\n", rd_num_count, rx_num_count);
	while(rd_num_count != rx_num_count)
	{
		can_msg = rx_frame[rd_num_count].msg_info.id & 0x1fffffff;
		can_msg_flag = GET_MSGFLAG(can_msg);
		dstAddr = GET_DSTADDR(can_msg);
		srcAddr = GET_SRCADDR(can_msg);

		printf_debug2("can_msg[0x%x]\n", can_msg);
		
		if(can_msg_flag >= PGN_MAX || srcAddr != EMU_ADDR || 
		   ((dstAddr != 0x50 + miro_write.slave_id) && dstAddr != 0xFF)) {
			rd_num_count++;
			continue;
		}
		memcpy(&frameData[can_msg_flag], rx_frame[rd_num_count].data, 8);
		rd_num_count++;
        emuOffline.hbCur++;
	}
	rd_num_count = 0;
	rx_num_count = 0;
}

static void value_pack(uint8_t *pData, uint16_t val1, uint16_t val2, uint16_t val3, uint16_t val4)
{
	pData[0] = val1 >> 8;
	pData[1] = val1 & 0xFF;
	pData[2] = val2 >> 8;
	pData[3] = val2 & 0xFF;
	pData[4] = val3 >> 8;
	pData[5] = val3 & 0xFF;
	pData[6] = val4 >> 8;
	pData[7] = val4 & 0xFF;
}

#define CAN_SEND_INTERVAL 100
void can_send_task(void const *arg)
{
	pcs_data_t *pPd = &pcsData;
	get_info_t *pPgi = &pPd->pPcs1->getInfo;
	short runMode;
	
	while(1)
	{
		pPgi->volAc[0] = getting_data_handle(Addr_Param256, Upper_Param[Addr_Param256]);
		pPgi->volAc[1] = getting_data_handle(Addr_Param257, Upper_Param[Addr_Param257]);
		pPgi->volAc[2] = getting_data_handle(Addr_Param258, Upper_Param[Addr_Param258]);
		memset(&tx_frame.msg_info, 0U, sizeof(ARM_CAN_MSG_INFO));
		tx_frame.msg_info.id = ARM_CAN_EXTENDED_ID(EMU_SEND_ID(0) + miro_write.slave_id);
		value_pack(&tx_frame.data[0], pPgi->volAc[0], pPgi->volAc[1], pPgi->volAc[2], 0);
		Can_transmit(&tx_frame);
		osDelay(CAN_SEND_INTERVAL);

		pPgi->curAc[0] = getting_data_handle(Addr_Param259, Upper_Param[Addr_Param259]);
		pPgi->curAc[1] = getting_data_handle(Addr_Param260, Upper_Param[Addr_Param260]);
		pPgi->curAc[2] = getting_data_handle(Addr_Param261, Upper_Param[Addr_Param261]);
		memset(&tx_frame.msg_info, 0U, sizeof(ARM_CAN_MSG_INFO));
		tx_frame.msg_info.id = ARM_CAN_EXTENDED_ID(EMU_SEND_ID(1) + miro_write.slave_id);
		value_pack(&tx_frame.data[0], pPgi->curAc[0], pPgi->curAc[1], pPgi->curAc[2], 0);
		Can_transmit(&tx_frame);
		osDelay(CAN_SEND_INTERVAL);

		pPgi->freq = getting_data_handle(Addr_Param271, Upper_Param[Addr_Param271]);
		pPgi->pAc = getting_data_handle(Addr_Param267, Upper_Param[Addr_Param267]);
		pPgi->qAc = getting_data_handle(Addr_Param268, Upper_Param[Addr_Param268]);
		memset(&tx_frame.msg_info, 0U, sizeof(ARM_CAN_MSG_INFO));
		tx_frame.msg_info.id = ARM_CAN_EXTENDED_ID(EMU_SEND_ID(2) + miro_write.slave_id);
		value_pack(&tx_frame.data[0], pPgi->freq, pPgi->pAc, pPgi->qAc, 0);
		Can_transmit(&tx_frame);
		osDelay(CAN_SEND_INTERVAL);

		pPgi->volBus = getting_data_handle(Addr_Param262, Upper_Param[Addr_Param262]);
		pPgi->curBus = getting_data_handle(Addr_Param263, Upper_Param[Addr_Param263]);
		pPgi->pwrBus = getting_data_handle(Addr_Param269, Upper_Param[Addr_Param269]);
		memset(&tx_frame.msg_info, 0U, sizeof(ARM_CAN_MSG_INFO));
		tx_frame.msg_info.id = ARM_CAN_EXTENDED_ID(EMU_SEND_ID(3) + miro_write.slave_id);
		value_pack(&tx_frame.data[0], pPgi->volBus, pPgi->curBus, pPgi->pwrBus, pPgi->heartbeat);
		Can_transmit(&tx_frame);
		osDelay(CAN_SEND_INTERVAL);

		pPgi->tempA = Upper_Param[MP_TEMP1];
		pPgi->tempB = Upper_Param[MP_TEMP2];
		pPgi->tempC = Upper_Param[MP_TEMP3];
		pPgi->pcsFsm = (fpga_read(STATE8_ADD) & 0xF);
		memset(&tx_frame.msg_info, 0U, sizeof(ARM_CAN_MSG_INFO));
		tx_frame.msg_info.id = ARM_CAN_EXTENDED_ID(EMU_SEND_ID(4) + miro_write.slave_id);
		value_pack(&tx_frame.data[0], pPgi->tempA, pPgi->tempB, pPgi->tempC, pPgi->pcsFsm);
		Can_transmit(&tx_frame);
		osDelay(CAN_SEND_INTERVAL);

		pPgi->state[0] = fpga_read(STATE0_ADD);
		pPgi->state[1] = fpga_read(STATE1_ADD);
		pPgi->state[2] = fpga_read(STATE2_ADD);
		pPgi->state[3] = fpga_read(STATE3_ADD);
		memset(&tx_frame.msg_info, 0U, sizeof(ARM_CAN_MSG_INFO));
		tx_frame.msg_info.id = ARM_CAN_EXTENDED_ID(EMU_SEND_ID(5) + miro_write.slave_id);
		value_pack(&tx_frame.data[0], pPgi->state[0], pPgi->state[1], pPgi->state[2], pPgi->state[3]);
		Can_transmit(&tx_frame);
		osDelay(CAN_SEND_INTERVAL);

		pPgi->state[4] = fpga_read(STATE4_ADD);
		pPgi->state[5] = fpga_read(STATE5_ADD);
		pPgi->state[6] = fpga_read(STATE6_ADD);
		pPgi->state[7] = fpga_read(STATE7_ADD);
		memset(&tx_frame.msg_info, 0U, sizeof(ARM_CAN_MSG_INFO));
		tx_frame.msg_info.id = ARM_CAN_EXTENDED_ID(EMU_SEND_ID(6) + miro_write.slave_id);
		value_pack(&tx_frame.data[0], pPgi->state[4], pPgi->state[5], pPgi->state[6], pPgi->state[7]);
		Can_transmit(&tx_frame);
		osDelay(CAN_SEND_INTERVAL);

		pPgi->stateLock[0] = fpga_read(ERR_LCK_STATE0_ADD);
		pPgi->stateLock[1] = fpga_read(ERR_LCK_STATE1_ADD);
		pPgi->stateLock[2] = fpga_read(ERR_LCK_STATE2_ADD);
		pPgi->stateLock[3] = fpga_read(ERR_LCK_STATE3_ADD);
		memset(&tx_frame.msg_info, 0U, sizeof(ARM_CAN_MSG_INFO));
		tx_frame.msg_info.id = ARM_CAN_EXTENDED_ID(EMU_SEND_ID(7) + miro_write.slave_id);
		value_pack(&tx_frame.data[0], pPgi->stateLock[0], pPgi->stateLock[1], pPgi->stateLock[2], pPgi->stateLock[3]);
		Can_transmit(&tx_frame);
		osDelay(CAN_SEND_INTERVAL);

		pPgi->stateLock[4] = fpga_read(ERR_LCK_STATE4_ADD);
		pPgi->stateLock[5] = fpga_read(ERR_LCK_STATE5_ADD);
		pPgi->stateLock[6] = fpga_read(ERR_LCK_STATE6_ADD);
		pPgi->stateLock[7] = fpga_read(ERR_LCK_STATE7_ADD);
		memset(&tx_frame.msg_info, 0U, sizeof(ARM_CAN_MSG_INFO));
		tx_frame.msg_info.id = ARM_CAN_EXTENDED_ID(EMU_SEND_ID(8) + miro_write.slave_id);
		value_pack(&tx_frame.data[0], pPgi->stateLock[4], pPgi->stateLock[5], pPgi->stateLock[6], pPgi->stateLock[7]);
		Can_transmit(&tx_frame);
		osDelay(CAN_SEND_INTERVAL);

		pPgi->heartbeat++;
		runMode = Pcs_Run_Mode_Get();
		switch(runMode)
		{
			case PQ_MODE:
				pPgi->runMode = 0x1111;
				break;
			case VF_MODE:
				pPgi->runMode = 0x2222;
				break;
			case VSG_MODE:
				pPgi->runMode = 0x3333;
				break;
			default:
				pPgi->runMode = 0x0;
				break;
		}
		pPgi->pcsFsm = (fpga_read(STATE8_ADD) & 0xF);
		memset(&tx_frame.msg_info, 0U, sizeof(ARM_CAN_MSG_INFO));
		tx_frame.msg_info.id = ARM_CAN_EXTENDED_ID(EMU_SEND_ID(9) + miro_write.slave_id);
		value_pack(&tx_frame.data[0], pPgi->heartbeat, pPgi->runMode, pPgi->pcsFsm, 0);
		Can_transmit(&tx_frame);
		osDelay(CAN_SEND_INTERVAL);
	}
}

static void value_assign(uint8_t *pData, void *pVal1, void *pVal2, void *pVal3, void *pVal4)
{
	if(pVal1) {
		*(int16_t *)pVal1 = pData[0] << 8 | pData[1];
	}
	if(pVal2) {
		*(int16_t *)pVal2 = pData[2] << 8 | pData[3];
	}
	if(pVal3) {
		*(int16_t *)pVal3 = pData[4] << 8 | pData[5];
	}
	if(pVal4) {
		*(int16_t *)pVal4 = pData[6] << 8 | pData[7];
	}
}

void data_parse_task(void const *arg) 
{
	pcs_data_t *pPd = &pcsData;
	set_info_t *pPsi = &pPd->pPcs1->setInfo;
	uint8_t *pData, i;
	int16_t valTmp;
	
	while(1)
	{
		i = 0;
		pData = frameData[i].dataBuf;
		pPsi->runMode = pData[2 * (miro_write.slave_id - 1)] << 8 | pData[2 * (miro_write.slave_id - 1) + 1];
		printf_debug6("\nrunMode[0x%04x]\n", pPsi->runMode);
		if(pPsi->runMode == 0x1111 && Pcs_Run_Mode_Get() != PQ_MODE) {
			Pcs_Run_Mode_Set(PQ_MODE);
			mb_data.run_mode = 0;
			miro_write.Run_Mode = 0;
		}
		if(pPsi->runMode == 0x2222 && Pcs_Run_Mode_Get() != VF_MODE) {
			Pcs_Run_Mode_Set(VF_MODE);
			mb_data.run_mode = 1;
			miro_write.Run_Mode = 1;
		}
		if(pPsi->runMode == 0x3333 && Pcs_Run_Mode_Get() != VSG_MODE) {
			Pcs_Run_Mode_Set(VSG_MODE);
			mb_data.run_mode = 2;
			miro_write.Run_Mode = 2;
		}

		i = 1;
		pData = frameData[i].dataBuf;
		pPsi->optCmd = pData[2 * (miro_write.slave_id - 1)] << 8 | pData[2 * (miro_write.slave_id - 1) + 1];
		printf_debug6("optCmd[0x%04x]\n", pPsi->optCmd);
		if(pPsi->optCmd == 0x55AA && 
		   ((fpga_read(STATE8_ADD) & 0xF) != FSM_IDLE && (fpga_read(STATE8_ADD) & 0xF) != FSM_STOP)) {
			miro_write.manShutdownFlag = 1;
            set_pcs_shutdown();
			log_add(ET_OPSTATE,EST_EMS_CLOSE);
			miro_write.bootup_flag = 0;				
			miro_write.auto_recover_flag = 0;
		}
		if(pPsi->optCmd == 0xAA55 && 
		   ((fpga_read(STATE8_ADD) & 0xF) == FSM_IDLE || (fpga_read(STATE8_ADD) & 0xF) == FSM_STOP)) {
			miro_write.manShutdownFlag = 0;
            set_pcs_bootup();
			log_add(ET_OPSTATE,EST_EMS_OPEN);
			miro_write.bootup_flag = 1;
		}

		i = 2;
		pData = frameData[i].dataBuf;
		pPsi->pAcSet = pData[2 * (miro_write.slave_id - 1)] << 8 | pData[2 * (miro_write.slave_id - 1) + 1];
		printf_debug6("pAcSet[%d]\n", pPsi->pAcSet);
		if(pPsi->pAcSet != pPsi->pAcSetLast) {
			pPsi->pAcSetLast = pPsi->pAcSet;
			valTmp = pPsi->pAcSet;
			if(pPsi->pAcSet > 1500 * 1.2f * MODULUS) {
				valTmp = (int16_t)(1500 * 1.2f * MODULUS);
			}
			if(pPsi->pAcSet < (-1) * 1500 * 1.2f * MODULUS) {
				valTmp = (int16_t)((-1) * 1500 * 1.2f * MODULUS);
			}
			fpga_write(Addr_Param26, setting_data_handle(Addr_Param26, valTmp));
		}

		i = 3;
		pData = frameData[i].dataBuf;
		pPsi->qAcSet = pData[2 * (miro_write.slave_id - 1)] << 8 | pData[2 * (miro_write.slave_id - 1) + 1];
		printf_debug6("qAcSet[%d]\n", pPsi->qAcSet);
		if(pPsi->qAcSet != pPsi->qAcSetLast) {
			pPsi->qAcSetLast = pPsi->qAcSet;
			valTmp = pPsi->qAcSet;
			if(pPsi->qAcSet > 1500 * 1.2f * MODULUS) {
				valTmp = (int16_t)(1500 * 1.2f * MODULUS);
			}
			if(pPsi->qAcSet < (-1) * 1500 * 1.2f * MODULUS) {
				valTmp = (int16_t)((-1) * 1500 * 1.2f * MODULUS);
			}
			fpga_write(Addr_Param27, setting_data_handle(Addr_Param27, valTmp));
		}
		
		osThreadYield();
	}
}


void CAN_Thread(void const *arg) 
{
	pcsData.pPcs1 = &pcs1;
	pcsData.pPcs2 = &pcs2;
	
	can_send_tid = osThreadCreate(osThread(can_send_task), NULL);
	data_parse_tid = osThreadCreate(osThread(data_parse_task), NULL);

	while(1)
	{
		can_rx_info();
		
		osDelay(10);
	}
}


