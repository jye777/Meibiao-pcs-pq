#ifndef __FLASHDATA_H__
#define __FLASHDATA_H__

#include "stm32f4xx.h"
#include <cmsis_os.h>
#include "stm32f4xx_hal.h"


/* size 4Bytes */
#define fdata_FLAG_BASE     0x0000

#define MD5_BASE     				0x0004

/* FPGA Data */
#define FPGA_FLASH_DATA_BASE      0x0014

/* ARM config */
#define CONFIG_FLASH_BASE         0x0214

/* env data*/
#define ENV_FLASH_BASE						0x0c00					

#define FLASH_DATA_SECTOR 			FLASH_Sector_11
#define FLASH_DATA_ADDR					ADDR_FLASH_SECTOR_11
#define FLASH_DATA_SETOR_LEN		0x20000
#define FLASH_DATA_END_ADDR			(FLASH_DATA_ADDR + FLASH_DATA_SETOR_LEN)
#define FLASH_DATA_WORD_LEN			(FLASH_DATA_SETOR_LEN / 4)


extern volatile short *arm_config_flash_data;

extern void flashdata_init(void);

uint32_t get_flashdata0_addr(uint32_t offset);
uint32_t get_flashdata1_addr(uint32_t offset);
void flashdata_alloc_all(void);
static void read_flashdata(uint8_t *buf,uint32_t len);
uint32_t get_flashdata_addr(uint32_t offset);
extern void flashdata_update_flash(void);
extern void flashdata_update_task(const void *pdata);
static void get_flashdatamd5(uint8_t *md5);
static void FLASH_Set_ErrorCode(void);

void flashdata_update(void);

#endif







