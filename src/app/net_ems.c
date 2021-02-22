#include <stdio.h>
#include <string.h>
#include <math.h>
#include <cpu.h>
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
#include "sdata_alloc.h"
#include "systime.h"
#include "custom.h"
#include "screen.h"
#include "Net_ems.h"

pcs_manage_t pcs_manage;
static uint8_t net_ems_recv_buf[256];
static uint8_t net_ems_send_buf[256];
extern system_control sys_ctl;
extern systime timeset;
extern meter_data_t bms_data;

static void net_ems_handle_task(const void *pdada);
osThreadDef(net_ems_handle_task, osPriorityNormal, 1, 0);
extern int mb_send (int sock, const char *buf, int len, int flags);
extern void thread_delete_selfmb(void);

short Power_Set_AC_get;
short Power_Set_AC_get_q;
	

void SetParamForaAll(unsigned short addr_tmp, short value)
{
    fpga_pdatabuf_get[addr_tmp]=setting_data_handle(addr_tmp, value);
    setpara(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
    fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
    fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
}

/************ ems为上位机设定的参数************/
int ems_read_reg(uint16_t addr, uint16_t *value)
{
	int err;
	err = 0;
	//miro_write.life_count++;
	switch(addr)
    {
		case 2000: //Uab
			*value = getting_data_handle(Addr_Param256, Upper_Param[Addr_Param256]);
			break;

		case 2001: //Ubc
			*value = getting_data_handle(Addr_Param257, Upper_Param[Addr_Param257]);
			break;

		case 2002: //Uca
			*value = getting_data_handle(Addr_Param258, Upper_Param[Addr_Param258]);
			break;

		case 2003: //Ia
			*value = getting_data_handle(Addr_Param259, Upper_Param[Addr_Param259]);
			break;

		case 2004: //Ib
			*value = getting_data_handle(Addr_Param260, Upper_Param[Addr_Param260]);
			break;

		case 2005: //Ic
			*value = getting_data_handle(Addr_Param261, Upper_Param[Addr_Param261]);
			break;

		case 2006: //freq
			*value = getting_data_handle(Addr_Param271, Upper_Param[Addr_Param271]);
			break;

		case 2007: //Pac
			*value = getting_data_handle(Addr_Param267, Upper_Param[Addr_Param267]);
			break;

		case 2008: //Qac
			*value = getting_data_handle(Addr_Param268, Upper_Param[Addr_Param268]);
			break;

	    case 2009: //Udc1			
	        *value = getting_data_handle(Addr_Param264, Upper_Param[Addr_Param264]);
	        break;

		case 2010: //Idc1
	        *value = getting_data_handle(Addr_Param263, Upper_Param[Addr_Param263]);
	        break;

    	case 2011: //Pdc1
			*value = getting_data_handle(Addr_Param269, Upper_Param[Addr_Param269]);
        	break;

	    case 2012: //Ubus
	        *value = getting_data_handle(Addr_Param262, Upper_Param[Addr_Param262]);
	        break;

	    case 2013: //Ibus
	        *value = getting_data_handle(Addr_Param263, Upper_Param[Addr_Param263]);
	        break;

	    case 2014: //Pbus
	        *value = getting_data_handle(Addr_Param269, Upper_Param[Addr_Param269]);
	        break;

	    case 2015: //IGBTA
	        *value = Upper_Param[MP_TEMP1];
	        break;

	    case 2016: //IGBTB
	        *value = Upper_Param[MP_TEMP2];
	        break;

	    case 2017: //IGBTC
	        *value = Upper_Param[MP_TEMP3];
	        break;

	    case 2018: //IGBTDC1
	        *value = Upper_Param[MP_TEMP4];
	        break;	
			
		case 2019: //IGBTDC2
			*value = Upper_Param[MP_TEMP5];
			break;	

	    case 2020: //FSM
	        *value = fpga_read(STATE8_ADD) & 0xF;
	        break;	
				
	    case 1000:
	        *value = miro_write.Connect_State;
	        break;

	    case 1001:
	        *value = miro_write.Run_Mode;
	        break;
			
	    case 1002:
	        *value = Power_Set_AC_get;
	        break;	
			
		case 1003:
			*value = Power_Set_AC_get_q;
			break;
			
	    default:
			*value = 0;
	        err = 0;
	        break;			 	
    }
    return err;
}

int ems_write_reg(uint16_t addr, uint16_t value)
{
    int err;
    err = 0;    
	short Power_Set_AC;	
	short SettingCMD1 = fpga_read(Addr_Param111);

	int a,b,c,d;
	
	//if(Pcs_Ctrl_Mode_Get()== EMS_CTRL)
    {
        switch(addr)
        {

			case 1000:
				 miro_write.Connect_State = value;
				 if(miro_write.Connect_State == 0) {				   
					fpga_write(Addr_Param26, 0);				   
					osDelay(20);
					set_pcs_shutdown();		   				   
					log_add(ET_OPSTATE,EST_EMS_CLOSE);
					miro_write.bootup_flag = 0;				
					miro_write.auto_recover_flag = 0;
				 } else if(miro_write.Connect_State == 1) {
					fpga_write(Addr_Param26, 0); 				  
					osDelay(20);
					set_pcs_bootup();
					log_add(ET_OPSTATE,EST_EMS_OPEN);
					miro_write.bootup_flag = 1;
				 } else if(miro_write.Connect_State == 2) {				   
					fpga_write(Addr_Param26, 0);				   
					osDelay(20);
					set_pcs_shutdown();		   				   
					log_add(ET_OPSTATE,EST_EMS_ESTOP);
					miro_write.bootup_flag = 0;
				 }
				 break;
			
			case 1001:
				 miro_write.Run_Mode = value;
				 arm_config_data_write(Run_Mode_ADD, value);
			
				 if(value == 0) {
					Pcs_Run_Mode_Set(PQ_MODE);	
					mb_data.run_mode = 0;
					miro_write.Run_Mode = 0;
				 } else if(value == 1) {
					Pcs_Run_Mode_Set(VF_MODE);	
					mb_data.run_mode = 1;
					miro_write.Run_Mode = 1;
				 }  else if(value == 2) {
					Pcs_Run_Mode_Set(VSG_MODE);	
					mb_data.run_mode = 2;
					miro_write.Run_Mode = 2;
				 }
				 break;
			
			 case 1002:		
			 	Power_Set_AC_get = value;
				Power_Set_AC = value;	
				if(Power_Set_AC > 0 && Power_Set_AC_get > 1500 * 1.2f * MODULUS) {
					Power_Set_AC = (int16_t)(1500 * 1.2f * MODULUS);
				}
				if(Power_Set_AC < 0 && Power_Set_AC_get < (-1) * 1500 * 1.2f * MODULUS) {
					Power_Set_AC = (int16_t)((-1) * 1500 * 1.2f * MODULUS);
				}
				miro_write.Power_Set_AC = setting_data_handle(Addr_Param26, Power_Set_AC);				
				fpga_write(Addr_Param26, miro_write.Power_Set_AC);
				break;
				   
			 case 1003:
			 	Power_Set_AC_get_q = value;
				Power_Set_AC = value;	
				if(Power_Set_AC > 0 && Power_Set_AC_get > 1500 * 1.2f * MODULUS) {
					Power_Set_AC = (int16_t)(1500 * 1.2f * MODULUS);
				}
				if(Power_Set_AC < 0 && Power_Set_AC_get < (-1) * 1500 * 1.2f * MODULUS) {
					Power_Set_AC = (int16_t)((-1) * 1500 * 1.2f * MODULUS);
				}
				miro_write.Power_Set_AC = setting_data_handle(Addr_Param27, Power_Set_AC);				
				fpga_write(Addr_Param27, miro_write.Power_Set_AC);
				break;
				
	        default:
	            err = 0;
	            break;
        }

    }	
    return err;

}


int ems_read_holding_reg(uint8_t *recv_buf, uint8_t *resp_buf, int datalen)
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
            if(ems_read_reg(saddr + i, &reg) != 0)
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

int ems_read_input_reg(uint8_t *recv_buf, uint8_t *resp_buf, int datalen)
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
            if(ems_read_reg(saddr + i, &reg) != 0)
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

int ems_write_single_reg(uint8_t *recv_buf, uint8_t *resp_buf, int datalen)
{
    uint16_t saddr;
    uint16_t value;
    uint8_t err = 0;

    saddr = (recv_buf[0] << 8) + recv_buf[1];
    value = (recv_buf[2] << 8) + recv_buf[3];

    err = ems_write_reg(saddr, value);

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

int ems_write_mul_reg(uint8_t *recv_buf, uint8_t *resp_buf, int datalen)
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
            err = ems_write_reg(saddr + i, value);

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

int ems_rw_mul_reg(uint8_t *recv_buf, uint8_t *resp_buf, int datalen)
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

            if(ems_write_reg(wsaddr + i, value))
            {
                err = 2;
                break;
            }
        }

        if(err == 0)
        {
            for(i = 0; i < rnum; i++)
            {
                if(ems_read_reg(rsaddr + i, &reg) != 0)
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

static int net_ems_recv_frame(int mbsd, int *frame_len)
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

static int net_ems_resp_frame(int mbsd, int frame_len)
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
        resplen = ems_read_holding_reg(&recv_buf[8], &resp_buf[7], leftlen - 1);
        break;

    case 0x04:
        resplen = ems_read_input_reg(&recv_buf[8], &resp_buf[7], leftlen - 1);
        break;

    case 0x06:
        resplen = ems_write_single_reg(&recv_buf[8], &resp_buf[7], leftlen - 1);
        break;

    case 0x10:
        resplen = ems_write_mul_reg(&recv_buf[8], &resp_buf[7], leftlen - 1);
        break;

    case 0x17:
        resplen = ems_rw_mul_reg(&recv_buf[8], &resp_buf[7], leftlen - 1);
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

static void net_ems_handle_task(const void *pdada)
{
    int err;
    int frame_len;
		NET_TASK_TYPE task_type;
	
		task_type = *((NET_TASK_TYPE *)pdada);
	
    while(1)
    {
        err = net_ems_recv_frame(task_type.mbsd, &frame_len);

        if(err != 0)
        {
            break;
        }

        err = net_ems_resp_frame(task_type.mbsd, frame_len);

        if(err != 0)
        {
            break;
        }
		
       	osDelay(2);
    }
    closesocket(task_type.mbsd);	
		((NET_TASK_TYPE *)pdada)->mbsd = 0;
		((NET_TASK_TYPE *)pdada)->tid_task = 0;
    osThreadTerminate(task_type.tid_task);
}

void net_ems_task(const void *pdata)
{
    SOCKADDR_IN addr;
    int mbsock, mbsd;
	NET_TASK_TYPE task_type[3];
	uint8_t tnum = 0,connum = 0; 
	uint8_t i = 0;
	
    mbsock = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_port = htons(502);
    addr.sin_family = PF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(mbsock, (SOCKADDR *)&addr, sizeof(addr));
    listen(mbsock,3);

    while(1)
    {
        mbsd = accept(mbsock, NULL, NULL);//等待连接

        if(mbsd > 0)
        {   
			//ty_printf("连接有效 mbsd is %d\n",mbsd);
			for(i = 0,connum = 0;i < 3;i++)
			{
				if(task_type[i].mbsd != NULL)			
				{
					connum++;
				}
				else
				{
					tnum = i;
				}
			}
			//tty_printf("EMS连接数connum is %d\n",connum);
			if(connum >= 3)
			{
			//tty_printf("服务器拒绝新建连接\n");
				continue;
			}
						
			for(i = 0;i < 3;i++)
			{
				if(task_type[i].mbsd == mbsd)
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
			if(task_type[tnum].mbsd != NULL)
			{
				closesocket(task_type[tnum].mbsd);						
				//tty_printf("关闭监听口mbsd is: %d,连接编号为%d\n",mbsd,tnum);
			}
			if(task_type[tnum].tid_task != NULL)
			{
				osThreadTerminate(task_type[tnum].tid_task);														
								//tty_printf("关闭监听线程，连接编号为%d\n",tnum);
			}
			task_type[tnum].mbsd = mbsd;
			task_type[tnum].tid_task = osThreadCreate(osThread(net_ems_handle_task), (void *)&(task_type[tnum]));
			//tty_printf("新建连接,mbsd is: %d，连接编号为%d\n",mbsd,tnum);
			if(task_type[tnum].tid_task == NULL)
			{
			//tty_printf("线程创建失败\n");
				closesocket(mbsd);
			}
		}

        osDelay(2);
    }
}


