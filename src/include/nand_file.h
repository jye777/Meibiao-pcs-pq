#ifndef NAND_FILE_H
#define NAND_FILE_H

#include "nand.h"
#include "cmsis_os.h"

extern osSemaphoreId nand_sem;

extern void fileint(void);
extern uint8_t get_lastfile(char *name, FILE_STRUCT *f);
extern uint8_t check_fileware_def(char *name, FILE_STRUCT *f);
extern void *get_file_file(uint32_t *addrblock,FILE_STRUCT *f);
extern FILE_STRUCT *getfileaddr(char *name);
void get_last_file(char *name1,FILE_STRUCT *f);
extern void nand_file_int(void);
extern int fs_get_md5(const char *filename, uint8_t md5[16]);
extern void write_file(FILE_STRUCT *f,uint8_t *buf,uint32_t offset,uint32_t len);
extern void read_file(FILE_STRUCT *f,uint8_t *buf,uint32_t offset,uint32_t len);
extern uint8_t copyfile(char *name1,char *name2);
#endif
