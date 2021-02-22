/**
 * @file			sdata_alloc.h
 * @brief			
 * @author			wangpeng
 * @date			2016/6/3
 * @version			Initial Draft
 * @par				Copyright (C),  2013-2023, SCNEE
 * @par History:
 * 1.Date: 			2016/6/3
 *   Author: 			wangpeng
 *   Modification: 		Created file
*/
#include "stdint.h"
#ifndef SDATA_ALLOC_H
#define SDATA_ALLOC_H

/* flag 4Bytes */
#define SDATA_FLAG_BASE     0x0000

/* FPGA Zero */
#define FPGA_ZERO_BASE      0x0010

/* FPGA Data */
#define FPGA_NAND_DATA_BASE      0x0100

/* Count Cal */
#define COUNT_CAL_NAND_BASE		0x0400

/* ARM config */
#define CONFIG_NAND_BASE         0x0800

#define ENV_NAND_BASE					0x0c00					

/* 发电量信息(电量)(8K byte) */
#define PW_BASE             0x6000

/* 发电量信息(时间)(8K byte) */
#define PW_TIME_BASE        0x8000

#define MODULUS_COUNT_CAL	(100000)

void sdata_alloc_all(void);
void count_cal_write(short id,float value);
extern short fpga_pdatabuf_write(unsigned short id, short value);
extern short fpga_pdatabuf_read(unsigned short id);

float count_cal_read(short id);
extern void arm_config_data_write(unsigned short id,short value);
extern uint32_t arm_config_data_read32(unsigned short id);
extern short arm_config_data_read(unsigned short id);


#endif /* SDATA_ALLOC_H */

