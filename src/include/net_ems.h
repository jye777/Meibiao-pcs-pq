/**
 * @file			net_ems.h
 * @brief			
 * @author			wangpeng
 * @date			2016/8/13
 * @version			Initial Draft
 * @par				Copyright (C),  2013-2023, SCNEE
 * @par History:
 * 1.Date: 			2016/8/13
 *   Author: 			wangpeng
 *   Modification: 		Created file
*/

#ifndef NET_EMS_H
#define NET_EMS_H

#include "cmsis_os.h"
typedef struct 
{
		int mbsd;
		osThreadId tid_task;		
}NET_TASK_TYPE;


void net_ems_task(const void *pdata);
void SetParamForaAll(unsigned short addr_tmp, short value);

#endif /* NET_EMS_H */

