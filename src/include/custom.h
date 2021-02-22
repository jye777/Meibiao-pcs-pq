/**************************************************************************
**  Copyright (c) 2017 Clou, Ltd.
**
**  File:    custom.h
**  Author:  sk
**  Date:    2017/03/30
**  Purpose:
**    定制功能头文件
**************************************************************************/

#ifndef __CUSTOM_H
#define __CUSTOM_H
#include <cmsis_os.h>


typedef struct
{
	unsigned char svg_en;
	unsigned char get_Pset_cmd;
	unsigned char get_Qset_cmd;
	unsigned short Uac_aver; //放大MODULUS倍
	unsigned short Freq_aver; //放大MODULUS倍
} svg_info_t;


typedef struct
{
	unsigned char mode_en;
	unsigned char chrg_disc_state; //充放状态: 0-正常充放 1-末期充电 2-末期放电
	unsigned short chrg_vol_limit;
	unsigned short disc_vol_limit;
	unsigned char work_flag;
	unsigned char work_cnt;
} chrg_disc_info_t;

typedef struct
{
	unsigned char ol1_flag; //过载标志
	unsigned char ol2_flag; //二级过载标志	
	unsigned char ol3_flag; //瞬时过载标志

	unsigned short ol1_tick;	
	unsigned short ol2_tick;
	
	unsigned char oc_flag; //过流标志
	unsigned char timer_level; //定时器等级: 0-未开启 1-过载开启 2-过流开启
} ol_shutdown_info_t;


extern svg_info_t svg_info;
extern chrg_disc_info_t chrg_disc_info;
void SVGModeEn_task(const void* pdata);
void ChrgDiscSwitch_task(const void* pdata);
void OverLoadShutdown_task(const void* pdata);
void custom_task(const void* pdata);

void Frequency_task(const void* pdata);


#endif 

