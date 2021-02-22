/**
 * @file			epcs.h
 * @brief			
 * @author			wangpeng
 * @date			2016/11/11
 * @version			Initial Draft
 * @par				Copyright (C),  2013-2023, SCNEE
 * @par History:
 * 1.Date: 			2016/11/11
 *   Author: 			wangpeng
 *   Modification: 		Created file
*/

#ifndef EPCS_H
#define EPCS_H

#include <stdint.h>

#define POF_HEAD_LENGTH	155													//POF_LOAD
#define	POF_TAIL_LENGTH 52
#define	POF_CODE_LENGTH	0x200000 //2M
#define POF_LENGTH (POF_CODE_LENGTH + POF_HEAD_LENGTH + POF_TAIL_LENGTH)

#define EPCS1_SILICON_ID				0x10
#define EPCS4_SILICON_ID				0x12
#define EPCS16_SILICON_ID				0x14
#define EPCS64_SILICON_ID				0x15

extern void epcs_init(void);
extern void epcs_close(void);
extern void epcs_Reset(void);
extern int epcs_erase_bulk(void);
extern int epcs_erase_sector(uint32_t addr);
extern int epcs_need_erase(uint32_t addr);
extern int epcs_read(uint8_t *buf, uint32_t addr, uint32_t len);
extern int epcs_read_bytes(uint8_t *buf, uint32_t addr, uint32_t size);
extern int epcs_read_silicon_id(uint8_t *id);
extern int epcs_read_status(uint8_t *status);
extern int epcs_transdata(uint8_t *buf, uint32_t len);
extern int epcs_write(uint8_t *buf, uint32_t addr, uint32_t len);
extern int epcs_write_bytes(uint8_t *buf, uint32_t addr, uint32_t size);
extern int epcs_write_disable(void);
extern int epcs_write_enable(void);
extern int epcs_write_status(uint8_t status);
extern int epcs_get_md5(uint32_t codesize, uint8_t md5[16]);
extern void spi_callback(uint32_t event);
extern int spi_write_and_read(uint8_t *rbuf, uint8_t *wbuf, uint32_t len);


#endif /* EPCS_H */

