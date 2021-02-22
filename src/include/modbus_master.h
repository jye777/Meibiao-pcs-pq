#ifndef MODBUS_MASTER_H
#define MODBUS_MASTER_H
#include "stdint.h"

typedef struct
{
	int32_t Ia;
	int32_t Ib;
	int32_t Ic;
	uint32_t Uab;
	uint32_t Ubc;
	uint32_t Uca;
	int32_t P;
	int32_t Q;
	uint32_t S;
	uint32_t posEnerTot;
	uint32_t negEnerTot;
}meter_data_t;

typedef struct
{
	short ac_cabinet_temp; //交流柜温度
	short filter_capacitance_temp1; //电容柜温度
	short filter_capacitance_temp2; //电容柜温度	
	short transformer_temp; //控制变压器温度
	short bus_capacitance_temp; //母线电容温度	
	short electric_reactor_temp; //电抗器温度
	unsigned short  ac_fun_run_flag; //风机运行标志
}collect_board_data_t;

extern collect_board_data_t collect_data;

extern meter_data_t meter0_data;
extern meter_data_t meter1_data;
extern short load_power;


void modbus_master_task(const void *pdata);
void modbus_master_power_task(const void *pdata);

#endif /* MODBUS_MASTER_H */


