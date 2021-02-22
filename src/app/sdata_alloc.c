#include "storedata.h"
#include "fpga.h"
#include "para.h"
#include "env.h"

volatile float *nvrom_pw_info;
volatile unsigned int *nvrom_pw_time_info;
volatile float *count_cal;
volatile short *arm_config_nand_data;
fpga_data_buffer *fpga_pdatabuf_nand;
fpga_zero *fpga_pzero;
extern fpga_data_buffer *fpga_pdatabuf_flash;
extern ub_env *env_cache;

short fpga_pdatabuf_read(unsigned short id)
{
//    if((id <= Addr_Param22) || ((id >= Addr_Param41) && (id < Addr_Param120)))
//    {
//        return *(&(fpga_pdatabuf_flash->set[0]) + id);
//    }
//		else if(((id >= Addr_Param23) && (id <= Addr_Param25)) || ((id >= Addr_Param28) && (id <= Addr_Param40)))
//		{
        return *(&(fpga_pdatabuf_nand->set[0]) + id);
//		}
//		return 0;
}

short fpga_pdatabuf_write(uint16_t id, short value)
{
//    if((id <= Addr_Param22) || ((id >= Addr_Param41) && (id < Addr_Param120)))
//    {
//        *(&(fpga_pdatabuf_flash->set[0]) + id) = value;
//        return 1;
//    }
//		else if(((id >= Addr_Param23) && (id <= Addr_Param25)) || ((id >= Addr_Param28) && (id <= Addr_Param40)))
//		{
//        *(&(fpga_pdatabuf_nand->set[0]) + id) = value;
//        return 1;
//		}
//    return 0;
	//test
	if(id <= Addr_Param255)
	{
		*(&(fpga_pdatabuf_nand->set[0]) + id) = value;
		return 1;
	}
	return 0;
}

void count_cal_write(short id, float value)
{
    count_cal[id] = value;
}

float count_cal_read(short id)
{
    return count_cal[id] / MODULUS_COUNT_CAL;
}

void arm_config_data_write(unsigned short id, short value)
{
//    if(id < 200)
//		{
//				arm_config_flash_data[id] = value;
//		}
//		else
//		{
				arm_config_nand_data[id] = value;
//		}
}

short arm_config_data_read(unsigned short id)
{
//    if(id < 200)
//		{
//				return arm_config_flash_data[id];
//		}
//		else
//		{
				return arm_config_nand_data[id];
//		}
}

uint32_t arm_config_data_read32(unsigned short id)
{
//    if(id < 200)
//		{
//				return *((uint32_t *)(&(arm_config_flash_data[id])));
//		}
//		else
//		{
				return *((uint32_t *)(&(arm_config_nand_data[id])));
//		}
}

void sdata_alloc_all(void)
{
    nvrom_pw_info = (float *)get_sdata_addr(PW_BASE);
    nvrom_pw_time_info = (unsigned int *)get_sdata_addr(PW_TIME_BASE);
    count_cal = (float *)get_sdata_addr(COUNT_CAL_NAND_BASE);
    arm_config_nand_data = (short *)get_sdata_addr(CONFIG_NAND_BASE);
    fpga_pdatabuf_nand = (fpga_data_buffer *)get_sdata_addr(FPGA_NAND_DATA_BASE);
    fpga_pzero = (fpga_zero *)get_sdata_addr(FPGA_ZERO_BASE);
    env_cache = (ub_env *)get_sdata_addr(ENV_NAND_BASE);	
}

