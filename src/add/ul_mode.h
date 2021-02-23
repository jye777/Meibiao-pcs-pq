#ifndef __UL_MODE_H
#define __UL_MODE_H

#include "ul_mode_control.h"
#include "micro.h"
typedef struct 
{
	volatile micro_set *pMs;
	ul_ExternalInputs *pRtu;
	ul_ExternalOutputs *pRty;
    int16_T pCmd;
    int16_T qCmd;
} ul_mode_opt_t; 

extern ul_mode_opt_t umo;
extern void ul_mode_task(const void *pdata);


#endif

