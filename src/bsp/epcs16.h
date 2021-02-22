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

#ifndef __EPCS16_H__
#define __EPCS16_H__

#include <stdint.h>


void EpcsSpi_Init(void);

void EpcsSpi_Close(void);
void EpcsSpi_Reset(void);

int EpcsSpi_WriteRead(uint8_t* pWriteBuff,uint32_t writeLength,
                            uint8_t* pReadBuff,uint32_t readLength);
int EpcsSpi_ChipSelect(void);
int EpcsSpi_ChipDeselect(void);


#endif /* EPCS_H */

