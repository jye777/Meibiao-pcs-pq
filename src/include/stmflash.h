#ifndef __STMFLASH_H__
#define __STMFLASH_H__
#include "stm32f4xx.h"
#include <cmsis_os.h>

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//STM32内部FLASH读写 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/9
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 



//FLASH起始地址
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH的起始地址
 

//FLASH 扇区的起始地址
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) 	//扇区0起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) 	//扇区1起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) 	//扇区2起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) 	//扇区3起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) 	//扇区4起始地址, 64 Kbytes  
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) 	//扇区5起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) 	//扇区6起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) 	//扇区7起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) 	//扇区8起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) 	//扇区9起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) 	//扇区10起始地址,128 Kbytes  
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) 	//扇区11起始地址,128 Kbytes  

//FLASH 扇区编号
#define FLASH_Sector_0					0
#define FLASH_Sector_1					1
#define FLASH_Sector_2					2
#define FLASH_Sector_3					3
#define FLASH_Sector_4					4
#define FLASH_Sector_5					5
#define FLASH_Sector_6					6
#define FLASH_Sector_7					7
#define FLASH_Sector_8					8
#define FLASH_Sector_9					9
#define FLASH_Sector_10					10
#define FLASH_Sector_11					11

/** @defgroup FLASH_Voltage_Range 
  * @{
  */ 
	
#define VoltageRange_1        ((uint8_t)0x00)  /*!<Device operating range: 1.8V to 2.1V */
#define VoltageRange_2        ((uint8_t)0x01)  /*!<Device operating range: 2.1V to 2.7V */
#define VoltageRange_3        ((uint8_t)0x02)  /*!<Device operating range: 2.7V to 3.6V */
#define VoltageRange_4        ((uint8_t)0x03)  /*!<Device operating range: 2.7V to 3.6V + External Vpp */

extern uint32_t STMFLASH_ReadByt(uint32_t faddr);
unsigned int STMFLASH_ReadWord(unsigned int faddr);		  	//读出字  
unsigned char STMFLASH_Write(uint16_t sector,uint8_t *pBuffer,uint32_t NumToWrite);	
void STMFLASH_Read(unsigned int ReadAddr,unsigned int *pBuffer,unsigned int NumToRead);   		//从指定地址开始读出指定长度的数据
void FLASH_DataCacheCmd(FunctionalState state);
FunctionalState FLASH_DataCacheRead(void);
						   
#endif

















