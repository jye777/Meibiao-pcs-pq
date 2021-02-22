/**************************************************************************
**  Copyright (c) 2017 Clou, Ltd.
**
**  File:    custom.h
**  Author:  sk
**  Date:    2017/03/30
**  Purpose:
**    ���ƹ���ͷ�ļ�
**************************************************************************/

#ifndef __CUSTOM_H
#define __CUSTOM_H
#include <cmsis_os.h>


typedef struct
{
	unsigned char svg_en;
	unsigned char get_Pset_cmd;
	unsigned char get_Qset_cmd;
	unsigned short Uac_aver; //�Ŵ�MODULUS��
	unsigned short Freq_aver; //�Ŵ�MODULUS��
} svg_info_t;


typedef struct
{
	unsigned char mode_en;
	unsigned char chrg_disc_state; //���״̬: 0-������� 1-ĩ�ڳ�� 2-ĩ�ڷŵ�
	unsigned short chrg_vol_limit;
	unsigned short disc_vol_limit;
	unsigned char work_flag;
	unsigned char work_cnt;
} chrg_disc_info_t;

typedef struct
{
	unsigned char ol1_flag; //���ر�־
	unsigned char ol2_flag; //�������ر�־	
	unsigned char ol3_flag; //˲ʱ���ر�־

	unsigned short ol1_tick;	
	unsigned short ol2_tick;
	
	unsigned char oc_flag; //������־
	unsigned char timer_level; //��ʱ���ȼ�: 0-δ���� 1-���ؿ��� 2-��������
} ol_shutdown_info_t;


extern svg_info_t svg_info;
extern chrg_disc_info_t chrg_disc_info;
void SVGModeEn_task(const void* pdata);
void ChrgDiscSwitch_task(const void* pdata);
void OverLoadShutdown_task(const void* pdata);
void custom_task(const void* pdata);

void Frequency_task(const void* pdata);


#endif 

