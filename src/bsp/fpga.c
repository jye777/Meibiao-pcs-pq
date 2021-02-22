#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cpu.h"
#include "cmsis_os.h"
#include "rl_net.h"
#include "bsp.h"
#include "fpga.h"
#include "systime.h"
#include "env.h"
#include "modbus_comm.h"
#include "log.h"
#include "para.h"
#include "record_wave.h"
#include "micro.h"
#include "storedata.h"
#include "crc.h"
#include "pcs.h"
#include "net_ems.h"
#include "tty.h"
#include "sys.h"
#include "screen.h"
#include "modbus_master.h"
#include "para.h"
//#include "feedback_control.h"
#include "freq_prot.h"
#include "soft_bootup.h"
#include "fault_rec.h"
#include "comm_offline.h"

#define NTC_SAMPLE
//#define PT100_SAMPLE

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc3;

extern meter_data_t bms_data;
extern _MonitorData MonitorData;
//extern collect_board_data_t collect_data;


volatile uint16_t *fpga = (uint16_t *)0x6c000000;
static dead_time dead;
//static fault_recover_info_t fault_recover_info;
static fault_record_info_t fr_info;
static switch_record_info_t sr_info;
static cmd_record_info_t cmd_info;
static strategy_record_info_t str_info;

//static opsstate_record_info_t or_info;
static uint16_t state8;
static uint16_t ad1_dma_buf[7];
static uint16_t ad3_dma_buf[7];

//osThreadDef(clear_onoff_task, osPriorityNormal, 1, 0);
//osTimerDef(auto_recover_timer, (os_ptimer)auto_recover_func);

#ifdef NTC_SAMPLE
/* NTC温度查找表 */
const temp_ad_t temp_tab[] =
{
    {-40, 85},
    {-35, 117},
    {-30, 160},
    {-25, 215},
    {-20, 284},
    {-15, 370},
    {-10, 475},
    {-5,  601},
    {0,   747},
    {5,   915},
    {10,  1102},
    {15,  1305},
    {20,  1520},
    {25,  1742},
    {30,  1966},
    {35,  2187},
    {40,  2400},
    {45,  2601},
    {50,  2787},
    {55,  2958},
    {60,  3112},
    {65,  3249},
    {70,  3371},
    {75,  3477},
    {80,  3571},
    {85,  3652},
    {90,  3722},
    {95,  3783},
    {100, 3835},
	{105, 3880},
};
#define TEMP_TAB_SIZE      (sizeof(temp_tab) / sizeof(temp_tab[0]))


/* AD值转温度_NTC */
static int16_t v2temp_ntc(uint16_t ad, int *err)
{
    int i;
    int16_t temp;
    int16_t t_a;
    uint16_t ad_a, ad_b;

    if(ad < temp_tab[0].ad || ad > temp_tab[TEMP_TAB_SIZE - 1].ad) 
	{
        *err = 1;
        return 0;
    } 
	else 
	{
        *err = 0;
    }

    if(ad <= temp_tab[0].ad) 
	{
        return temp_tab[0].temp;
    }
	
    if(ad >= temp_tab[TEMP_TAB_SIZE - 1].ad) 
	{
        return temp_tab[TEMP_TAB_SIZE - 1].temp;
    }

    for(i = 0; i < TEMP_TAB_SIZE - 1; i++) 
	{
        t_a = temp_tab[i].temp;
        ad_a = temp_tab[i].ad;
        ad_b = temp_tab[i + 1].ad;

        if(ad >= ad_a && ad < ad_b) 
		{
            temp = t_a + (ad_b - ad) * 5.0f / (ad_b - ad_a) + 0.5f;
            return temp;
        }
    }
    return 0;
}


/* 温度转AD值_NTC */
static uint16_t temp2v_ntc(int16_t t, int *err)
{
    int i;
    uint16_t ad;
    int16_t t_a, t_b;
    uint16_t ad_a, ad_b;

    if(t > 120 || t < -50) {
        *err = 1;
        return 0;
    } else {
        *err = 0;
    }

    if(t <= temp_tab[0].temp) {
        return temp_tab[0].ad;
    }

    if(t >= temp_tab[TEMP_TAB_SIZE - 1].temp) {
        return temp_tab[TEMP_TAB_SIZE - 1].ad;
    }

    for(i = 0; i < TEMP_TAB_SIZE - 1; i++) {
        t_a = temp_tab[i].temp;
        t_b = temp_tab[i + 1].temp;
        ad_a = temp_tab[i].ad;
        ad_b = temp_tab[i + 1].ad;

        if(t >= t_a && t < t_b) {
            ad = ad_a - (t_a - t) * (ad_a - ad_b) / (t_a - t_b);
            return ad;
        }
    }

    return 0;
}

#else

/* PT100温度查找表 */
const temp_ad_t temp_pt100_tab[] =
{
    {0,   786},
    {5,   944},
    {10,  1101},
    {15,  1258},
    {20,  1416},
    {25,  1573},
    {30,  1730},
    {35,  1887},
    {40,  2045},
    {45,  2202},
    {50,  2359},
    {55,  2517},
    {60,  2674},
    {65,  2831},
    {70,  2988},
    {75,  3146},
    {80,  3303},
    {85,  3460},
    {90,  3618},
    {95,  3775},
    {100, 3932},
};
#define TEMP_PT100_TAB_SIZE      (sizeof(temp_pt100_tab) / sizeof(temp_pt100_tab[0]))

/* AD值转温度_PT100 */
static int16_t v2temp_pt100(uint16_t ad, int *err)
{
    int i;
    int16_t temp;
    int16_t t_a;
    uint16_t ad_a, ad_b;

    if(ad > temp_pt100_tab[TEMP_PT100_TAB_SIZE - 1].temp || ad < temp_pt100_tab[0].temp) {
        *err = 1;
        return 0;
    } else {
        *err = 0;
    }

    if(ad <= temp_pt100_tab[0].ad) {
        return temp_pt100_tab[0].temp;
    }

    if(ad >= temp_pt100_tab[TEMP_PT100_TAB_SIZE - 1].ad) {
        return temp_pt100_tab[TEMP_PT100_TAB_SIZE - 1].temp;
    }

    for(i = 0; i < TEMP_PT100_TAB_SIZE - 1; i++) {
        t_a = temp_pt100_tab[i].temp;
        ad_a = temp_pt100_tab[i].ad;
        ad_b = temp_pt100_tab[i + 1].ad;

        if(ad >= ad_a && ad < ad_b) {
            temp = t_a + (ad_a - ad) * 5.0 / (ad_a - ad_b) + 0.5;
            return temp;
        }
    }
    return 0;
}
#endif

/*温度冗余计算20180102*/
#if 1
int16_t v2tmp(uint16_t ad, int *err)
{
    int i;
    int16_t temp;
    int16_t t_a;
    uint16_t ad_a, ad_b;

    if(ad > 4070 || ad < 100)
    {
        *err = 1;
        return 0;
    }
    else
    {
        *err = 0;
    }
    
    if(ad >= temp_tab[0].ad)
    {
        return temp_tab[0].temp;
    }

    if(ad <= temp_tab[TEMP_TAB_SIZE - 1].ad)
    {
        return temp_tab[TEMP_TAB_SIZE - 1].temp;
    }

    for(i = 0; i < TEMP_TAB_SIZE - 1; i++)
    {
        t_a = temp_tab[i].temp;
        ad_a = temp_tab[i].ad;
        ad_b = temp_tab[i + 1].ad;
        
        if(ad < ad_a && ad >= ad_b)
        {
            temp = t_a + (ad_a - ad) * 5.0 / (ad_a - ad_b) + 0.5;
            return temp;
        }
    }

    return 0;
}

static void find_max_min(uint16_t ad_value,uint16_t *max,uint16_t *min)
{	
		if(ad_value > *max)
		{
				*max = ad_value;
		}
		if(ad_value < *min)
		{
				*min = ad_value;
		}
}

#define TEMP_ERR_Threshold 10
static void find_err_and_correct(float avg_temp,uint16_t ad_value1,uint16_t ad_value2)
{
		int err1,err2;
		float dif1 = 0 ,dif2 = 0;
		uint16_t value1,value2;
	
		value1 = v2tmp(ad_value1,&err1);
		value2 = v2tmp(ad_value2,&err2);
	
		if(err1 && (!err2))
		{
				ad_value1 = ad_value2;
				return;
		}
		else if((!err1) && err2)
		{
				ad_value2 = ad_value1;
				return;
		}
		else if(err1 && err2)
		{
				return;		
		}
		
		dif1 = fabs(avg_temp - value1);
		dif2 = fabs(avg_temp - value2);		

		if((dif1 > TEMP_ERR_Threshold) && (dif2 < TEMP_ERR_Threshold))
		{
				ad_value1 = ad_value2;
		}
		else if((dif2 > TEMP_ERR_Threshold) && (dif1 < TEMP_ERR_Threshold))
		{
				ad_value2 = ad_value1;
		}
}

//温度计算用
float getThermistorResistance(float voltage)
/* 传入测试点电压获得当前热敏电阻阻值 */
{
    /* 采样的是放大电路放大后的电压,所以需要还原回去 */
    float Vtemp=voltage/(8*(20/10));
    float R1,R2,Rt=0;
    R1= 6800;//欧姆
    R2= 220;//欧姆
    Rt=(5*R2)/Vtemp - R1 -R2;
    return Rt;
}

float V2T_GetKelvinTemperature(float voltage)
/* 根据电压值获得当前开式温度值 */
{
    float temperature=0;
    float Rt=0;
    float Ka=273.15;
    //T2常温下的标称阻值.
    float Rp=10000;
    //T2为25摄氏度，折算为开尔文温度
    float T2=Ka+25;
    float Bx=3950;
    
    Rt=getThermistorResistance(voltage);
    //temperature=1/(1/T2+log(Rt/Rp)/Bx);
    
    //折算成摄氏温度
    temperature=1/(1/T2+log(Rt/Rp)/Bx)-Ka; 
	temperature = temperature*10; //实际值放大10倍给出
    return temperature;
}


static void ad_check_err_a(void)
{
	float sum,avg;
	uint16_t i=0;

	//采样温度排序
	pcs_manage.temp_a[0] = pcs_manage.temp_a1_ad;
	pcs_manage.temp_a[1] = pcs_manage.temp_a2_ad;
	pcs_manage.temp_a[2] = pcs_manage.temp_a3_ad;
	pcs_manage.temp_a[3] = pcs_manage.temp_a4_ad;
		
	for(i=0;i<3;i++)
	{
		if(pcs_manage.temp_a[i] > pcs_manage.temp_a[i+1])
		{
			pcs_manage.temp_compare_a= pcs_manage.temp_a[i] ;
			pcs_manage.temp_a[i] = pcs_manage.temp_a[i+1];		
			pcs_manage.temp_a[i+1] = pcs_manage.temp_compare_a;
		}
	}
		
	sum = pcs_manage.temp_a1_ad + pcs_manage.temp_a2_ad + pcs_manage.temp_a3_ad 
		+ pcs_manage.temp_a4_ad - pcs_manage.temp_a[1] - pcs_manage.temp_a[3];
		
	avg = (float)sum / 2;	

	pcs_manage.temp_a_max = V2T_GetKelvinTemperature(pcs_manage.temp_a1_ad*(2.5)/0x0FFF);
	//fpga_write(Addr_Param220,avg);
	fpga_write(Addr_Param220,pcs_manage.temp_a[0]);
}

static void ad_check_err_b(void)
{
	float sum,avg;
	uint16_t i=0;

	//采样温度排序
	pcs_manage.temp_b[0] = pcs_manage.temp_b1_ad;
	pcs_manage.temp_b[1] = pcs_manage.temp_b2_ad;
	pcs_manage.temp_b[2] = pcs_manage.temp_b3_ad;
	pcs_manage.temp_b[3] = pcs_manage.temp_b4_ad;
		
	for(i=0;i<3;i++)
	{
		if(pcs_manage.temp_b[i] > pcs_manage.temp_b[i+1])
		{
			pcs_manage.temp_compare_b = pcs_manage.temp_b[i] ;
			pcs_manage.temp_b[i] = pcs_manage.temp_b[i+1];		
			pcs_manage.temp_b[i+1] = pcs_manage.temp_compare_b;
		}
	}
		
	sum = pcs_manage.temp_b1_ad + pcs_manage.temp_b2_ad + pcs_manage.temp_b3_ad 
		+ pcs_manage.temp_b4_ad - pcs_manage.temp_b[1] - pcs_manage.temp_b[3];
		
	avg = (float)sum / 2;		
	pcs_manage.temp_b_max = V2T_GetKelvinTemperature(pcs_manage.temp_b[3]*(2.5)/0x0FFF);
	//fpga_write(Addr_Param221,avg);
	fpga_write(Addr_Param221,pcs_manage.temp_b[0]);
}


static void ad_check_err_c(void)
{
	float sum,avg;
	uint16_t i=0;

	//采样温度排序
	pcs_manage.temp_c[0] = pcs_manage.temp_c1_ad;
	pcs_manage.temp_c[1] = pcs_manage.temp_c2_ad;
	pcs_manage.temp_c[2] = pcs_manage.temp_c3_ad;
	pcs_manage.temp_c[3] = pcs_manage.temp_c4_ad;
		
	for(i=0;i<3;i++)
	{
		if(pcs_manage.temp_c[i] > pcs_manage.temp_c[i+1])
		{
			pcs_manage.temp_compare_c = pcs_manage.temp_c[i] ;
			pcs_manage.temp_c[i] = pcs_manage.temp_c[i+1];		
			pcs_manage.temp_c[i+1] = pcs_manage.temp_compare_c;
		}
	}
		
	sum = pcs_manage.temp_c1_ad + pcs_manage.temp_c2_ad + pcs_manage.temp_c3_ad 
		+ pcs_manage.temp_c4_ad - pcs_manage.temp_c[1] - pcs_manage.temp_c[3];
		
	avg = (float)sum / 2;		
	pcs_manage.temp_c_max = V2T_GetKelvinTemperature(pcs_manage.temp_c[3]*(2.5)/0x0FFF);
	//fpga_write(Addr_Param222,avg);
	fpga_write(Addr_Param222,pcs_manage.temp_c[0]);
}

#endif

/*
	A: ad1[236] ad3[6]
	B: ad1[45] ad2[45]
	C: ad3[0-3]
*/


#define AD_MAX(a,b) (a <= b ? a : b) //负温度系数, 反的
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	
	if(hadc == &hadc1)
    {
        //fpga_write(Addr_Param220, ad1_dma_buf[4]);
        pcs_manage.temp_a1_ad = ad1_dma_buf[2];
        pcs_manage.temp_a2_ad = ad1_dma_buf[3];
        pcs_manage.temp_b1_ad = ad1_dma_buf[4]; //无c相3温度点采集值用临近值替代
        pcs_manage.temp_b2_ad = ad1_dma_buf[5];
        pcs_manage.temp_a3_ad = ad1_dma_buf[6];
    }
    else if(hadc == &hadc3)
    {
        pcs_manage.temp_c1_ad = ad3_dma_buf[0];
        pcs_manage.temp_c2_ad = ad3_dma_buf[1];
        pcs_manage.temp_c3_ad = ad3_dma_buf[2]; //无c相3温度点采集值用临近值替代
        pcs_manage.temp_c4_ad = ad3_dma_buf[3];
        pcs_manage.temp_b3_ad = ad3_dma_buf[4];
        pcs_manage.temp_b4_ad = ad3_dma_buf[5];  
		pcs_manage.temp_a4_ad = ad3_dma_buf[6];     
    }
	
	ad_check_err_a();        //温度冗余计算20180102
	ad_check_err_b();        //温度冗余计算20180102
	ad_check_err_c();        //温度冗余计算20180102

	
}

static void ad_start(void)
{
    static uint32_t t = 0;

    if(t == 0) 
	{
        HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ad1_dma_buf, 7);
        HAL_ADC_Start_DMA(&hadc3, (uint32_t *)ad3_dma_buf, 7);
    }

    if(++t > 12) 
	{
        t = 0;
    }
}


static void udelay(int x)
{
    int i;
    for(i = 0; i < 2 * x; i++);
}


void f_fpga_write(uint32_t addr, uint16_t v)
{
    udelay(10);
    fpga[addr] = v;
    udelay(10);
}

uint16_t f_fpga_read(uint32_t addr)
{
    uint16_t value;
    udelay(10);
    value=fpga[addr];
    udelay(10);
    return value;
}


static void fpga_state_init(void)
{
//    setpara(MP_DEVICE_TYPE, DEVICE_NO);
}

static void fpga_data_init(void)
{
    int i;
	char test_flag = 0;
	short temp;

    for(i = 0; i < 220; i++) 
	{
        fpga_write(i, fpga_pdatabuf_read(i));
		if(test_flag == 0)
		{
			temp = 	fpga_read(i);
			if(temp != fpga_pdatabuf_read(i))
			{
				setenv("fpga write err","1");
				test_flag = 1;
			}
		}
        setpara(i, fpga_pdatabuf_read(i));
    }

    fpga_write(100, 5000); //波形采样频率设置
    setpara(100, 5000);

    for(i = 0; i < 512; i++) 
	{
        fpga_pdatabuf_get[i] = 0;
    }

    dead.dead_time = 1;
}


static void fpga_zero_init(void)
{
    int i;
    unsigned short crc;

    crc = crc16((unsigned char *)&fpga_pzero->zero[0], sizeof(fpga_pzero->zero));

    if(fpga_pzero->zero_check == crc) 
		{
        for(i = 0; i < ZERO_NUM; i++) 
		{
            fpga_write((Addr_Param237 + i), fpga_pzero->zero[i]);
        }
    } 
	else 
	{
        printf("fpga_zero_init() error\n");
    }
}


void fpga_init(void)
{
    fpga_data_init();
    fpga_zero_init();
    fpga_state_init();
}


int zero_adjust(unsigned short cmd)
{
    int i;
    short value;

    switch(cmd)
    {
    case 1: //校零
        for(i = 0; i < ZERO_NUM; i++) 
		{
            fpga_pzero->zero[i] = (short)fpga_read(Addr_Param307 + i);
        }

        fpga_pzero->zero_check = crc16((unsigned char *)&fpga_pzero->zero[0], sizeof(fpga_pzero->zero));

        for(i = 0; i < ZERO_NUM; i++) 
		{
            fpga_write((Addr_Param237+ i), fpga_pzero->zero[i]);
            readpara(Addr_Param237 + i, &value);
            setpara(Addr_Param237 + i, value);
        }
        break;



    case 2: //取消校零
        for(i = 0; i < ZERO_NUM; i++) 
		{
            fpga_pzero->zero[i] = 0;
            fpga_write((Addr_Param237 + i), fpga_pzero->zero[i]);
            setpara(Addr_Param237 + i, fpga_pzero->zero[i]);
        }
        fpga_pzero->zero_check = crc16((unsigned char *)&fpga_pzero->zero[0], sizeof(fpga_pzero->zero));
        break;

    default:
        break;
    }

    return 0;
}



void Lifetask(void *pdata)
{
    pdata = pdata;

    while(1)
    {
        dead.life_time += 1;
        fpga_write(Addr_Param116, dead.life_time);
        osDelay(40);
    }
}


static void power_ac_hour_done(int id)
{
    float power_temp = 0;
    unsigned int bcd_time = 0;

    read_power_time(((id - 1) * 2) + POWER_AC_HOUR_BASE_ADDR + POWER_AC_HOUR_ADDR_OFFSET * 5, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_AC_HOUR_BASE_ADDR + POWER_AC_HOUR_ADDR_OFFSET * 5, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_AC_HOUR_BASE_ADDR + POWER_AC_HOUR_ADDR_OFFSET * 6, power_temp); //前6天

    read_power_time(((id - 1) * 2) + POWER_AC_HOUR_BASE_ADDR + POWER_AC_HOUR_ADDR_OFFSET * 4, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_AC_HOUR_BASE_ADDR + POWER_AC_HOUR_ADDR_OFFSET * 4, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_AC_HOUR_BASE_ADDR + POWER_AC_HOUR_ADDR_OFFSET * 5, power_temp); //前5天

    read_power_time(((id - 1) * 2) + POWER_AC_HOUR_BASE_ADDR + POWER_AC_HOUR_ADDR_OFFSET * 3, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_AC_HOUR_BASE_ADDR + POWER_AC_HOUR_ADDR_OFFSET * 3, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_AC_HOUR_BASE_ADDR + POWER_AC_HOUR_ADDR_OFFSET * 4, power_temp); //前4天

    read_power_time(((id - 1) * 2) + POWER_AC_HOUR_BASE_ADDR + POWER_AC_HOUR_ADDR_OFFSET * 2, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_AC_HOUR_BASE_ADDR + POWER_AC_HOUR_ADDR_OFFSET * 2, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_AC_HOUR_BASE_ADDR + POWER_AC_HOUR_ADDR_OFFSET * 3, power_temp); //前3天

    read_power_time(((id - 1) * 2) + POWER_AC_HOUR_BASE_ADDR + POWER_AC_HOUR_ADDR_OFFSET * 1, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_AC_HOUR_BASE_ADDR + POWER_AC_HOUR_ADDR_OFFSET * 1, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_AC_HOUR_BASE_ADDR + POWER_AC_HOUR_ADDR_OFFSET * 2, power_temp); //前2天

    read_power_time(((id - 1) * 2) + POWER_AC_HOUR_BASE_ADDR + POWER_AC_HOUR_ADDR_OFFSET * 0, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_AC_HOUR_BASE_ADDR + POWER_AC_HOUR_ADDR_OFFSET * 0, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_AC_HOUR_BASE_ADDR + POWER_AC_HOUR_ADDR_OFFSET * 1, power_temp); //前1天
}

static void power_ac_day_done(int id)
{
    float power_temp = 0;
    unsigned int bcd_time = 0;

    read_power_time(((id - 1) * 2) + POWER_AC_DAY_BASE_ADDR + POWER_AC_DAY_ADDR_OFFSET * 1, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_AC_DAY_BASE_ADDR + POWER_AC_DAY_ADDR_OFFSET * 1, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_AC_DAY_BASE_ADDR + POWER_AC_DAY_ADDR_OFFSET * 2, power_temp); //前2月

    read_power_time(((id - 1) * 2) + POWER_AC_DAY_BASE_ADDR + POWER_AC_DAY_ADDR_OFFSET * 0, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_AC_DAY_BASE_ADDR + POWER_AC_DAY_ADDR_OFFSET * 0, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_AC_DAY_BASE_ADDR + POWER_AC_DAY_ADDR_OFFSET * 1, power_temp); //前1月

}

static void power_ac_month_done(int id)
{
    float power_temp = 0;
    unsigned int bcd_time = 0;

    read_power_time(((id - 1) * 2) + POWER_AC_MON_BASE_ADDR + POWER_AC_MON_ADDR_OFFSET * 3, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_AC_MON_BASE_ADDR + POWER_AC_MON_ADDR_OFFSET * 3, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_AC_MON_BASE_ADDR + POWER_AC_MON_ADDR_OFFSET * 4, power_temp); //前4年

    read_power_time(((id - 1) * 2) + POWER_AC_MON_BASE_ADDR + POWER_AC_MON_ADDR_OFFSET * 2, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_AC_MON_BASE_ADDR + POWER_AC_MON_ADDR_OFFSET * 2, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_AC_MON_BASE_ADDR + POWER_AC_MON_ADDR_OFFSET * 3, power_temp); //前3年

    read_power_time(((id - 1) * 2) + POWER_AC_MON_BASE_ADDR + POWER_AC_MON_ADDR_OFFSET * 1, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_AC_MON_BASE_ADDR + POWER_AC_MON_ADDR_OFFSET * 1, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_AC_MON_BASE_ADDR + POWER_AC_MON_ADDR_OFFSET * 2, power_temp); //前2年

    read_power_time(((id - 1) * 2) + POWER_AC_MON_BASE_ADDR + POWER_AC_MON_ADDR_OFFSET * 0, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_AC_MON_BASE_ADDR + POWER_AC_MON_ADDR_OFFSET * 0, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_AC_MON_BASE_ADDR + POWER_AC_MON_ADDR_OFFSET * 1, power_temp); //前1年
}

static void power_ac_c_hour_done(int id)
{
    float power_temp = 0;
    unsigned int bcd_time = 0;

    read_power_time(((id - 1) * 2) + POWER_AC_C_HOUR_BASE_ADDR + POWER_AC_C_HOUR_ADDR_OFFSET * 5, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_AC_C_HOUR_BASE_ADDR + POWER_AC_C_HOUR_ADDR_OFFSET * 5, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_AC_C_HOUR_BASE_ADDR + POWER_AC_C_HOUR_ADDR_OFFSET * 6, power_temp); //前6天

    read_power_time(((id - 1) * 2) + POWER_AC_C_HOUR_BASE_ADDR + POWER_AC_C_HOUR_ADDR_OFFSET * 4, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_AC_C_HOUR_BASE_ADDR + POWER_AC_C_HOUR_ADDR_OFFSET * 4, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_AC_C_HOUR_BASE_ADDR + POWER_AC_C_HOUR_ADDR_OFFSET * 5, power_temp); //前5天

    read_power_time(((id - 1) * 2) + POWER_AC_C_HOUR_BASE_ADDR + POWER_AC_C_HOUR_ADDR_OFFSET * 3, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_AC_C_HOUR_BASE_ADDR + POWER_AC_C_HOUR_ADDR_OFFSET * 3, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_AC_C_HOUR_BASE_ADDR + POWER_AC_C_HOUR_ADDR_OFFSET * 4, power_temp); //前4天

    read_power_time(((id - 1) * 2) + POWER_AC_C_HOUR_BASE_ADDR + POWER_AC_C_HOUR_ADDR_OFFSET * 2, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_AC_C_HOUR_BASE_ADDR + POWER_AC_C_HOUR_ADDR_OFFSET * 2, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_AC_C_HOUR_BASE_ADDR + POWER_AC_C_HOUR_ADDR_OFFSET * 3, power_temp); //前3天

    read_power_time(((id - 1) * 2) + POWER_AC_C_HOUR_BASE_ADDR + POWER_AC_C_HOUR_ADDR_OFFSET * 1, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_AC_C_HOUR_BASE_ADDR + POWER_AC_C_HOUR_ADDR_OFFSET * 1, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_AC_C_HOUR_BASE_ADDR + POWER_AC_C_HOUR_ADDR_OFFSET * 2, power_temp); //前2天

    read_power_time(((id - 1) * 2) + POWER_AC_C_HOUR_BASE_ADDR + POWER_AC_C_HOUR_ADDR_OFFSET * 0, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_AC_C_HOUR_BASE_ADDR + POWER_AC_C_HOUR_ADDR_OFFSET * 0, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_AC_C_HOUR_BASE_ADDR + POWER_AC_C_HOUR_ADDR_OFFSET * 1, power_temp); //前1天
}

static void power_ac_c_day_done(int id)
{
    float power_temp = 0;
    unsigned int bcd_time = 0;

    read_power_time(((id - 1) * 2) + POWER_AC_C_DAY_BASE_ADDR + POWER_AC_C_DAY_ADDR_OFFSET * 1, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_AC_C_DAY_BASE_ADDR + POWER_AC_C_DAY_ADDR_OFFSET * 1, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_AC_C_DAY_BASE_ADDR + POWER_AC_C_DAY_ADDR_OFFSET * 2, power_temp); //前2月

    read_power_time(((id - 1) * 2) + POWER_AC_C_DAY_BASE_ADDR + POWER_AC_C_DAY_ADDR_OFFSET * 0, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_AC_C_DAY_BASE_ADDR + POWER_AC_C_DAY_ADDR_OFFSET * 0, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_AC_C_DAY_BASE_ADDR + POWER_AC_C_DAY_ADDR_OFFSET * 1, power_temp); //前1月

}

static void power_ac_c_month_done(int id)
{
    float power_temp = 0;
    unsigned int bcd_time = 0;

    read_power_time(((id - 1) * 2) + POWER_AC_C_MON_BASE_ADDR + POWER_AC_C_MON_ADDR_OFFSET * 3, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_AC_C_MON_BASE_ADDR + POWER_AC_C_MON_ADDR_OFFSET * 3, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_AC_C_MON_BASE_ADDR + POWER_AC_C_MON_ADDR_OFFSET * 4, power_temp); //前4年

    read_power_time(((id - 1) * 2) + POWER_AC_C_MON_BASE_ADDR + POWER_AC_C_MON_ADDR_OFFSET * 2, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_AC_C_MON_BASE_ADDR + POWER_AC_C_MON_ADDR_OFFSET * 2, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_AC_C_MON_BASE_ADDR + POWER_AC_C_MON_ADDR_OFFSET * 3, power_temp); //前3年

    read_power_time(((id - 1) * 2) + POWER_AC_C_MON_BASE_ADDR + POWER_AC_C_MON_ADDR_OFFSET * 1, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_AC_C_MON_BASE_ADDR + POWER_AC_C_MON_ADDR_OFFSET * 1, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_AC_C_MON_BASE_ADDR + POWER_AC_C_MON_ADDR_OFFSET * 2, power_temp); //前2年

    read_power_time(((id - 1) * 2) + POWER_AC_C_MON_BASE_ADDR + POWER_AC_C_MON_ADDR_OFFSET * 0, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_AC_C_MON_BASE_ADDR + POWER_AC_C_MON_ADDR_OFFSET * 0, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_AC_C_MON_BASE_ADDR + POWER_AC_C_MON_ADDR_OFFSET * 1, power_temp); //前1年
}


static void power_dc2_hour_done(int id)
{
    float power_temp = 0;
    unsigned int bcd_time = 0;

    read_power_time(((id - 1) * 2) + POWER_DC2_HOUR_BASE_ADDR + POWER_DC2_HOUR_ADDR_OFFSET * 5, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_DC2_HOUR_BASE_ADDR + POWER_DC2_HOUR_ADDR_OFFSET * 5, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_DC2_HOUR_BASE_ADDR + POWER_DC2_HOUR_ADDR_OFFSET * 6, power_temp); //前6天

    read_power_time(((id - 1) * 2) + POWER_DC2_HOUR_BASE_ADDR + POWER_DC2_HOUR_ADDR_OFFSET * 4, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_DC2_HOUR_BASE_ADDR + POWER_DC2_HOUR_ADDR_OFFSET * 4, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_DC2_HOUR_BASE_ADDR + POWER_DC2_HOUR_ADDR_OFFSET * 5, power_temp); //前5天

    read_power_time(((id - 1) * 2) + POWER_DC2_HOUR_BASE_ADDR + POWER_DC2_HOUR_ADDR_OFFSET * 3, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_DC2_HOUR_BASE_ADDR + POWER_DC2_HOUR_ADDR_OFFSET * 3, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_DC2_HOUR_BASE_ADDR + POWER_DC2_HOUR_ADDR_OFFSET * 4, power_temp); //前4天

    read_power_time(((id - 1) * 2) + POWER_DC2_HOUR_BASE_ADDR + POWER_DC2_HOUR_ADDR_OFFSET * 2, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_DC2_HOUR_BASE_ADDR + POWER_DC2_HOUR_ADDR_OFFSET * 2, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_DC2_HOUR_BASE_ADDR + POWER_DC2_HOUR_ADDR_OFFSET * 3, power_temp); //前3天

    read_power_time(((id - 1) * 2) + POWER_DC2_HOUR_BASE_ADDR + POWER_DC2_HOUR_ADDR_OFFSET * 1, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_DC2_HOUR_BASE_ADDR + POWER_DC2_HOUR_ADDR_OFFSET * 1, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_DC2_HOUR_BASE_ADDR + POWER_DC2_HOUR_ADDR_OFFSET * 2, power_temp); //前2天

    read_power_time(((id - 1) * 2) + POWER_DC2_HOUR_BASE_ADDR + POWER_DC2_HOUR_ADDR_OFFSET * 0, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_DC2_HOUR_BASE_ADDR + POWER_DC2_HOUR_ADDR_OFFSET * 0, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_DC2_HOUR_BASE_ADDR + POWER_DC2_HOUR_ADDR_OFFSET * 1, power_temp); //前1天
}

static void power_dc2_day_done(int id)
{
    float power_temp = 0;
    unsigned int bcd_time = 0;

    read_power_time(((id - 1) * 2) + POWER_DC2_DAY_BASE_ADDR + POWER_DC2_DAT_ADDR_OFFSET * 1, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_DC2_DAY_BASE_ADDR + POWER_DC2_DAT_ADDR_OFFSET * 1, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_DC2_DAY_BASE_ADDR + POWER_DC2_DAT_ADDR_OFFSET * 2, power_temp);   //前2月

    read_power_time(((id - 1) * 2) + POWER_DC2_DAY_BASE_ADDR + POWER_DC2_DAT_ADDR_OFFSET * 0, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_DC2_DAY_BASE_ADDR + POWER_DC2_DAT_ADDR_OFFSET * 0, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_DC2_DAY_BASE_ADDR + POWER_DC2_DAT_ADDR_OFFSET * 1, power_temp);   //前1月
}

static void power_dc2_month_done(int id)
{
    float power_temp = 0;
    unsigned int bcd_time = 0;

    read_power_time(((id - 1) * 2) + POWER_DC2_MON_BASE_ADDR + POWER_DC2_MON_ADDR_OFFSET * 3, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_DC2_MON_BASE_ADDR + POWER_DC2_MON_ADDR_OFFSET * 3, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_DC2_MON_BASE_ADDR + POWER_DC2_MON_ADDR_OFFSET * 4, power_temp);   //前4年

    read_power_time(((id - 1) * 2) + POWER_DC2_MON_BASE_ADDR + POWER_DC2_MON_ADDR_OFFSET * 2, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_DC2_MON_BASE_ADDR + POWER_DC2_MON_ADDR_OFFSET * 2, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_DC2_MON_BASE_ADDR + POWER_DC2_MON_ADDR_OFFSET * 3, power_temp);   //前3年

    read_power_time(((id - 1) * 2) + POWER_DC2_MON_BASE_ADDR + POWER_DC2_MON_ADDR_OFFSET * 1, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_DC2_MON_BASE_ADDR + POWER_DC2_MON_ADDR_OFFSET * 1, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_DC2_MON_BASE_ADDR + POWER_DC2_MON_ADDR_OFFSET * 2, power_temp);   //前2年

    read_power_time(((id - 1) * 2) + POWER_DC2_MON_BASE_ADDR + POWER_DC2_MON_ADDR_OFFSET * 0, &bcd_time);
    read_power_data(((id - 1) * 2) + POWER_DC2_MON_BASE_ADDR + POWER_DC2_MON_ADDR_OFFSET * 0, &power_temp);
    set_power_data(bcd_time, ((id - 1) * 2) + POWER_DC2_MON_BASE_ADDR + POWER_DC2_MON_ADDR_OFFSET * 1, power_temp);   //前1年
}

#define SAVE_TIME	(360) //电量保存周期(10秒)
void fpga_cal_task(const void *pdata)
{
    //该函数中不要调用count_cal_write(), 否则电量保存Flash的操作太过频繁
    unsigned int save_cnt = 0;	
    struct tm tm;
    int ms, hour, hour_old;
    int year, month, dateTime;
    int year_old, month_old, dateTime_old;
    float p_gird = 0, power_ac = 0;
    float pbtra = 0, power_a = 0;
    float pbtrb = 0, power_b = 0;
    float c_life = 0;
    unsigned int bcd_time = 0;
    unsigned int bcd_time_ram = 0;
    unsigned char i = 0;
	
	int pcs_run_time=0;
	int pcs_on_time=0;

    bcd_time_ram = bcd_time_ram;
    pbtrb = pbtrb;
    power_b = power_b;

    get_time(&tm, &ms);
    year_old = tm.tm_year - 100;
    month_old = tm.tm_mon + 1;
    dateTime_old = tm.tm_mday;
    hour_old = tm.tm_hour;

    while(1)
    {
        get_time(&tm, &ms);
        year = tm.tm_year - 100;
        month = tm.tm_mon + 1;
        dateTime = tm.tm_mday;
        hour = tm.tm_hour;
        //tty_printf("%d %d %d  %d\n", tm.tm_year,tm.tm_mon,tm.tm_mday,tm.tm_hour);

        Arm_time.time_year_get = tm.tm_year + 1900;
        Arm_time.time_month_get = tm.tm_mon + 1;
        Arm_time.time_date_get = tm.tm_mday;
        Arm_time.time_ap_get = 0;
        Arm_time.time_week_get = tm.tm_wday == 0 ? 7 : tm.tm_wday;
        Arm_time.time_hour_get = tm.tm_hour;
        Arm_time.time_minute_get = tm.tm_min;
        Arm_time.time_second_get = tm.tm_sec;
        //tty_printf("%d %d %d\n", Arm_time.time_year_get, Arm_time.time_month_get, Arm_time.time_date_get);

        bcd_time = PW_BCD_TIME(year, month, dateTime, hour); //读取时间后重设BCD时间，若有必要向NVSRAM存储数据时，BCD时间再根据需要调整

        if(hour != hour_old) {
            printf("整点变更\n");

            bcd_time = PW_BCD_TIME(year, month, dateTime_old, hour_old);
            hour_old += 1;
            set_power_data(bcd_time,
                           ((hour_old - 1) * 2) + POWER_AC_HOUR_BASE_ADDR + POWER_AC_HOUR_ADDR_OFFSET * 0,
                           CountData.power_ac_day - CountData.power_ac_day_cache);
            CountData.power_ac_day_cache = CountData.power_ac_day;

            set_power_data(bcd_time,
                           ((hour_old - 1) * 2) + POWER_AC_C_HOUR_BASE_ADDR + POWER_AC_C_HOUR_ADDR_OFFSET * 0,
                           CountData.power_ac_c_day - CountData.power_ac_c_day_cache);
            CountData.power_ac_c_day_cache = CountData.power_ac_c_day;

            set_power_data(bcd_time,
                           ((hour_old - 1) * 2) + POWER_DC2_HOUR_BASE_ADDR + POWER_DC2_HOUR_ADDR_OFFSET * 0,
                           CountData.power_dc2_day - CountData.power_dc2_day_cache);
            CountData.power_dc2_day_cache = CountData.power_dc2_day;

            hour_old -= 1;
            hour_old = hour;
        }

        if(dateTime != dateTime_old) {
            printf("日期变更\n");

				//增加一次调频统计
				CountData.fre_adjust_count_today = 0;
				CountData.fre_lock_count_today = 0;

				#if 0
				//增加密钥有效期判定			
				if(arm_config_data_read(CODE_REMAIN_DATE) != 9999 && arm_config_data_read(CODE_REMAIN_DATE) >0)
				{
					miro_write.code_remain_date = miro_write.code_remain_date -1;
					arm_config_data_write(CODE_REMAIN_DATE,miro_write.code_remain_date);   
				}
				#endif
				
				for(i = 1; i <= POWER_RECORD_HOUR; i++) {
					power_ac_hour_done(i);
					power_ac_c_hour_done(i);
					power_dc2_hour_done(i);
					bcd_time = PW_BCD_TIME(year, month_old, dateTime, i);
					set_power_data(bcd_time,
								   ((i - 1) * 2) + POWER_AC_HOUR_BASE_ADDR + POWER_AC_HOUR_ADDR_OFFSET * 0,
								   0);
					set_power_data(bcd_time,
								   ((i - 1) * 2) + POWER_AC_C_HOUR_BASE_ADDR + POWER_AC_C_HOUR_ADDR_OFFSET * 0,
								   0);
					set_power_data(bcd_time,
								   ((i - 1) * 2) + POWER_DC2_HOUR_BASE_ADDR + POWER_DC2_HOUR_ADDR_OFFSET * 0,
								   0);
				}
	
				if(1 != dateTime) 
				{
					bcd_time = PW_BCD_TIME(year, month, dateTime_old, hour);
				} 
				else 
				{
					if(1 != month) 
					{
						bcd_time = PW_BCD_TIME(year, month - 1, dateTime_old, hour);
					} 
					else 
					{
						bcd_time = PW_BCD_TIME(year, 12, dateTime_old, hour);
					}
				}
	
				set_power_data(bcd_time,
							   ((dateTime_old - 1) * 2) + POWER_AC_DAY_BASE_ADDR + POWER_AC_DAY_ADDR_OFFSET * 0,
							   CountData.power_ac_day); //当日
				set_power_data(bcd_time,
							   ((dateTime_old - 1) * 2) + POWER_AC_C_DAY_BASE_ADDR + POWER_AC_C_DAY_ADDR_OFFSET * 0,
							   CountData.power_ac_c_day); //当日
				set_power_data(bcd_time,
							   ((dateTime_old - 1) * 2) + POWER_DC2_DAY_BASE_ADDR + POWER_DC2_DAT_ADDR_OFFSET * 0,
							   CountData.power_dc2_day); //当日
	
				CountData.power_ac_yesterday	= CountData.power_ac_day; //AC昨日发电量
				CountData.power_ac_c_yesterday	= CountData.power_ac_c_day; //AC昨日充电量
				CountData.power_dc2_yesterday	= CountData.power_dc2_day; //DC2昨日发电量
				CountData.power_ac_day			= 0; //AC日发电量
				CountData.power_ac_c_day		= 0; //AC日充电量
				CountData.power_dc1_day 		= 0; //DC1日发电量
				CountData.power_dc1_c_day		= 0; //DC1日充电量
				CountData.power_dc2_day 		= 0; //DC2日发电量
				count_cal[POWER_AC_Y_ADD]		= CountData.power_ac_yesterday;
				count_cal[POWER_AC_C_Y_ADD] 	= CountData.power_ac_c_yesterday;
				count_cal[POWER_DC2_Y_ADD]		= CountData.power_dc2_yesterday;
				count_cal[POWER_AC_DAY_ADD] 	= CountData.power_ac_day;
				count_cal[POWER_AC_C_DAY_ADD]	= CountData.power_ac_c_day;
				count_cal[POWER_DC1_DAY_ADD]	= CountData.power_dc1_day;
				count_cal[POWER_DC1_C_DAY_ADD]	= CountData.power_dc1_c_day;
				count_cal[POWER_DC2_DAY_ADD]	= CountData.power_dc2_day;
	
				CountData.power_ac_day_cache	= 0;
				CountData.power_ac_c_day_cache	= 0;
				CountData.power_dc2_day_cache	= 0;
	
				c_life = count_cal[C_life];
				c_life = c_life - 1;
				count_cal[C_life] = c_life;
	
				dateTime_old = dateTime;
			}
	
			if(month != month_old) {
				printf("月份变更\n");
	
				bcd_time = PW_BCD_TIME(year, month_old, dateTime, hour);
				set_power_data(bcd_time,
							   ((month_old - 1) * 2) + POWER_AC_MON_BASE_ADDR + POWER_AC_MON_ADDR_OFFSET * 0,
							   CountData.power_ac_month);
				set_power_data(bcd_time,
							   ((month_old - 1) * 2) + POWER_AC_C_MON_BASE_ADDR + POWER_AC_C_MON_ADDR_OFFSET * 0,
							   CountData.power_ac_c_month);
				set_power_data(bcd_time,
							   ((month_old - 1) * 2) + POWER_DC2_MON_BASE_ADDR + POWER_DC2_MON_ADDR_OFFSET * 0,
							   CountData.power_dc2_month);
	
				for(i = 1; i <= POWER_RECORD_DATE; i++) {
					power_ac_day_done(i);
					power_ac_c_day_done(i);
					power_dc2_day_done(i);
					bcd_time = PW_BCD_TIME(year, month, i, hour_old);
					set_power_data(bcd_time,
								   ((i - 1) * 2) + POWER_AC_DAY_BASE_ADDR + POWER_AC_DAY_ADDR_OFFSET * 0,
								   0);
					set_power_data(bcd_time,
								   ((i - 1) * 2) + POWER_AC_C_DAY_BASE_ADDR + POWER_AC_C_DAY_ADDR_OFFSET * 0,
								   0);
					set_power_data(bcd_time,
								   ((i - 1) * 2) + POWER_DC2_DAY_BASE_ADDR + POWER_DC2_DAT_ADDR_OFFSET * 0,
								   0);
				}
	
				CountData.power_ac_month		= 0;
				CountData.power_ac_c_month		= 0;
				CountData.power_dc2_month		= 0;
				count_cal[POWER_AC_MONTH_ADD]	= CountData.power_ac_month;
				count_cal[POWER_AC_C_MONTH_ADD] = CountData.power_ac_c_month;
				count_cal[POWER_DC2_MON_ADD]	= CountData.power_dc2_month;
	
				month_old = month;
			}
	
			if(year != year_old) {
				printf("年份变更\n");
	
				for(i = 1; i <= POWER_RECORD_MONTH; i++) {
					power_ac_month_done(i);
					power_ac_c_month_done(i);
					power_dc2_month_done(i);
					bcd_time = PW_BCD_TIME(year, i, dateTime_old, hour_old);
					set_power_data(bcd_time,
								   ((i - 1) * 2) + POWER_AC_MON_BASE_ADDR + POWER_AC_MON_ADDR_OFFSET * 0,
								   0);
					set_power_data(bcd_time,
								   ((i - 1) * 2) + POWER_AC_C_MON_BASE_ADDR + POWER_AC_C_MON_ADDR_OFFSET * 0,
								   0);
					set_power_data(bcd_time,
								   ((i - 1) * 2) + POWER_DC2_MON_BASE_ADDR + POWER_DC2_MON_ADDR_OFFSET * 0,
								   0);
				}
	
				CountData.power_ac_year 		= 0;
				CountData.power_ac_c_year		= 0;
				count_cal[POWER_AC_YEAR_ADD]	= CountData.power_ac_year;
				count_cal[POWER_AC_C_YEAR_ADD]	= CountData.power_ac_c_year;
	
				year_old = year;
			}
	
			#if 0
			/*数据容错*/
			bcd_time = PW_BCD_TIME(year, month, dateTime, hour);
			/*hour : 当前小时所属日期与存储小时所属日期*/
			read_power_time(POWER_AC_HOUR_BASE_ADDR, &bcd_time_ram);
			//tty_printf("bcd_time[%d] bcd_time_ram[%d]\n", bcd_time, bcd_time_ram);
			//tty_printf("[%d] [%d]\n", PW_BCD_TIME_GET_D(bcd_time), PW_BCD_TIME_GET_D(bcd_time_ram));
	
			if(PW_BCD_TIME_GET_D(bcd_time) != PW_BCD_TIME_GET_D(bcd_time_ram))
			{
				printf("bcd is %x\n", PW_BCD_TIME_GET_D(bcd_time));
				printf("bcd ram is %x\n", PW_BCD_TIME_GET_D(bcd_time_ram));
	
				for(i = 1; i <= POWER_RECORD_HOUR; i++)
				{
					power_ac_hour_done(i);
					power_dc2_hour_done(i);
					bcd_time = PW_BCD_TIME(year, month_old, dateTime, i);
					set_power_data(bcd_time,
								   ((i - 1) * 2) + POWER_AC_HOUR_BASE_ADDR + POWER_AC_HOUR_ADDR_OFFSET * 0, 0);
					set_power_data(bcd_time,
								   ((i - 1) * 2) + POWER_DC2_HOUR_BASE_ADDR + POWER_DC2_HOUR_ADDR_OFFSET * 0, 0);
				}
				count_cal[POWER_COUNT_DAY_ADD] = 0; //日发电量
				//fpga_write(99,1);
				//fpga_pdatabuf_write[99]=1;
				//printf("1");
				count_cal[POWER_DC2_DAY_ADD] = 0;	//DC2日发电量
				CountData.power_count_day	 = 0;	//日发电量
				CountData.power_dc2_day 	 = 0;	//DC2日发电量
				CountData.power_ac_day_cache = 0;
				CountData.power_dc2_day_cache = 0;
			}
	
			/*date : 当前日期所属月份与存储日期所属月份*/
			bcd_time = PW_BCD_TIME(year, month, dateTime, hour);
			read_power_time(POWER_AC_DAY_BASE_ADDR, &bcd_time_ram);
	
			if(PW_BCD_TIME_GET_M(bcd_time) != PW_BCD_TIME_GET_M(bcd_time_ram))
			{
				for(i = 1; i <= POWER_RECORD_DATE; i++)
				{
					power_ac_day_done(i);
					power_dc2_day_done(i);
					bcd_time = PW_BCD_TIME(year, month, i, hour_old);
					set_power_data(bcd_time,
								   ((i - 1) * 2) + POWER_AC_DAY_BASE_ADDR + POWER_AC_DAY_ADDR_OFFSET * 0, 0);
					set_power_data(bcd_time,
								   ((i - 1) * 2) + POWER_DC2_DAY_BASE_ADDR + POWER_DC2_DAT_ADDR_OFFSET * 0, 0);
				}
	
				count_cal[POWER_COUNT_MONTH_ADD] = 0;
				CountData.power_count_month = 0;
				count_cal[POWER_DC2_COUNT_MON_ADD] = 0;
				CountData.power_dc2_count_month = 0;
			}
	
			/*month : 当前月所属年份与存储月所属年份*/
			bcd_time = PW_BCD_TIME(year, month, dateTime, hour);
			read_power_time(POWER_AC_MON_BASE_ADDR, &bcd_time_ram);
	
			if(PW_BCD_TIME_GET_Y(bcd_time) != PW_BCD_TIME_GET_Y(bcd_time_ram))
			{
				for(i = 1; i <= POWER_RECORD_MONTH; i++)
				{
					power_ac_month_done(i);
					power_dc2_month_done(i);
					bcd_time = PW_BCD_TIME(year, i, dateTime_old, hour_old);
					set_power_data(bcd_time,
								   ((i - 1) * 2) + POWER_AC_MON_BASE_ADDR + POWER_AC_MON_ADDR_OFFSET * 0, 0);
					set_power_data(bcd_time,
								   ((i - 1) * 2) + POWER_DC2_MON_BASE_ADDR + POWER_DC2_MON_ADDR_OFFSET * 0, 0);
				}
	
				count_cal[POWER_COUNT_YEAR_ADD] = 0;
				CountData.power_count_year = 0;
			}
			#endif
			
			/* 电量统计 */
			state8 = fpga_read(STATE8_ADD);
			if(FSM_FAULT == (state8 & 0xf)) 
			{
				p_gird = 0;
				pbtra = 0;
				pbtrb = 0;
			} 
			else if(((state8 & 0xf) >= FSM_IDLE) && ((state8 & 0xf) < FSM_FAULT)) 
			{
				//p_gird = 1000;
				p_gird = getting_data_handle(Addr_Param267, Upper_Param[Addr_Param267])/10.0f;
				pbtra  = getting_data_handle(Addr_Param269, Upper_Param[Addr_Param269])/10.0f;
	
				/* AC */
				if(p_gird > 0) 
				{ //í?2à·?μ?
					power_ac = p_gird / 3600.0f;
					CountData.Power_Con += power_ac;
	
					if((CountData.Power_Con > 4000000000u) || (CountData.Power_Con < 0)) 
					{
						CountData.Power_Con = 0;
					}
	
					//count_cal[POWER_CON_ADD] = CountData.Power_Con;
					CountData.power_ac_day += power_ac;
					//count_cal[POWER_AC_DAY_ADD] = CountData.power_ac_day;
					CountData.power_ac_month += power_ac;
					//count_cal[POWER_AC_MONTH_ADD] = CountData.power_ac_month;
					CountData.power_ac_year += power_ac;
					//count_cal[POWER_AC_YEAR_ADD] = CountData.power_ac_year;
					CountData.power_ac_con += power_ac;
					//count_cal[POWER_AC_CON_ADD] = CountData.power_ac_con;
					CountData.co2_dec = CountData.power_ac_con * 0.785f;
					CountData.C_dec = CountData.power_ac_con * 0.9f;
					//count_cal[CO2_DEC_ADD] = CountData.co2_dec;
					//count_cal[C_DEC_ADD] = CountData.C_dec;
					bcd_time = PW_BCD_TIME(year, month, dateTime, hour);
	
					if(0 == hour) 
					{
						hour = 24;
						set_power_data(bcd_time,
									   ((hour - 1) * 2) + POWER_AC_HOUR_BASE_ADDR + POWER_AC_HOUR_ADDR_OFFSET * 0,
									   CountData.power_ac_day - CountData.power_ac_day_cache);
						set_power_data(bcd_time,
									  ((dateTime - 1) * 2) + POWER_AC_DAY_BASE_ADDR + POWER_AC_DAY_ADDR_OFFSET * 0,
									   CountData.power_ac_day);
						set_power_data(bcd_time,
									   ((month - 1) * 2) + POWER_AC_MON_BASE_ADDR + POWER_AC_MON_ADDR_OFFSET * 0,
									   CountData.power_ac_month);
					}
	
					if(24 == hour) 
					{
						hour = 0;
						set_power_data(bcd_time,
									   ((hour - 1) * 2) + POWER_AC_HOUR_BASE_ADDR + POWER_AC_HOUR_ADDR_OFFSET * 0,
									   CountData.power_ac_day - CountData.power_ac_day_cache);
						set_power_data(bcd_time,
									  ((dateTime - 1) * 2) + POWER_AC_DAY_BASE_ADDR + POWER_AC_DAY_ADDR_OFFSET * 0,
									   CountData.power_ac_day);
						set_power_data(bcd_time,
									   ((month - 1) * 2) + POWER_AC_MON_BASE_ADDR + POWER_AC_MON_ADDR_OFFSET * 0,
									   CountData.power_ac_month);					
					}
				} 
				else 
				{ //í?2à3?μ?
					power_ac = p_gird / 3600.0f;
					power_ac *= (-1);
					CountData.power_ac_c_day += power_ac;
					//count_cal[POWER_AC_C_DAY_ADD] = CountData.power_ac_c_day;
					CountData.power_ac_c_con += power_ac;
					//count_cal[POWER_AC_C_CON_ADD] = CountData.power_ac_c_con;
					CountData.power_ac_c_month += power_ac;
					//count_cal[POWER_AC_C_MONTH_ADD] = CountData.power_ac_c_month;
					CountData.power_ac_c_year += power_ac;
					//count_cal[POWER_AC_YEAR_ADD] = CountData.power_ac_c_year;
					bcd_time = PW_BCD_TIME(year, month, dateTime, hour);
	
					if(0 == hour) 
					{
						hour = 24;
						set_power_data(bcd_time,
									   ((hour - 1) * 2) + POWER_AC_C_HOUR_BASE_ADDR + POWER_AC_C_HOUR_ADDR_OFFSET * 0,
									   CountData.power_ac_c_day - CountData.power_ac_c_day_cache);
						set_power_data(bcd_time,
									   ((dateTime - 1) * 2) + POWER_AC_C_DAY_BASE_ADDR + POWER_AC_C_DAY_ADDR_OFFSET * 0,
									   CountData.power_ac_c_day);
						set_power_data(bcd_time,
									   ((month - 1) * 2) + POWER_AC_C_MON_BASE_ADDR + POWER_AC_C_MON_ADDR_OFFSET * 0,
									   CountData.power_ac_c_month); 				
					}
	
					if(24 == hour) 
					{
						hour = 0;
						set_power_data(bcd_time,
									   ((hour - 1) * 2) + POWER_AC_C_HOUR_BASE_ADDR + POWER_AC_C_HOUR_ADDR_OFFSET * 0,
									   CountData.power_ac_c_day - CountData.power_ac_c_day_cache);
						set_power_data(bcd_time,
									   ((dateTime - 1) * 2) + POWER_AC_C_DAY_BASE_ADDR + POWER_AC_C_DAY_ADDR_OFFSET * 0,
									   CountData.power_ac_c_day);
						set_power_data(bcd_time,
									   ((month - 1) * 2) + POWER_AC_C_MON_BASE_ADDR + POWER_AC_C_MON_ADDR_OFFSET * 0,
									   CountData.power_ac_c_month); 				
					}
				}
	
				/* DC1 */
				if(pbtra > 0) 
				{ 
					power_a = pbtra / 3600.0f;
					CountData.power_dc1_c_day += power_a;
					//count_cal[POWER_DC1_C_DAY_ADD] = CountData.power_dc1_c_day;
					CountData.power_dc1_c_con += power_a;
					//count_cal[POWER_DC1_C_CON_ADD] = CountData.power_dc1_c_con;
				} 
				else 
				{ 
					power_a = (-1)*pbtra / 3600.0f;
					CountData.power_dc1_day += power_a;
					//count_cal[POWER_DC1_DAY_ADD] = CountData.power_dc1_day;
					CountData.power_dc1_con += power_a;
					//count_cal[POWER_DC1_CON_ADD] = CountData.power_dc1_con;
				}
	
		#if 0
            /* DC2 */
            if(pbtrb > 0) { //1a·ü·￠μ?
                power_b = pbtrb / 3600.0f;
                CountData.power_dc2_day += power_b;
                count_cal[POWER_DC2_DAY_ADD] = CountData.power_dc2_day;
                CountData.power_dc2_count_month += power_b;
                count_cal[POWER_DC2_COUNT_MON_ADD] = CountData.power_dc2_count_month;
                CountData.power_dc2_con += power_b;
                count_cal[POWER_DC2_CON_ADD] = CountData.power_dc2_con;

                bcd_time = PW_BCD_TIME(year, month, dateTime, hour);

                if(0 == hour) {
                    hour = 24;
                }

                set_power_data(bcd_time,
                               ((hour - 1) * 2) + POWER_DC2_HOUR_BASE_ADDR + POWER_DC2_HOUR_ADDR_OFFSET * 0,
                               CountData.power_dc2_day - CountData.power_dc2_day_cache);
                set_power_data(bcd_time,
                               ((dateTime - 1) * 2) + POWER_DC2_DAY_BASE_ADDR + POWER_DC2_DAT_ADDR_OFFSET * 0,
                               CountData.power_dc2_day);
                set_power_data(bcd_time,
                               ((month - 1) * 2) + POWER_DC2_MON_BASE_ADDR + POWER_DC2_MON_ADDR_OFFSET * 0,
                               CountData.power_dc2_count_month);

                if(24 == hour) {
                    hour = 0;
                }
            }
		#endif
        }


        //给Upper显示的值
        setpara(MP_POWER_AC_DAY_H, ((short)((unsigned int)CountData.power_ac_day >> 16)));
        setpara(MP_POWER_AC_DAY_L, (short)((unsigned int)CountData.power_ac_day & 0xffff));
        setpara(MP_POWER_AC_H, (short)((unsigned int)CountData.power_ac_con >> 16));
        setpara(MP_POWER_AC_L, (short)((unsigned int)CountData.power_ac_con & 0xffff));
        setpara(MP_POWER_AC_C_DAY_H, ((short)((unsigned int)CountData.power_ac_c_day >> 16)));
        setpara(MP_POWER_AC_C_DAY_L, (short)((unsigned int)CountData.power_ac_c_day & 0xffff));
        setpara(MP_POWER_AC_C_H, (short)((unsigned int)CountData.power_ac_c_con >> 16));
        setpara(MP_POWER_AC_C_L, (short)((unsigned int)CountData.power_ac_c_con & 0xffff));
        setpara(MP_POWER_DC1_DAY_H, ((short)((unsigned int)CountData.power_dc1_day >> 16)));
        setpara(MP_POWER_DC1_DAY_L, (short)((unsigned int)CountData.power_dc1_day & 0xffff));
        setpara(MP_POWER_DC1_CON_H, (short)((unsigned int)CountData.power_dc1_con >> 16));
        setpara(MP_POWER_DC1_CON_L, (short)((unsigned int)CountData.power_dc1_con & 0xffff));
        setpara(MP_POWER_DC1_C_DAY_H, ((short)((unsigned int)CountData.power_dc1_c_day >> 16)));
        setpara(MP_POWER_DC1_C_DAY_L, (short)((unsigned int)CountData.power_dc1_c_day & 0xffff));
        setpara(MP_POWER_DC1_C_CON_H, (short)((unsigned int)CountData.power_dc1_c_con >> 16));
        setpara(MP_POWER_DC1_C_CON_L, (short)((unsigned int)CountData.power_dc1_c_con & 0xffff));
        setpara(MP_POWER_DC2_DAY_H, ((short)((unsigned int)CountData.power_dc2_day >> 16)));
        setpara(MP_POWER_DC2_DAY_L, (short)((unsigned int)CountData.power_dc2_day & 0xffff));
        setpara(MP_POWER_DC2_CON_H, (short)((unsigned int)CountData.power_dc2_con >> 16));
        setpara(MP_POWER_DC2_CON_L, (short)((unsigned int)CountData.power_dc2_con & 0xffff));
        setpara(MP_CO2_H, (short)((unsigned int)CountData.co2_dec >> 16));
        setpara(MP_CO2_L, (short)((unsigned int)CountData.co2_dec & 0xffff));
        setpara(MP_C_H, (short)((unsigned int)CountData.C_dec >> 16));
        setpara(MP_C_L, (short)((unsigned int)CountData.C_dec & 0xffff));

		save_cnt++;

		#if 1
        if(save_cnt == SAVE_TIME) 
		{			
			count_cal[POWER_CON_ADD] = CountData.Power_Con;	
			
			count_cal[POWER_AC_CON_ADD] = CountData.power_ac_con; 
			count_cal[POWER_AC_DAY_ADD] = CountData.power_ac_day;
			count_cal[POWER_AC_Y_ADD] = CountData.power_ac_yesterday;
			count_cal[POWER_AC_MONTH_ADD] = CountData.power_ac_month;
			count_cal[POWER_AC_YEAR_ADD] = CountData.power_ac_year;
			
			count_cal[CO2_DEC_ADD] = CountData.co2_dec;
			count_cal[C_DEC_ADD] = CountData.C_dec;
					
			count_cal[POWER_AC_C_CON_ADD] = CountData.power_ac_c_con;
			count_cal[POWER_AC_C_DAY_ADD] = CountData.power_ac_c_day;
			count_cal[POWER_AC_C_Y_ADD] = CountData.power_ac_c_yesterday;			
			count_cal[POWER_AC_C_MONTH_ADD] = CountData.power_ac_c_month;
			count_cal[POWER_AC_C_YEAR_ADD] = CountData.power_ac_c_year;
						
			count_cal[POWER_DC1_C_DAY_ADD] = CountData.power_dc1_c_day;
			count_cal[POWER_DC1_C_CON_ADD] = CountData.power_dc1_c_con;
			count_cal[POWER_DC1_DAY_ADD] = CountData.power_dc1_day;
			count_cal[POWER_DC1_CON_ADD] = CountData.power_dc1_con;

            save_cnt = 0;
						
        }
		#endif
		
		if(FSM_WORKING == (state8 & 0xf))  //运行时间
		{	
			pcs_run_time++;
			if(pcs_run_time>=360) 
			{
				CountData.run_time_old = count_cal[RUN_TIME];
				CountData.run_time_now++;
				CountData.run_time_total = CountData.run_time_old + 1;		
				count_cal[RUN_TIME] = CountData.run_time_total;
				pcs_run_time=0;
			}
		}
		else
		{
			CountData.run_time_now = 0;
			pcs_run_time = 0;
		}
		
		#if 1
		/*程序运行时间统计*/	
		pcs_on_time++;
		if(pcs_on_time>=360) 
		{
			CountData.on_time_now++;				
			CountData.on_time_total= CountData.on_time_now + CountData.on_time_old;		
			count_cal[ON_TIME] = CountData.on_time_total;
			pcs_on_time=0;
		}				
		#endif
		
        osDelay(1000);
    }
}

/*Carmack Square root algorithm*/
static float SquareRootFloat(float value)
{
    long i;
    float x, y;
    const float f = 1.5F;
    x  = value * 0.5F;
    y  = value;
    i  = * (long *) &y;
    i  = 0x5f3759df - (i >> 1); //Mysterious figures 0x5f3759df
    y  = * (float *) &i;
    y  = y * (f - (x * y * y));
    y  = y * (f - (x * y * y));
    return value * y;
}

void fpga_data_refresh_task(const void *pdata)
{
    int i;
    int err;
    short cmd1, value;
    pdata = pdata;
    cmd1 = cmd1;
    mb_data.last_onoff_mark = 0;
	short current_power=0;
	short Power_Set_AC;

	short charge_power_limit;
	short discharge_power_limit;
	
	short collect_tick=0;		
	short copy_tick;
	short j;	
	short time_tick;
	short date_tick;	
	short date_tick1;
	unsigned short para_vision;	
	
    while(1)
    {
        ad_start();

        if(fpga_read(STATE8_ADD) != 0xffff)
        {
            for(i = 0; i < 512; i++)
            {
                fpga_pdatabuf_get[i] = fpga_read(i);
                //osDelay(1);
                setpara(i, fpga_pdatabuf_get[i]);
            }
			
			//fpga_pdatabuf_get[257] = fpga_read(257);

			
            for(i = Addr_Param256; i < 512; i++) 
			{ //前110个设置参数这样读会导致HMI回显变化问题: BUG  
                Upper_Param[i] = fpga_pdatabuf_get[i];				
            }
			
            //温度监控参数处理	
            #if 1
	        readpara(220,&value);
	        pcs_manage.temp_a_max = v2temp_ntc(value, &err);
			Upper_Param[MP_TEMP1]= pcs_manage.temp_a_max;	
            setpara(MP_TEMP1, Upper_Param[MP_TEMP1]);
			
	        readpara(221,&value);
	        pcs_manage.temp_b_max = v2temp_ntc(value, &err);			
			Upper_Param[MP_TEMP2]= pcs_manage.temp_b_max;
            setpara(MP_TEMP2, Upper_Param[MP_TEMP2]);
			
			readpara(222,&value);
	        pcs_manage.temp_c_max = v2temp_ntc(value, &err);
			Upper_Param[MP_TEMP3]= pcs_manage.temp_c_max;	
            setpara(MP_TEMP3, Upper_Param[MP_TEMP3]);
			#endif
			
            //用作Upper主界面显示
			//温度显示
			setpara(862,pcs_manage.temp_a_max);
			setpara(863,pcs_manage.temp_b_max);
			setpara(864,pcs_manage.temp_c_max);
			
            //电压
            setpara(870, (short)(getting_data_handle(Addr_Param256, Upper_Param[Addr_Param256])));
            setpara(871, (short)(getting_data_handle(Addr_Param257, Upper_Param[Addr_Param257])));
            setpara(872, (short)(getting_data_handle(Addr_Param258, Upper_Param[Addr_Param258])));
			//电流
			setpara(873, (short)(getting_data_handle(Addr_Param259, Upper_Param[Addr_Param259])));
            setpara(874, (short)(getting_data_handle(Addr_Param260, Upper_Param[Addr_Param260])));
            setpara(875, (short)(getting_data_handle(Addr_Param261, Upper_Param[Addr_Param261])));
			//直流电压电流
            setpara(876, (short)(getting_data_handle(Addr_Param262, Upper_Param[Addr_Param262])));
            setpara(877, (short)(getting_data_handle(Addr_Param263, Upper_Param[Addr_Param263])));
            setpara(878, (short)(getting_data_handle(Addr_Param264, Upper_Param[Addr_Param264])));
            setpara(879, (short)(getting_data_handle(Addr_Param265, Upper_Param[Addr_Param265])));
            setpara(880, (short)(getting_data_handle(Addr_Param266, Upper_Param[Addr_Param266])));			
			//功率
            setpara(881, (short)(getting_data_handle(Addr_Param267, Upper_Param[Addr_Param267])));
            setpara(882, (short)(getting_data_handle(Addr_Param268, Upper_Param[Addr_Param268])));
            setpara(883, (short)(getting_data_handle(Addr_Param269, Upper_Param[Addr_Param269])));
			
            setpara(884, (short)(getting_data_handle(Addr_Param270, Upper_Param[Addr_Param270])));
            setpara(885, (short)(getting_data_handle(Addr_Param271, Upper_Param[Addr_Param271])));
			
			pcs_manage.w_32 = (float)(Upper_Param[Addr_Param271]*100000/343.595f);
        	setpara(W_H, ((short)((unsigned int)pcs_manage.w_32 >> 16)));
        	setpara(W_L, (short)((unsigned int)pcs_manage.w_32 & 0xffff));	
			
			setpara(COS_SETA_PC_SET, 		arm_config_data_read(cos_seta));
            setpara(Ctrl_Mode_SET, 			arm_config_data_read(Prio_Set_ADD));		
            setpara(Run_Mode_Set, 			arm_config_data_read(Run_Mode_ADD));		
            setpara(Communication_En, 		arm_config_data_read(COMMUICATION_PRO_EN));
            setpara(Strategy_En, 			arm_config_data_read(STRATEGY_EN));			

	        //setpara(MP_LOACAL_ASK, 			arm_config_data_read(ASK_LOCAL));
            //setpara(MP_INI_RESIS_FD,        arm_config_data_read(INI_RESIS_FD_ADD));
            //setpara(MP_INI_RESIS_REF,       arm_config_data_read(INI_RESIS_REF_ADD));	
            
            #if 0
			//参数版本号处理
			miro_write.para_vision=0;
            for(i = 0; i < 26; i++) 
			{ 
                miro_write.para_vision += fpga_pdatabuf_get[i];				
            }			
            for(i = 35; i < 120; i++) 
			{ 
                miro_write.para_vision += fpga_pdatabuf_get[i];				
            }	
			miro_write.para_vision += arm_config_data_read(COMMUICATION_PRO_EN);
			miro_write.para_vision += arm_config_data_read(STRATEGY_EN);
			#endif
			
			setpara(MP_ARM_VISION, SOFT_VER1);
			setpara(MP_PARA_VISION, miro_write.para_vision); //参数版本号
            
            setpara(MP_INI_MONITOR_ADDR,    arm_config_data_read(INI_MONITOR_ADDR_ADD));

			//调频参数
			setpara(MB_AGC_EN, 		arm_config_data_read(AGC_EN));			
			setpara(MB_FRE_OVER, 		arm_config_data_read(FRE_OVER));			
			setpara(MB_FRE_UNDER, 		arm_config_data_read(FRE_UNDER));			

            setpara(MP_MAX_P_ADD, 			arm_config_data_read(MAX_P_ADD));		
            setpara(MP_MIN_P_ADD, 			arm_config_data_read(MIN_P_ADD));
            setpara(MP_MAX_BA_C_ADD, 		arm_config_data_read(MAX_BA_C_ADD));
            setpara(MP_MIN_BA_C_ADD, 		arm_config_data_read(MIN_BA_C_ADD));			
            setpara(MP_MAX_BA_U_ADD, 		arm_config_data_read(MAX_BA_U_ADD));			
            setpara(MP_MIN_BA_U_ADD, 		arm_config_data_read(MIN_BA_U_ADD));
            setpara(MP_MAX_Q_ADD, 			arm_config_data_read(MAX_Q_ADD));
            setpara(MP_MIN_Q_ADD, 			arm_config_data_read(MIN_Q_ADD));
			
            setpara(MP_CHRG_VOL_LIMIT,  	arm_config_data_read(CHRG_VOL_LIMIT_ADD));
            setpara(MP_DISC_VOL_LIMIT,  	arm_config_data_read(DISC_VOL_LIMIT_ADD));			
            //setpara(MP_SVG_EN,				arm_config_data_read(SVG_EN_ADD));
            //setpara(MP_ISO_ISLAND,  		arm_config_data_read(ISO_ISLAND_ADD));

			miro_write.modbus_addr = arm_config_data_read(INI_MONITOR_ADDR_ADD);
			
			#if 1
			//密钥
            setpara(MB_DEVICE_CODE_YEAR, 	arm_config_data_read(DEVICE_CODE_YEAR));
            setpara(MB_DEVICE_CODE_MD, 		arm_config_data_read(DEVICE_CODE_MD));
            setpara(MB_DEVICE_CODE_TYPE, 	arm_config_data_read(DEVICE_CODE_TYPE));
            setpara(MB_DEVICE_CODE_NUM, 	arm_config_data_read(DEVICE_CODE_NUM));
            setpara(MB_DEVICE_WRITE_CODE, 	arm_config_data_read(DEVICE_WRITE_CODE));
            setpara(MB_LECENSE_KEY, 		miro_write.lecense_key);
			#endif

			#if 1
            setpara(AC_FUN_START_TEMP, 	arm_config_data_read(AC_FUN_START_TEMP_REC));
            setpara(AC_FUN_STOP_TEMP, 	arm_config_data_read(AC_FUN_STOP_TEMP_REC));
            setpara(FILTER_CAPAC_ERR_TEMP, 	arm_config_data_read(FILTER_CAPAC_ERR_TEMP_REC));
            setpara(TRANSFORMER_ERR_TEMP, 	arm_config_data_read(TRANSFORMER_ERR_TEMP_REC));
            setpara(BUS_CAPAC_ERR_TEMP, 	arm_config_data_read(BUS_CAPAC_ERR_TEMP_REC));
            setpara(ELEC_REACTOR_ERR_TEMP, 	arm_config_data_read(ELEC_REACTOR_ERR_TEMP_REC));
            setpara(AC_CABINET_ERR_TEMP, 	arm_config_data_read(AC_CABINET_ERR_TEMP_REC));
			#endif

			setpara(MB_OF_Thr_1, freqProt.fpi[0].thr);
			setpara(MB_OF_Thr_2, freqProt.fpi[1].thr);
			setpara(MB_UF_Thr_1, freqProt.fpi[2].thr);
			setpara(MB_UF_Thr_2, freqProt.fpi[3].thr);
			setpara(MB_OF_Time_1, freqProt.fpi[0].time);
			setpara(MB_OF_Time_2, freqProt.fpi[1].time);
			setpara(MB_UF_Time_1, freqProt.fpi[2].time);
			setpara(MB_UF_Time_2, freqProt.fpi[3].time);
	
			setpara(MB_SB_EN, softBootup.sbEnble);
			setpara(MB_SB_TIME, softBootup.sbTime);
			setpara(MB_SB_GOPWR, softBootup.sbGoalPwr);
			setpara(MB_FR_EN, faultRec.frEnble);
			setpara(MB_FR_WTIME, faultRec.frWaitTime);
            setpara(MB_CO_EN, offline.commEn);
			setpara(MB_CO_JTIME, offline.commJdgTime);
            setpara(MB_BOOTUP_TIME, faultRec.bootupTime);

            setpara(UL_strtg_en_Addr, miro_write.UL_strtg_en);
        	setpara(PF_x100_Addr, miro_write.PF_x100);
        	setpara(VQ_V1_Addr, miro_write.VQ_V1);
        	setpara(VQ_V2_Addr, miro_write.VQ_V2);
        	setpara(VQ_V3_Addr, miro_write.VQ_V3);
        	setpara(VQ_V4_Addr, miro_write.VQ_V4);
        	setpara(VQ_Q1_Addr, miro_write.VQ_Q1);
        	setpara(VQ_Q2_Addr, miro_write.VQ_Q2);
        	setpara(VQ_Q3_Addr, miro_write.VQ_Q3);
        	setpara(VQ_Q4_Addr, miro_write.VQ_Q4);
        	setpara(PQ_n_P3_Addr, miro_write.PQ_n_P3);
        	setpara(PQ_n_P2_Addr, miro_write.PQ_n_P2);
        	setpara(PQ_n_P1_Addr, miro_write.PQ_n_P1);
        	setpara(PQ_p_P1_Addr, miro_write.PQ_p_P1);
        	setpara(PQ_p_P2_Addr, miro_write.PQ_p_P2);
        	setpara(PQ_p_P3_Addr, miro_write.PQ_p_P3);
        	setpara(PQ_n_Q3_Addr, miro_write.PQ_n_Q3);
        	setpara(PQ_n_Q2_Addr, miro_write.PQ_n_Q2);
        	setpara(PQ_n_Q1_Addr, miro_write.PQ_n_Q1);
        	setpara(PQ_p_Q1_Addr, miro_write.PQ_p_Q1);
        	setpara(PQ_p_Q2_Addr, miro_write.PQ_p_Q2);
        	setpara(PQ_p_Q3_Addr, miro_write.PQ_p_Q3);
        	setpara(VP_V1_Addr, miro_write.VP_V1);
        	setpara(VP_V2_Addr, miro_write.VP_V2);
        	setpara(VP_P1_Addr, miro_write.VP_P1);
        	setpara(VP_P2_Addr, miro_write.VP_P2);
        	setpara(Uac_rate_Addr, miro_write.Uac_rate);
        	setpara(P_rate_Addr, miro_write.P_rate);
        	setpara(P_max_Addr, miro_write.P_max);
        	setpara(PF_min_x100_Addr, miro_write.PF_min_x100);
            setpara(Lpf_times_Addr, miro_write.Lpf_times);
            setpara(Freq_rate_Addr, miro_write.Freq_rate); //频率额定值
        	setpara(Pfr_dbUF_Addr, miro_write.Pfr_dbUF); //一次调频欠频死区
        	setpara(Pfr_kUF_Addr, miro_write.Pfr_kUF); //一次调频欠频不等率
            setpara(Pfr_dbOF_Addr, miro_write.Pfr_dbOF); //一次调频过频死区
            setpara(Pfr_kOF_Addr, miro_write.Pfr_kOF); //一次调频过频不等率
            setpara(Pfr_Tresp_ms_Addr, miro_write.Pfr_Tresp_ms); //一次调频响应时间（单位ms）
			
            mb_data_update();	
			
        }				
	
        osDelay(40);
    }
}




void mb_data_update(void)
{
    float Power_S, p, q;
    short p1, q1;
    /* 功率因数 */
    readpara(881, &p1);
    readpara(882, &q1);
    p = p1;
    q = q1;
    Power_S = SquareRootFloat((((p) / MODULUS) * ((p) / MODULUS)) + ((q / MODULUS) * (q / MODULUS)));
    if(Power_S != 0) {
        mb_data.pfactor = ((p) / MODULUS) / Power_S * 100;
    } else {
        mb_data.pfactor = 0;
    }
    /* 效率: 放电才有效率 */
    readpara(881, &p1);
    readpara(883, &q1);
    p = p1;
    q = q1;
    if((p) > 0) {
        mb_data.efficiency = p / q * 100;
    } else {
        mb_data.efficiency = 0;
    }
}


int EDGEPOS(unsigned char pv, unsigned char *buffer)
{
    unsigned char oldpv = *buffer;
    unsigned char flag = 0;

    if(pv && !oldpv) {
        flag = 1;
    } else {
        flag = 0;
    }

    *buffer = pv;
    return flag;
}

int EDGENPO(unsigned char pv, unsigned char *buffer)
{
    unsigned char oldpv = *buffer;
    unsigned char flag = 0;

    if(pv && !oldpv) {
        flag = 1;
    }

    if((!pv) && oldpv) {
        flag = 2;
    }

    *buffer = pv;
    return flag;
}

int EDGENPO1(unsigned char pv, unsigned char pv_old)
{
    unsigned char flag = 0;

    if((pv == 1) && (pv_old == 0)) 
	{
        flag = 1;
    }

 	if((pv == 0) && (pv_old == 1)) 
 	{
        flag = 2;
    }
    return flag;
}

int edgepos_bit(short state, short *buffer, short j)
{
    short oldpv = *buffer;
    short flag = 0;
    short flag1 = GETBIT(state, j);
    short Poldpv = GETBIT(oldpv, j);

    if(flag1 && !Poldpv) {
        flag = 1;
    } else {
        flag = 0;
    }

    return flag;
}

int edgeng_bit(short state, short *buffer, short j)
{
    short oldpv = *buffer;
    short flag = 0;
    short flag1 = GETBIT(state, j);
    short Poldpv = GETBIT(oldpv, j);

    if(!flag1 && Poldpv) {
        flag = 1;
    } else {
        flag = 0;
    }

    return flag;
}


void Determine_scope(short min, short max, volatile short *value)
{
    if(*value >= max) {
        *value = max;
    } else if(*value <= min) {
        *value = min;
    }
}


void Request_Control(short addr,short value)
{
    if(addr == MP_LOACAL_ASK) {
        if(value != 0) {
            //SETBIT(out_state_info.Sys_State1,11);
        } else {
            //out_state_info.Sys_State1 = RESETBIT(out_state_info.Sys_State1,11);
        }
    }
}

void Pcs_Run_Mode_Set(short mode)
{
    short cmd1 = fpga_read(Addr_Param111);
    short cmd2 = fpga_read(Addr_Param112);

    switch(mode)
    {
    case VF_MODE:
		cmd2 = RESETBIT(cmd2, 2);
        fpga_write(Addr_Param112, cmd2);
        fpga_pdatabuf_write(Addr_Param112,cmd2);
        cmd1 = RESETBIT(cmd1, 5);
        cmd1 = RESETBIT(cmd1, 6);
        cmd1 = RESETBIT(cmd1, 7);
        fpga_write(Addr_Param111, cmd1);
        fpga_pdatabuf_write(Addr_Param111,cmd1);
		log_add(ET_MODE,EST_VF_MODE);
        break;

    case PQ_MODE:
		cmd2 = RESETBIT(cmd2, 2);
        fpga_write(Addr_Param112, cmd2);
        fpga_pdatabuf_write(Addr_Param112,cmd2);
        SETBIT(cmd1, 7);
        cmd1 = RESETBIT(cmd1, 5);
        cmd1 = RESETBIT(cmd1, 6);
        fpga_write(Addr_Param111, cmd1);
        fpga_pdatabuf_write(Addr_Param111,cmd1);
		log_add(ET_MODE,EST_PQ_MODE);
        break;

    case IV_MODE:
		cmd2 = RESETBIT(cmd2, 2);
        fpga_write(Addr_Param112, cmd2);
        fpga_pdatabuf_write(Addr_Param112,cmd2);
		cmd1 = RESETBIT(cmd1, 5);
        SETBIT(cmd1, 6);
        cmd1 = RESETBIT(cmd1, 7);
        fpga_write(Addr_Param111, cmd1);
        fpga_pdatabuf_write(Addr_Param111,cmd1);	
		log_add(ET_MODE,EST_IV_MODE);
        break;
		
    case CP_MODE:
		cmd2 = RESETBIT(cmd2, 2);
        fpga_write(Addr_Param112, cmd2);
        fpga_pdatabuf_write(Addr_Param112,cmd2);
		cmd1 = RESETBIT(cmd1, 5);
        SETBIT(cmd1, 6);
        cmd1 = RESETBIT(cmd1, 7);
        fpga_write(Addr_Param111, cmd1);
        fpga_pdatabuf_write(Addr_Param111,cmd1);
		log_add(ET_MODE,EST_CP_MODE);
        break;
		
    case CI_MODE:
		cmd2 = RESETBIT(cmd2, 2);
        fpga_write(Addr_Param112, cmd2);
        fpga_pdatabuf_write(Addr_Param112,cmd2);
		cmd1 = RESETBIT(cmd1, 5);
        SETBIT(cmd1, 6);
        cmd1 = RESETBIT(cmd1, 7);
        fpga_write(Addr_Param111, cmd1);
        fpga_pdatabuf_write(Addr_Param111,cmd1);		
		log_add(ET_MODE,EST_CI_MODE);
        break;
		
    case CU_MODE:
		cmd2 = RESETBIT(cmd2, 2);
        fpga_write(Addr_Param112, cmd2);
        fpga_pdatabuf_write(Addr_Param112,cmd2);
        cmd1 = RESETBIT(cmd1, 5);
        SETBIT(cmd1, 6);
        cmd1 = RESETBIT(cmd1, 7);
        fpga_write(Addr_Param111, cmd1);
        fpga_pdatabuf_write(Addr_Param111,cmd1);
		log_add(ET_MODE,EST_CU_MODE);
        break;
		
	case VSG_MODE:		
		cmd1 = RESETBIT(cmd1, 5);
        cmd1 = RESETBIT(cmd1, 6);
        cmd1 = RESETBIT(cmd1, 7);
        fpga_write(Addr_Param111, cmd1);
        fpga_pdatabuf_write(Addr_Param111,cmd1);		
		SETBIT(cmd2, 2);
        fpga_write(Addr_Param112, cmd2);
        fpga_pdatabuf_write(Addr_Param112,cmd2);	
		log_add(ET_MODE,EST_VSG_MODE);
		break;

	case CR_MODE:
		break;

    default:
        break;
    }
    arm_config_data_write(Run_Mode_ADD, mode);
}


short Pcs_Run_Mode_Get(void)
{
    short cmd1 = fpga_read(Addr_Param111);
    short cmd2 = fpga_read(Addr_Param112);

    if(0 == GETBIT(cmd1, 5)) 
	{
        if((0 == GETBIT(cmd1, 6)) && (0 == GETBIT(cmd1, 7))&&(0 == GETBIT(cmd2, 2))) 
		{
            return VF_MODE;			
        } 
		else if((0 == GETBIT(cmd1, 6)) && (1 == GETBIT(cmd1, 7))) 
		{
            return PQ_MODE;		
        } 
		else if((1 == GETBIT(cmd1, 6)) && (0 == GETBIT(cmd1, 7))) 
		{
			if( arm_config_data_read(Run_Mode_ADD)== IV_MODE)			
			{
				return IV_MODE;
			}
			
			else if( arm_config_data_read(Run_Mode_ADD)== CP_MODE)			
			{
				return CP_MODE;
			}
			
			else if( arm_config_data_read(Run_Mode_ADD)== CI_MODE)			
			{
				return CI_MODE;
			}
			
			else if( arm_config_data_read(Run_Mode_ADD)== CU_MODE)			
			{
				return CU_MODE;
			}
		}
		else if((0 == GETBIT(cmd1, 6)) && (0 == GETBIT(cmd1, 7))&&(1== GETBIT(cmd2, 2))) 		
		{
			return VSG_MODE;
		}
    } 
	else 
		{
            return -1;
        }
	return 0;
}


void Pcs_Ctrl_Mode_Set(short mode)
{
    if(mode >= CTRL_MODE_MAX) 
	{
        return;
    }
	if(mode ==EMS_CTRL)
	{
		log_add(ET_MODE,EST_EMS_CTR);
	}
	else if(mode ==BMS_CTRL)
	{
		log_add(ET_MODE,EST_BMS_CTR);
	}	
	else if(mode ==HMI_CTRL)
	{
		log_add(ET_MODE,EST_HMI_CTR);
	}
    miro_write.Prio_Set = mode;
    arm_config_data_write(Prio_Set_ADD, miro_write.Prio_Set);
}

short Pcs_Ctrl_Mode_Get(void)
{
    miro_write.Prio_Set = arm_config_data_read(Prio_Set_ADD);
    return miro_write.Prio_Set;
}


void set_pcs_bootup(void)
{
    short cmd0 = fpga_read(Addr_Param110);
    //if(GETBIT(Upper_Param[Addr_Param148], 4) == 1) //钥匙状态
    //{
        SETBIT(cmd0, 0);
        fpga_write(Addr_Param110, cmd0);
        osDelay(80);
        cmd0 = 0;
        fpga_write(Addr_Param110, cmd0);
        printf("开机");
        //mb_data.last_onoff_mark = 2;
        sys_ctl.onoff = 1;
		miro_write.bms_err_flag = 0;
    //}
}


void set_pcs_shutdown(void)
{
    fault_rec_t *pFr = &faultRec;
    freq_prot_t *pFp = &freqProt;
    uint8_t i;
    
    if(miro_write.manShutdownFlag == 1) {
        miro_write.manShutdownFlag = 0;

        pFr->handleFlag = 0;
        if(pFr->startFlag == 1) {
        	pFr->startFlag = 0;
        	osTimerStop(pFr->timerId);
        	printf_debug9("fault_rec timer stop\n");
        }

        pFp->freqProtFault = 0;
	    for(i = 0; i < FREQ_PROT_NUM; i++) {
        	if(pFp->fpi[i].startFlag == 1) {
        		pFp->fpi[i].startFlag = 0;
        		osTimerStop(pFp->fpi[i].timerId);
        		printf_debug9("freq_prot %d timer stop\n", i);
        	}
	    }
    }
    
    short cmd0 = fpga_read(Addr_Param110);
    SETBIT(cmd0, 1);
    fpga_write(Addr_Param110, cmd0);
    osDelay(80);
    cmd0 = 0;
    fpga_write(Addr_Param110, cmd0);
    printf("停机\n");
    //mb_data.last_onoff_mark = 1;
    sys_ctl.onoff = 0;
    fpga_write(Addr_Param26, 0);
    fpga_write(Addr_Param27, 0);
}


void sys_reset(void)
{	
	set_pcs_shutdown(); //避免开机情况复位的危险
    osDelay(1000);
    SCB->AIRCR = 0x05FA0004;	
    while(1);
}

/* 故障记录与清除 */
static void do_fault_record(void)
{
    unsigned char ops_flag;
    unsigned char j;

    /*故障日志*/
    if((state8 & 0xf) == FSM_FAULT) { //故障状态
        fr_info.err_flag = 1;
        fr_info.err_flag1 = 1;
    } else {
        fr_info.err_flag = 0;
        fr_info.err_flag1 = 0;
    }

    ops_flag = EDGEPOS(fr_info.err_flag, &(fr_info.err_flag_old));
    if(ops_flag == 1) {
        /*故障状态0判断*/
        fr_info.err_state[0] = fpga_read(ERR_LCK_STATE0_ADD);
        for(j = 0; j < 16; j++)
        {
            if(edgepos_bit(fr_info.err_state[0], &(fr_info.err_state_last[0]), j))
            {
                log_add(ET_FAULT, j);
                fr_info.fault_err[j] = 1;
                savefinf(j, &(fr_info.fault_err[j]));
            }
        }
        fr_info.err_state_last[0] = fr_info.err_state[0];

        /*故障状态1判断*/
        fr_info.err_state[1] = fpga_read(ERR_LCK_STATE1_ADD);
        for(j = 0; j < 16; j++)
        {
            if(edgepos_bit(fr_info.err_state[1], &(fr_info.err_state_last[1]), j))
            {
                log_add(ET_FAULT, j + 16);
                fr_info.fault_err[j + 16] = 1;
                savefinf(j + 16, &(fr_info.fault_err[j + 16]));
            }
        }
        fr_info.err_state_last[1] = fr_info.err_state[1];

        /*故障状态2判断*/
        fr_info.err_state[2] = fpga_read(ERR_LCK_STATE2_ADD);
        for(j = 0; j < 16; j++)
        {
            if(edgepos_bit(fr_info.err_state[2], &(fr_info.err_state_last[2]), j))
            {
                fr_info.fault_err[j + 32] = 1;
                log_add(ET_FAULT, j + 32);
                savefinf(j + 32, &(fr_info.fault_err[j + 32]));
            }
        }
        fr_info.err_state_last[2] = fr_info.err_state[2];

        /*故障状态3判断*/
        fr_info.err_state[3] = fpga_read(ERR_LCK_STATE3_ADD);
        for(j = 0; j < 16; j++)
        {
            if(edgepos_bit(fr_info.err_state[3], &(fr_info.err_state_last[3]), j))
            {
                fr_info.fault_err[j + 48] = 1;
                log_add(ET_FAULT, j + 48);
                savefinf(j + 48, &(fr_info.fault_err[j + 48]));
            }
        }
        fr_info.err_state_last[3] = fr_info.err_state[3];

        /*故障状态4判断*/
        fr_info.err_state[4] = fpga_read(ERR_LCK_STATE4_ADD);
        for(j = 0; j < 16; j++)
        {
            if(edgepos_bit(fr_info.err_state[4], &(fr_info.err_state_last[4]), j))
            {
                fr_info.fault_err[j + 64] = 1;
                log_add(ET_FAULT, j + 64);
                savefinf(j + 64, &(fr_info.fault_err[j + 64]));
            }
        }
        fr_info.err_state_last[4] = fr_info.err_state[4];
		
        /*故障状态5判断*/  
        fr_info.err_state[5] = fpga_read(ERR_LCK_STATE5_ADD);
        for(j = 0; j < 16; j++)
        {
            if(edgepos_bit(fr_info.err_state[5], &(fr_info.err_state_last[5]), j))
            {
                fr_info.fault_err[j + 80] = 1;
                log_add(ET_FAULT, j + 80);
                savefinf(j + 80, &(fr_info.fault_err[j + 80]));
            }
        }
        fr_info.err_state_last[5] = fr_info.err_state[5];

		/*故障状态6判断*/  
		fr_info.err_state[6] = fpga_read(ERR_LCK_STATE6_ADD);
		for(j = 0; j < 16; j++)
		{
			if(edgepos_bit(fr_info.err_state[6], &(fr_info.err_state_last[6]), j))
			{
				fr_info.fault_err[j + 96] = 1;
				log_add(ET_FAULT, j + 96);
				savefinf(j + 96, &(fr_info.fault_err[j + 96]));
			}
		}
		fr_info.err_state_last[6] = fr_info.err_state[6];
	
    }


    if(EDGENPO(fr_info.err_flag1, &(fr_info.err_flag_old1)) == 2) 
	{
        /*故障状态0判断*/
        fr_info.err_state[0] = fpga_read(ERR_LCK_STATE0_ADD);
        for(j = 0; j < 16; j++)
        {
            if(edgeng_bit(fr_info.err_state[0], &(fr_info.err_state_last[0]), j))
            {
                fr_info.fault_err[j] = 0;
                clearftwave(&(fr_info.fault_err[j]));
            }
        }
        fr_info.err_state_last[0] = fr_info.err_state[0];

        /*故障状态1判断*/
        fr_info.err_state[1] = fpga_read(ERR_LCK_STATE1_ADD);
        for(j = 0; j < 16; j++)
        {
            if(edgeng_bit(fr_info.err_state[1], &(fr_info.err_state_last[1]), j))
            {
                fr_info.fault_err[j + 16] = 0;
                clearftwave(&(fr_info.fault_err[j + 16]));
            }
        }
        fr_info.err_state_last[1] = fr_info.err_state[1];

        /*故障状态2判断*/
        fr_info.err_state[2] = fpga_read(ERR_LCK_STATE2_ADD);
        for(j = 0; j < 16; j++)
        {
            if(edgeng_bit(fr_info.err_state[2], &(fr_info.err_state_last[2]), j))
            {
                fr_info.fault_err[j + 32] = 0;
                clearftwave(&(fr_info.fault_err[j + 32]));
            }
        }
        fr_info.err_state_last[2] = fr_info.err_state[2];

        /*故障状态3判断*/
        fr_info.err_state[3] = fpga_read(ERR_LCK_STATE3_ADD);
        for(j = 0; j < 16; j++)
        {
            if(edgeng_bit(fr_info.err_state[3], &(fr_info.err_state_last[3]), j))
            {
                fr_info.fault_err[j + 48] = 0;
                clearftwave(&(fr_info.fault_err[j + 48]));
            }
        }
        fr_info.err_state_last[3] = fr_info.err_state[3];

        /*故障状态4判断*/
        fr_info.err_state[4] = fpga_read(ERR_LCK_STATE4_ADD);
        for(j = 0; j < 16; j++)
        {
            if(edgeng_bit(fr_info.err_state[4], &(fr_info.err_state_last[4]), j))
            {
                fr_info.fault_err[j + 64] = 0;
                clearftwave(&(fr_info.fault_err[j + 64]));
            }
        }
        fr_info.err_state_last[4] = fr_info.err_state[4];

        /*故障状态5判断*/
        fr_info.err_state[5] = fpga_read(ERR_LCK_STATE5_ADD);
        for(j = 0; j < 16; j++)
        {
            if(edgeng_bit(fr_info.err_state[5], &(fr_info.err_state_last[5]), j))
            {
                fr_info.fault_err[j + 80] = 0;
                clearftwave(&(fr_info.fault_err[j + 80]));
            }
        }
        fr_info.err_state_last[5] = fr_info.err_state[5];	

		/*故障状态6判断*/
		fr_info.err_state[6] = fpga_read(ERR_LCK_STATE6_ADD);
		for(j = 0; j < 16; j++)
		{
			if(edgeng_bit(fr_info.err_state[6], &(fr_info.err_state_last[6]), j))
			{
				fr_info.fault_err[j + 96] = 0;
				clearftwave(&(fr_info.fault_err[j + 96]));
			}
		}
		fr_info.err_state_last[6] = fr_info.err_state[6];	
		
    }
}


/* 开关记录与清除 */
static void do_switch_record(void)
{
    short state9, state10, state11;
    unsigned char i;

    state9 = fpga_read(STATE9_ADD);
    for(i = 0; i < 16; i++)
    {
        if(edgepos_bit(state9, &(sr_info.state9_last), i))
        {
            log_add(ET_SWITCH, i);
        }
        if(edgeng_bit(state9, &(sr_info.state9_last), i))
        {
            log_add(ET_SWITCH, i + 16);
        }
    }
    sr_info.state9_last = state9;

	#if 0
	/*系统状态10判断*/
    state10 = fpga_read(STATE10_ADD);
    for(i = 0; i < 16; i++)
    {
        if(edgepos_bit(state10, &(sr_info.state10_last), i))
        {
            log_add(ET_SWITCH, i + 32);
        }
        if(edgeng_bit(state10, &(sr_info.state10_last), i))
        {
            log_add(ET_SWITCH, i + 48);
        }
    }
    sr_info.state10_last = state10;
	#endif

    /*系统状态11判断*/
    state11 = fpga_read(STATE11_ADD);
    for(i = 0; i < 16; i++)
    {
        if(edgepos_bit(state11, &(sr_info.state11_last), i))
        {
            log_add(ET_SWITCH, i + 64);
        }
        if(edgeng_bit(state11, &(sr_info.state11_last), i))
        {
            log_add(ET_SWITCH, i + 80);
        }
    }
    sr_info.state11_last = state11;
}

/* CMD命令记录*/
static void do_cmd_record(void)
{
    //short cmd0, cmd1, cmd2;
    unsigned char i;

    short cmd0 = fpga_read(Addr_Param110);
    for(i = 0; i < 16; i++)
    {
        if(edgepos_bit(cmd0, &(cmd_info.cmd0_last), i))
        {
            log_add(ET_TIP, i);
        }
        if(edgeng_bit(cmd0, &(cmd_info.cmd0_last), i))
        {
            log_add(ET_TIP, i + 16);
        }
    }
    cmd_info.cmd0_last = cmd0;
	
	//cmd1判断
    short cmd1 = fpga_read(Addr_Param111);
    for(i = 0; i < 9; i++)
    {
        if(edgepos_bit(cmd1, &(cmd_info.cmd1_last), i))
        {
            log_add(ET_TIP, i + 32);
        }
        if(edgeng_bit(cmd1, &(cmd_info.cmd1_last), i))
        {
            log_add(ET_TIP, i + 48);
        }
    }
    cmd_info.cmd1_last = cmd1;

    /*cmd2判断*/
    short cmd2 = fpga_read(Addr_Param112);
    for(i = 0; i < 16; i++)
    {
        if(edgepos_bit(cmd2, &(cmd_info.cmd2_last), i))
        {
            log_add(ET_TIP, i + 64);
        }
        if(edgeng_bit(cmd2, &(cmd_info.cmd2_last), i))
        {
            log_add(ET_TIP, i + 80);
        }
    }
    cmd_info.cmd2_last = cmd2;
}

/* 策略及通讯保护使能判定*/
static void do_strategy_record(void)
{
    //short cmd0, cmd1, cmd2;
    unsigned char i;

	//strategy判定
    for(i = 0; i < 16; i++)
    {
        if(edgepos_bit(miro_write.strategy_en, &(str_info.strategy_last), i))
        {
            log_add(ET_STRATEGY, i);
        }
        if(edgeng_bit(miro_write.strategy_en, &(str_info.strategy_last), i))
        {
            log_add(ET_STRATEGY, i + 16);
        }
    }
    str_info.strategy_last = miro_write.strategy_en;
	
	//通讯判定
       for(i = 0; i < 16; i++)
    {
        if(edgepos_bit(miro_write.communication_en, &(str_info.communication_last), i))
        {
            log_add(ET_STRATEGY, i + 32);
        }
        if(edgeng_bit(miro_write.communication_en, &(str_info.communication_last), i))
        {
            log_add(ET_STRATEGY, i + 48);
        }
    }
    str_info.communication_last = miro_write.communication_en;
}

static void do_Sys_State1(void)
{
    short pcs_state;
    short pcs_run_mode, pcs_ctrl_mode;
	short power_set_value;
	short power_run_value;
	
    pcs_state = state8 & 0xf;
	power_set_value = getting_data_handle(Addr_Param26, fpga_pdatabuf_get[Addr_Param26]);
	power_run_value = getting_data_handle(Addr_Param267, Upper_Param[Addr_Param267]);

    /*PCS设备状态 [0:1]*/
    if((pcs_state==FSM_IDLE)||(pcs_state==FSM_STOP)) 
	{
        out_state_info.Sys_State1=RESETBIT(out_state_info.Sys_State1,0);
        out_state_info.Sys_State1=RESETBIT(out_state_info.Sys_State1,1);
        setpara(MP_SYS_STATUS,0);
    } 
	else if((pcs_state>FSM_IDLE)&&(pcs_state<FSM_FAULT)) 
	{
        SETBIT(out_state_info.Sys_State1,0);
        out_state_info.Sys_State1=RESETBIT(out_state_info.Sys_State1,1);
        setpara(MP_SYS_STATUS,1);

    } 
	else if(pcs_state==FSM_FAULT) 
	{
        SETBIT(out_state_info.Sys_State1,1);
        out_state_info.Sys_State1=RESETBIT(out_state_info.Sys_State1,0);
        setpara(MP_SYS_STATUS,2);
    }

	/*pcs充放电状态[2:3]*/
	#if 1	
 	if((pcs_state==FSM_WORKING) &&(PQ_MODE == pcs_run_mode))
	{	
		if((power_set_value == 0))
		{			
			SETBIT(out_state_info.Sys_State1,2);
	        out_state_info.Sys_State1=RESETBIT(out_state_info.Sys_State1,3);
		}
		else if((power_set_value < 0))
		{
	        out_state_info.Sys_State1=RESETBIT(out_state_info.Sys_State1,2);
			SETBIT(out_state_info.Sys_State1,3);
		}
		else if((power_set_value > 0))
		{			
			SETBIT(out_state_info.Sys_State1,2);
			SETBIT(out_state_info.Sys_State1,3);
		}
	}
	else
	{
		out_state_info.Sys_State1=RESETBIT(out_state_info.Sys_State1,2);
	    out_state_info.Sys_State1=RESETBIT(out_state_info.Sys_State1,3);		
	}

	if(VF_MODE == pcs_run_mode)
	{
		if((power_run_value == 0))
		{			
			SETBIT(out_state_info.Sys_State1,2);
	        out_state_info.Sys_State1=RESETBIT(out_state_info.Sys_State1,3);
		}
		else if((power_run_value < 0))
		{
	        out_state_info.Sys_State1=RESETBIT(out_state_info.Sys_State1,2);
			SETBIT(out_state_info.Sys_State1,3);
		}
		else if((power_run_value > 0))
		{			
			SETBIT(out_state_info.Sys_State1,2);
			SETBIT(out_state_info.Sys_State1,3);
		}	
	}
	#endif		

	
    /*PCS控制模式 [5:4]*/
    pcs_ctrl_mode = Pcs_Ctrl_Mode_Get();
    if(pcs_ctrl_mode == EMS_CTRL) 
	{
        out_state_info.Sys_State1=RESETBIT(out_state_info.Sys_State1,4);
        out_state_info.Sys_State1=RESETBIT(out_state_info.Sys_State1,5);
    } 
	else if(pcs_ctrl_mode == BMS_CTRL) 
	{
        SETBIT(out_state_info.Sys_State1,4);
        out_state_info.Sys_State1=RESETBIT(out_state_info.Sys_State1,5);
    } 
	else if(pcs_ctrl_mode == HMI_CTRL) 
	{
        out_state_info.Sys_State1=RESETBIT(out_state_info.Sys_State1,4);
        SETBIT(out_state_info.Sys_State1,5);
    }

	
    /*充放电 [6]*/
    (Upper_Param[Addr_Param267] > 0)?(out_state_info.Sys_State1=RESETBIT(out_state_info.Sys_State1,6)):(SETBIT(out_state_info.Sys_State1,6));

	/*断路器链路状态 [7]*/
    (1 == GETBIT(Upper_Param[STATE9_ADD],0))?(SETBIT(out_state_info.Sys_State1,8)):(out_state_info.Sys_State1=RESETBIT(out_state_info.Sys_State1,8));

	/*接触器状态 [8]*/
    (0 == GETBIT(Upper_Param[STATE9_ADD],2))?(SETBIT(out_state_info.Sys_State1,7)):(out_state_info.Sys_State1=RESETBIT(out_state_info.Sys_State1,7));


	/*防雷器状态 [11]*/
    (1 == GETBIT(Upper_Param[STATE9_ADD],1))?(SETBIT(out_state_info.Sys_State1,11)):(out_state_info.Sys_State1=RESETBIT(out_state_info.Sys_State1,11));

	/*一次调频状态状态 [11:10]*/	
	if(Upper_Param[191] == 0)
	{
		out_state_info.Sys_State1=RESETBIT(out_state_info.Sys_State1,10);
		out_state_info.Sys_State1=RESETBIT(out_state_info.Sys_State1,11);		
	}
	else if(Upper_Param[191] == 1)
	{
		SETBIT(out_state_info.Sys_State1,10);
		out_state_info.Sys_State1=RESETBIT(out_state_info.Sys_State1,11);
	} 
	else if(Upper_Param[191] == 2)
	{
		out_state_info.Sys_State1=RESETBIT(out_state_info.Sys_State1,10);
		SETBIT(out_state_info.Sys_State1,11);
	}	
    /*PCS运行模式 [14:12]*/
    pcs_run_mode = Pcs_Run_Mode_Get();
    if(VF_MODE == pcs_run_mode) 
	{
		out_state_info.Sys_State1=SETBIT(out_state_info.Sys_State1,12);
		out_state_info.Sys_State1=RESETBIT(out_state_info.Sys_State1,13);
		out_state_info.Sys_State1=RESETBIT(out_state_info.Sys_State1,14);
	} 
	else if(PQ_MODE == pcs_run_mode) 
	{
		out_state_info.Sys_State1=RESETBIT(out_state_info.Sys_State1,12);
		out_state_info.Sys_State1=RESETBIT(out_state_info.Sys_State1,13);
		out_state_info.Sys_State1=RESETBIT(out_state_info.Sys_State1,14);
	} 
	else if(IV_MODE == pcs_run_mode) 
	{
		out_state_info.Sys_State1=SETBIT(out_state_info.Sys_State1,12);
		out_state_info.Sys_State1=RESETBIT(out_state_info.Sys_State1,13);
		out_state_info.Sys_State1=SETBIT(out_state_info.Sys_State1,14);
	} 
	else 
	{
		out_state_info.Sys_State1=SETBIT(out_state_info.Sys_State1,12);
		out_state_info.Sys_State1=SETBIT(out_state_info.Sys_State1,13);
		out_state_info.Sys_State1=SETBIT(out_state_info.Sys_State1,14);

    }

	/*绝缘检测状态[15]*/
	if((GETBIT(Upper_Param[STATE2_ADD],10)==1) || (GETBIT(fpga_read(ERR_LCK_STATE2_ADD),10)==1))
	{
		out_state_info.Sys_State1 = SETBIT(out_state_info.Sys_State1,15);
	}
	else
	{
		out_state_info.Sys_State1 = RESETBIT(out_state_info.Sys_State1,15);
	}
	
}

//static void do_Sys_State2(void)
//{

//}


static void do_Alarm_State1()
{
    short Pac, Uab, Ubc, Uca, f;
	short temp1;
	short temp2;
	short temp3;
	short power_level;
	short over_voltage;
	short under_voltage;
    Pac = getting_data_handle(Addr_Param267, Upper_Param[Addr_Param267]) / MODULUS;
    Uab = getting_data_handle(Addr_Param256, Upper_Param[Addr_Param256]) / MODULUS;
    Ubc = getting_data_handle(Addr_Param257, Upper_Param[Addr_Param257]) / MODULUS;
    Uca = getting_data_handle(Addr_Param258, Upper_Param[Addr_Param258]) / MODULUS;
    f = getting_data_handle(Addr_Param271, Upper_Param[Addr_Param271]) / MODULUS;
    temp1 = pcs_manage.temp_a_max;//Upper_Param[Addr_Param196];
    temp2 = pcs_manage.temp_b_max;//Upper_Param[Addr_Param197];
    temp3 = pcs_manage.temp_c_max;//Upper_Param[Addr_Param198];
	
	power_level = (short)(getting_data_handle(Addr_Param32, fpga_pdatabuf_get[Addr_Param32])*1.1f/MODULUS);
	over_voltage = (short)(getting_data_handle(Addr_Param0, fpga_pdatabuf_get[Addr_Param0])/MODULUS);
	under_voltage = (short)(getting_data_handle(Addr_Param1, fpga_pdatabuf_get[Addr_Param1])/MODULUS);

    /*高温告警 [1]*/	
	#if 1		
	if((temp1>90) || (temp2>90) || (temp3>90)) 
	{
		SETBIT(out_state_info.Alarm_State1,1);	
		miro_write.pcs_htemp_flag = 1;
		//log_add(ET_OPSTATE,EST_HTEP_ALM); 
		SETBIT(out_state_info.Alarm_HMI,1);
	} 
	if((miro_write.pcs_htemp_flag==1) && (temp1<=85) && (temp2<=85) && (temp3<=85))
	{
		out_state_info.Alarm_State1 = RESETBIT(out_state_info.Alarm_State1,1);			
		out_state_info.Alarm_HMI = RESETBIT(out_state_info.Alarm_HMI,1);
		miro_write.pcs_htemp_flag = 0;
	}
	#endif

    /*过载告警 [1]*/
    (abs(Pac) > power_level)?(SETBIT(out_state_info.Alarm_State1,1)):(out_state_info.Alarm_State1 = RESETBIT(out_state_info.Alarm_State1,1));//过载

	/*过压告警 [2]*/
    if((Uab > over_voltage)||(Ubc > over_voltage)||(Uca > over_voltage)) 
	{
        SETBIT(out_state_info.Alarm_State1,2);
    } 
	else 
	{
        out_state_info.Alarm_State1 = RESETBIT(out_state_info.Alarm_State1,2);
    }
	
    /*欠压告警 [3]*/
    if((Uab < under_voltage)||(Ubc < under_voltage)||(Uca < under_voltage)) 
	{
        SETBIT(out_state_info.Alarm_State1,3);
    } 
	else 
	{
        out_state_info.Alarm_State1 = RESETBIT(out_state_info.Alarm_State1,3);
    }
	
    /*过频告警 [4]*/
    (f > 52)? (SETBIT(out_state_info.Alarm_State1,4)):(out_state_info.Alarm_State1 = RESETBIT(out_state_info.Alarm_State1,4));	//过频

		/*欠频告警 [5]*/
	    (f < 47)? (SETBIT(out_state_info.Alarm_State1,5)):(out_state_info.Alarm_State1 = RESETBIT(out_state_info.Alarm_State1,5));	//欠频

		/*告警总汇总*/
		if((GETBIT(out_state_info.Alarm_State1,0) == 1) || (GETBIT(out_state_info.Alarm_State1,1) == 1) || (GETBIT(out_state_info.Alarm_State1,2) == 1) 
			||(GETBIT(out_state_info.Alarm_State1,3) == 1) || (GETBIT(out_state_info.Alarm_State1,4) == 1) ||(GETBIT(out_state_info.Alarm_State1,5) == 1))
		{		
	       SETBIT(out_state_info.Alarm_State1,6);
		}
		else
		{
			out_state_info.Alarm_State1 = RESETBIT(out_state_info.Alarm_State1,6);
		}

		/*保留 [7:15]*/
	    out_state_info.Alarm_State1=RESETBIT(out_state_info.Alarm_State1,7);
	    out_state_info.Alarm_State1=RESETBIT(out_state_info.Alarm_State1,8);
	    out_state_info.Alarm_State1=RESETBIT(out_state_info.Alarm_State1,9);
	    out_state_info.Alarm_State1=RESETBIT(out_state_info.Alarm_State1,10);
	    out_state_info.Alarm_State1=RESETBIT(out_state_info.Alarm_State1,11);
	    out_state_info.Alarm_State1=RESETBIT(out_state_info.Alarm_State1,12);
	    out_state_info.Alarm_State1=RESETBIT(out_state_info.Alarm_State1,13);
	    out_state_info.Alarm_State1=RESETBIT(out_state_info.Alarm_State1,14);
	    out_state_info.Alarm_State1=RESETBIT(out_state_info.Alarm_State1,15);

		
}

//static void do_Alarm_State2(void)
//{

//}

static void do_Sys_Err1()
{
	#if 1
	/*系统状态流保护 [0]*/
    if((GETBIT(fpga_read(ERR_LCK_STATE1_ADD),9)==1)||(GETBIT(fpga_read(ERR_LCK_STATE1_ADD),10)==1)	||(GETBIT(fpga_read(ERR_LCK_STATE1_ADD),11)==1)
            ||(GETBIT(fpga_read(ERR_LCK_STATE1_ADD),12)==1) ||(GETBIT(fpga_read(ERR_LCK_STATE1_ADD),13)==1)||(GETBIT(fpga_read(ERR_LCK_STATE1_ADD),14)==1)
            ||(GETBIT(fpga_read(ERR_LCK_STATE1_ADD),15)==1)||(GETBIT(fpga_read(ERR_LCK_STATE3_ADD),4)==1) ||(GETBIT(fpga_read(ERR_LCK_STATE3_ADD),5)==1)
            ||(GETBIT(fpga_read(ERR_LCK_STATE3_ADD),6)==1)||(GETBIT(fpga_read(ERR_LCK_STATE3_ADD),7)==1)
            ||(GETBIT(fpga_read(ERR_LCK_STATE3_ADD),8)==1)||(GETBIT(fpga_read(ERR_LCK_STATE3_ADD),9)==1)||(GETBIT(fpga_read(ERR_LCK_STATE3_ADD),10)==1)
            ||(GETBIT(fpga_read(ERR_LCK_STATE3_ADD),11)==1)||(GETBIT(fpga_read(ERR_LCK_STATE4_ADD),0)==1)||(GETBIT(fpga_read(ERR_LCK_STATE4_ADD),1)==1)
            ||(GETBIT(fpga_read(ERR_LCK_STATE4_ADD),2)==1)||(GETBIT(fpga_read(ERR_LCK_STATE4_ADD),3)==1)||(GETBIT(fpga_read(ERR_LCK_STATE4_ADD),4)==1)
            ||(GETBIT(fpga_read(ERR_LCK_STATE4_ADD),5)==1)||(GETBIT(fpga_read(ERR_LCK_STATE4_ADD),6)==1)||(GETBIT(fpga_read(ERR_LCK_STATE4_ADD),7)==1)
            ||(GETBIT(fpga_read(ERR_LCK_STATE4_ADD),8)==1)||(GETBIT(fpga_read(ERR_LCK_STATE4_ADD),9)==1)||(GETBIT(fpga_read(ERR_LCK_STATE4_ADD),10)==1)
            ||(GETBIT(fpga_read(ERR_LCK_STATE4_ADD),11)==1)||(GETBIT(fpga_read(ERR_LCK_STATE4_ADD),12)==1)||(GETBIT(fpga_read(ERR_LCK_STATE4_ADD),13)==1)
            ||(GETBIT(fpga_read(ERR_LCK_STATE4_ADD),14)==1) ||(GETBIT(fpga_read(STATE9_ADD),6)==0) ||(GETBIT(fpga_read(STATE9_ADD),4)==0))
    {
        SETBIT(out_state_info.Sys_Err1,0);
    } else {
        out_state_info.Sys_Err1=RESETBIT(out_state_info.Sys_Err1,0);
    }
	#endif
	
    /*开关状态异常 [1]*/
    if((GETBIT(fpga_read(ERR_LCK_STATE3_ADD),14)==1)||(GETBIT(fpga_read(ERR_LCK_STATE3_ADD),15)==1)||
            (GETBIT(fpga_read(ERR_LCK_STATE4_ADD),0)==1)||(GETBIT(fpga_read(ERR_LCK_STATE4_ADD),2)==1))
    {
        SETBIT(out_state_info.Sys_Err1,1);
    } 
	else 
	{
        out_state_info.Sys_Err1=RESETBIT(out_state_info.Sys_Err1,1);
    }
    /*绝缘保护 [2]*/
    (1 == GETBIT(fpga_read(ERR_LCK_STATE2_ADD),10))?(SETBIT(out_state_info.Sys_Err1,2)):(out_state_info.Sys_Err1=RESETBIT(out_state_info.Sys_Err1,2));
    /*过温保护 [3]*/
    if((GETBIT(fpga_read(ERR_LCK_STATE4_ADD),1)==1)||(GETBIT(fpga_read(ERR_LCK_STATE4_ADD),9)==1))
    {
        SETBIT(out_state_info.Sys_Err1,3);
    } else {
        out_state_info.Sys_Err1=RESETBIT(out_state_info.Sys_Err1,3);
    }
    /*内部短路 [4]*/
    out_state_info.Sys_Err1=RESETBIT(out_state_info.Sys_Err1,4);
	
    /*直流反接 [5]*/
	if(GETBIT(fpga_read(ERR_LCK_STATE5_ADD),0)==1)
    {
        SETBIT(out_state_info.Sys_Err1,5);
    } 
	else 
	{
        out_state_info.Sys_Err1 = RESETBIT(out_state_info.Sys_Err1,5);
    }
    //out_state_info.Sys_Err1=RESETBIT(out_state_info.Sys_Err1,5);
    /*直流欠压 [6]*/
    if((GETBIT(fpga_read(ERR_LCK_STATE1_ADD),14)==1)||(GETBIT(fpga_read(ERR_LCK_STATE1_ADD),15)==1)||(GETBIT(fpga_read(ERR_LCK_STATE2_ADD),0)==1)
            ||(GETBIT(fpga_read(ERR_LCK_STATE2_ADD),7)==1)||(GETBIT(fpga_read(ERR_LCK_STATE2_ADD),8)==1)||(GETBIT(fpga_read(ERR_LCK_STATE2_ADD),9)==1))
    {
        SETBIT(out_state_info.Sys_Err1,6);
    } 
	else 
	{
        out_state_info.Sys_Err1=RESETBIT(out_state_info.Sys_Err1,6);
    }
	
    /*直流过压 [7]*/
    if((GETBIT(fpga_read(ERR_LCK_STATE1_ADD),11)==1)||(GETBIT(fpga_read(ERR_LCK_STATE1_ADD),12)==1)	||(GETBIT(fpga_read(ERR_LCK_STATE1_ADD),13)==1)
        ||(GETBIT(fpga_read(ERR_LCK_STATE2_ADD),4)==1)||(GETBIT(fpga_read(ERR_LCK_STATE2_ADD),5)==1)||(GETBIT(fpga_read(ERR_LCK_STATE2_ADD),6)==1) 
        ||(GETBIT(fpga_read(ERR_LCK_STATE3_ADD),1)==1)||(GETBIT(fpga_read(ERR_LCK_STATE3_ADD),3)==1))
    {
        SETBIT(out_state_info.Sys_Err1,7);
    } 
	else 
	{
        out_state_info.Sys_Err1=RESETBIT(out_state_info.Sys_Err1,7);
    }
    /*直流过流 [8]*/
    if((GETBIT(fpga_read(ERR_LCK_STATE2_ADD),1)==1)||(GETBIT(fpga_read(ERR_LCK_STATE2_ADD),2)==1)	
		||(GETBIT(fpga_read(ERR_LCK_STATE2_ADD),3)==1)||(GETBIT(fpga_read(ERR_LCK_STATE3_ADD),2)==1) )
    {
        SETBIT(out_state_info.Sys_Err1,8);
    } else {
        out_state_info.Sys_Err1=RESETBIT(out_state_info.Sys_Err1,8);
    }
    /*过载 [9]*/
    out_state_info.Sys_Err1=RESETBIT(out_state_info.Sys_Err1,9);
    /*相序错误 [10]*/
    out_state_info.Sys_Err1=RESETBIT(out_state_info.Sys_Err1,10);
    /*欠频 [11]*/
    if((GETBIT(fpga_read(ERR_LCK_STATE4_ADD),6)==1)||(GETBIT(fpga_read(ERR_LCK_STATE4_ADD),7)==1)	||(GETBIT(fpga_read(ERR_LCK_STATE4_ADD),8)==1))
    {
        SETBIT(out_state_info.Sys_Err1,11);
    } else {
        out_state_info.Sys_Err1=RESETBIT(out_state_info.Sys_Err1,11);
    }
    /*过频 [12]*/			/*Sys_Err1 bit:(12)*/		/*No reported*/
    if((GETBIT(fpga_read(ERR_LCK_STATE4_ADD),3)==1)||(GETBIT(fpga_read(ERR_LCK_STATE4_ADD),4)==1)	||(GETBIT(fpga_read(ERR_LCK_STATE4_ADD),5)==1))
    {
        SETBIT(out_state_info.Sys_Err1,12);
    } else {
        out_state_info.Sys_Err1=RESETBIT(out_state_info.Sys_Err1,12);
    }
    /*交流欠压 [13]*/		/*Sys_Err1 bit:(13)*/
    if((GETBIT(fpga_read(ERR_LCK_STATE0_ADD),3)==1)||(GETBIT(fpga_read(ERR_LCK_STATE0_ADD),4)==1)||(GETBIT(fpga_read(ERR_LCK_STATE0_ADD),5)==1) ||
            (GETBIT(fpga_read(ERR_LCK_STATE0_ADD),9)==1)||(GETBIT(fpga_read(ERR_LCK_STATE0_ADD),10)==1)||(GETBIT(fpga_read(ERR_LCK_STATE0_ADD),11)==1) ||
            (GETBIT(fpga_read(ERR_LCK_STATE0_ADD),15)==1)||(GETBIT(fpga_read(ERR_LCK_STATE1_ADD),0)==1)||(GETBIT(fpga_read(ERR_LCK_STATE1_ADD),1)==1))
    {
        SETBIT(out_state_info.Sys_Err1,13);
    } 
	else 
	{
        out_state_info.Sys_Err1=RESETBIT(out_state_info.Sys_Err1,13);
    }
    /*交流过压 [14]*/		/*Sys_Err1 bit:(14)*/
    if((GETBIT(fpga_read(ERR_LCK_STATE0_ADD),0)==1)||(GETBIT(fpga_read(ERR_LCK_STATE0_ADD),1)==1)||(GETBIT(fpga_read(ERR_LCK_STATE0_ADD),2)==1) ||
       (GETBIT(fpga_read(ERR_LCK_STATE0_ADD),6)==1)||(GETBIT(fpga_read(ERR_LCK_STATE0_ADD),7)==1)||(GETBIT(fpga_read(ERR_LCK_STATE0_ADD),8)==1) ||
       (GETBIT(fpga_read(ERR_LCK_STATE0_ADD),12)==1)||(GETBIT(fpga_read(ERR_LCK_STATE0_ADD),13)==1)||(GETBIT(fpga_read(ERR_LCK_STATE0_ADD),14)==1)||
       (GETBIT(fpga_read(ERR_LCK_STATE2_ADD),11)==1) ||(GETBIT(fpga_read(ERR_LCK_STATE2_ADD),12)==1)||(GETBIT(fpga_read(ERR_LCK_STATE1_ADD),13)==1))
    {
        SETBIT(out_state_info.Sys_Err1,14);
    } 
	else 
	{
        out_state_info.Sys_Err1=RESETBIT(out_state_info.Sys_Err1,14);
    }
    /*交流过流 [15]*/		/*Sys_Err1 bit:(15)*/
    if((GETBIT(fpga_read(ERR_LCK_STATE1_ADD),2)==1)||(GETBIT(fpga_read(ERR_LCK_STATE1_ADD),3)==1)||(GETBIT(fpga_read(ERR_LCK_STATE1_ADD),4)==1) ||
       (GETBIT(fpga_read(ERR_LCK_STATE1_ADD),5)==1)||(GETBIT(fpga_read(ERR_LCK_STATE1_ADD),6)==1)||(GETBIT(fpga_read(ERR_LCK_STATE1_ADD),7)==1) ||
       (GETBIT(fpga_read(ERR_LCK_STATE1_ADD),8)==1)||(GETBIT(fpga_read(ERR_LCK_STATE1_ADD),9)==1)||(GETBIT(fpga_read(ERR_LCK_STATE1_ADD),10)==1)||
       (GETBIT(fpga_read(ERR_LCK_STATE2_ADD),14)==1)||(GETBIT(fpga_read(ERR_LCK_STATE2_ADD),15)==1)||(GETBIT(fpga_read(ERR_LCK_STATE3_ADD),0)==1))
    {
        SETBIT(out_state_info.Sys_Err1,15);
    } 
	else 
	{
        out_state_info.Sys_Err1=RESETBIT(out_state_info.Sys_Err1,15);
    }
	
}

static void do_Sys_Err2(void)
{

}

#if 0
static void do_pwm_on_off(void)
{
	short power_set;
	short power_set_zero_flag;
	short SettingSwitchCMD1;
	
	SettingSwitchCMD1 = fpga_read(Addr_Param111);
	if(GETBIT(SettingSwitchCMD1,8)==1)
	{
		power_set_zero_flag = 1;
	}
	else if(GETBIT(SettingSwitchCMD1,8)==0)
	{
		power_set_zero_flag = 0;
	}
	
	if(Pcs_Run_Mode_Get() == PQ_MODE)
	{
		power_set = fpga_read(Addr_Param26);
		if((power_set == 0) && (power_set_zero_flag == 0))
		{		
			miro_write.pwm_off_tick++;
			if(miro_write.pwm_off_tick > 5)
			{
				miro_write.pwm_off_tick = 0;
				SETBIT(SettingSwitchCMD1,8);
				fpga_write(Addr_Param111, SettingSwitchCMD1);
			}
		}
		else if((power_set !=0) && (power_set_zero_flag == 1))
		{
			SettingSwitchCMD1 = RESETBIT(SettingSwitchCMD1,8);
			fpga_write(Addr_Param111, SettingSwitchCMD1);
		}
	}
	else
	{		
		if(power_set_zero_flag == 1)
		{
			SettingSwitchCMD1 = RESETBIT(SettingSwitchCMD1,8);
			fpga_write(Addr_Param111, SettingSwitchCMD1);
		}
	}
}
#endif

static void do_Com_state(void)
{
	//bms_line_state

	
	//pcs_EMS_state
	if(EDGENPO1(pcs_manage.meter_off_line_flag, pcs_manage.meter_off_line_flag_old)==1) 
	{		
		log_add(ET_CBB, EST_EMS_OUTLINE);
		pcs_manage.meter_off_line_flag_old = pcs_manage.meter_off_line_flag;		
		SETBIT(out_state_info.com_err,0);
	}
	if(EDGENPO1(pcs_manage.meter_off_line_flag, pcs_manage.meter_off_line_flag_old)==2) 
	{		
		log_add(ET_CBB, EST_EMS_OUTLINE_REC);		
		pcs_manage.meter_off_line_flag_old = pcs_manage.meter_off_line_flag;
		out_state_info.com_err = RESETBIT(out_state_info.com_err,0);
	}
	
}

#if 0
osThreadId tid_ul_mode_task;
osThreadDef(ul_mode_task, osPriorityNormal, 1, 0);

#define UNIT_PU	8912

enum
{
	UL_MODE_NONE	 = 0,	//非美标
	UL_MODE_CONST_PF = 1,	//恒功率因数
	UL_MODE_VQ		 = 2,	//电压无功
	UL_MODE_PQ		 = 3,	//有功无功
	UL_MODE_CONST_Q	 = 4,	//恒无功
	UL_MODE_VP		 = 5,	//电压有功
	
	UL_MODE_MAX
};


ul_opt_info_t ulOptInfo;


static void rtu_data_init(ul_opt_info_t *pUoi)
{
	pUoi->pRtu->ul_mode = UL_MODE_NONE;
	pUoi->pRtu->p_cmd = fpga_pdatabuf_get[Addr_Param26];
	pUoi->pRtu->q_cmd = fpga_pdatabuf_get[Addr_Param27];
	pUoi->pRtu->uac_now = (int16_t)((fpga_pdatabuf_get[Addr_Param256] + fpga_pdatabuf_get[Addr_Param257] + fpga_pdatabuf_get[Addr_Param258]) / 3.0f);
	pUoi->pRtu->uac_rate = setting_data_handle(Addr_Param24, pUoi->pMs->Uac_rate * 10);
	pUoi->pRtu->p_rate = setting_data_handle(Addr_Param26, pUoi->pMs->P_rate * 10);
	pUoi->pRtu->p_max = (int16_t)(pUoi->pMs->P_max * UNIT_PU /100);
	pUoi->pRtu->cosPhi = (int16_t)(pUoi->pMs->PF_x100 * UNIT_PU /100);
	pUoi->pRtu->cosMin = (int16_t)(pUoi->pMs->PF_min_x100 * UNIT_PU /100);
	pUoi->pRtu->m2V1 = (int16_t)(pUoi->pMs->VQ_V1 * UNIT_PU /100);
	pUoi->pRtu->m2V2 = (int16_t)(pUoi->pMs->VQ_V2 * UNIT_PU /100);
	pUoi->pRtu->m2V3 = (int16_t)(pUoi->pMs->VQ_V3 * UNIT_PU /100);
	pUoi->pRtu->m2V4 = (int16_t)(pUoi->pMs->VQ_V4 * UNIT_PU /100);
	pUoi->pRtu->m2Q1 = (int16_t)(pUoi->pMs->VQ_Q1 * UNIT_PU /100);
	pUoi->pRtu->m2Q2 = (int16_t)(pUoi->pMs->VQ_Q2 * UNIT_PU /100);
	pUoi->pRtu->m2Q3 = (int16_t)(pUoi->pMs->VQ_Q3 * UNIT_PU /100);
	pUoi->pRtu->m2Q4 = (int16_t)(pUoi->pMs->VQ_Q4 * UNIT_PU /100);
	pUoi->pRtu->P3neg = (int16_t)(pUoi->pMs->PQ_n_P3 * UNIT_PU /100);
	pUoi->pRtu->P2neg = (int16_t)(pUoi->pMs->PQ_n_P2 * UNIT_PU /100);
	pUoi->pRtu->P2pos = (int16_t)(pUoi->pMs->PQ_p_P2 * UNIT_PU /100);
	pUoi->pRtu->P3pos = (int16_t)(pUoi->pMs->PQ_p_P3 * UNIT_PU /100);
	pUoi->pRtu->Q3neg = (int16_t)(pUoi->pMs->PQ_n_Q3 * UNIT_PU /100);
	pUoi->pRtu->Q2neg = (int16_t)(pUoi->pMs->PQ_n_Q2 * UNIT_PU /100);
	pUoi->pRtu->Q2pos = (int16_t)(pUoi->pMs->PQ_p_Q2 * UNIT_PU /100);
	pUoi->pRtu->Q3pos = (int16_t)(pUoi->pMs->PQ_p_Q3 * UNIT_PU /100);
	pUoi->pRtu->m3V1 = (int16_t)(pUoi->pMs->VP_V1 * UNIT_PU /100);
	pUoi->pRtu->m3V2 = (int16_t)(pUoi->pMs->VP_V2 * UNIT_PU /100);
	pUoi->pRtu->m3P1 = (int16_t)(pUoi->pMs->VP_P1 * UNIT_PU /100);
	pUoi->pRtu->m3P2 = (int16_t)(pUoi->pMs->VP_P2 * UNIT_PU /100);
}


static int rtu_data_update(ul_opt_info_t *pUoi)
{
	printf_debug8("\nul_mode[%d] p_cmd[%d] q_cmd[%d]\n", pUoi->pRtu->ul_mode, pUoi->pRtu->p_cmd, pUoi->pRtu->q_cmd);
	switch(pUoi->pRtu->ul_mode)
	{
		case UL_MODE_CONST_PF:
			pUoi->pRtu->p_cmd = fpga_pdatabuf_get[Addr_Param26];
			pUoi->pRtu->q_cmd = fpga_pdatabuf_get[Addr_Param27];
			pUoi->pRtu->p_rate = setting_data_handle(Addr_Param26, pUoi->pMs->P_rate * 10);
			pUoi->pRtu->p_max = (int16_t)(pUoi->pMs->P_max * UNIT_PU /100);
			pUoi->pRtu->cosPhi = (int16_t)(pUoi->pMs->PF_x100 * UNIT_PU /100);
			pUoi->pRtu->cosMin = (int16_t)(pUoi->pMs->PF_min_x100 * UNIT_PU /100);
			printf_debug8("---p_rate[%d] p_max[%d] cosPhi[%d] cosMin[%d]\n", pUoi->pRtu->p_rate, pUoi->pRtu->p_max, pUoi->pRtu->cosPhi, pUoi->pRtu->cosMin);
			break;

		case UL_MODE_VQ:
			pUoi->pRtu->p_cmd = fpga_pdatabuf_get[Addr_Param26];
			pUoi->pRtu->q_cmd = fpga_pdatabuf_get[Addr_Param27];
			pUoi->pRtu->uac_now = (int16_t)((fpga_pdatabuf_get[Addr_Param256] + fpga_pdatabuf_get[Addr_Param257] + fpga_pdatabuf_get[Addr_Param258]) / 3.0f);
			pUoi->pRtu->uac_rate = setting_data_handle(Addr_Param24, pUoi->pMs->Uac_rate * 10);
			pUoi->pRtu->p_rate = setting_data_handle(Addr_Param26, pUoi->pMs->P_rate * 10);
			pUoi->pRtu->p_max = (int16_t)(pUoi->pMs->P_max * UNIT_PU /100);
			pUoi->pRtu->m2V1 = (int16_t)(pUoi->pMs->VQ_V1 * UNIT_PU /100);
			pUoi->pRtu->m2V2 = (int16_t)(pUoi->pMs->VQ_V2 * UNIT_PU /100);
			pUoi->pRtu->m2V3 = (int16_t)(pUoi->pMs->VQ_V3 * UNIT_PU /100);
			pUoi->pRtu->m2V4 = (int16_t)(pUoi->pMs->VQ_V4 * UNIT_PU /100);
			pUoi->pRtu->m2Q1 = (int16_t)(pUoi->pMs->VQ_Q1 * UNIT_PU /100);
			pUoi->pRtu->m2Q2 = (int16_t)(pUoi->pMs->VQ_Q2 * UNIT_PU /100);
			pUoi->pRtu->m2Q3 = (int16_t)(pUoi->pMs->VQ_Q3 * UNIT_PU /100);
			pUoi->pRtu->m2Q4 = (int16_t)(pUoi->pMs->VQ_Q4 * UNIT_PU /100);
			printf_debug8("---p_rate[%d] p_max[%d] cosPhi[%d] cosMin[%d]\n", pUoi->pRtu->p_rate, pUoi->pRtu->p_max, pUoi->pRtu->cosPhi, pUoi->pRtu->cosMin);
			printf_debug8("---v1-v4[%d][%d][%d][%d]\n", pUoi->pRtu->m2V1, pUoi->pRtu->m2V2, pUoi->pRtu->m2V3, pUoi->pRtu->m2V4);
			printf_debug8("---q1-q4[%d][%d][%d][%d]\n", pUoi->pRtu->m2Q1, pUoi->pRtu->m2Q2, pUoi->pRtu->m2Q3, pUoi->pRtu->m2Q4);
			break;

		case UL_MODE_PQ:
			pUoi->pRtu->p_cmd = fpga_pdatabuf_get[Addr_Param26];
			pUoi->pRtu->q_cmd = fpga_pdatabuf_get[Addr_Param27];
			pUoi->pRtu->p_rate = setting_data_handle(Addr_Param26, pUoi->pMs->P_rate * 10);
			pUoi->pRtu->p_max = (int16_t)(pUoi->pMs->P_max * UNIT_PU /100);
			pUoi->pRtu->P3neg = (int16_t)(pUoi->pMs->PQ_n_P3 * UNIT_PU /100);
			pUoi->pRtu->P2neg = (int16_t)(pUoi->pMs->PQ_n_P2 * UNIT_PU /100);
			pUoi->pRtu->P2pos = (int16_t)(pUoi->pMs->PQ_p_P2 * UNIT_PU /100);
			pUoi->pRtu->P3pos = (int16_t)(pUoi->pMs->PQ_p_P3 * UNIT_PU /100);
			pUoi->pRtu->Q3neg = (int16_t)(pUoi->pMs->PQ_n_Q3 * UNIT_PU /100);
			pUoi->pRtu->Q2neg = (int16_t)(pUoi->pMs->PQ_n_Q2 * UNIT_PU /100);
			pUoi->pRtu->Q2pos = (int16_t)(pUoi->pMs->PQ_p_Q2 * UNIT_PU /100);
			pUoi->pRtu->Q3pos = (int16_t)(pUoi->pMs->PQ_p_Q3 * UNIT_PU /100);
			printf_debug8("---p_rate[%d] p_max[%d]\n", pUoi->pRtu->p_rate, pUoi->pRtu->p_max);
			printf_debug8("---pn32pp23[%d][%d][%d][%d]\n", pUoi->pRtu->P3neg, pUoi->pRtu->P2neg, pUoi->pRtu->P2pos, pUoi->pRtu->P3pos);
			printf_debug8("---qn32qp23[%d][%d][%d][%d]\n", pUoi->pRtu->Q3neg, pUoi->pRtu->Q2neg, pUoi->pRtu->Q2pos, pUoi->pRtu->Q3pos);
			break;

		case UL_MODE_CONST_Q:
			pUoi->pRtu->p_cmd = fpga_pdatabuf_get[Addr_Param26];
			pUoi->pRtu->q_cmd = fpga_pdatabuf_get[Addr_Param27];
			break;

		case UL_MODE_VP:
			pUoi->pRtu->p_cmd = fpga_pdatabuf_get[Addr_Param26];
			pUoi->pRtu->q_cmd = fpga_pdatabuf_get[Addr_Param27];
			pUoi->pRtu->uac_now = (int16_t)((fpga_pdatabuf_get[Addr_Param256] + fpga_pdatabuf_get[Addr_Param257] + fpga_pdatabuf_get[Addr_Param258]) / 3.0f);
			pUoi->pRtu->uac_rate = setting_data_handle(Addr_Param24, pUoi->pMs->Uac_rate * 10);
			pUoi->pRtu->p_rate = setting_data_handle(Addr_Param26, pUoi->pMs->P_rate * 10);
			pUoi->pRtu->p_max = (int16_t)(pUoi->pMs->P_max * UNIT_PU /100);
			pUoi->pRtu->m3V1 = (int16_t)(pUoi->pMs->VP_V1 * UNIT_PU /100);
			pUoi->pRtu->m3V2 = (int16_t)(pUoi->pMs->VP_V2 * UNIT_PU /100);
			pUoi->pRtu->m3P1 = (int16_t)(pUoi->pMs->VP_P1 * UNIT_PU /100);
			pUoi->pRtu->m3P2 = (int16_t)(pUoi->pMs->VP_P2 * UNIT_PU /100);
			printf_debug8("---p_rate[%d] p_max[%d] cosPhi[%d] cosMin[%d]\n", pUoi->pRtu->p_rate, pUoi->pRtu->p_max, pUoi->pRtu->cosPhi, pUoi->pRtu->cosMin);
			printf_debug8("---v12p12[%d][%d][%d][%d]\n", pUoi->pRtu->m3V1, pUoi->pRtu->m3V2, pUoi->pRtu->m3P1, pUoi->pRtu->m3P2);
			break;

		default:
			break;
	}
}


void ul_mode_task(const void *pdata)
{
	ul_opt_info_t *pUoi = (ul_opt_info_t *)pdata;
	
	rtu_data_init(pUoi);
	
	while(1)
	{
		if(GETBIT(pUoi->pMs->UL_strtg_en, 0)) {
			pUoi->pRtu->ul_mode = UL_MODE_CONST_PF;
		} else if(GETBIT(pUoi->pMs->UL_strtg_en, 1)){
			pUoi->pRtu->ul_mode = UL_MODE_VQ;
		} else if(GETBIT(pUoi->pMs->UL_strtg_en, 2)){
			pUoi->pRtu->ul_mode = UL_MODE_PQ;
		} else if(GETBIT(pUoi->pMs->UL_strtg_en, 3)){
			pUoi->pRtu->ul_mode = UL_MODE_CONST_Q;
		} else if(GETBIT(pUoi->pMs->UL_strtg_en, 4)){
			pUoi->pRtu->ul_mode = UL_MODE_VP;
		} else {
			pUoi->pRtu->ul_mode = UL_MODE_NONE;
		}

		if(pUoi->pRtu->ul_mode != UL_MODE_NONE) {
			pUoi->rtuCb(pUoi);
			feedback_control_step();
			fpga_write(Addr_Param26, pUoi->pRty->p_cmd_out);
			fpga_write(Addr_Param27, pUoi->pRty->q_cmd_out);
			printf_debug8("------p_cmd_out[%d] q_cmd_out[%d]\n", pUoi->pRty->p_cmd_out, pUoi->pRty->q_cmd_out);
		}

		osDelay(1000);
	}
}
#endif


void pcs_handle_task(const void *pdata)
{
	unsigned short cnt = 0, i = 0;
	short pcs_state;
	short SwitchCMD1;
	
	unsigned short time_ti=0, time_ti1=0,time_ti4=0; 
	
	unsigned short time_ti2=0, time_ti3=0; 
	//int life_count_tick2=0;
	//int time_ti2=0;	
	short off_line_power;
	short permit_open_flag;
	short break_down_flag;
	short time_open_tick=0;
	short re_open_flag;
	short tick_clearn;
	short tick_total;
	short tm_first_open_flag;
	short auto_clearn_flag;	

	short Udc_charge_limt;
	short Udc_discharge_limt;
	short Idc_charge_limt;
	short Idc_discharge_limt;

	short Udc_set;
	short Ubt_get;

	short Udc_set1; //电池维护模式下最后设定电压值

	short max_temp;
//	short min_temp;
	short delt_temp;
	short hight_temp_derating;
	short derating_Iac;
	short derating_Iac_set;
	
	int power_zero_tick=0;
	short SettingSwitchCMD1;
	
	short SettingSwitchCMD3;
	short power_run_value;	
	
	
	cnt = cnt;
	i = i;
	pcs_state = pcs_state;
	mb_data.last_onoff_mark = 0;
	sr_info.state9_last = fpga_read(STATE9_ADD);
    sr_info.state10_last = fpga_read(STATE10_ADD);
	sr_info.state11_last = fpga_read(STATE11_ADD);
#if 0
	ulOptInfo.pMs = &miro_write;
	ulOptInfo.pRtu = &rtU;
	ulOptInfo.pRty = &rtY;
	ulOptInfo.rtuCb = rtu_data_update;
	tid_ul_mode_task = osThreadCreate(osThread(ul_mode_task), (void *)&ulOptInfo);
#endif	
    
    while(1)
    {
		state8 = fpga_read(STATE8_ADD);
		pcs_state = state8 & 0xf;
		
		#if 1
		if(state8 != 0xffff) 
		{      
			do_fault_record();
            do_switch_record();
			do_cmd_record();
			do_strategy_record();
			//do_opsstate_record();            
		}
		#endif
		
		do_Sys_State1();
		//do_Sys_State2();
		do_Alarm_State1();
		//do_Alarm_State2();
		do_Sys_Err1();
		do_Sys_Err2();

	//AGC_state
	
			
	#if 1
	/*故障复位自动启动*/
	if((Pcs_Ctrl_Mode_Get()== EMS_CTRL)&&(GETBIT(miro_write.strategy_en,7)==1))		
	{
		if(miro_write.bootup_flag == 1)
		{
			if((fpga_read(ERR_LCK_STATE0_ADD) != 0 || fpga_read(ERR_LCK_STATE1_ADD) != 0 || fpga_read(ERR_LCK_STATE2_ADD) != 0 ||
				fpga_read(ERR_LCK_STATE3_ADD) != 0 || fpga_read(ERR_LCK_STATE4_ADD) != 0 || fpga_read(ERR_LCK_STATE5_ADD) != 0) 
				&& (GETBIT(fpga_read(ERR_LCK_STATE1_ADD),14)!= 1) && (GETBIT(fpga_read(ERR_LCK_STATE1_ADD),15)!= 1) &&
				(GETBIT(fpga_read(ERR_LCK_STATE2_ADD),0)!= 1) && (GETBIT(fpga_read(ERR_LCK_STATE2_ADD),10)!= 1) &&
				(GETBIT(fpga_read(ERR_LCK_STATE2_ADD),14)!= 1) && (GETBIT(fpga_read(ERR_LCK_STATE2_ADD),15)!= 1) &&			
				(GETBIT(fpga_read(ERR_LCK_STATE3_ADD),0)!= 1) && (GETBIT(fpga_read(ERR_LCK_STATE3_ADD),1)!= 1) &&
				(GETBIT(fpga_read(ERR_LCK_STATE3_ADD),2)!= 1) && (GETBIT(fpga_read(ERR_LCK_STATE3_ADD),3)!= 1) &&
				(GETBIT(fpga_read(ERR_LCK_STATE3_ADD),4)!= 1) && (GETBIT(fpga_read(ERR_LCK_STATE3_ADD),5)!= 1) &&
				(GETBIT(fpga_read(ERR_LCK_STATE3_ADD),6)!= 1) && (GETBIT(fpga_read(ERR_LCK_STATE3_ADD),7)!= 1) &&
				(GETBIT(fpga_read(ERR_LCK_STATE3_ADD),8)!= 1) && (GETBIT(fpga_read(ERR_LCK_STATE3_ADD),9)!= 1) &&
				(GETBIT(fpga_read(ERR_LCK_STATE3_ADD),15)!= 1) && (GETBIT(fpga_read(ERR_LCK_STATE4_ADD),0)!= 1) &&
				(GETBIT(fpga_read(ERR_LCK_STATE4_ADD),2)!= 1) && (GETBIT(fpga_read(ERR_LCK_STATE4_ADD),12)!= 1) &&
				(GETBIT(fpga_read(ERR_LCK_STATE4_ADD),13)!= 1) && (GETBIT(fpga_read(ERR_LCK_STATE4_ADD),14)!= 1) &&
				(GETBIT(fpga_read(ERR_LCK_STATE4_ADD),15)!= 1)) 
			{
				break_down_flag = 1;				
			}
			else
			{		
				break_down_flag = 0;			
			}
				
			if(break_down_flag == 1 && fpga_read(STATE0_ADD) == 0 && fpga_read(STATE1_ADD) == 0 && 
				fpga_read(STATE2_ADD) == 0 && fpga_read(STATE3_ADD) == 0 && fpga_read(STATE4_ADD) == 0) 
			{
				re_open_flag = 1;			
			}
			else
			{
				re_open_flag = 0;
			}
		
			if(re_open_flag == 1)
			{			
				//tty_printf("enter clearn err\n");
				set_pcs_shutdown(); 		
				log_add(ET_OPSTATE,EST_CLOSE);	//清除故障	
				auto_clearn_flag = 1;					
				mb_data.last_onoff_mark = 1;	
				miro_write.recover_flag = 1;
			}
			
			/***清除故障后计时***/
			if(auto_clearn_flag == 1)
			{
				tick_clearn++;		
				if(tick_clearn >= 10)
				{				
					permit_open_flag = 1;
					tick_clearn = 0;
					auto_clearn_flag = 0;
				}
				else
				{
					permit_open_flag = 0;
				}				
			}
				
			if(permit_open_flag == 1)
			{
				set_pcs_bootup();			
				log_add(ET_OPSTATE,EST_OPEN);					
				mb_data.last_onoff_mark = 2;
				miro_write.recover_flag = 0;
				time_open_tick++;		
					
				//tty_printf("重启动次数 %d\n",time_open_tick);
				//tty_printf("enter re_open_pcs\n");
					
				if(time_open_tick==1)
				{			
					tm_first_open_flag = 1; 			
				}
							
				if(time_open_tick >= 5)
				{
					miro_write.bootup_flag = 0; 
					time_open_tick = 0;
					miro_write.auto_recover_flag = 1;
				}
				else
				{
					miro_write.auto_recover_flag = 0;
				}
				permit_open_flag = 0;			
			}		
		}
			
		if(tm_first_open_flag == 1)
		{
			tick_total++;
			if(tick_total >= 3600)
			{
				miro_write.bootup_flag = 1; 				
				time_open_tick = 0;
				tick_total = 0;
				tm_first_open_flag = 0;
				//tty_printf("entern 重启动次数清零\n");		
			}		
		}
				
	}
	#endif
	
	#if 0
	/*直流电压满足条件自动开机功能*/
	if(GETBIT(miro_write.strategy_en,2)==1 && (miro_write.recover_flag == 0))	
	{			

		Ubt_get = (short)(getting_data_handle(Addr_Param126, Upper_Param[Addr_Param126])/10);
		if((Ubt_get >= arm_config_data_read(MIN_BA_U_ADD)) && (Ubt_get <= arm_config_data_read(MAX_BA_U_ADD)) &&(pcs_state == FSM_IDLE))
		{
			if(fpga_read(STATE0_ADD) == 0 && fpga_read(STATE1_ADD) == 0 && 
				fpga_read(STATE2_ADD) == 0 && fpga_read(STATE3_ADD) == 0 && fpga_read(STATE4_ADD) == 0)
			{
				set_pcs_bootup();					
				log_add(ET_OPSTATE,EST_AUTO_OPEN);
				miro_write.bootup_flag = 1;
			}			
		}
		else if(((Ubt_get > arm_config_data_read(MAX_BA_U_ADD)) || (Ubt_get < arm_config_data_read(MIN_BA_U_ADD))) && (pcs_state == FSM_WORKING))
		{
			set_pcs_shutdown();			
			log_add(ET_OPSTATE,EST_AUTO_CLOSE);
		}
	}
	#endif


	do_Com_state();

	#if 0
	/*电池维护模式策略*/
	if((GETBIT(miro_write.strategy_en,3) == 1) && (miro_write.enter_cu_mode_flag == 0) &&(miro_write.pre_mode_end_flag == 0))
	{
		Udc_set = getting_data_handle(Addr_Param23, Upper_Param[Addr_Param23]);
		Ubt_get = getting_data_handle(Addr_Param126, Upper_Param[Addr_Param126]);
		if(miro_write.PRE_mode_first_set == 1)
		{
			Pcs_Run_Mode_Set(IV_MODE);
			Udc_charge_limt = setting_data_handle(Addr_Param31, (short)((-1)*(arm_config_data_read(MAX_P_ADD))*MODULUS));		
			Udc_discharge_limt = setting_data_handle(Addr_Param32, (short)((arm_config_data_read(MAX_P_ADD))*MODULUS));	
			Idc_charge_limt = setting_data_handle(Addr_Param33, (short)((-1)*(arm_config_data_read(MAX_BA_C_ADD))*MODULUS));		
			Idc_discharge_limt = setting_data_handle(Addr_Param34, (short)((arm_config_data_read(MAX_BA_C_ADD))*MODULUS));	
			fpga_write(Addr_Param31,Udc_charge_limt);		
			fpga_write(Addr_Param32,Udc_discharge_limt);
			fpga_write(Addr_Param33,Idc_charge_limt);		
			fpga_write(Addr_Param34,Idc_discharge_limt);
			miro_write.PRE_mode_first_set = 0;
		}
		if((abs(Udc_set - Ubt_get)<= 5))
		{
			miro_write.cu_mode_tick++;
			if(miro_write.cu_mode_tick == 3600)
			{
				miro_write.enter_cu_mode_flag = 1; //进入恒压模式
			}
		}
		else
		{
			miro_write.cu_mode_tick = 0;
		}	
		
		if((miro_write.enter_cu_mode_flag == 1) &&(miro_write.pre_mode_end_flag == 0))
		{
			Udc_set = getting_data_handle(Addr_Param23, Upper_Param[Addr_Param23]);
			Udc_set1 = (short)(Udc_set - 30);
			Udc_set1 = GETMAX(Udc_set1,(arm_config_data_read(MIN_BA_U_ADD)));
			Udc_set1 = setting_data_handle(Addr_Param23, (short)((Udc_set1)*MODULUS));		
			fpga_write(Addr_Param23,Udc_set1);	
			miro_write.enter_cu_mode_flag = 0;	
			miro_write.pre_mode_end_flag = 1;
		}
	}
	
	/*SOC标定模式策略*/
	if((GETBIT(miro_write.strategy_en,5)==1)&&(miro_write.soc_mode_end_flag ==0))
	{
		Udc_set = getting_data_handle(Addr_Param23, Upper_Param[Addr_Param23]);
		Ubt_get = getting_data_handle(Addr_Param126, Upper_Param[Addr_Param126]);
		
		if(miro_write.SOC_mode_first_set == 1)
		{			
			Pcs_Run_Mode_Set(IV_MODE);
			Udc_charge_limt = setting_data_handle(Addr_Param31, (short)((-0.25)*(arm_config_data_read(MAX_P_ADD))*MODULUS));		
			fpga_write(Addr_Param31,Udc_charge_limt);
			miro_write.SOC_mode_first_set = 0;			
		}
		if((abs(Udc_set - Ubt_get)<= 5))
		{
			miro_write.soc_charge_tick++;		
			if(miro_write.soc_charge_tick==600)
			{
				miro_write.soc_charge_ok_flag = 1; //充满
			}
		}
		else
		{
			miro_write.soc_charge_tick = 0;
		}

		if((miro_write.soc_charge_ok_flag == 1) &&(miro_write.soc_mode_end_flag ==0))
		{
			Udc_discharge_limt = setting_data_handle(Addr_Param32, (short)((0.25)*(arm_config_data_read(MAX_P_ADD))*MODULUS));		
			fpga_write(Addr_Param32,Udc_discharge_limt);	
			Udc_set1 = arm_config_data_read(MIN_BA_U_ADD);
			Udc_set1 = setting_data_handle(Addr_Param23, (short)((Udc_set1)*MODULUS));			
			miro_write.soc_charge_ok_flag = 0;
			miro_write.soc_mode_end_flag = 1;
		}
		
		
	}	
	
	/*均压维护模式策略*/
	if((GETBIT(miro_write.strategy_en,6)==1))
	{
		if(miro_write.CU_mode_first_set == 1)
		{
			Pcs_Run_Mode_Set(IV_MODE);
			Udc_charge_limt = setting_data_handle(Addr_Param31, (short)((-1)*(50)*MODULUS));		
			Udc_discharge_limt = setting_data_handle(Addr_Param32, (short)((50)*MODULUS));
			fpga_write(Addr_Param31,Udc_charge_limt);		
			fpga_write(Addr_Param32,Udc_discharge_limt);
			miro_write.CU_mode_first_set = 0;
		}
	}	
	#endif
	
	#if 1
	/*高温降额*/
	if((GETBIT(miro_write.strategy_en,10)==1))
	{
		hight_temp_derating = arm_config_data_read(HIGHT_TEMP_DERAT);	
		if((Upper_Param[Addr_Param196] > hight_temp_derating) || (Upper_Param[Addr_Param196] > hight_temp_derating) ||(Upper_Param[Addr_Param198] > hight_temp_derating))
		{		
			if(Upper_Param[Addr_Param196] > Upper_Param[Addr_Param197])
			{
				max_temp = Upper_Param[Addr_Param196];
			}
			else
			{
				max_temp = Upper_Param[Addr_Param197];

			}
			if(max_temp > Upper_Param[Addr_Param198])
			{
				max_temp = max_temp;
			}
			else
			{
				max_temp = Upper_Param[Addr_Param198];
			}
			
			if(max_temp <=(hight_temp_derating+10))
			{
				delt_temp = (short)((max_temp - hight_temp_derating));
				derating_Iac = (short)((arm_config_data_read(MAX_IAC_ADD)*MODULUS)-(delt_temp*arm_config_data_read(MAX_IAC_ADD)));
				derating_Iac_set = setting_data_handle(Addr_Param28,derating_Iac);				
				fpga_write(Addr_Param28,derating_Iac_set);
				miro_write.pcs_derat_flag = 1;
			}
			else
			{			
				fpga_write(Addr_Param28,0);			
				miro_write.pcs_derat_flag = 1;
			}
		}	
				
		else if((miro_write.pcs_derat_flag == 1) && (Upper_Param[Addr_Param196] <= (short)(hight_temp_derating-5)) &&
				(Upper_Param[Addr_Param197] <= (short)(hight_temp_derating-5)) &&(Upper_Param[Addr_Param198] <= (short)(hight_temp_derating-5)))
		{
			derating_Iac = setting_data_handle(Addr_Param28,(short)(arm_config_data_read(MAX_IAC_ADD)*10));				
			fpga_write(Addr_Param28,derating_Iac);	
			miro_write.pcs_derat_flag = 0;
		}
	}
	#endif
	
	
	#if 1
	/*交流脉冲封锁使能策洛*/
	if((GETBIT(miro_write.strategy_en,8)==1))
	{	
		#if 1
		if(Pcs_Ctrl_Mode_Get()== EMS_CTRL)
		{
			if((miro_write.power_zero_flag == 1) && (miro_write.pwm_off_flag == 0))
			{
				power_zero_tick++;
				if(power_zero_tick >= 10)
				{						
					SettingSwitchCMD1 = fpga_read(Addr_Param111);
					SETBIT(SettingSwitchCMD1,8);
					fpga_write(Addr_Param111, SettingSwitchCMD1);	
					miro_write.pwm_off_flag = 1;
				}
			}
			else
			{
				power_zero_tick = 0;
			}
		}
			
		//交流脉冲封锁解除(本地模式下)
		if(Pcs_Ctrl_Mode_Get()== HMI_CTRL)
		{		
				if(miro_write.pwm_off_flag == 1)
				{			
					fpga_write(Addr_Param26,0); 		
					osDelay(20);
					SettingSwitchCMD1 = fpga_read(Addr_Param111);
					SettingSwitchCMD1 = RESETBIT(SettingSwitchCMD1,8);
					fpga_write(Addr_Param111, SettingSwitchCMD1);	
					//miro_write.pwm_shutdown_flag = 0; 		
					miro_write.pwm_off_flag = 0;
				}
		
		}	
		#endif	
	}
	else
	{		
		SwitchCMD1 = fpga_read(Addr_Param111);
		if(GETBIT(SwitchCMD1,8)==1)
		{
			SwitchCMD1 = RESETBIT(SwitchCMD1,8);
			fpga_write(Addr_Param111, SwitchCMD1);
		}
	}
	#endif	

	#if 1
	//参数版本号处理
	miro_write.para_vision=0;
	for(i = 0; i < 26; i++) 
	{ 
		miro_write.para_vision += fpga_pdatabuf_get[i]; 			
	}			
	for(i = 35; i < 120; i++) 
	{ 
		miro_write.para_vision += fpga_pdatabuf_get[i]; 			
	}	
	miro_write.para_vision += arm_config_data_read(COMMUICATION_PRO_EN);
	miro_write.para_vision += arm_config_data_read(STRATEGY_EN);
	#endif


	#if 0
	//增加密钥有效期判定			
	if(arm_config_data_read(CODE_REMAIN_DATE) != 9999 && arm_config_data_read(CODE_REMAIN_DATE) >0)
	{
		miro_write.code_remain_tick++;
		count_cal[CODE_DAY_COUNT] = miro_write.code_remain_tick;
		if(miro_write.code_remain_tick>=172800)
		//if(miro_write.code_remain_tick>=30)
		{
			miro_write.code_remain_date = miro_write.code_remain_date -1;
			arm_config_data_write(CODE_REMAIN_DATE,miro_write.code_remain_date);   
			miro_write.code_remain_tick = 0;
		}
	}
	#endif

	#if 0
	//密钥过期处理
	if(miro_write.code_remain_date == 0)
	{
		if((state8 & 0xf) == FSM_WORKING) //密钥到期后设备运行就停机
		{
			set_pcs_shutdown(); 		
			log_add(ET_OPSTATE,EST_CODE_INVALID);	//密钥到期停机
		}	
		miro_write.code_remain_tick = 0;
	}
	#endif


	#if 1
	//交流柜散热处理
	if((collect_data.ac_cabinet_temp > arm_config_data_read(AC_FUN_START_TEMP_REC)) && 
		(collect_data.ac_cabinet_temp < arm_config_data_read(AC_CABINET_ERR_TEMP_REC))) 
	{
		
		collect_data.ac_fun_run_flag = 1;
		SettingSwitchCMD3 = fpga_read(Addr_Param113);
		SettingSwitchCMD3 = SETBIT(SettingSwitchCMD3,4);
		fpga_write(Addr_Param113, SettingSwitchCMD3);			
	}
	if((collect_data.ac_fun_run_flag == 1) && 
		(collect_data.ac_cabinet_temp < arm_config_data_read(AC_FUN_STOP_TEMP_REC)))
	{
		collect_data.ac_fun_run_flag = 0;
		SettingSwitchCMD3 = fpga_read(Addr_Param113);
		SettingSwitchCMD3 = RESETBIT(SettingSwitchCMD3,4);
		fpga_write(Addr_Param113, SettingSwitchCMD3);	
	}
	
	//过温判定
	if((collect_data.filter_capacitance_temp1 > arm_config_data_read(FILTER_CAPAC_ERR_TEMP_REC)) ||	
		(collect_data.filter_capacitance_temp2 > arm_config_data_read(FILTER_CAPAC_ERR_TEMP_REC)))
	{
		set_pcs_shutdown(); 		
		log_add(ET_FAULT,EST_Filter_Temp_Over);	//滤波电容温度过温停机	
	}
	
	//过温判定
	if(collect_data.transformer_temp > arm_config_data_read(TRANSFORMER_ERR_TEMP_REC))		
	{
		set_pcs_shutdown(); 		
		log_add(ET_FAULT,121);	
	}

	//过温判定
	if(collect_data.bus_capacitance_temp > arm_config_data_read(BUS_CAPAC_ERR_TEMP_REC))		
	{
		set_pcs_shutdown(); 		
		log_add(ET_FAULT,122);	
	}

	//过温判定
	if(collect_data.electric_reactor_temp > arm_config_data_read(ELEC_REACTOR_ERR_TEMP_REC))		
	{
		set_pcs_shutdown(); 		
		log_add(ET_FAULT,123);	
	}

	//过温判定
	if(collect_data.ac_cabinet_temp > arm_config_data_read(AC_CABINET_ERR_TEMP_REC))		
	{
		set_pcs_shutdown(); 		
		log_add(ET_FAULT,124);	
	}

	#endif

	#if 1	
	SettingSwitchCMD3 = fpga_read(Addr_Param113);
	if(((state8 & 0xf)>FSM_IDLE)&&((state8 & 0xf)<7)) 			
	{
		SETBIT(SettingSwitchCMD3,4);
		fpga_write(Addr_Param113, SettingSwitchCMD3);	
		
		power_run_value = abs(getting_data_handle(Addr_Param267, Upper_Param[Addr_Param267]));
		if(power_run_value >2500)
		{
			miro_write.speed_value = 100;
		}
		else
		{		
			miro_write.speed_value = 60;
		}
	}
	else
	{
		SettingSwitchCMD3 = RESETBIT(SettingSwitchCMD3,4);
		fpga_write(Addr_Param113, SettingSwitchCMD3);		
		miro_write.speed_value = 0;
	}
	#endif

    #if 1
    if((pcs_manage.temp_a_max>900) ||(pcs_manage.temp_b_max>900) || (pcs_manage.temp_b_max>900))
    {
		set_pcs_shutdown(); 		
		log_add(ET_FAULT,110);	
    }
    #endif

	printf_debug7("\n");
	for(uint8_t i = 0; i < 7; i++) {
		printf_debug7("ad1[%d]=%d  ad3[%d]=%d\n", i, ad1_dma_buf[i], i, ad3_dma_buf[i]);
	}

	osDelay(500);
	}
}
