#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "cmsis_os.h"
#include "systime.h"
#include "fpga.h"
#include "pcs.h"
#include "para.h"
#include "modbus_master.h"
#include <time.h>
#include "tty.h"
#include "modbus_comm.h"
#include "log.h"
#include "micro.h"
#include "screen.h"
#include "sdata_alloc.h"

extern meter_data_t bms_data;
extern _MonitorData MonitorData;

void power_ctl_task(const void *pdata)
{	
	short pcs_state;	
	short pcs_state1;
	while(1)
	{
		pcs_state1 = fpga_read(STATE8_ADD);
		pcs_state = pcs_state1 & 0xf;

		//bms_data.discharge_pre_power = (short)((bms_data.discharge_pre_power_h*65535 + bms_data.discharge_pre_power_l)/10);
		//bms_data.charge_pre_power = (short)((bms_data.charge_pre_power_h*65535 + bms_data.charge_pre_power_l)/10);	
		//bms_data.bms_voltalge = (short)((bms_data.bms_voltalge_h*65535 + bms_data.bms_voltalge_l)/10);	
		//bms_data.bms_current= (short)((bms_data.bms_current_h*65535 + bms_data.bms_current_l)/10);	
		
		bms_data.discharge_pre_power = (short)((MonitorData.discharge_pre_power_h*65536 + MonitorData.discharge_pre_power_l)/10);
		bms_data.charge_pre_power = (short)((MonitorData.charge_pre_power_h*65536 + MonitorData.charge_pre_power_l)/10);
		
		bms_data.discharge_pre_power = GETMIN(bms_data.discharge_pre_power,5000);		
		bms_data.charge_pre_power = GETMIN(bms_data.charge_pre_power,5000);
		

				
		if(pcs_state==FSM_WORKING)
		{
			bms_data.discharge_pre_power = bms_data.discharge_pre_power;
			bms_data.charge_pre_power = bms_data.charge_pre_power;
		}
		else
		{
			bms_data.discharge_pre_power = 0;
			bms_data.charge_pre_power = 0;
		}
		
		//高温告警限制半功率
		if(miro_write.pcs_htemp_flag==1)
		{
			bms_data.discharge_pre_power = GETMIN(bms_data.discharge_pre_power,1000);
			bms_data.charge_pre_power = GETMIN(bms_data.charge_pre_power,1000);
		}
		else
		{
			bms_data.discharge_pre_power = bms_data.discharge_pre_power;
			bms_data.charge_pre_power = bms_data.charge_pre_power;
		}
		//bms_data.bms_voltalge = (short)((MonitorData.bms_voltalge_h*65536 + MonitorData.bms_voltalge_l)/10);	
		//bms_data.bms_current= (short)((MonitorData.bms_current_h*65536 + MonitorData.bms_current_l)/10);

		#if 0		
		if((bms_data.battery1_err != 0  || bms_data.battery2_err != 0 || bms_data.battery3_err != 0 || bms_data.battery4_err != 0 || 
			bms_data.battery5_err != 0 || bms_data.battery6_err != 0 || bms_data.battery7_err != 0 || bms_data.battery8_err != 0 ||
			bms_data.battery9_err != 0 || bms_data.battery10_err != 0) && (miro_write.bms_err_flag == 0))
		{
			fpga_write(Addr_Param26, 0);				   
			osDelay(20);
			set_pcs_shutdown();		   				   
			log_add(ET_OPSTATE,EST_BMS_ESTOP);
			miro_write.bootup_flag = 0;		
			miro_write.bms_err_flag = 1;
		}
		#endif
		
    	osDelay(200);
	}
}

