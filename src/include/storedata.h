/**
 * @file			storedata.h
 * @brief			
 * @author			wangpeng
 * @date			2016/6/2
 * @version			Initial Draft
 * @par				Copyright (C),  2013-2023, SCNEE
 * @par History:
 * 1.Date: 			2016/6/2
 *   Author: 			wangpeng
 *   Modification: 		Created file
*/

#ifndef STOREDATA_H
#define STOREDATA_H

#include <stdint.h>

#include "sdata_alloc.h"

void sdata_init(void);
void sdata_update(void);
uint32_t get_sdata_addr(uint32_t offset);
extern void save_sdata(void);

#endif /* STOREDATA_H */

