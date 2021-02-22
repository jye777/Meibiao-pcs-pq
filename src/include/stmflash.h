#ifndef __STMFLASH_H__
#define __STMFLASH_H__
#include "stm32f4xx.h"
#include <cmsis_os.h>

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//STM32�ڲ�FLASH��д ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/9
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 



//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH����ʼ��ַ
 

//FLASH ��������ʼ��ַ
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) 	//����0��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) 	//����1��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) 	//����2��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) 	//����3��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) 	//����4��ʼ��ַ, 64 Kbytes  
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) 	//����5��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) 	//����6��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) 	//����7��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) 	//����8��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) 	//����9��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) 	//����10��ʼ��ַ,128 Kbytes  
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) 	//����11��ʼ��ַ,128 Kbytes  

//FLASH �������
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
unsigned int STMFLASH_ReadWord(unsigned int faddr);		  	//������  
unsigned char STMFLASH_Write(uint16_t sector,uint8_t *pBuffer,uint32_t NumToWrite);	
void STMFLASH_Read(unsigned int ReadAddr,unsigned int *pBuffer,unsigned int NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����
void FLASH_DataCacheCmd(FunctionalState state);
FunctionalState FLASH_DataCacheRead(void);
						   
#endif

















