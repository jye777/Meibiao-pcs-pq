/*****************************************************************
 * 美标：软启动模块
*****************************************************************/

#include "soft_bootup.h"
#include "tty.h"
#include "fpga.h"
#include "para.h"
#include <stdlib.h>

extern short setting_data_handle(short addr, short value);
soft_bootup_t softBootup;


#if (TEST_MODE == 0) 
#define SB_TASK_PERIOD	50
#else
#define SB_TASK_PERIOD	500
#endif
void soft_bootup_task(const void *pdata)
{
	soft_bootup_t *pSb = (soft_bootup_t *)pdata;
	int32_t goalPwrFpga, sumPwrFpga = 0, stepPwrFpga; //x1000
	uint8_t cpltFlag = 0;
	
	while(1)
	{
#if (TEST_MODE == 0) 
		if((fpga_read(STATE8_ADD) & 0xF) != FSM_WORKING) {
			cpltFlag = 0;
			sumPwrFpga = 0;
			osDelay(50);
			continue;
		}
#endif		
		if(pSb->sbEnble == 0) {
			cpltFlag = 0;
			sumPwrFpga = 0;
			osDelay(50);
			continue;
		}
		if(cpltFlag == 0) {
			goalPwrFpga = setting_data_handle(Addr_Param26, pSb->sbGoalPwr) * 1000;
			stepPwrFpga = (int32_t)((float)goalPwrFpga / pSb->sbTime * 10 / (1000.0f / SB_TASK_PERIOD));
			sumPwrFpga += stepPwrFpga;
			printf_debug4("goalPwrFpga[%d] stepPwrFpga[%d] sumPwrFpga[%d]\n", goalPwrFpga, stepPwrFpga, sumPwrFpga);
			if(abs(sumPwrFpga) > abs(goalPwrFpga) && abs(sumPwrFpga) < abs(goalPwrFpga + stepPwrFpga)) {
				sumPwrFpga = goalPwrFpga;
			}
			if(abs(sumPwrFpga) >= abs(goalPwrFpga + stepPwrFpga)) {
				cpltFlag = 1;
			}
			printf_debug4("cpltFlag[%d] sumPwrFpga[%d]\n\n", cpltFlag, sumPwrFpga);
			if(cpltFlag == 0) {
				fpga_write(Addr_Param26, (int16_t)(sumPwrFpga / 1000));
			}
		}
		
		osDelay(SB_TASK_PERIOD);
	}
}



