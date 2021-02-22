/**
 * @file			led.h
 * @brief			
 * @author			wangpeng
 * @date			2016/4/19
 * @version			Initial Draft
 * @par				Copyright (C),  2013-2023, SCNEE
 * @par History:
 * 1.Date: 			2016/4/19
 *   Author: 			wangpeng
 *   Modification: 		Created file
*/

#ifndef LED_H
#define LED_H

uint32_t LED_GetCount (void);
int32_t LED_Initialize (void);
int32_t LED_Off (uint32_t num);
int32_t LED_On (uint32_t num);
int32_t LED_Flicker (uint32_t num);
int32_t LED_SetOut (uint32_t val);
int32_t LED_Uninitialize (void);

#endif /* LED_H */

