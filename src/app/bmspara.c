#include <stdio.h>
#include <string.h>

#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "rl_net.h"
#include "cmsis_os.h"
#include "para.h"
#include "fpga.h"
#include "pcs.h"
#include "modbus_comm.h"
#include "modbus_master.h"
#include "micro.h"
#include "log.h"
#include "sys.h"
#include "tty.h"
#include "pcs.h"
#include "bms_ems.h"

//pcs_manage_t pcs_manage;
static uint8_t net_ems_recv_buf[256];
static uint8_t net_ems_send_buf[256];
_MonitorData MonitorData;

extern osThreadId tid_bms_task;
void bms_ems_task(const void *pdata);

extern short arm_config_data_read(unsigned short id);
extern void arm_config_data_write(unsigned short id, short value);
extern short getting_data_handle(unsigned char addr, short value);
extern int mb_send (int sock, const char *buf, int len, int flags);
extern void thread_delete_selfmb(void);

static void bms_handle_task(const void *pdada);
osThreadDef(bms_handle_task, osPriorityNormal, 1, 0);

/************ BMS电池写入参数**********/

int bms_read_reg(uint16_t addr, uint16_t *value)
{
    int err;
    err = 0;
	pcs_manage.bms_life_count++;
    switch(addr)
    {
		
    case 0:
		 *value = MonitorData.life_count_h;
		break;
		
	case 1:
		*value = pcs_manage.bms_life_count;	
		break;
		
	case 2:
		*value = MonitorData.sysstate_h;
		break;
		
	case 3:
		*value = MonitorData.sysstate_l;		
		break;
		
	case 4:
		*value = MonitorData.discharge_pre_power_h;	
		break;
		
	case 5:
		*value = MonitorData.discharge_pre_power_l;
		break;
		
	case 6:
		*value = MonitorData.charge_pre_power_h;
		break;
		
	case 7:
		*value = MonitorData.charge_pre_power_l;
		break;	
		
	case 8:
		*value = MonitorData.q_discharge_h;
		break;
		
	case 9:
		*value = MonitorData.q_discharge_l;
		break;			
		
	case 10:
		*value = MonitorData.q_charge_h;
		break;	
		
	case 11:
		*value = MonitorData.q_charge_l;
		break;
		
	case 12:
		*value = MonitorData.bms_voltalge_h;	
		break;
		
	case 13:
		*value = MonitorData.bms_voltalge_l;
		break;	
		
	case 14:
		*value = MonitorData.bms_current_h;
		break;
		
	case 15:
		*value = MonitorData.bms_current_l;
		break;
		
	case 16:
		*value = MonitorData.bms_soc_h;
		break;		
		
	case 17:
		*value = MonitorData.bms_soc_l;
		break;
		
	case 18:
		*value = MonitorData.battery1_waring;
		break;
				
	case 19:
		*value = MonitorData.battery1_err;
		break;
				
	case 20:
		*value = MonitorData.battery2_waring;
		break;
		
	case 21: 
		*value = MonitorData.battery2_err;
		break;
		
	case 22: 
		*value = MonitorData.battery3_waring;
		break;
		
	case 23: 
		*value = MonitorData.battery3_err;
		break;
		
	case 24: 
		*value = MonitorData.battery4_waring;
		break;
		
	case 25: 
		*value = MonitorData.battery4_err;
		break;
		
	case 26: 
		*value = MonitorData.battery5_waring;
		break;
		
	case 27: 
		*value = MonitorData.battery5_err;
		break;
		
	case 28: 
		*value = MonitorData.battery6_waring;
		break;
		
	case 29:
		*value = MonitorData.battery6_err;
		break;
		
	case 30:
		*value = MonitorData.battery7_waring;
		break;
		
	case 31:
		*value = MonitorData.battery7_err;
		break;
		
	case 32:
		*value = MonitorData.battery8_waring;
		break;
		
	case 33:
		*value = MonitorData.battery8_err;
		break;
		
	case 34:
		*value = MonitorData.battery9_waring;
		break;
		
	case 35:
		*value = MonitorData.battery9_err;
		break;
			
	case 36:
		*value = MonitorData.battery10_waring;
		break;	

	case 37:
		*value = MonitorData.battery10_err;
		break;	

	case 38:
		*value = out_state_info.Sys_State1;
		break;	
		
	case 39:
		*value = getting_data_handle(Addr_Param131, Upper_Param[Addr_Param131]);
		break;	

		
    default:
        err = 0;

    }
    return err;
}

int bms_write_reg(uint16_t addr, uint16_t value)
{
    int err;
    err = 0;
	pcs_manage.bms_life_count++;
	switch(addr)
	{
	case 0:
		MonitorData.life_count_h = value;			
		break;
		
	case 1:
		MonitorData.life_count_l = value;		
		break;
		
	case 2:
		MonitorData.sysstate_h = value;
		break;
		
	case 3:
		MonitorData.sysstate_l = value;		
		break;
		
	case 4:
		MonitorData.discharge_pre_power_h = value;		
		break;
		
	case 5:
		MonitorData.discharge_pre_power_l = value;
		break;
		
	case 6:
		MonitorData.charge_pre_power_h = value;
		break;
		
	case 7:
		MonitorData.charge_pre_power_l = value;
		break;	
		
	case 8:
		MonitorData.q_discharge_h = value;
		break;
		
	case 9:
		MonitorData.q_discharge_l = value;
		break;	
		
	case 10:
		MonitorData.q_charge_h = value;
		break;
		
	case 11:
		MonitorData.q_charge_l = value;		
		break;
		
	case 12:
		MonitorData.bms_voltalge_h = value;
		break;	
		
	case 13:
		MonitorData.bms_voltalge_l = value;
		break;
		
	case 14:
		MonitorData.bms_current_h = value;
		break;
		
	case 15:
		MonitorData.bms_current_l = value;
		break;		
		
	case 16:
		MonitorData.bms_soc_h = value;
		break;
		
	case 17:
		MonitorData.bms_soc_l = value;
		break;
				
	case 18:
		MonitorData.battery1_waring = value;
		break;
		
	case 19:
		MonitorData.battery1_err = value;
		break;
		
	case 20:
		MonitorData.battery2_waring = value;
		break;
		
	case 21:
		MonitorData.battery2_err = value;
		break;

	case 22:
		MonitorData.battery3_waring = value;
		break;
		
	case 23:
		MonitorData.battery3_err = value;
		break;

	case 24:
		MonitorData.battery4_waring = value;
		break;
		
	case 25:
		MonitorData.battery4_err = value;
		break;

	case 26:
		MonitorData.battery5_waring = value;
		break;
		
	case 27:
		MonitorData.battery5_err = value;
		break;

	case 28:
		MonitorData.battery6_waring = value;
		break;
		
	case 29:
		MonitorData.battery6_err = value;
		break;		

	case 30:
		MonitorData.battery7_waring = value;
		break;
		
	case 31:
		MonitorData.battery7_err = value;
		break;

	case 32:
		MonitorData.battery8_waring = value;
		break;
		
	case 33:
		MonitorData.battery8_err = value;
		break;

	case 34:
		MonitorData.battery9_waring = value;
		break;
		
	case 35:
		MonitorData.battery9_err = value;
		break;

	case 36:
		MonitorData.battery10_waring = value;
		break;
		
	case 37:
		MonitorData.battery10_err = value;
		break;
			
    default:
        err = 0;
    }
		
    return err;
}


int bms_read_holding_reg(uint8_t *recv_buf, uint8_t *resp_buf, int datalen)
{
    uint16_t saddr, num, i;
    uint16_t reg;
    uint8_t err = 0;

    saddr = (recv_buf[0] << 8) + recv_buf[1];
    num = (recv_buf[2] << 8) + recv_buf[3];

    if(num < 1 || num > 125)
    {
        err = 3;
    }
    else
    {
        for(i = 0; i < num; i++)
        {
            if(bms_read_reg(saddr + i, &reg) != 0)
            {
                err = 2;
                break;
            }

            resp_buf[(i << 1) + 2] = reg >> 8;
            resp_buf[(i << 1) + 3] = reg & 0xff;
        }
    }

    if(err == 0)
    {
        resp_buf[0] = 0x03;
        resp_buf[1] = i << 1;
        return 2 + 2 * i;
    }
    else
    {
        resp_buf[0] = 0x83;
        resp_buf[1] = err;
        return 2;
    }
}

int bms_read_input_reg(uint8_t *recv_buf, uint8_t *resp_buf, int datalen)
{
    uint16_t saddr, num, i;
    uint16_t reg;
    uint8_t err = 0;

    saddr = (recv_buf[0] << 8) + recv_buf[1];
    num = (recv_buf[2] << 8) + recv_buf[3];

    if(num < 1 || num > 125)
    {
        err = 3;
    }
    else
    {
        for(i = 0; i < num; i++)
        {
            if(bms_read_reg(saddr + i, &reg) != 0)
            {
                err = 2;
                break;
            }

            resp_buf[(i << 1) + 2] = reg >> 8;
            resp_buf[(i << 1) + 3] = reg & 0xff;
        }
    }

    if(err == 0)
    {
        resp_buf[0] = 0x04;
        resp_buf[1] = i << 1;
        return 2 + 2 * i;
    }
    else
    {
        resp_buf[0] = 0x84;
        resp_buf[1] = err;
        return 2;
    }
}

int bms_write_single_reg(uint8_t *recv_buf, uint8_t *resp_buf, int datalen)
{
    uint16_t saddr;
    uint16_t value;
    uint8_t err = 0;

    saddr = (recv_buf[0] << 8) + recv_buf[1];
    value = (recv_buf[2] << 8) + recv_buf[3];

    err = bms_write_reg(saddr, value);

    if(err == 0)
    {
        resp_buf[0] = 0x06;
        resp_buf[1] = saddr >> 8;
        resp_buf[2] = saddr & 0xff;
        resp_buf[3] = value >> 8;
        resp_buf[4] = value & 0xff;
        return 5;
    }
    else
    {
        resp_buf[0] = 0x86;
        resp_buf[1] = err;
        return 2;
    }
}

int bms_write_mul_reg(uint8_t *recv_buf, uint8_t *resp_buf, int datalen)
{
    uint16_t saddr, num, bytecount, i;
    uint16_t value;
    uint8_t err = 0;

    saddr = (recv_buf[0] << 8) + recv_buf[1];
    num = (recv_buf[2] << 8) + recv_buf[3];
    bytecount = recv_buf[4];

    if(num < 1 || num > 125 || (bytecount != (num << 1)))
    {
        err = 3;
    }
    else
    {
        for(i = 0; i < num; i++)
        {
            value = (recv_buf[5 + (i << 1)] << 8) + (recv_buf[6 + (i << 1)]);
            err = bms_write_reg(saddr + i, value);

            if(err != 0)
            {
                break;
            }
        }
    }

    if(err == 0)
    {
        resp_buf[0] = 0x10;
        resp_buf[1] = saddr >> 8;
        resp_buf[2] = saddr & 0xff;
        resp_buf[3] = num >> 8;
        resp_buf[4] = num & 0xff;
        return 5;
    }
    else
    {
        resp_buf[0] = 0x90;
        resp_buf[1] = err;
        return 2;
    }
}

int bms_rw_mul_reg(uint8_t *recv_buf, uint8_t *resp_buf, int datalen)
{
    uint16_t rsaddr, wsaddr, rnum, wnum, bytecount, i;
    uint16_t reg, value;
    uint8_t err = 0;

    rsaddr = (recv_buf[0] << 8) + recv_buf[1];
    rnum = (recv_buf[2] << 8) + recv_buf[3];
    wsaddr = (recv_buf[4] << 8) + recv_buf[5];
    wnum = (recv_buf[6] << 8) + recv_buf[7];
    bytecount = recv_buf[8];

    if(rnum < 1 || rnum > 125 || wnum < 1 || wnum > 125 ||
       (bytecount != wnum << 1))
    {
        err = 3;
    }
    else
    {
        for(i = 0; i < wnum; i++)
        {
            value = recv_buf[9 + (i << 1)] << 8 + recv_buf[10 + (i << 1)];

            if(bms_write_reg(wsaddr + i, value))
            {
                err = 2;
                break;
            }
        }

        if(err == 0)
        {
            for(i = 0; i < rnum; i++)
            {
                if(bms_read_reg(rsaddr + i, &reg) != 0)
                {
                    err = 2;
                    break;
                }

                resp_buf[(i << 1) + 2] = (reg) >> 8;
                resp_buf[(i << 1) + 3] = (reg) & 0xff;
            }
        }
    }

    if(err == 0)
    {
        resp_buf[0] = 0x17;
        resp_buf[1] = i << 1;
        return 2 + (i << 1);
    }
    else
    {
        resp_buf[0] = 0x97;
        resp_buf[1] = err;
        return 2;
    }
}

static int bms_ems_recv_frame(int mbsd, int *frame_len)
{
    int leftlen;
    int res;
    char *recv_buf = (char *)net_ems_recv_buf;

    *frame_len = 0;
    res = recv(mbsd, recv_buf, 6, 0);

    if(res <= 0)
    {
        return 1;
    }

    leftlen = (recv_buf[4] << 8) | recv_buf[5];

    res = recv(mbsd, &recv_buf[6], leftlen, 0);

    if(res <= 0)
    {
        return 1;
    }

    *frame_len = res + 6;

    return 0;
}

static int bms_ems_resp_frame(int mbsd, int frame_len)
{
    int err;
    int leftlen;
    int resplen = 0;
    uint8_t *recv_buf = net_ems_recv_buf;
    uint8_t *resp_buf = net_ems_send_buf;

    resp_buf[0] = recv_buf[0];
    resp_buf[1] = recv_buf[1];
    resp_buf[2] = recv_buf[2];
    resp_buf[3] = recv_buf[3];

    leftlen = (recv_buf[4] << 8) + recv_buf[5];

    if(frame_len < (leftlen + 6) || recv_buf[2] != 0 || recv_buf[3] != 0)
    {
        return 0;
    }

    resp_buf[6] = recv_buf[6];

    switch(recv_buf[7])
    {
    case 0x03:
        resplen = bms_read_holding_reg(&recv_buf[8], &resp_buf[7], leftlen - 1);
        break;

    case 0x04:
        resplen = bms_read_input_reg(&recv_buf[8], &resp_buf[7], leftlen - 1);
        break;

    case 0x06:
        resplen = bms_write_single_reg(&recv_buf[8], &resp_buf[7], leftlen - 1);
        break;

    case 0x10:
        resplen = bms_write_mul_reg(&recv_buf[8], &resp_buf[7], leftlen - 1);
        break;

    case 0x17:
        resplen = bms_rw_mul_reg(&recv_buf[8], &resp_buf[7], leftlen - 1);
        break;

    default:
        return 0;
    }

    resp_buf[4] = (resplen + 1) >> 8;
    resp_buf[5] = (resplen + 1) & 0xff;

    err = mb_send(mbsd, (char *)resp_buf, resplen + 7, 0);

    if(err <= 0)
    {
        return 1;
    }
    return 0;
}

#if 0
static void bms_handle_task(const void *pdada)
{
    int err;
    int frame_len;	
	int mbsd;
	mbsd = (int)pdada;

    while(1)
    {
        err = bms_ems_recv_frame(mbsd, &frame_len);

        if(err != 0)
        {
            break;
        }

        err = bms_ems_resp_frame(mbsd, frame_len);

        if(err != 0)
        {
            break;
        }
       	osDelay(2);		
    }
    closesocket(mbsd);	
	thread_delete_selfmb();
}

void bms_ems_task(const void *pdata)
{

	SOCKADDR_IN addr;
    int mbsock, mbsd;	
	
    mbsock = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_port = htons(1314);
    addr.sin_family = PF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(mbsock, (SOCKADDR *)&addr, sizeof(addr));
    listen(mbsock,3);

    while(1)
    {   
        mbsd = accept(mbsock, NULL, NULL);//等待连接
        if(mbsd > 0)
        {        
    
			if(osThreadCreate(osThread(bms_handle_task), (void *)mbsd) == NULL)
			{
				 closesocket(mbsd);
				 continue;
			 }
        }
        osDelay(2);
    }
}
#endif

#if 1
static void bms_handle_task(const void *pdada)
{
    int err;
    int frame_len;
		NET_TASK_TYPE1 task_type1;
	
		task_type1 = *((NET_TASK_TYPE1 *)pdada);
	
    while(1)
    {
        err = bms_ems_recv_frame(task_type1.mbsd, &frame_len);

        if(err != 0)
        {
            break;
        }

        err = bms_ems_resp_frame(task_type1.mbsd, frame_len);

        if(err != 0)
        {
            break;
        }
		
       	osDelay(2);
    }
    closesocket(task_type1.mbsd);	
		((NET_TASK_TYPE1 *)pdada)->mbsd = 0;
		((NET_TASK_TYPE1 *)pdada)->tid_task = 0;
    osThreadTerminate(task_type1.tid_task);
}

void bms_ems_task(const void *pdata)
{
    SOCKADDR_IN addr;
    int mbsock, mbsd;
		NET_TASK_TYPE1 task_type1[3];
		uint8_t tnum = 0,connum = 0; 
		uint8_t i = 0;
	
    mbsock = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_port = htons(1314);
    addr.sin_family = PF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(mbsock, (SOCKADDR *)&addr, sizeof(addr));
    listen(mbsock,3);

    while(1)
    {
        mbsd = accept(mbsock, NULL, NULL);//等待连接

        if(mbsd > 0)
        {   
			//tty_printf("连接有效 mbsd is %d\n",mbsd);
			for(i = 0,connum = 0;i < 3;i++)
			{
				if(task_type1[i].mbsd != NULL)			
				{
					connum++;
				}
				else
				{
					tnum = i;
				}
			}
			//tty_printf("BMS连接数connum is %d\n",connum);
			if(connum >= 3)
			{
			//tty_printf("服务器拒绝新建连接\n");
				continue;
			}
						
			for(i = 0;i < 3;i++)
			{
				if(task_type1[i].mbsd == mbsd)
				{
					//tty_printf("监听口已被占用,连接编号为%d\n",i);
					break;
				}
			}
			if(i < 3)
			{
				//tty_printf("服务器拒绝新建连接\n");
				continue;
			}
			if(task_type1[tnum].mbsd != NULL)
			{
				closesocket(task_type1[tnum].mbsd);						
				//tty_printf("关闭监听口mbsd is: %d,连接编号为%d\n",mbsd,tnum);
			}
			if(task_type1[tnum].tid_task != NULL)
			{
				osThreadTerminate(task_type1[tnum].tid_task);														
				//tty_printf("关闭监听线程，连接编号为%d\n",tnum);
			}
			task_type1[tnum].mbsd = mbsd;
			task_type1[tnum].tid_task = osThreadCreate(osThread(bms_handle_task), (void *)&(task_type1[tnum]));
			//tty_printf("新建连接,mbsd is: %d，连接编号为%d\n",mbsd,tnum);
			if(task_type1[tnum].tid_task == NULL)
			{
			//tty_printf("线程创建失败\n");
				closesocket(mbsd);
			}
		}

        osDelay(2);
    }
}
#endif

