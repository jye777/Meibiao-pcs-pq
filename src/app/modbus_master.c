#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "cmsis_os.h"
#include "cpu.h"
#include "stm32f4xx_hal.h"
#include "rl_net.h"
#include "Driver_USART.h"
#include "modbus_master.h"
#include "modbus_comm.h"
#include "crc.h"
#include "systime.h"
#include "fpga.h"
#include "pcs.h"
#include "sdata_alloc.h"
#include "para.h"
#include "tty.h"
#include "micro.h"


#define METER0_ADDR             1
#define METER1_ADDR             2

meter_data_t meter0_data;
meter_data_t meter1_data;
collect_board_data_t collect_data;

extern pcs_manage_t pcs_manage;
extern modbus_data mb_data;

static uint8_t master_comm_buf[256];
static uint8_t master_data_buf[256];
short load_power;
//modbus_config_t modbus_config_u6;

extern ARM_DRIVER_USART Driver_USART6;
extern osThreadId tid_modbus_master;
extern float SquareRootFloat(float value);


//#if(METER_NUM != 0)
void usart6_callback(uint32_t event)
{
    switch(event)
    {
	    case ARM_USART_EVENT_TX_COMPLETE:
	        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	        osSignalSet(tid_modbus_master, 0x02);
	        break;
			
	    case ARM_USART_EVENT_RX_TIMEOUT:
	        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
	        osSignalSet(tid_modbus_master, 0x01);
	        break;

	    default:
	        break;
    }
}
//#endif

static int master_send_packet(uint8_t *buf, uint16_t cnt)
{
    ARM_DRIVER_USART *drv = &Driver_USART6;

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);

    while(1)
    {
        if(drv->GetStatus().tx_busy)
        {
            osDelay(1);
            continue;
        }

        drv->Send(buf, cnt);
        osSignalWait(2, 100);
        break;
    }

    return 0;
}

static int master_send_packet2(uint8_t *buf, uint16_t cnt)
{
    ARM_DRIVER_USART *drv = &Driver_USART6;

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);

    while(1)
    {
        if(drv->GetStatus().tx_busy)
        {
            osDelay(1);
            continue;
        }

        drv->Send(buf, cnt);
        osSignalWait(2,50);
        break;
    }

    return 0;
}


static int master_recv_packet(uint8_t *buf, uint16_t buf_size, uint16_t *recv_len)
{
    uint16_t plen;
    ARM_DRIVER_USART *drv = &Driver_USART6;

    *recv_len = 0;

    while(drv->GetStatus().tx_busy)
    {
        osDelay(1);
    }
    
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
    drv->Control(ARM_USART_ABORT_RECEIVE, 0);
    drv->Receive(buf, buf_size);

    /* 等待数据包接收完成 */
    osSignalWait(1, 500);

    plen = drv->GetRxCount();

    *recv_len = plen;

    return plen;
}

int master_crc_set(unsigned char *pdata, unsigned short cntlen)
{
    unsigned short crc;

    crc = crc16(pdata, cntlen);
    pdata[cntlen] = crc >> 8;
    pdata[cntlen + 1] = crc & 0xff;

    return 0;
}


static int master_read_reg(uint8_t slave, uint16_t addr, uint8_t *buf)
{
    uint8_t *comm_buf;
    uint16_t buf_size;
    uint16_t recv_len;
    comm_buf = master_comm_buf;
    buf_size = (uint16_t)sizeof(master_comm_buf);

	comm_buf[0] = slave;
	comm_buf[1] = 0x03;
	comm_buf[2] = addr >> 8;
	comm_buf[3] = addr & 0xff;
	comm_buf[4] = 2 >> 8;
	comm_buf[5] = 2 & 0xff;
	master_crc_set(comm_buf, 6);

    master_send_packet(comm_buf, 8);

	buf_size = 5 + 2 * 2;
    if(master_recv_packet(comm_buf, buf_size, &recv_len)  == 0) {
        return 1;
    } else {
		if(comm_buf[1] & 0x80) {
			switch(comm_buf[2])
			{
				case 0x01:
					tty_printf("Illegal function id.\n");
					break;
				case 0x02:
					tty_printf("Illegal address.\n");
					break;
				case 0x03:
					tty_printf("Illegal quantity.\n");
					break;
				case 0x04:
					tty_printf("Invaild operation.\n");
					break;
				default:
					tty_printf("Unkown error.\n");
					break;
			}
			return 2;
		} else {
			memcpy(buf, comm_buf, 9);
			tty_printf("\n[RX]:");
			for(uint8_t i = 0; i < 9; i++) {
				tty_printf(" 0x%02x", buf[i]);
			}
			tty_printf("\n");
		}
	}

    return 0;
}


static int master_read_reg2(uint8_t slave, uint16_t addr, uint8_t *buf)
{
    uint8_t *comm_buf;
    uint16_t buf_size;
    uint16_t recv_len;
    comm_buf = master_comm_buf;
    buf_size = (uint16_t)sizeof(master_comm_buf);

	comm_buf[0] = slave;
	comm_buf[1] = 0x03;
	comm_buf[2] = addr >> 8;
	comm_buf[3] = addr & 0xff;
	comm_buf[4] = 1 >> 8;
	comm_buf[5] = 1 & 0xff;
	master_crc_set(comm_buf, 6);

    master_send_packet(comm_buf, 8);

	buf_size = 5 + 2 * 1;
    if(master_recv_packet(comm_buf, buf_size, &recv_len)  == 0) {
        return 1;
    } else {
		if(comm_buf[1] & 0x80) {
			switch(comm_buf[2])
			{
				case 0x01:
					tty_printf("Illegal function id.\n");
					break;
				case 0x02:
					tty_printf("Illegal address.\n");
					break;
				case 0x03:
					tty_printf("Illegal quantity.\n");
					break;
				case 0x04:
					tty_printf("Invaild operation.\n");
					break;
				default:
					tty_printf("Unkown error.\n");
					break;
			}
			return 2;
		} else {
			memcpy(buf, comm_buf, 7);
			tty_printf("\n[RX]:");
			for(uint8_t i = 0; i < 7; i++) {
				tty_printf(" 0x%02x", buf[i]);
			}
			tty_printf("\n");
		}
	}

    return 0;
}

static int master_read_reg3(uint8_t slave, uint16_t addr, uint8_t *buf)
{
    uint8_t *comm_buf;
    uint16_t buf_size;
    uint16_t recv_len;
    comm_buf = master_comm_buf;
    buf_size = (uint16_t)sizeof(master_comm_buf);

	comm_buf[0] = slave;
	comm_buf[1] = 0x03;
	comm_buf[2] = addr >> 8;
	comm_buf[3] = addr & 0xff;
	comm_buf[4] = 19 >> 8;
	comm_buf[5] = 19 & 0xff;
	master_crc_set(comm_buf, 6);

    master_send_packet(comm_buf, 8);

	buf_size = 5 + 2 * 19;
    if(master_recv_packet(comm_buf, buf_size, &recv_len)  == 0) {
        return 1;
    } else {
		if(comm_buf[1] & 0x80) {
			switch(comm_buf[2])
			{
				case 0x01:
					tty_printf("Illegal function id.\n");
					break;
				case 0x02:
					tty_printf("Illegal address.\n");
					break;
				case 0x03:
					tty_printf("Illegal quantity.\n");
					break;
				case 0x04:
					tty_printf("Invaild operation.\n");
					break;
				default:
					tty_printf("Unkown error.\n");
					break;
			}
			return 2;
		} else {
			memcpy(buf, comm_buf, 43);
			tty_printf("\n[RX]:");
			for(uint8_t i = 0; i < 43; i++) {
				tty_printf(" 0x%02x", buf[i]);
			}
			tty_printf("\n");
		}
	}

    return 0;
}




static int master_send_packet5(uint8_t *buf, uint16_t cnt)
{
    ARM_DRIVER_USART *drv = &Driver_USART6;

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);

    while(1)
    {
        if(drv->GetStatus().tx_busy)
        {
            osDelay(1);
            continue;
        }

        drv->Send(buf, cnt);
        osSignalWait(2,50);
        break;
    }

    return 0;
}

static int master_recv_packet5(uint8_t *buf, uint16_t buf_size, uint16_t *recv_len)
{
    uint16_t plen;
    ARM_DRIVER_USART *drv = &Driver_USART6;

    *recv_len = 0;

    while(drv->GetStatus().tx_busy)
    {
        osDelay(1);
    }
    
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
    drv->Control(ARM_USART_ABORT_RECEIVE, 0);
    drv->Receive(buf, buf_size);

    /* 等待数据包接收完成 */
    osSignalWait(1, 200);

    plen = drv->GetRxCount();

    *recv_len = plen;

    return 0;
}

static int master_read_mul_reg5(uint8_t slave, uint16_t addr,
                               uint8_t *buf, uint16_t num)
{
    uint8_t *comm_buf;
    uint16_t buf_size;
    uint16_t recv_len;
    uint16_t crc_value;

    comm_buf = master_comm_buf;
    buf_size = (uint16_t)sizeof(master_comm_buf);

    comm_buf[0] = slave;
    comm_buf[1] = 0x03;
    comm_buf[2] = addr >> 8;
    comm_buf[3] = addr & 0xff;
    comm_buf[4] = num >> 8;
    comm_buf[5] = num & 0xff;
    crc_value = crc16(comm_buf, 6);
    comm_buf[6] = crc_value >> 8;
    comm_buf[7] = crc_value & 0xff;

    master_send_packet5(comm_buf, 8);

    if(master_recv_packet5(comm_buf, buf_size, &recv_len))
    {
        return 1;
    }

    if(recv_len < 5 + 2 * num)
    {
        return 1;
    }

    crc_value = crc16(comm_buf, 3 + 2 * num);

    if(comm_buf[0] != slave)
    {
        return 1;
    }

    if(crc_value != (comm_buf[3 + 2 * num] << 8 | comm_buf[4 + 2 * num]))
    {
        return 1;
    }

    memcpy(buf, &comm_buf[3], 2 * num);

    return 0;
}

static void master_get_data5(uint8_t slave, collect_board_data_t *collect_data)
{
	if(master_read_mul_reg5(slave,0x00,master_data_buf,0x6) == 0)
	{
		collect_data->ac_cabinet_temp = master_data_buf[0]<< 8 | master_data_buf[1];
		collect_data->filter_capacitance_temp1 = master_data_buf[2]<< 8 | master_data_buf[3];
		collect_data->filter_capacitance_temp2 = master_data_buf[4]<< 8 | master_data_buf[5];
		collect_data->transformer_temp = master_data_buf[6]<< 8 | master_data_buf[7];
		collect_data->bus_capacitance_temp = master_data_buf[8]<< 8 | master_data_buf[9];
		collect_data->electric_reactor_temp = master_data_buf[10]<< 8 | master_data_buf[11];
										
	}
}

static int master_write_reg5(uint8_t slave, uint16_t addr, uint16_t value)
{
    uint8_t *comm_buf;
    uint16_t buf_size;
    uint16_t recv_len;
    uint16_t crc_value;

    comm_buf = master_comm_buf;
    buf_size = (uint16_t)sizeof(master_comm_buf);

    comm_buf[0] = slave;
    comm_buf[1] = 0x06;
    comm_buf[2] = addr >> 8;
    comm_buf[3] = addr & 0xff;
    comm_buf[4] = value >> 8;
    comm_buf[5] = value & 0xff;
    crc_value = crc16(comm_buf, 6);
    comm_buf[6] = crc_value >> 8;
    comm_buf[7] = crc_value & 0xff;
    master_send_packet5(comm_buf, 8);
	
	return 0;
}


#if 0
static int master_read_reg3(uint8_t slave, uint8_t ch, uint16_t maxByte, uint8_t *buf)
{
    uint8_t *comm_buf;
    uint16_t buf_size;
    uint16_t recv_len, num;
    comm_buf = master_comm_buf;
    buf_size = (uint16_t)sizeof(master_comm_buf);

	comm_buf[0] = slave;
	comm_buf[1] = 105;
	comm_buf[2] = ch;
	comm_buf[3] = maxByte >> 8;
	comm_buf[4] = maxByte & 0xff;
	master_crc_set(comm_buf, 5);

    master_send_packet(comm_buf, 7);

    if(master_recv_packet(comm_buf, buf_size, &recv_len)  == 0) {
        return 1;
    } else {
    	num = maxByte + 5;
		memcpy(buf, comm_buf, num);
		/*
		printf_debug2("\n[RX]:");
		for(uint8_t i = 0; i < num; i++) {
			printf_debug2(" 0x%02x", buf[i]);
		}
		printf_debug2("\n");
		*/
	}

    return 0;
}


static int master_read_reg4(uint8_t slave, uint8_t ch, uint8_t *buf)
{
    uint8_t *comm_buf;
    uint16_t buf_size;
    uint16_t recv_len, num;
    comm_buf = master_comm_buf;
    buf_size = (uint16_t)sizeof(master_comm_buf);

	comm_buf[0] = slave;
	comm_buf[1] = 106;
	comm_buf[2] = ch;
	master_crc_set(comm_buf, 3);

    master_send_packet(comm_buf, 5);

    if(master_recv_packet(comm_buf, buf_size, &recv_len)  == 0) {
        return 1;
    } else {
    	num = 7;
		memcpy(buf, comm_buf, num);
		/*
		printf_debug2("\n[RX]:");
		for(uint8_t i = 0; i < num; i++) {
			printf_debug2(" 0x%02x", buf[i]);
		}
		printf_debug2("\n");
		*/
	}

    return 0;
}
#endif


union {
	uint32_t u32;
	float f32;
} uPower;


iso_info_t isoInfo;


#define CT_COEF 60
void modbus_master_task(const void *pdata)
{
	uint32_t regAddr;
	uint8_t slaveAddr, ofs = 0, i = 0;
	iso_info_t *pIi = &isoInfo;
	uint8_t bms_id;		
	uint16_t collect_board_id;

	osDelay(2000);
	
    while(1)
    {    
/* 风机 */    
#if 1
		collect_board_id = arm_config_data_read(INI_MONITOR_ADDR_ADD); 
		master_get_data5(collect_board_id,&collect_data);
    	osDelay(500);
		
		master_write_reg5(0x01,0x01,01);//风机使能命令	
		osDelay(1000);
		master_write_reg5(0x01,0x02,miro_write.speed_value); //风机1调速
		osDelay(1000);
		
		master_write_reg5(0x02,0x01,01);//风机使能命令	
		osDelay(1000);		
		master_write_reg5(0x02,0x02,miro_write.speed_value); //风机2调速		
    	osDelay(1000);
#endif	
/* 绝缘板 */ 
		pIi->cntComm = 0;
    	slaveAddr = 0x03; 
		ofs = 0;
	
		regAddr = 0; 
		if (master_read_reg3(slaveAddr, regAddr, master_data_buf) == 0) {
			pIi->cntComm++;
			for(i = 0; i < 8; i++) {
				pIi->temp[i] = (int16_t)((master_data_buf[3+2*i] << 8) | master_data_buf[4+2*i]);
				pIi->temp[i] -= 2730;
				pIi->temp[i] = (pIi->temp[i] < -400 ? 0 : pIi->temp[i]);
			}
			ofs = 19;
			pIi->volGridAB = (uint16_t)((master_data_buf[ofs] << 8) | master_data_buf[ofs+1]);ofs+=2;
			pIi->volGridBC = (uint16_t)((master_data_buf[ofs] << 8) | master_data_buf[ofs+1]);ofs+=2;
			pIi->volGridCA = (uint16_t)((master_data_buf[ofs] << 8) | master_data_buf[ofs+1]);ofs+=2;
			pIi->isoResPos = (uint16_t)((master_data_buf[ofs] << 8) | master_data_buf[ofs+1]);ofs+=2;
			pIi->isoResNeg = (uint16_t)((master_data_buf[ofs] << 8) | master_data_buf[ofs+1]);ofs+=2;
			pIi->curGridA = (int16_t)((master_data_buf[ofs] << 8) | master_data_buf[ofs+1]);ofs+=2;
			pIi->curGridB = (int16_t)((master_data_buf[ofs] << 8) | master_data_buf[ofs+1]);ofs+=2;
			pIi->curGridC = (int16_t)((master_data_buf[ofs] << 8) | master_data_buf[ofs+1]);ofs+=2;
			pIi->curLeak = (int16_t)((master_data_buf[ofs] << 8) | master_data_buf[ofs+1]);ofs+=2;
			pIi->isoVolPos = (uint16_t)((master_data_buf[ofs] << 8) | master_data_buf[ofs+1]);ofs+=2;
			pIi->isoVolNeg = (uint16_t)((master_data_buf[ofs] << 8) | master_data_buf[ofs+1]);ofs+=2;
			for(i = 0; i < 8; i++) {
				printf_debug7("temp%d[%d]\n", i, pIi->temp[i]);
			}
			printf_debug7("volGridAB[%d]\n", pIi->volGridAB);
			printf_debug7("volGridBC[%d]\n", pIi->volGridBC);
			printf_debug7("volGridCA[%d]\n", pIi->volGridCA);
			printf_debug7("isoResPos[%d]\n", pIi->isoResPos);
			printf_debug7("isoResNeg[%d]\n", pIi->isoResNeg);
			printf_debug7("curGridA[%d]\n", pIi->curGridA);
			printf_debug7("curGridB[%d]\n", pIi->curGridB);
			printf_debug7("curGridC[%d]\n", pIi->curGridC);
			printf_debug7("curLeak[%d]\n", pIi->curLeak);
			printf_debug7("isoVolPos[%d]\n", pIi->isoVolPos);
			printf_debug7("isoVolNeg[%d]\n", pIi->isoVolNeg);
		} else {
			osDelay(100);
			if (master_read_reg3(slaveAddr, regAddr, master_data_buf) == 0) {
				pIi->cntComm++;
				for(i = 0; i < 8; i++) {
					pIi->temp[i] = (int16_t)((master_data_buf[3+2*i] << 8) | master_data_buf[4+2*i]);
					pIi->temp[i] -= 2730;
					pIi->temp[i] = (pIi->temp[i] < -400 ? 0 : pIi->temp[i]);
				}
				ofs = 19;
				pIi->volGridAB = (uint16_t)((master_data_buf[ofs] << 8) | master_data_buf[ofs+1]);ofs+=2;
				pIi->volGridBC = (uint16_t)((master_data_buf[ofs] << 8) | master_data_buf[ofs+1]);ofs+=2;
				pIi->volGridCA = (uint16_t)((master_data_buf[ofs] << 8) | master_data_buf[ofs+1]);ofs+=2;
				pIi->isoResPos = (uint16_t)((master_data_buf[ofs] << 8) | master_data_buf[ofs+1]);ofs+=2;
				pIi->isoResNeg = (uint16_t)((master_data_buf[ofs] << 8) | master_data_buf[ofs+1]);ofs+=2;
				pIi->curGridA = (int16_t)((master_data_buf[ofs] << 8) | master_data_buf[ofs+1]);ofs+=2;
				pIi->curGridB = (int16_t)((master_data_buf[ofs] << 8) | master_data_buf[ofs+1]);ofs+=2;
				pIi->curGridC = (int16_t)((master_data_buf[ofs] << 8) | master_data_buf[ofs+1]);ofs+=2;
				pIi->curLeak = (int16_t)((master_data_buf[ofs] << 8) | master_data_buf[ofs+1]);ofs+=2;
				pIi->isoVolPos = (uint16_t)((master_data_buf[ofs] << 8) | master_data_buf[ofs+1]);ofs+=2;
				pIi->isoVolNeg = (uint16_t)((master_data_buf[ofs] << 8) | master_data_buf[ofs+1]);ofs+=2;
				for(i = 0; i < 8; i++) {
					printf_debug7("temp%d[%d]\n", i, pIi->temp[i]);
				}
				printf_debug7("volGridAB[%d]\n", pIi->volGridAB);
				printf_debug7("volGridBC[%d]\n", pIi->volGridBC);
				printf_debug7("volGridCA[%d]\n", pIi->volGridCA);
				printf_debug7("isoResPos[%d]\n", pIi->isoResPos);
				printf_debug7("isoResNeg[%d]\n", pIi->isoResNeg);
				printf_debug7("curGridA[%d]\n", pIi->curGridA);
				printf_debug7("curGridB[%d]\n", pIi->curGridB);
				printf_debug7("curGridC[%d]\n", pIi->curGridC);
				printf_debug7("curLeak[%d]\n", pIi->curLeak);
				printf_debug7("isoVolPos[%d]\n", pIi->isoVolPos);
				printf_debug7("isoVolNeg[%d]\n", pIi->isoVolNeg);
			} else {
				pIi->cntComm = 0;
				for(i = 0; i < 8; i++) {
					pIi->temp[i] = 0;
				}
				pIi->volGridAB = 0;
				pIi->volGridBC = 0;
				pIi->volGridCA = 0;
				pIi->isoResPos = 0;
				pIi->isoResNeg = 0;
				pIi->curGridA = 0;
				pIi->curGridB = 0;
				pIi->curGridC = 0;
				pIi->curLeak = 0;
				pIi->isoVolPos = 0;
				pIi->isoVolNeg = 0;
				printf_debug7("JY-Receive failed.\n");
			}
		}		
		osDelay(100);
		
		pIi->isoComm = (pIi->cntComm == 0 ? 0 : 1);	
    }
}



