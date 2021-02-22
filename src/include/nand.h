#ifndef NAND_H
#define NAND_H
#include "stdint.h"
#include <stdbool.h>
#include "stm32f4xx_hal.h"

#define BITSET(a,b) 	(a |= ( (0x01<<b)))
#define BITCLEAR(a,b) (a &= (~(0x01<<b)))
#define BITGET(a,b)   ((a>>b) & 0x01)

#define ROWADDR(ADDRESS) (((ADDRESS)->Page & 0x003f) | ((((ADDRESS)->Zone) & 0x0001) << 6) | ((((ADDRESS)->Block) & 0x3ff) << 7))
//#define ROWADDR(ADDRESS) (((ADDRESS)->Page & 0x003f) | ((((ADDRESS)->Block) & 0x3ff) << 6))
#define NANDCACHE_CMD_WRITE_TRUE1 0x15

/* NAND memory Ð´»º´æÄ£Ê½×´Ì¬ */
#define NAND_CACHE_PROGRAM_BUSY                  	((uint8_t)0x00)
#define NAND_CACHE_PROGRAM_N_0_ERR                ((uint8_t)0x01)
#define NAND_CACHE_PROGRAM_N_1_ERR                ((uint8_t)0x02)
#define NAND_CACHE_PROGRAM_COMPLETE               ((uint8_t)0x20)
#define NAND_CACHE_PROGRAM_READY                	((uint8_t)0x40)

#define NAND_CMD_STATUS_START                     ((uint8_t)0x70U)
#define NAND_CMD_STATUS_END                       ((uint8_t)0x78U)

#define NAND_TIMEOUT															 0x000003e8

typedef enum 
{
  NAND_IS_OK               = 0x00U,
  NAND_BLOCK_IS_ERROR      = 0x01U,
  NAND_IS_BUSY             = 0x02U,
	NAND_ADDRESS_IS_INVALID  = 0x03U,
  NAND_IS_TIMEOUT          = 0x04U
} NAND_StatusTypeDef;

typedef struct 
{
  uint16_t column;   /*column address in the page*/
	
  uint8_t  Page;     /*page address in the block */
	
	uint8_t  Zone;    /* plane address (for multiplane operations) / block address (for normal operations)*/

	uint16_t Block;    /* block address*/
	
}NAND_Address;

typedef struct
{
	char fname[30];
	uint8_t fmd5[16];
	uint32_t fsize;
	uint32_t flastsize;
	uint32_t fsizemax;
	uint32_t fwritennum;
	uint32_t freadnum;
	uint32_t fnewstartaddr;
	uint32_t flaststartaddr;
	uint32_t fstartaddr;
	uint32_t fcuraddr;
	uint32_t fendaddr;
	uint32_t faddrmin;
	uint32_t faddrmax;
	uint32_t funused;
}FILE_STRUCT;

extern uint8_t isbadblock(uint16_t blockno);
NAND_StatusTypeDef fwrite_w(FILE_STRUCT *file,uint8_t *buf,uint32_t len);
NAND_StatusTypeDef fread_w(FILE_STRUCT *file,uint8_t *buf,uint32_t len);
extern NAND_Address SerialAddress_to_CycleAddress(uint32_t addr,NAND_HandleTypeDef *hnand);
extern uint32_t CycleAddress_to_SerialAddress(NAND_Address *pAddress,NAND_HandleTypeDef *hnand);
extern void nand_int(void);
static void block_table_update(NAND_HandleTypeDef *hnand);
extern uint32_t get_filedat_addr(uint32_t addr,uint8_t *isnandused);
extern uint32_t get_filedat_block(uint32_t addr);
extern void set_filedat_addr(uint32_t addr);
void format(void);
extern void format_all(void);
extern void badblockreint(void);
extern uint8_t get_file_update(void);
extern void clear_file_update(void);
extern bool set_readnum(FILE_STRUCT *f , uint32_t readnum);
extern bool set_writenum(FILE_STRUCT *f , uint32_t writenum);
#endif
