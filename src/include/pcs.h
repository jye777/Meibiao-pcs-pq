#ifndef PCS_H
#define PCS_H

#define PERIOD_NUM_MAX	(24) //尖峰平谷时间段最大数量

typedef struct
{
	uint8_t start_time_h; //开始时间_小时
	uint8_t start_time_m; //开始时间_分钟
	uint8_t stop_time_h; //结束时间_小时
	uint8_t stop_time_m; //结束时间_分钟
	int16_t set_power; //充放电功率
	int16_t res;
} period_info_t;

typedef struct
{
	period_info_t period_info[PERIOD_NUM_MAX];
	short capacity; //变压器容量
	//short chrg_power_limit; //充电功率限制
	//short disc_power_limit; //放电功率限制
	short xftg_en; //削峰填谷策略使能
	
    uint8_t charge_start_hour;
    uint8_t charge_start_min;
    uint8_t charge_stop_hour;
    uint8_t charge_stop_min;

    uint8_t charge_start_hour2;
    uint8_t charge_start_min2;
    uint8_t charge_stop_hour2;
    uint8_t charge_stop_min2;	

    uint8_t charge_start_hour3;
    uint8_t charge_start_min3;
    uint8_t charge_stop_hour3;
    uint8_t charge_stop_min3;

    uint8_t charge_start_hour4;
    uint8_t charge_start_min4;
    uint8_t charge_stop_hour4;
    uint8_t charge_stop_min4;	

	
    uint8_t discharge_start_hour;
    uint8_t discharge_start_min;
    uint8_t discharge_stop_hour;
    uint8_t discharge_stop_min;

	uint8_t discharge_start_hour2;
    uint8_t discharge_start_min2;
    uint8_t discharge_stop_hour2;
    uint8_t discharge_stop_min2;

	uint8_t discharge_start_hour3;
    uint8_t discharge_start_min3;
    uint8_t discharge_stop_hour3;
    uint8_t discharge_stop_min3;

	uint8_t discharge_start_hour4;
    uint8_t discharge_start_min4;
    uint8_t discharge_stop_hour4;
    uint8_t discharge_stop_min4;

	int16_t set_power;
	int16_t set_power2;
	int16_t set_power3;
	int16_t set_power4;

	uint8_t meter1_a0;	
	uint8_t meter1_a1;
	uint8_t meter1_a2;
	uint8_t meter1_a3;
	uint8_t meter1_a4;
	uint8_t meter1_a5;

	uint8_t meter2_a0;	
	uint8_t meter2_a1;
	uint8_t meter2_a2;
	uint8_t meter2_a3;
	uint8_t meter2_a4;
	uint8_t meter2_a5;

	int16_t meter1_vrc;
	int16_t	meter2_vrc;

	int16_t grid_min_power;
	

	int16_t soc;
	int16_t charge_power_lmt;
	int16_t discharge_power_lmt;
	int16_t life;
	
	int16_t charge_Lmtpower_old;
	int16_t charge_power_lmt_old;
	int16_t discharge_power_lmt_old;	
    //int16_t hmi_set_power_enable;	
    
    int16_t cos_seta_value;
    int16_t time1_power;
	int16_t time2_power;
    int16_t time3_power;
	int16_t time4_power;    
	int16_t time5_power;
	int16_t time6_power;    
	int16_t time7_power;
	int16_t time8_power;

	
	short bms_life_count_old;		
	short bms_life_count_old1;	
	
	short bms_life_count;
	unsigned char off_line_flag;
	unsigned char off_line_flag_old;

	short modbus_collect_flag;
	short modbus_err_tick;

	short modbus_collect_flag1;
	short modbus_err_tick1;	
	unsigned char meter_off_line_flag;
	unsigned char meter_off_line_flag_old;
	
	short temp_a_max;
	short temp_b_max;
	short temp_c_max;

	short temp_a1_ad;
	short temp_a2_ad;
	short temp_a3_ad;
	short temp_a4_ad;
	short temp_a[4];
	uint16_t temp_compare_a;
	
	short temp_b1_ad;
	short temp_b2_ad;
	short temp_b3_ad;
	short temp_b4_ad;
	short temp_b[4];
	uint16_t temp_compare_b;
	
	short temp_c1_ad;
	short temp_c2_ad;
	short temp_c3_ad;
	short temp_c4_ad;
	short temp_c[4];
	uint16_t temp_compare_c;	
	
	float w_32;
}pcs_manage_t;


extern pcs_manage_t pcs_manage;
void power_ctl_task(const void *pdata);
#endif



