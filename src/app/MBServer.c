/**
 * @file MBServer.c
 *
 * @brief Modbus TCP/IP implement
 *
 * @date 2012/4/10
 *
 */
#include <string.h>
#include "MBServer.h"
#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "rl_net.h"
#include "cmsis_os.h"
#include "cpu.h"
#include "stdio.h"
#include "bsp.h"
#include "para.h"
#include "sys.h"
#include "log.h"
#include "fpga.h"
#include "tty.h"
#include "storedata.h"
#include "micro.h"
#include "modbus_comm.h"
//#include "filesystem.h"
#include "env.h"
#include "custom.h"
#include "screen.h"
#include "nand_file.h"
#include "epcs.h"
#include "wave_file.h"
#include "storedata.h"
#include "fmlog.h"
#include "recordwave.h"
#include "freq_prot.h"
#include "soft_bootup.h"
#include "fault_rec.h"
#include "comm_offline.h"
#include "ul_mode_control.h"

_mbser_inf mbser_inf[MAX_CONN_NUM];
_update_inf update_inf;
uint8_t	pof_head[POF_HEAD_LENGTH];
uint8_t	pof_tail[POF_TAIL_LENGTH];
int longdataex_remain;
#define MS2TICK(ms)     osKernelSysTickMicroSec(1000 * (ms))

static void mbresp_task(const void *pdada);
osThreadDef(mbresp_task, osPriorityNormal, 1, 0);

static void firmware_update(const void *pdata);
osThreadDef(firmware_update, osPriorityNormal, 1, 0);

osThreadId tid_firmware_check;
static void firmware_check(const void *pdata);
osThreadDef(firmware_check, osPriorityNormal, 1, 0);

osSemaphoreId mb_send_sem;
osSemaphoreDef(mb_send_sem);

extern fpga_data_buffer *fpga_pdatabuf;
extern short fpga_pdatabuf_get[512];
extern system_control sys_ctl;
extern freq_prot_t freqProt;

extern int setfifoclk(unsigned int samp);
extern void getmd5(unsigned char *buf, unsigned int len, unsigned char md5[16]);
extern void fault_clear_manu(void);
extern void sys_reset(void);

void firmware_update_timout_set(_mbser_inf *msinf)
{
    msinf->update_ctrl.timeout_counter = osKernelSysTick();
    msinf->update_ctrl.timeout_check = 1;
}

void firmware_update_timout_clear(_mbser_inf *msinf)
{
    msinf->update_ctrl.timeout_counter = 0;
    msinf->update_ctrl.timeout_check = 0;
}

int is_firmware_update_timeout(_mbser_inf *msinf)
{
    unsigned int elapse;
    unsigned int curtick;

    if(msinf->update_ctrl.timeout_check == 0)
    {
        return 0;
    }

    curtick = osKernelSysTick();

    if(msinf->update_ctrl.timeout_counter <= curtick)
    {
        elapse = curtick - msinf->update_ctrl.timeout_counter;
    }
    else
    {
        elapse = 0xffffffff - msinf->update_ctrl.timeout_counter + 1 + curtick;
    }

    if(elapse >= MS2TICK(FIRMWARE_TIMEOUT_MS))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int is_firmware_task_busy(_mbser_inf *msinf)
{
    if(msinf->update_ctrl.state == UPDATE_PROGRAM_DOING)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static int firmware_update_file(char *name)
{
    FILE_STRUCT *f;
    int err = 0;

    f = getfileaddr(name);

    if(f == NULL)
    {
        err = 1;
        tty_printf("open file failed\n");
        return err;
    }
		
		if(f->fwritennum == 0)
    {
        f->flastsize = f->fsize;
				f->fsize = update_inf.codesize;
    }
    osSemaphoreWait(nand_sem, osWaitForever);
    fwrite_w(f,update_inf.buf,update_inf.len);
		osSemaphoreRelease(nand_sem);

    return err;
}

static int firmware_update_epcs(void)
{
    int err = 0;

    if(update_inf.offset == 0)
    {
        epcs_init();
    }

    epcs_transdata(update_inf.buf, update_inf.len);

    if(epcs_write(update_inf.buf, update_inf.offset, update_inf.len) != 0)
    {
        err = 1;
    }

    return err;
}

static void firmware_update(const void *pdata)
{
    int err = 0;
    char *name;
    uint32_t max;
    _mbser_inf *msinf;

    msinf = (_mbser_inf *)pdata;

    switch(update_inf.type)
    {
    case 2:
				name = "main_cur.bin";
        max = 0xE0000;
        break;
    case 3:
				name = "fpga_cur.pof";
        max = POF_LENGTH;				//POF_LOAD
        break;

    default:
        err = 1;
        goto firmware_update_return;
    }

    tty_printf("offset:0x%x,len:0x%x\n", update_inf.offset, update_inf.len);

    if(update_inf.offset + update_inf.len > max)
    {
        err = 1;
        goto firmware_update_return;
    }

    if(update_inf.type == 2)
    {
        err = firmware_update_file(name);
    }

    if(update_inf.type == 3)
    {
        //err = firmware_update_file(name);
        //if(!err)
        {
            err = firmware_update_epcs();
        }
    }

firmware_update_return:

    if(err == 0)
    {
        update_inf.recved += update_inf.re_temp;
        msinf->update_ctrl.state = UPDATE_PROGRAM_SUCCESS;
    }
    else
    {
        msinf->update_ctrl.state = UPDATE_PROGRAM_FAILED;
    }

    osThreadTerminate(osThreadGetId());

    while(1)
    {
        osDelay(1);
    }
}

static void print_md5(uint8_t *buf)
{
    uint32_t i;

    for(i = 0; i < 16; i++)
    {
        //tty_printf("%02x", buf[i]);
    }

    //tty_printf("\n");
}

static void firmware_check(const void *pdata) //POF_LOAD
{
    int i, err = 0;
    uint8_t md5buf[16];
    char md5_str[33],md5_last_str[33],*str;
    uint32_t max;
    char *name;
    _mbser_inf *msinf;

    msinf = (_mbser_inf *)pdata;
//    firmware_check_delthreah();
    switch(update_inf.type)
    {
    case 2:
				name = "main_cur.bin";
        max = 0xE0000;
        break;
    case 3:
				name = "fpga_cur.pof";
        max = POF_CODE_LENGTH;		//POF_LOAD
        break;
    default:
        err = 1;
        goto firmware_check_return;
    }

    if(update_inf.codesize > max)
    {
        err = 1;
        tty_printf("firmware_check : 代码长度过大\n");
        goto firmware_check_return;
    }

    if(update_inf.type == 2)
    {
        fs_get_md5(name, md5buf);
    }

    if(update_inf.type == 3)
    {
        //epcs_get_md5(update_inf.codesize, md5buf);
        memcpy(md5buf, update_inf.md5, 16);
        epcs_Reset();
        //osDelay(200);
        //sys_reset();
    }

    if(memcmp(md5buf, update_inf.md5, 16) == 0)
    {
        tty_printf("md5校验相同\n");
        if(update_inf.type == 2)
        {
            for(i = 0; i < 16; i++)
            {
                snprintf(&md5_str[i * 2], 3, "%02x", md5buf[i]);
            }

            /* just make sure str[32] == 0 */
            md5_str[32] = '\0';
            str = getenv("main_cur_md5");				
            if(str != NULL)
            {
                strcpy(md5_last_str,str);
                setenv("main_last_md5", md5_last_str);
            }
            setenv("main_cur_md5", md5_str);
            setenv("main_update", "1");
            save_sdata();
        }
        if(update_inf.type == 3)
        {
            for(i = 0; i < 16; i++)
            {
                snprintf(&md5_str[i * 2], 3, "%02x", md5buf[i]);
            }

            /* just make sure str[32] == 0 */
            md5_str[32] = '\0';
            str = getenv("fpga_cur_md5");	
            if(str != NULL)
            {
                strcpy(md5_last_str,str);
                setenv("fpga_last_md5", md5_last_str);
            }
            setenv("fpga_cur_md5", md5_str);
            save_sdata();
        }
        err = saveenv();
    }
    else
    {
        err = 1;
        tty_printf("md5校验失败\n");
//        tty_printf("远端校验码:");
//        print_md5(update_inf.md5);
//        tty_printf("\n");
//        tty_printf("本地校验码:");
//        print_md5(md5buf);
//        tty_printf("\n");
    }

firmware_check_return:

    //		if(err == 0)
    if(err == 0)
    //    if((err == 0) && (firmware_version_update(update_inf.type,md5_str) == 0))
    {
        msinf->update_ctrl.state = UPDATE_CHECK_SUCCESS;
    }
    else
    {
        msinf->update_ctrl.state = UPDATE_CHECK_FAILED;
    }
    firmware_check_createthreah();  //POF_LOAD
    osThreadTerminate(tid_firmware_check);//POF_LOAD

    while(1)
    {
        osDelay(1);
    }
}

int firmware_update_start(_mbser_inf *msinf)
{
    osThreadId id;

    msinf->update_ctrl.state = UPDATE_PROGRAM_DOING;
    firmware_update_timout_set(msinf);

    id = osThreadCreate(osThread(firmware_update), msinf);

    if(id == NULL)
    {
        //tty_printf("create task failed\n");
        return 1;
    }

    return 0;
}

int firmware_check_start(_mbser_inf *msinf)
{
    osThreadId id;

    msinf->update_ctrl.state = UPDATE_CHECK_DOING;
    firmware_update_timout_clear(msinf);

    id = osThreadCreate(osThread(firmware_check), msinf);
		tid_firmware_check = id;
    if(id == NULL)
    {
        return 1;
    }

    return 0;
}

void firmware_update_finish(_mbser_inf *msinf)
{
    memset(&msinf->update_ctrl, 0, sizeof(msinf->update_ctrl));
    update_inf.flag = 0;
}

/**
 * mb_writereg --- 根据写入的值设置寄存器
 * saddr:参数号
 * value:值
 * 返回错误代码.0:正确,2:参数号错误,4:写入失败
*/
unsigned char mb_writereg(unsigned short saddr, unsigned short value, short *rt)
{
		//CPU_SR cpu_sr;
		int err = 0;
		short zero_adjust_cmd;
		sys_ctl.dev_opt_mode = (fpga_read(Addr_Param111) >> 0) & 0x1;
		printf_debug5("[UPPER SET]: addr:val=%d:%d\n", saddr, value);
	
		if(sys_ctl.dev_opt_mode == 0)
		{
			if((saddr <= Addr_Param109) || ((saddr >= Addr_Param120)&&(saddr <= Addr_Param255)))
			{
				//tty_printf("upper write setting reg, saddr = %d, value = %d\n", saddr, value);
				setpara(saddr, value);
				if((saddr != Addr_Param26) && (saddr != Addr_Param27) && (saddr != Addr_Param54)) 
				{
					fpga_write(saddr, value);
					fpga_pdatabuf_write(saddr, value);
				} 
				else if(saddr == Addr_Param26) 
				{
					if(svg_info.svg_en == 0) 
					{
						fpga_write(saddr, value);
						fpga_pdatabuf_write(saddr, value);
					}
					PQNonSVG.p=getting_data_handle(saddr, value);
                    umo.pCmd = value;
				} 
				else if(saddr == Addr_Param27) 
				{
					if(svg_info.svg_en == 0) 
					{
						fpga_write(saddr, value);
						fpga_pdatabuf_write(saddr, value);
					}
					PQNonSVG.q=getting_data_handle(saddr, value);
                    umo.qCmd = value;
				}
                else if(saddr == Addr_Param54) 
				{
					fpga_write(saddr, value);
					fpga_pdatabuf_write(saddr, value);
                    umo.freqCmd = value;
				}
				*rt = value;
				return 0;
			}
	
			switch(saddr)
			{
				case Addr_Param110:
				{
					setpara(Addr_Param110,value);
		
					if(value==1) 
					{ //开机			
						miro_write.manShutdownFlag = 0;
                        set_pcs_bootup();
						log_add(ET_OPSTATE,EST_MB_OPEN);
						mb_data.last_onoff_mark = 8;					
						miro_write.bootup_flag = 1; 
						//sys_ctl.onoff = 1;
					} 
					else if(value==2) 
					{ //关机
					
						miro_write.manShutdownFlag = 1;
                        set_pcs_shutdown();
						log_add(ET_OPSTATE,EST_MB_CLOSE);
						mb_data.last_onoff_mark = 4;					
						miro_write.bootup_flag = 0; 
						//sys_ctl.onoff = 0;
					} 
					else if((GETBIT(value,6) == 1) || (GETBIT(value,7) == 1)) 
					{ //急停
						mb_data.last_onoff_mark = 4;
						sys_ctl.onoff = 0;				
						miro_write.manShutdownFlag = 1;
                        set_pcs_shutdown();	
					}
		
					//fpga_write(saddr, value);
					//fpga_pdatabuf_write(saddr, value);
					//*rt=value;
		
					else if((GETBIT(value,3) == 1))				
					{ 
						zero_adjust_cmd = 1;
						zero_adjust(zero_adjust_cmd);
					}
					
					else if((GETBIT(value,4) == 1))				
					{ 
						zero_adjust_cmd = 2;
						zero_adjust(zero_adjust_cmd);
					}				
					else 
					{
						*rt=value;
					}				
				}			
					break;
					
				//TODO: 需不需要在Upper修改运行模式时同步到HMI
				case Addr_Param111:
					
					setpara(Addr_Param111,value);
					fpga_write(saddr, value);
					fpga_pdatabuf_write(saddr, value);
					*rt=value;
					break;
		
				case Addr_Param112:
					if(GETBIT(value, 2) == 1) 
					{
						//Pcs_Run_Mode_Set(VF_MODE); //虚拟同步需要在VF模式有效
					}
					setpara(Addr_Param112,value);
					fpga_write(saddr, value);
					fpga_pdatabuf_write(saddr, value);
					*rt=value;
					break;
		
				case Addr_Param113:
					setpara(Addr_Param113,value);
					fpga_write(saddr, value);
					fpga_pdatabuf_write(saddr, value);
					*rt=value;
					break;
		
				case Addr_Param114:
					setpara(Addr_Param114,value);
					fpga_write(saddr, value);
					fpga_pdatabuf_write(saddr, value);
					*rt=value;
					break;
		
		
				case FD_UINX_ADD_H:
					setpara(FD_UINX_ADD_H,value);
					arm_config_data_write(FD_UINX_ADDH,value);
					*rt=value;
					break;
					
				case FD_UINX_ADD_L:
					setpara(FD_UINX_ADD_L,value);
					arm_config_data_write(FD_UINX_ADD,value);
					*rt=value;
					break;
					
				case FD_IINX_ADD_H:
					setpara(FD_IINX_ADD_H,value);
					arm_config_data_write(FD_IINX_ADDH,value);
					*rt=value;
					break;
					
				case FD_IINX_ADD_L:
					setpara(FD_IINX_ADD_L,value);
					arm_config_data_write(FD_IINX_ADD,value);
					*rt=value;
					break;
		
				case FD_UDC_ADD_H:
					setpara(FD_UDC_ADD_H,value);
					arm_config_data_write(FD_UDC_ADDH,value);
					*rt=value;
					break;
					
				case FD_UDC_ADD_L:
					setpara(FD_UDC_ADD_L,value);
					arm_config_data_write(FD_UDC_ADD,value);
					*rt=value;
					break;
					
				case FD_IDC_ADD_H:
					setpara(FD_IDC_ADD_H,value);
					arm_config_data_write(FD_IDC_ADDH,value);
					*rt=value;
					break;
					
				case FD_IDC_ADD_L:
					setpara(FD_IDC_ADD_L,value);
					arm_config_data_write(FD_IDC_ADD,value);
					*rt=value;
					break;
		
				case FD_UBTR_ADD_H:
					setpara(FD_UBTR_ADD_H,value);
					arm_config_data_write(FD_UBTR_ADDH,value);
					*rt=value;
					break;
					
				case FD_UBTR_ADD_L:
					setpara(FD_UBTR_ADD_L,value);
					arm_config_data_write(FD_UBTR_ADD,value);
					*rt=value;
					break;
				case FD_UPOS_GND_ADD_H:
					setpara(FD_UPOS_GND_ADD_H,value);
					arm_config_data_write(FD_UPOS_GND_ADDH,value);
					*rt=value;
					break;
					
				case FD_UPOS_GND_ADD_L:
					setpara(FD_UPOS_GND_ADD_L,value);
					arm_config_data_write(FD_UPOS_GND_ADD,value);
					*rt=value;
					break;
					
				case FD_UNEG_GND_ADD_H:
					setpara(FD_UNEG_GND_ADD_H,value);
					arm_config_data_write(FD_UNEG_GND_ADDH,value);
					*rt=value;
					break;
					
				case FD_UNEG_GND_ADD_L:
					setpara(FD_UNEG_GND_ADD_L,value);
					arm_config_data_write(FD_UNEG_GND_ADD,value);
					*rt=value;
					break;
		
				case FD_PAC_ADD_H:
					setpara(FD_PAC_ADD_H,value);
					arm_config_data_write(FD_PAC_ADDH,value);
					*rt=value;
					break;
					
				case FD_PAC_ADD_L:
					setpara(FD_PAC_ADD_L,value);
					arm_config_data_write(FD_PAC_ADD,value);
					*rt=value;
					break;
		
				case FD_PDC_ADD_H:
					setpara(FD_PDC_ADD_H,value);
					arm_config_data_write(FD_PDC_ADDH,value);
					*rt=value;
					break;
					
				case FD_PDC_ADD_L:
					setpara(FD_PDC_ADD_L,value);
					arm_config_data_write(FD_PDC_ADD,value);
					*rt=value;
					break;
		
				case FD_IAC_CT_ADD_H:
					setpara(FD_IAC_CT_ADD_H,value);
					arm_config_data_write(FD_IAC_CT_ADDH,value);
					*rt=value;
					break;
					
				case FD_IAC_CT_ADD_L:
					setpara(FD_IAC_CT_ADD_L,value);
					arm_config_data_write(FD_IAC_CT_ADD,value);
					*rt=value;
					break;
		
				case FD_PAC_CT_ADD_H:
					setpara(FD_PAC_CT_ADD_H,value);
					arm_config_data_write(FD_PAC_CT_ADDH,value);
					*rt=value;
					break;
					
				case FD_PAC_CT_ADD_L:
					setpara(FD_PAC_CT_ADD_L,value);
					arm_config_data_write(FD_PAC_CT_ADD,value);
					*rt=value;
					break;
					
				case FD_IAC_LKG_H:
					setpara(FD_IAC_LKG_H,value);
					arm_config_data_write(FD_IAC_LKG_ADDH,value);
					*rt=value;
					break;
					
				case FD_IAC_LKG_L:
					setpara(FD_IAC_LKG_L,value);
					arm_config_data_write(FD_IAC_LKG_ADD,value);
					*rt=value;
					break;

				case DEBUG_ONOFF:
					setpara(DEBUG_ONOFF, value);
					sys_ctl.dbg = value;
					*rt=value;
					break;

				case Ctrl_Mode_SET:
					setpara(Ctrl_Mode_SET,value); 		  
					Pcs_Ctrl_Mode_Set(value);
					*rt=value;			  
					break;
				
				case Run_Mode_Set:
					setpara(Run_Mode_Set,value);						  
					Pcs_Run_Mode_Set(value);
					*rt=value;			  
					break;
				
				case Communication_En:				
					miro_write.communication_en = value;
					setpara(Communication_En,value);			
					arm_config_data_write(COMMUICATION_PRO_EN,(short)value);
					*rt=value;				
					break;
						
				case Strategy_En:
					miro_write.strategy_en = value; 					
					setpara(Strategy_En,value); 		
					arm_config_data_write(STRATEGY_EN,(short)value);						
					*rt=value;				
					break;

				case MP_INI_MONITOR_ADDR:
					setpara(MP_INI_MONITOR_ADDR,value);
					arm_config_data_write(INI_MONITOR_ADDR_ADD,(short)value);
					*rt=value;
					break;
		
		
				case MP_CHRG_VOL_LIMIT:
					setpara(MP_CHRG_VOL_LIMIT,value);
					arm_config_data_write(CHRG_VOL_LIMIT_ADD,(short)value);
					*rt=value;
					break;
		
				case MP_DISC_VOL_LIMIT:
					setpara(MP_DISC_VOL_LIMIT,value);
					arm_config_data_write(DISC_VOL_LIMIT_ADD,(short)value);
					*rt=value;
					break;
		
				case MP_SVG_EN:
					setpara(MP_SVG_EN,value);
					arm_config_data_write(SVG_EN_ADD,(short)value);
					svg_info.svg_en = value;
					*rt=value;
					break;
		
				case MP_ISO_ISLAND:
					setpara(MP_ISO_ISLAND, value);
					arm_config_data_write(ISO_ISLAND_ADD,(short)value);
					*rt=value;
					break;
				
				case MP_INI_RESIS_FD:
					setpara(MP_INI_RESIS_FD,value);
					arm_config_data_write(INI_RESIS_FD_ADD,(short)value);
					*rt=value;
					break;
				case MP_INI_RESIS_REF:
					setpara(MP_INI_RESIS_REF,value);
					arm_config_data_write(INI_RESIS_REF_ADD,(short)value);
					*rt=value;
					break;
		
				case MP_MAX_P_ADD:
					setpara(MP_MAX_P_ADD,value);
					arm_config_data_write(MAX_P_ADD,(short)value);
					*rt=value;
					break;
					
				case MP_MIN_P_ADD:
					setpara(MP_MIN_P_ADD,value);
					arm_config_data_write(MIN_P_ADD,(short)value);
					*rt=value;
					break;
					
				case MP_MAX_BA_C_ADD:
					setpara(MP_MAX_BA_C_ADD,value);
					arm_config_data_write(MAX_BA_C_ADD,(short)value);
					*rt=value;
					break;
					
				case MP_MIN_BA_C_ADD:
					setpara(MP_MIN_BA_C_ADD,value);
					arm_config_data_write(MIN_BA_C_ADD,(short)value);
					*rt=value;
					break;
					
				case MP_MAX_BA_U_ADD:
					setpara(MP_MAX_BA_U_ADD,value);
					arm_config_data_write(MAX_BA_U_ADD,(short)value);
					*rt=value;
					break;
					
				case MP_MIN_BA_U_ADD:
					setpara(MP_MIN_BA_U_ADD,value);
					arm_config_data_write(MIN_BA_U_ADD,(short)value);
					*rt=value;
					break;
					
				case MP_MAX_Q_ADD:
					setpara(MP_MAX_Q_ADD,value);
					arm_config_data_write(MAX_Q_ADD,(short)value);
					*rt=value;
					break;
					
				case MP_MIN_Q_ADD:
					setpara(MP_MIN_Q_ADD,value);
					arm_config_data_write(MIN_Q_ADD,(short)value);
					*rt=value;
					break;

				case MP_LOACAL_ASK:
					if(value != 0)
					value = 1;
					setpara(MP_LOACAL_ASK,value);
					arm_config_data_write(ASK_LOCAL,(short)value);
					Request_Control(MP_LOACAL_ASK,(short)value);
					*rt=value;
					break;	

				
				/*****增加温度控制信息**********/
				case AC_FUN_START_TEMP:
					setpara(AC_FUN_START_TEMP,value);
					arm_config_data_write(AC_FUN_START_TEMP_REC,(short)value);
					*rt=value;
					break;

				case AC_FUN_STOP_TEMP:
					setpara(AC_FUN_STOP_TEMP,value);
					arm_config_data_write(AC_FUN_STOP_TEMP_REC,(short)value);
					*rt=value;
					break;

				case FILTER_CAPAC_ERR_TEMP:
					setpara(FILTER_CAPAC_ERR_TEMP,value);
					arm_config_data_write(FILTER_CAPAC_ERR_TEMP_REC,(short)value);
					*rt=value;
					break;

				case TRANSFORMER_ERR_TEMP:
					setpara(TRANSFORMER_ERR_TEMP,value);
					arm_config_data_write(TRANSFORMER_ERR_TEMP_REC,(short)value);
					*rt=value;
					break;

				case BUS_CAPAC_ERR_TEMP:
					setpara(BUS_CAPAC_ERR_TEMP,value);
					arm_config_data_write(BUS_CAPAC_ERR_TEMP_REC,(short)value);
					*rt=value;
					break;

				case ELEC_REACTOR_ERR_TEMP:
					setpara(ELEC_REACTOR_ERR_TEMP,value);
					arm_config_data_write(ELEC_REACTOR_ERR_TEMP_REC,(short)value);
					*rt=value;
					break;

				case AC_CABINET_ERR_TEMP:
					setpara(AC_CABINET_ERR_TEMP,value);
					arm_config_data_write(AC_CABINET_ERR_TEMP_REC,(short)value);
					*rt=value;
					break;		
					
				/*****电表1地址配置**********/
				case AMMETER1_A0:
					setpara(AMMETER1_A0,value);
					arm_config_data_write(ammeter1_A0,(short)value);
					*rt=value;
					break;
			
				case AMMETER1_A1:
					setpara(AMMETER1_A1,value);
					arm_config_data_write(ammeter1_A1,(short)value);
					*rt=value;	  
					break; 
			
				case AMMETER1_A2:
					setpara(AMMETER1_A2,value);
					arm_config_data_write(ammeter1_A2,(short)value);
					*rt=value;					  
					break;
			
				case AMMETER1_A3:
					setpara(AMMETER1_A3,value);
					arm_config_data_write(ammeter1_A3,(short)value);
					*rt=value;
					break;
			
				case AMMETER1_A4:
					setpara(AMMETER1_A4,value);
					arm_config_data_write(ammeter1_A4,(short)value);
					*rt=value;					  
					break;	 
			
				case AMMETER1_A5:
					setpara(AMMETER1_A5,value);
					arm_config_data_write(ammeter1_A5,(short)value);
					*rt=value;					  
					break;
			
				/*****电表2地址配置**********/
				case AMMETER2_A0:
					setpara(AMMETER2_A0,value);
					arm_config_data_write(ammeter2_A0,(short)value);
					*rt=value;
					break;
			
				case AMMETER2_A1:
					setpara(AMMETER2_A1,value);
					arm_config_data_write(ammeter2_A1,(short)value);
					*rt=value;					  
					break; 
			
				case AMMETER2_A2:
					setpara(AMMETER2_A2,value);
					arm_config_data_write(ammeter2_A2,(short)value);
					*rt=value;					  
					break;
			
				case AMMETER2_A3:
					setpara(AMMETER2_A3,value);
					arm_config_data_write(ammeter2_A3,(short)value);
					*rt=value;				  
					break;
			
				case AMMETER2_A4:
					setpara(AMMETER2_A4,value);
					arm_config_data_write(ammeter2_A4,(short)value);
					*rt=value;					  
					break;	
			
				case AMMETER2_A5:
					setpara(AMMETER2_A5,value);
					arm_config_data_write(ammeter2_A5,(short)value);
					*rt=value;					  
					break;
					
				//电表变比系数配置	  
				case AMMETER1_VRC:
					setpara(AMMETER1_VRC,value);
					arm_config_data_write(ammeter1_vrc,(short)value);
					*rt=value;			  
					break;
			
				case AMMETER2_VRC:
					setpara(AMMETER2_VRC,value);
					arm_config_data_write(ammeter2_vrc,(short)value);
					*rt=value;			  
					break;
					
				case DIS_MIN_POWER:
					setpara(DIS_MIN_POWER,value);
					arm_config_data_write(dis_min_power,(short)value);
					*rt=value;			  
					break;
						  
				case COS_SETA_PC_SET:
					setpara(COS_SETA_PC_SET,value);
					arm_config_data_write(cos_seta,(short)value);
					*rt=value;			  
					break;
						  				  
				/**增加削封填谷参数设置20171225**/
				case MB_XFTG_EN:
					setpara(MB_XFTG_EN,value);
					arm_config_data_write(XFTG_EN,(short)value);
					*rt=value;
					break;
						  
				case MB_CAPACITY:
					setpara(MB_CAPACITY,value);
					arm_config_data_write(CAPACITY,(short)value);
					*rt=value;			  
					break;
						  
				case MB_START_TIME_0:
					setpara(MB_START_TIME_0,value);
					arm_config_data_write(START_TIME_0,(short)value);
					*rt=value;			  
					break;	
						  
				case MB_STOP_TIME_0:
					setpara(MB_STOP_TIME_0,value);
					arm_config_data_write(STOP_TIME_0,(short)value);
					*rt=value;			  
					break;	
						  
				case MB_SET_POWER_0:
					setpara(MB_SET_POWER_0,value);
					arm_config_data_write(SET_POWER_0,(short)value);
					*rt=value;			  
					break;
		
				case MB_START_TIME_1:
					setpara(MB_START_TIME_1,value);
					arm_config_data_write(START_TIME_1,(short)value);
					*rt=value;			  
					break;	
		
				case MB_STOP_TIME_1:
					setpara(MB_STOP_TIME_1,value);
					arm_config_data_write(STOP_TIME_1,(short)value);
					*rt=value;			  
					break;
		
				case MB_SET_POWER_1:
					setpara(MB_SET_POWER_1,value);
					arm_config_data_write(SET_POWER_1,(short)value);
					*rt=value;			  
					break;
		
				case MB_START_TIME_2:
					setpara(MB_START_TIME_2,value);
					arm_config_data_write(START_TIME_2,(short)value);
					*rt=value;			  
					break;	
						  
				case MB_STOP_TIME_2:
					setpara(MB_STOP_TIME_2,value);
					arm_config_data_write(STOP_TIME_2,(short)value);
					*rt=value;			  
					break;	
						  
				case MB_SET_POWER_2:
					setpara(MB_SET_POWER_2,value);
					arm_config_data_write(SET_POWER_2,(short)value);
					*rt=value;			  
					break;
		
				case MB_START_TIME_3:
					setpara(MB_START_TIME_3,value);
					arm_config_data_write(START_TIME_3,(short)value);
					*rt=value;			  
					break;
						  
				case MB_STOP_TIME_3:
					setpara(MB_STOP_TIME_3,value);
					arm_config_data_write(STOP_TIME_3,(short)value);
					*rt=value;			  
					break;				  
		
				case MB_SET_POWER_3:
					setpara(MB_SET_POWER_3,value);
					arm_config_data_write(SET_POWER_3,(short)value);
					*rt=value;			  
					break;
					
				case MB_START_TIME_4:
					setpara(MB_START_TIME_4,value);
					arm_config_data_write(START_TIME_4,(short)value);
					*rt=value;			  
					break;	
						  
				case MB_STOP_TIME_4:
					setpara(MB_STOP_TIME_4,value);
					arm_config_data_write(STOP_TIME_4,(short)value);
					*rt=value;			  
					break;	
						  
				case MB_SET_POWER_4:
					setpara(MB_SET_POWER_4,value);
					arm_config_data_write(SET_POWER_4,(short)value);
					*rt=value;			  
					break;
		
				case MB_START_TIME_5:
					setpara(MB_START_TIME_5,value);
					arm_config_data_write(START_TIME_5,(short)value);
					*rt=value;			  
					break;	
		
				case MB_STOP_TIME_5:
					setpara(MB_STOP_TIME_5,value);
					arm_config_data_write(STOP_TIME_5,(short)value);
					*rt=value;			  
					break;
		
				case MB_SET_POWER_5:
					setpara(MB_SET_POWER_5,value);
					arm_config_data_write(SET_POWER_5,(short)value);
					*rt=value;			  
					break;
		
				case MB_START_TIME_6:
					setpara(MB_START_TIME_6,value);
					arm_config_data_write(START_TIME_6,(short)value);
					*rt=value;			  
					break;	
						  
				case MB_STOP_TIME_6:
					setpara(MB_STOP_TIME_6,value);
					arm_config_data_write(STOP_TIME_6,(short)value);
					*rt=value;			  
					break;	
						  
				case MB_SET_POWER_6:
					setpara(MB_SET_POWER_6,value);
					arm_config_data_write(SET_POWER_6,(short)value);
					*rt=value;			  
					break;
		
				case MB_START_TIME_7:
					setpara(MB_START_TIME_7,value);
					arm_config_data_write(START_TIME_7,(short)value);
					*rt=value;			  
					break;
		
				case MB_STOP_TIME_7:
					setpara(MB_STOP_TIME_7,value);
					arm_config_data_write(STOP_TIME_7,(short)value);
					*rt=value;			  
					break;
		
				case MB_SET_POWER_7:
					setpara(MB_SET_POWER_7,value);
					arm_config_data_write(SET_POWER_7,(short)value);
					*rt=value;			  
					break;
					  
				/**增加调频参数20180102**/
				case MB_AGC_EN:
					setpara(MB_AGC_EN,value);
					arm_config_data_write(AGC_EN,(short)value);
					*rt=value;				
					break;

				case MB_FRE_OVER:
					setpara(MB_FRE_OVER,value);
					arm_config_data_write(FRE_OVER,(short)value);
					*rt=value;				
					break;

				case MB_FRE_UNDER:
					setpara(MB_FRE_UNDER,value);
					arm_config_data_write(FRE_UNDER,(short)value);
					*rt=value;				
					break;

						  
				#if 1			
				case MB_DEVICE_CODE_YEAR:
					if(miro_write.lecense_key ==0xff)
					{
					  setpara(MB_DEVICE_CODE_YEAR,value);
					  arm_config_data_write(DEVICE_CODE_YEAR,(short)value);
					  *rt=value;	
					}
					break;
					  
				case MB_DEVICE_CODE_MD:
					if(miro_write.lecense_key ==0xff)
					{
					  setpara(MB_DEVICE_CODE_MD,value);
					  arm_config_data_write(DEVICE_CODE_MD,(short)value);
					  *rt=value;
					}
					break;
					  
				case MB_DEVICE_CODE_TYPE:
					if(miro_write.lecense_key ==0xff)
					{
					  setpara(MB_DEVICE_CODE_TYPE,value);
					  arm_config_data_write(DEVICE_CODE_TYPE,(short)value);
					  *rt=value;	
					}
					break;

				case MB_DEVICE_CODE_NUM:
					if(miro_write.lecense_key ==0xff)
					{
					  setpara(MB_DEVICE_CODE_NUM,value);
					  arm_config_data_write(DEVICE_CODE_NUM,(short)value);
					  *rt=value;		
					}
					break;				  

				case MB_DEVICE_WRITE_CODE:
					 setpara(MB_DEVICE_WRITE_CODE,value);
					 arm_config_data_write(DEVICE_WRITE_CODE,(short)value);
					 *rt=value;	
					 
					 #if 1	//密钥验证				
					 miro_write.code_input	= value;
					 if(miro_write.code_input == miro_write.code_release)
					 {
						 miro_write.code_type = 1;
						 
						 if(arm_config_data_read(CODE_RELEASE_FLAG)==0) //第一次写正确密钥后计算有效时间
						 {
							 miro_write.code_remain_date = 7;
							 arm_config_data_write(CODE_REMAIN_DATE,miro_write.code_remain_date);						 
							 arm_config_data_write(CODE_RELEASE_FLAG,1);						 
						 }
						 
					 }
					 
					 else if (miro_write.code_input == miro_write.code_debug)
					 {
						 miro_write.code_type = 2;					 
						 if(arm_config_data_read(CODE_DEBUG_FLAG)==0) //第一次写正确密钥后计算有效时间
						 {
							 miro_write.code_remain_date = 30;
							 arm_config_data_write(CODE_REMAIN_DATE,miro_write.code_remain_date);	
							 arm_config_data_write(CODE_DEBUG_FLAG,1);							 
						 }
					 }
						 
					 else if (miro_write.code_input == miro_write.code_temporary1)
					 {
						 miro_write.code_type = 4;
						 if(arm_config_data_read(CODE_TEMPORARY1_FLAG)==0) //第一次写正确密钥后计算有效时间
						 {
							 miro_write.code_remain_date = 7;
							 arm_config_data_write(CODE_REMAIN_DATE,miro_write.code_remain_date);	
							 arm_config_data_write(CODE_TEMPORARY1_FLAG,1); 						 
						 }					 
					 }
					 
					 else if (miro_write.code_input == miro_write.code_temporary2)
					 {
						 miro_write.code_type = 8;
						 if(arm_config_data_read(CODE_TEMPORARY2_FLAG)==0) //第一次写正确密钥后计算有效时间
						 {
							 miro_write.code_remain_date = 30;
							 arm_config_data_write(CODE_REMAIN_DATE,miro_write.code_remain_date);	
							 arm_config_data_write(CODE_TEMPORARY2_FLAG,1); 						 
						 }						 
					 }
					 
					 else if (miro_write.code_input == miro_write.code_temporary3)
					 {
						 miro_write.code_type = 16;
						 if(arm_config_data_read(CODE_TEMPORARY3_FLAG)==0) //第一次写正确密钥后计算有效时间
						 {
							 miro_write.code_remain_date = 90;
							 arm_config_data_write(CODE_REMAIN_DATE,miro_write.code_remain_date);	
							 arm_config_data_write(CODE_TEMPORARY3_FLAG,1); 						 
						 }						 
					 }
					 
					 else if (miro_write.code_input == miro_write.code_permanent)
					 {
						 miro_write.code_type = 32;
						 if(arm_config_data_read(CODE_PERMANENT_FLAG)==0) //第一次写正确密钥后计算有效时间
						 {
							 miro_write.code_remain_date = 9999;
							 arm_config_data_write(CODE_REMAIN_DATE,miro_write.code_remain_date);	
							 arm_config_data_write(CODE_PERMANENT_FLAG,1);							 
						 }					 
					 }
					 
					 else
					 {
						 miro_write.code_type = 0;
						 //miro_write.code_remain_date = 0;
						 //arm_config_data_write(CODE_REMAIN_DATE,0);	
						 log_add(ET_OPSTATE,EST_ERR_CODE);	 //密钥错误
					 }
					 arm_config_data_write(CODE_TPYE,miro_write.code_type);						 
									 
					#endif	
					
					break;	

				case MB_LECENSE_KEY:
					 setpara(MB_LECENSE_KEY,value);
					 miro_write.lecense_key = value; 
					 *rt=value;			  
					 break;		
				#endif

//UL ADD
				case UL_strtg_en_Addr: // 美标策略使能
					setpara(UL_strtg_en_Addr,value);
					miro_write.UL_strtg_en = value; 
					arm_config_data_write(ARM_UL_strtg_en, value);
					*rt=value;	
					break;

				case PF_x100_Addr: //	功率因数值
					setpara(PF_x100_Addr,value);
					miro_write.PF_x100 = value; 
					arm_config_data_write(ARM_PF_x100, value);
					*rt=value;	
					break;
					
				case VQ_V1_Addr: //	电压无功_电压值1
					setpara(VQ_V1_Addr,value);
					miro_write.VQ_V1 = value; 
					arm_config_data_write(ARM_VQ_V1, value);
					*rt=value;	
					break;
					
				case VQ_V2_Addr: //	电压无功_电压值2
					setpara(VQ_V2_Addr,value);
					miro_write.VQ_V2 = value; 
					arm_config_data_write(ARM_VQ_V2, value);
					*rt=value;	
					break;
					
				case VQ_V3_Addr: //	电压无功_电压值3
					setpara(VQ_V3_Addr,value);
					miro_write.VQ_V3 = value; 
					arm_config_data_write(ARM_VQ_V3, value);
					*rt=value;	
					break;
					
				case VQ_V4_Addr: //	电压无功_电压值4
					setpara(VQ_V4_Addr,value);
					miro_write.VQ_V4 = value; 
					arm_config_data_write(ARM_VQ_V4, value);
					*rt=value;	
					break;
					
				case VQ_Q1_Addr: //	电压无功_无功值1
					setpara(VQ_Q1_Addr,value);
					miro_write.VQ_Q1 = value; 
					arm_config_data_write(ARM_VQ_Q1, value);
					*rt=value;	
					break;
					
				case VQ_Q2_Addr: //	电压无功_无功值2
					setpara(VQ_Q2_Addr,value);
					miro_write.VQ_Q2 = value; 
					arm_config_data_write(ARM_VQ_Q2, value);
					*rt=value;	
					break;
					
				case VQ_Q3_Addr: //	电压无功_无功值3
					setpara(VQ_Q3_Addr,value);
					miro_write.VQ_Q3 = value; 
					arm_config_data_write(ARM_VQ_Q3, value);
					*rt=value;	
					break;
					
				case VQ_Q4_Addr: //	电压无功_无功值4 
					setpara(VQ_Q4_Addr,value);
					miro_write.VQ_Q4 = value; 
					arm_config_data_write(ARM_VQ_Q4, value);
					*rt=value;	
					break;
					
				case PQ_n_P3_Addr: //	有功无功_负轴_有功值3
					setpara(PQ_n_P3_Addr,value);
					miro_write.PQ_n_P3 = value; 
					arm_config_data_write(ARM_PQ_n_P3, value);
					*rt=value;	
					break;
					
				case PQ_n_P2_Addr: //	有功无功_负轴_有功值2
					setpara(PQ_n_P2_Addr,value);
					miro_write.PQ_n_P2 = value; 
					arm_config_data_write(ARM_PQ_n_P2, value);
					*rt=value;	
					break;
					
				case PQ_n_P1_Addr: //	有功无功_负轴_有功值1
					setpara(PQ_n_P1_Addr,value);
					miro_write.PQ_n_P1 = value; 
					arm_config_data_write(ARM_PQ_n_P1, value);
					*rt=value;	
					break;
					
				case PQ_p_P1_Addr: //	有功无功_正轴_有功值1
					setpara(PQ_p_P1_Addr,value);
					miro_write.PQ_p_P1 = value; 
					arm_config_data_write(ARM_PQ_p_P1, value);
					*rt=value;	
					break;
					
				case PQ_p_P2_Addr: //	有功无功_正轴_有功值2
					setpara(PQ_p_P2_Addr,value);
					miro_write.PQ_p_P2 = value; 
					arm_config_data_write(ARM_PQ_p_P2, value);
					*rt=value;	
					break;
					
				case PQ_p_P3_Addr: //	有功无功_正轴_有功值3 
					setpara(PQ_p_P3_Addr,value);
					miro_write.PQ_p_P3 = value; 
					arm_config_data_write(ARM_PQ_p_P3, value);
					*rt=value;	
					break;
					
				case PQ_n_Q3_Addr: //	有功无功_负轴_无功值3
					setpara(PQ_n_Q3_Addr,value);
					miro_write.PQ_n_Q3 = value; 
					arm_config_data_write(ARM_PQ_n_Q3, value);
					*rt=value;	
					break;
					
				case PQ_n_Q2_Addr: //	有功无功_负轴_无功值2
					setpara(PQ_n_Q2_Addr,value);
					miro_write.PQ_n_Q2 = value; 
					arm_config_data_write(ARM_PQ_n_Q2, value);
					*rt=value;	
					break;
					
				case PQ_n_Q1_Addr: //	有功无功_负轴_无功值1
					setpara(PQ_n_Q1_Addr,value);
					miro_write.PQ_n_Q1 = value; 
					arm_config_data_write(ARM_PQ_n_Q1, value);
					*rt=value;	
					break;
					
				case PQ_p_Q1_Addr: //	有功无功_正轴_无功值1
					setpara(PQ_p_Q1_Addr,value);
					miro_write.PQ_p_Q1 = value; 
					arm_config_data_write(ARM_PQ_p_Q1, value);
					*rt=value;	
					break;
					
				case PQ_p_Q2_Addr: //	有功无功_正轴_无功值2
					setpara(PQ_p_Q2_Addr,value);
					miro_write.PQ_p_Q2 = value; 
					arm_config_data_write(ARM_PQ_p_Q2, value);
					*rt=value;	
					break;
					
				case PQ_p_Q3_Addr: //	有功无功_正轴_无功值3 
					setpara(PQ_p_Q3_Addr,value);
					miro_write.PQ_p_Q3 = value; 
					arm_config_data_write(ARM_PQ_p_Q3, value);
					*rt=value;	
					break;
					
				case VP_V1_Addr: //	电压有功_电压值1
					setpara(VP_V1_Addr,value);
					miro_write.VP_V1 = value; 
					arm_config_data_write(ARM_VP_V1, value);
					*rt=value;	
					break;
					
				case VP_V2_Addr: //	电压有功_电压值2
					setpara(VP_V2_Addr,value);
					miro_write.VP_V2 = value; 
					arm_config_data_write(ARM_VP_V2, value);
					*rt=value;	
					break;
					
				case VP_P1_Addr: //	电压有功_有功值1
					setpara(VP_P1_Addr,value);
					miro_write.VP_P1 = value; 
					arm_config_data_write(ARM_VP_P1, value);
					*rt=value;	
					break;
					
				case VP_P2_Addr: //	电压有功_有功值2
					setpara(VP_P2_Addr,value);
					miro_write.VP_P2 = value; 
					arm_config_data_write(ARM_VP_P2, value);
					*rt=value;	
					break;

				case Uac_rate_Addr: //	交流额定电压
					setpara(Uac_rate_Addr,value);
					miro_write.Uac_rate = value; 
					arm_config_data_write(ARM_Uac_rate, value);
					*rt=value;	
					break;
					
				case P_rate_Addr: // 交流额定功率
					setpara(P_rate_Addr,value);
					miro_write.P_rate = value; 
					arm_config_data_write(ARM_P_rate, value);
					*rt=value;	
					break;
					
				case P_max_Addr: //	交流最大输出功率
					setpara(P_max_Addr,value);
					miro_write.P_max = value; 
					arm_config_data_write(ARM_P_max, value);
					*rt=value;	
					break;
					
				case PF_min_x100_Addr: //	允许最小的功率因数
					setpara(PF_min_x100_Addr,value);
					miro_write.PF_min_x100 = value; 
					arm_config_data_write(ARM_PF_min_x100, value);
					*rt=value;	
					break;

                case Lpf_times_Addr: //	允许最小的功率因数
					setpara(Lpf_times_Addr,value);
					miro_write.Lpf_times = value; 
					arm_config_data_write(ARM_Lpf_times, value);
					*rt=value;	
					break;

                case Freq_rate_Addr: //频率额定值
                    setpara(Freq_rate_Addr,value);
					miro_write.Freq_rate = value; 
					arm_config_data_write(ARM_Freq_rate, value);
					*rt=value;	
					break;
                    
            	case Pfr_dbUF_Addr: //一次调频欠频死区
            	    setpara(Pfr_dbUF_Addr,value);
					miro_write.Pfr_dbUF = value; 
					arm_config_data_write(ARM_Pfr_dbUF, value);
					*rt=value;	
					break;
                    
            	case Pfr_kUF_Addr: //一次调频欠频不等率
            	    setpara(Pfr_kUF_Addr,value);
					miro_write.Pfr_kUF = value; 
					arm_config_data_write(ARM_Pfr_kUF, value);
					*rt=value;	
					break;
                    
                case Pfr_dbOF_Addr: //一次调频过频死区
                    setpara(Pfr_dbOF_Addr,value);
					miro_write.Pfr_dbOF = value; 
					arm_config_data_write(ARM_Pfr_dbOF, value);
					*rt=value;	
					break;
                    
                case Pfr_kOF_Addr: //一次调频过频不等率
                    setpara(Pfr_kOF_Addr,value);
					miro_write.Pfr_kOF = value; 
					arm_config_data_write(ARM_Pfr_kOF, value);
					*rt=value;	
					break;
                    
                case Pfr_Tresp_ms_Addr: //一次调频响应时间（单位ms）
                    setpara(Pfr_Tresp_ms_Addr,value);
					miro_write.Pfr_Tresp_ms = value; 
					arm_config_data_write(ARM_Pfr_Tresp_ms, value);
					*rt=value;	
					break;

				case MB_SB_EN:
					setpara(MB_SB_EN,value);						  
					arm_config_data_write(AD_SB_EN,value);
					softBootup.sbEnble = value;
					*rt=value;			  
					break;

				case MB_SB_TIME:
					setpara(MB_SB_TIME,value);						  
					arm_config_data_write(AD_SB_TIME,value);
					softBootup.sbTime= value;
					*rt=value;			  
					break;

				case MB_SB_GOPWR:
					setpara(MB_SB_GOPWR,value);						  
					arm_config_data_write(AD_SB_GOPWR,value);
					softBootup.sbGoalPwr = value;
					*rt=value;			  
					break;

				case MB_FR_EN:
					setpara(MB_FR_EN,value);						  
					arm_config_data_write(AD_FR_EN,value);
					faultRec.frEnble = value;
					*rt=value;			  
					break;

				case MB_FR_WTIME:
					setpara(MB_FR_WTIME,value);						  
					arm_config_data_write(AD_FR_WTIME,value);
					faultRec.frWaitTime = value;
					*rt=value;			  
					break;

                
                case MB_CO_EN:
					setpara(MB_CO_EN,value);						  
					arm_config_data_write(AD_CO_EN,value);
					offline.commEn = value;
					*rt=value;			  
					break;

                case MB_CO_JTIME:
					setpara(MB_CO_JTIME,value);						  
					arm_config_data_write(AD_CO_JTIME,value);
					offline.commJdgTime = value;
					*rt=value;			  
					break;

                case MB_BOOTUP_TIME:
					setpara(MB_BOOTUP_TIME,value);						  
					arm_config_data_write(AD_BOOTUP_TIME,value);
					faultRec.bootupTime = value;
					*rt=value;			  
					break;
                    
				case MB_OF_Thr_1:
					setpara(MB_OF_Thr_1,value);						  
					arm_config_data_write(AD_OF_Thr_1,value);
					freqProt.fpi[0].thr = value;
					*rt=value;			  
					break;

				case MB_OF_Thr_2:
					setpara(MB_OF_Thr_2,value);						  
					arm_config_data_write(AD_OF_Thr_2,value);
					freqProt.fpi[1].thr = value;
					*rt=value;			  
					break;

				case MB_UF_Thr_1:
					setpara(MB_UF_Thr_1,value);						  
					arm_config_data_write(AD_UF_Thr_1,value);
					freqProt.fpi[2].thr = value;
					*rt=value;			  
					break;

				case MB_UF_Thr_2:
					setpara(MB_UF_Thr_2,value);						  
					arm_config_data_write(AD_UF_Thr_2,value);
					freqProt.fpi[3].thr = value;
					*rt=value;			  
					break;

				case MB_OF_Time_1:
					setpara(MB_OF_Time_1,value);						  
					arm_config_data_write(AD_OF_Time_1,value);
					freqProt.fpi[0].time = value;
					*rt=value;			  
					break;

				case MB_OF_Time_2:
					setpara(MB_OF_Time_2,value);						  
					arm_config_data_write(AD_OF_Time_2,value);
					freqProt.fpi[1].time = value;
					*rt=value;			  
					break;

				case MB_UF_Time_1:
					setpara(MB_UF_Time_1,value);						  
					arm_config_data_write(AD_UF_Time_1,value);
					freqProt.fpi[2].time = value;
					*rt=value;			  
					break;

				case MB_UF_Time_2:
					setpara(MB_UF_Time_2,value);						  
					arm_config_data_write(AD_UF_Time_2,value);
					freqProt.fpi[3].time = value;
					*rt=value;			  
					break;
						  
				default:
					err = 2;
					break;
			}
		} 
	
		else 
		{ //组态屏控制
			switch(saddr)
			{
					
				case Addr_Param110:
				{
					setpara(Addr_Param110,value);
		
					if(value==1) 
					{ //开机
						miro_write.manShutdownFlag = 0;
                        set_pcs_bootup();
						log_add(ET_OPSTATE,EST_MB_OPEN);
						mb_data.last_onoff_mark = 8;				
						miro_write.bootup_flag = 1; 
						//sys_ctl.onoff = 1;
					} 
					else if(value==2) 
					{ //关机
						miro_write.manShutdownFlag = 1;
                        set_pcs_shutdown();
						log_add(ET_OPSTATE,EST_MB_CLOSE);
						mb_data.last_onoff_mark = 4;
						miro_write.bootup_flag = 0; 
						//sys_ctl.onoff = 0;
					} 
					else if((GETBIT(value,6) == 1) || (GETBIT(value,7) == 1)) 
					{ //急停
						mb_data.last_onoff_mark = 4;
						sys_ctl.onoff = 0;				
						miro_write.manShutdownFlag = 1;
                        set_pcs_shutdown();
					}
					break;
				}
				case Addr_Param111:
					setpara(Addr_Param111,value);
					fpga_write(saddr, value);
					fpga_pdatabuf_write(saddr, value);
					*rt=value;
					break;
		
				default:
					err = 2;
					break;
			}
		}
	
		if(saddr == RESET_FLAG) 
		{
			if(value == 1) 
			{
				sys_reset();
			}
		}
	
		return err;
	}



/**
 * mbrs_readholdingreg --- 响应Read Holding Reg 请求
 * data:输入缓冲区
 * resp:输出缓冲区
 * len:输入缓冲区长度
 * 返回输出缓冲区长度
*/
int mbrs_readholdingreg(unsigned char *data, unsigned char *resp,
                        unsigned short len)
{
    unsigned short saddr, num, i;
    unsigned char err = 0;
    short reg;
    int rs;

    saddr = (data[0] << 8) + data[1];
    num = (data[2] << 8) + data[3];

    if(num < 1 || num > paranum)
    {
        err = 3;
    }
    else
    {
        for(i = 0; i < num; i++)
        {
            rs = readpara(saddr + i, &reg);
            if(rs != 0)
            {
                err = 2;
                break;
            }

            resp[(i << 1) + 2] = reg >> 8;
            resp[(i << 1) + 3] = reg & 0xff;
        }
    }

    if(err == 0)
    {
        resp[0] = 0x03;
        resp[1] = i << 1;
        return 2 + 2 * i;
    }
    else
    {
        resp[0] = 0x83;
        resp[1] = err;
        return 2;
    }
}

/**
 * mbrs_readinputreg --- 响应Read Input Reg 请求
 * data:输入缓冲区
 * resp:输出缓冲区
 * len:输入缓冲区长度
 * 返回输出缓冲区长度
*/
int mbrs_readinputreg(unsigned char *data, unsigned char *resp,
                      unsigned short len)
{
    unsigned short saddr, num, i;
    short reg;
    unsigned char err = 0;
    saddr = (data[0] << 8) + data[1];
    num = (data[2] << 8) + data[3];

    if(num < 1 || num > paranum)
    {
        err = 3;
    }
    else
    {
        for(i = 0; i < num; i++)
        {
            if(readpara(saddr + i, &reg) != 0)
            {
                err = 2;
                break;
            }

            resp[(i << 1) + 2] = (reg) >> 8;
            resp[(i << 1) + 3] = (reg) & 0xff;
        }
    }

    if(err == 0)
    {
        resp[0] = 0x04;
        resp[1] = i << 1;
        return 2 + 2 * i;
    }
    else
    {
        resp[0] = 0x84;
        resp[1] = err;
        return 2;
    }
}

/**
 * mbrs_writesinreg --- 响应Write Single Reg 请求
 * data:输入缓冲区
 * resp:输出缓冲区
 * len:输入缓冲区长度
 * 返回输出缓冲区长度
*/
int mbrs_writesinreg(unsigned char *data, unsigned char *resp,
                     unsigned short len)
{
    unsigned short saddr;
    short value, reg;
    unsigned char err = 0;

    saddr = (data[0] << 8) + data[1];
    value = (data[2] << 8) + data[3];

    tty_printf("saddr:%d(0x%x), value:0x%x\n", saddr, saddr, value);

    err = mb_writereg(saddr, value, &reg);
    tty_printf("err:%d\n", err);

    if(err == 0)
    {
        resp[0] = 0x06;
        resp[1] = saddr >> 8;
        resp[2] = saddr & 0xff;
        resp[3] = (reg) >> 8;
        resp[4] = (reg) & 0xff;
        return 5;
    }
    else
    {
        resp[0] = 0x86;
        resp[1] = err;
        return 2;
    }
}

/**
 * mbrs_writemulreg --- 响应Write Multiple Reg 请求
 * data:输入缓冲区
 * resp:输出缓冲区
 * len:输入缓冲区长度
 * 返回输出缓冲区长度
*/
int mbrs_writemulreg(unsigned char *data, unsigned char *resp,
                     unsigned short len)
{
    unsigned short saddr, num, bytecount, i;
    short reg, value;
    unsigned char err = 0;

    saddr = (data[0] << 8) + data[1];
    num = (data[2] << 8) + data[3];
    bytecount = data[4];

    if(num < 1 || num > paranum || (bytecount != (num << 1)))
    {
        err = 3;
    }
    else
    {
        for(i = 0; i < num; i++)
        {
            value = (data[5 + (i << 1)] << 8) + (data[6 + (i << 1)]);
            err = mb_writereg(saddr + i, value, &reg);

            if(err != 0)
            {
                break;
            }
        }
    }

    if(err == 0)
    {
        resp[0] = 0x10;
        resp[1] = saddr >> 8;
        resp[2] = saddr & 0xff;
        resp[3] = num >> 8;
        resp[4] = num & 0xff;
        return 5;
    }
    else
    {
        resp[0] = 0x90;
        resp[1] = err;
        return 2;
    }
}

/**
 * mbrs_rwmulreg --- 响应Read/Write Multiple Reg 请求
 * data:输入缓冲区
 * resp:输出缓冲区
 * len:输入缓冲区长度
 * 返回输出缓冲区长度
*/
int mbrs_rwmulreg(unsigned char *data, unsigned char *resp,
                  unsigned short len)
{
    unsigned short rsaddr, wsaddr, rnum, wnum, bytecount, i;
    short reg, value;
    unsigned char err = 0;

    rsaddr = (data[0] << 8) + data[1];
    rnum = (data[2] << 8) + data[3];
    wsaddr = (data[4] << 8) + data[5];
    wnum = (data[6] << 8) + data[7];
    bytecount = data[8];

    if(rnum < 1 || rnum > paranum || wnum < 1 || wnum > paranum ||
            (bytecount != wnum << 1))
    {
        err = 3;
    }
    else
    {
        for(i = 0; i < wnum; i++)
        {
            value = data[9 + (i << 1)] << 8 + data[10 + (i << 1)];

            if((mb_writereg(wsaddr + i, value, &reg)))
            {
                err = 2;
                break;
            }
        }

        if(err == 0)
        {
            for(i = 0; i < rnum; i++)
            {
                if(readpara(rsaddr + i, &reg) != 0)
                {
                    err = 2;
                    break;
                }

                resp[(i << 1) + 2] = (reg) >> 8;
                resp[(i << 1) + 3] = (reg) & 0xff;
            }
        }
    }

    if(err == 0)
    {
        resp[0] = 0x17;
        resp[1] = i << 1;
        return 2 + (i << 1);
    }
    else
    {
        resp[0] = 0x97;
        resp[1] = err;
        return 2;
    }
}

/**
 * @brief Transmit real-time waveform
 *
 * @param a pointer to the Modbus connection information
 *
 * @param data received data segment
 *
 * @param resp the buffer respond to client
 *
 * @param len the length of PDU(inlcude function code)
 *
 * @return the length of respond buffer
 */
int mbrs_transcurwave(_mbser_inf* msrinf, unsigned char* data,
                      unsigned char* resp, unsigned short len)
{
    unsigned short inv[CHANNEL_MAX], nps[CHANNEL_MAX];
    unsigned int curtail;
    unsigned char num = data[8];
    int i, j;

    msrinf->sendwave_inf.num = num;
    msrinf->sendwave_inf.resp[0] = data[0];
    msrinf->sendwave_inf.resp[1] = data[1];
    msrinf->sendwave_inf.resp[2] = data[2];
    msrinf->sendwave_inf.resp[3] = data[3];
    msrinf->sendwave_inf.resp[4] = data[6];

    for(i = 0; i < CHANNEL_MAX; i++)
    {
        inv[i] = msrinf->sendwave_inf.inv[i];
        nps[i] = msrinf->sendwave_inf.nps[i];
        msrinf->sendwave_inf.inv[i] = 0;
        msrinf->sendwave_inf.nps[i] = 0;
    }

    msrinf->sendwave_inf.enable = 0;

    i = 9;
    curtail = rtwq_tail;

    while(num)
    {
        if(data[i] < CHANNEL_MAX)
        {
            msrinf->sendwave_inf.nps[data[i]] = (data[i + 1] << 8) + data[i + 2];
            msrinf->sendwave_inf.inv[data[i]] = (data[i + 3] << 8) + data[i + 4];
            //tty_printf("channel%d:%d,%d\n", data[i], msrinf->sendwave_inf.inv[data[i]], msrinf->sendwave_inf.nps[data[i]]);

            if(inv[data[i]] == 0)
            {
                for(j = 0; j < CHANNEL_MAX; j++)
                {
                    if(msrinf->sendwave_inf.nps[data[i]] == nps[j] &&
                            msrinf->sendwave_inf.inv[data[i]] == inv[j])
                    {
                        msrinf->sendwave_inf.tail[data[i]] = msrinf->sendwave_inf.tail[j];
                        break;
                    }
                }

                if(j >= CHANNEL_MAX)
                {
                    msrinf->sendwave_inf.tail[data[i]] = curtail;
                }

                nps[data[i]] = msrinf->sendwave_inf.nps[data[i]];
                inv[data[i]] = msrinf->sendwave_inf.inv[data[i]];
            }
        }

        i += 5;
        num--;
    }

    msrinf->sendwave_inf.enable = 1;

    return 1;
}

/**
 * @brief Transmit fault records(in RAM)
 *
 * @param data received data segment
 *
 * @param resp the buffer respond to client
 *
 * @param len the length of PDU(inlcude function code)
 *
 * @return the length of respond buffer
 */
int mbrs_transfaultinf(unsigned char *data, unsigned char *resp,
                       unsigned short len)
{
    int err;
    //unsigned int widx, wch, plen;
    //unsigned short wnum;

    switch(data[0])
    {
        //huangwh 传输记录的状态信息
    case 0:
        //            resp[0] = 0x65;
        //            resp[1] = 0x0;
        //            resp[2] = faultinf.flag;
        //            tty_printf("上位机读取故障记录信息\n");

        //            if(faultinf.flag == 0)
        //            {
        //                tty_printf("无故障记录\n");
        //                memset(&resp[3], 0, 28);
        //            }
        //            else
        //            {
        //                resp[3] = (fault_save_data.brief.fid >> 8) & 0xff;
        //                resp[4] = fault_save_data.brief.fid & 0xff;
        //                resp[5] = BCD(fault_save_data.brief.year);
        //                resp[6] = BCD(fault_save_data.brief.mon);
        //                resp[7] = BCD(fault_save_data.brief.date);
        //                resp[8] = BCD(fault_save_data.brief.hour);
        //                resp[9] = BCD(fault_save_data.brief.min);
        //                resp[10] = BCD(fault_save_data.brief.sec);
        //                resp[11] = BCD(fault_save_data.brief.ms / 100);
        //                resp[12] = BCD(fault_save_data.brief.ms % 100);
        //                resp[13] = (fault_save_data.brief.wnum >> 24) & 0xff;
        //                resp[14] = (fault_save_data.brief.wnum >> 16) & 0xff;
        //                resp[15] = (fault_save_data.brief.wnum >> 8) & 0xff;
        //                resp[16] = (fault_save_data.brief.wnum) & 0xff;
        //                resp[17] = 0;
        //                resp[18] = 0;
        //                resp[19] = 0;
        //                resp[20] = 0;
        //                resp[21] = (fault_save_data.brief.widx >> 24) & 0xff;
        //                resp[22] = (fault_save_data.brief.widx >> 16) & 0xff;
        //                resp[23] = (fault_save_data.brief.widx >> 8) & 0xff;
        //                resp[24] = (fault_save_data.brief.widx) & 0xff;
        //                resp[25] = 0;
        //                resp[26] = 0;
        //                resp[27] = 0;
        //                resp[28] = 0;
        //                resp[29] = (paranum >> 8) & 0xff;
        //                resp[30] = paranum & 0xff;
        //            }

        return 31;

        //huangwh 传输波形数据
    case 1:

        //            if(faultinf.flag == 0)
        //            {
        //                err = 3;
        //                break;
        //            }

        //            widx = (data[2] << 24) | (data[3] << 16) | (data[4] << 8) | data[5];
        //            wnum = (data[6] << 8) | data[7];
        //            wch = data[1];

        //            if((wnum + widx) > fault_save_data.brief.wnum)
        //            {
        //                err = 2;
        //                break;
        //            }

        //            resp[0] = 0x65;
        //            resp[1] = 0x1;
        //            resp[2] = data[1];
        //            resp[3] = data[2];
        //            resp[4] = data[3];
        //            resp[5] = data[4];
        //            resp[6] = data[5];
        //            resp[7] = data[6];
        //            resp[8] = data[7];
        //            tty_printf("通道:%d,数量:0x%x,偏移:0x%x\n", wch, wnum, widx);
        //            copyftwave((short*)&resp[9], wch, wnum, widx);
        //            plen = 9 + (wnum << 1);
        //            return plen;
        //huangwh 传输参数数据
    case 2:

        //            if(faultinf.flag == 0)
        //            {
        //                err = 3;
        //                break;
        //            }

        //            resp[0] = 0x65;
        //            resp[1] = 0x2;
        //            resp[2] = (paranum >> 8) & 0xff;
        //            resp[3] = paranum & 0xff;
        //            memcpy(&resp[4], fault_save_data.para.para, paranum << 2);
        //            plen = 4 + (paranum << 2);
        //            return plen;
    default:
        err = 2;
        break;
    }

    resp[0] = 229;
    resp[1] = err;
    return 2;
}

/**
 * @brief Transmit fault history records(in ROM)
 *
 * @param data received data segment
 *
 * @param resp the buffer respond to client
 *
 * @param len the length of PDU(inlcude function code)
 *
 * @return the length of respond buffer
 */
int mbrs_transfaultinf2(unsigned char *data, unsigned char *resp,
                        unsigned short len)
{
    int err;
    int j;
    unsigned int widx, wch, plen;
    unsigned short wnum;
    uint16_t fidx,i,n = 0;

    switch(data[0])
    {
        //huangwh 传输记录的状态信息
    case 0:

        resp[0] = 0x6a;   //huangwh 功能码
        resp[1] = 0x0;    //huangwh 子功能码
        resp[2] = 1;
        resp[3] = 1;
        /* 获取故障记录数量 */
        fidx = getfaultnum();
        j = 4;
        for(i = 0; i < fidx; i++)
        {
            tty_printf("读取故障概要: %d\n", i);

            if(fault_read_brief(i+1) == 0)
            {
                /* 计算有效的记录数量 */
                n++;
                /* 索引 */
                resp[j++] = (fidx >> 8) & 0xff;
                resp[j++] = fidx & 0xff;
                /* 故障类型 */
                resp[j++] = (wave.fault.brief.fid >> 8) & 0xff;
                resp[j++] = wave.fault.brief.fid & 0xff;
                /* 记录时间 */
                resp[j++] = BCD(wave.fault.brief.year);
                resp[j++] = BCD(wave.fault.brief.mon);
                resp[j++] = BCD(wave.fault.brief.date);
                resp[j++] = BCD(wave.fault.brief.hour);
                resp[j++] = BCD(wave.fault.brief.min);
                resp[j++] = BCD(wave.fault.brief.sec);
                resp[j++] = BCD(wave.fault.brief.ms / 100);
                resp[j++] = BCD(wave.fault.brief.ms % 100);
                /* 波形节点数 */
                tty_printf("节点数:%d\n", wave.fault.brief.wnum);
                resp[j++] = (wave.fault.brief.wnum >> 24) & 0xff;
                resp[j++] = (wave.fault.brief.wnum >> 16) & 0xff;
                resp[j++] = (wave.fault.brief.wnum >> 8) & 0xff;
                resp[j++] = (wave.fault.brief.wnum) & 0xff;
                /* 故障点索引 */
                tty_printf("故障点索引:%d\n", wave.fault.brief.widx);
                resp[j++] = (wave.fault.brief.widx >> 24) & 0xff;
                resp[j++] = (wave.fault.brief.widx >> 16) & 0xff;
                resp[j++] = (wave.fault.brief.widx >> 8) & 0xff;
                resp[j++] = (wave.fault.brief.widx) & 0xff;
                /* 波形通道数 */
                resp[j++] = CHANNEL_MAX;
                /* 参数数量 */
                resp[j++] = (paranum >> 8) & 0xff;
                resp[j++] = paranum & 0xff;
            }
            else
            {
                tty_printf("读取故障概要: %d失败\n", i);
            }
        }

        //huangwh resp[2] resp[3] 故障记录数量
        resp[2] = (n >> 8) & 0xff;
        resp[3] = n & 0xff;

        return j;
        //huangwh 传输波形数据
    case 1:
        tty_printf("客户端要求发送故障波形\n");
        fidx = (data[1] << 8) | data[2];
        wch  = data[3];
        widx = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7];
        wnum = (data[8] << 8) | data[9];

        tty_printf("索引: %d, 通道: %d, 数量: 0x%x, 偏移: 0x%x\n", fidx, wch, wnum, widx);

        if(widx == 0 && wch == 0)
        {
            if(fault_read(fidx))
            {
                err = 3;
                break;
            }
        }

        if((wnum + widx) > wave.fault.brief.wnum)
        {
            err = 2;
            break;
        }

        if(wch >= wave.fault.brief.cnum)
        {
            err = 2;
            break;
        }

        /* 为保证传输速度，只在传输第一组波形的时候校验波形数据 */
        if(wch == 0 && widx == 0)
        {
//                  if(fs_wave_valid(fidx) == 0)
//                  {
//                      err = 4;
//											break;
//                  }
        }

        resp[0] = 0x6a;
        resp[1] = 0x1;
        /* 索引 */
        resp[2] = data[1];
        resp[3] = data[2];
        /* 通道标识 */
        resp[4] = data[3];
        /* 波形代码 */
        resp[5] = wave.fault.brief.cid[wch];
        /* 起始节点号 */
        resp[6] = data[4];
        resp[7] = data[5];
        resp[8] = data[6];
        resp[9] = data[7];
        /* 传输数量 */
        resp[10] = data[8];
        resp[11] = data[9];
        memcpy((short*)&resp[12], &wave.fault.wave.ft_cv[wch][widx], wnum <<1);

//              fs_wave_copy((short*)&resp[12], fidx, wch, widx, wnum);
        plen = 12 + (wnum << 1);
        return plen;
        //huangwh 传输参数数据
    case 2:
        tty_printf("客户端要求发送故障参数\n");

        fidx = (data[1] << 8) | data[2];
        tty_printf("索引: %d\n", fidx);
        if(fault_read(fidx))
        {
            err = 3;
            break;
        }
        resp[0] = 0x6a;
        resp[1] = 0x2;
        resp[2] = data[1];
        resp[3] = data[2];
        resp[4] = (wave.fault.brief.num >> 8) & 0xff;
        resp[5] = wave.fault.brief.num & 0xff;
        memcpy((short *)&resp[6], wave.fault.para.para, wave.fault.brief.num << 2);
        plen = 6 + (wave.fault.brief.num << 2);
        return plen;
    default:
        err = 2;
        break;
    }

//        resp[0] = 234;
    resp[1] = err;
    return 2;
}


extern struct fmFileType fmFile;

int mbrs_transfmlog(unsigned char *data, unsigned char *resp,
                        unsigned short len)
{

    int err;
    int j;
    unsigned int widx, wch, plen;
    unsigned short wnum;
    uint16_t fidx,i,n = 0;

    switch(data[0])
    {
        //huangwh 传输记录的状态信息
    case 0:

        resp[0] = 0x6b;   //huangwh 功能码
        resp[1] = 0x0;    //huangwh 子功能码
        /* 获取故障记录数量 */
        fidx = FM_GetFMLogQty();
        j = 4;
        for(i = 0; i < fidx; i++)
        {
            tty_printf("读取故障概要: %d\n", i);

            if(FM_ReadBriefInfor(i+1) == 0)
            {
                /* 计算有效的记录数量 */
                n++;
                /* 索引 */
                resp[j++] = ((i+1) >> 8) & 0xff;
                resp[j++] = (i+1) & 0xff;
                /* 故障类型 */
                resp[j++] = (fmFile.fft_infor.brief.fid >> 8) & 0xff;
                resp[j++] = fmFile.fft_infor.brief.fid & 0xff;
                /* 记录时间 */
                resp[j++] = BCD(fmFile.fft_infor.brief.year);
                resp[j++] = BCD(fmFile.fft_infor.brief.mon);
                resp[j++] = BCD(fmFile.fft_infor.brief.date);
                resp[j++] = BCD(fmFile.fft_infor.brief.hour);
                resp[j++] = BCD(fmFile.fft_infor.brief.min);
                resp[j++] = BCD(fmFile.fft_infor.brief.sec);
                resp[j++] = BCD(fmFile.fft_infor.brief.ms / 100);
                resp[j++] = BCD(fmFile.fft_infor.brief.ms % 100);
                /* 波形节点数 */
                tty_printf("节点数:%d\n", fmFile.fft_infor.brief.wnum);
                resp[j++] = (fmFile.fft_infor.brief.wnum >> 24) & 0xff;
                resp[j++] = (fmFile.fft_infor.brief.wnum >> 16) & 0xff;
                resp[j++] = (fmFile.fft_infor.brief.wnum >> 8) & 0xff;
                resp[j++] = (fmFile.fft_infor.brief.wnum) & 0xff;
                /* 故障点索引 */
                tty_printf("故障点索引:%d\n", fmFile.fft_infor.brief.widx);
                resp[j++] = (fmFile.fft_infor.brief.widx >> 24) & 0xff;
                resp[j++] = (fmFile.fft_infor.brief.widx >> 16) & 0xff;
                resp[j++] = (fmFile.fft_infor.brief.widx >> 8) & 0xff;
                resp[j++] = (fmFile.fft_infor.brief.widx) & 0xff;
                /* 波形通道数 */
                resp[j++] = FM_CHANNEL_MAX;
                /* 参数数量 */
                resp[j++] = 0;
                resp[j++] = 0;
            }
            else
            {
                tty_printf("读取故障概要: %d失败\n", i);
            }
        }

        //huangwh resp[2] resp[3] 故障记录数量
        resp[2] = (fidx >> 8) & 0xff;
        resp[3] = fidx & 0xff;

        return j;
        //huangwh 传输波形数据
    case 1:
        tty_printf("客户端要求发送故障波形\n");
        fidx = (data[1] << 8) | data[2];
        wch  = data[3];
        widx = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7];
        wnum = (data[8] << 8) | data[9];

        tty_printf("索引: %d, 通道: %d, 数量: 0x%x, 偏移: 0x%x\n", fidx, wch, wnum, widx);

        if(widx == 0 && wch == 0)
        {
            if(FM_ReadFMLog(fidx))
            {
                tty_printf("错误1\n");
                err = 3;
                break;
            }
        }

        if((wnum + widx) > fmFile.fft_infor.brief.wnum)
        {
            tty_printf("错误2\n");
            err = 2;
            break;
        }

        if(wch >= fmFile.fft_infor.brief.cnum)
        {
            tty_printf("错误3\n");
            err = 2;
            break;
        }

        /* 为保证传输速度，只在传输第一组波形的时候校验波形数据 */
        if(wch == 0 && widx == 0)
        {
//                  if(fs_wave_valid(fidx) == 0)
//                  {
//                      err = 4;
//                                          break;
//                  }
        }

        resp[0] = 0x6b;
        resp[1] = 0x1;
        /* 索引 */
        resp[2] = data[1];
        resp[3] = data[2];
        /* 通道标识 */
        resp[4] = data[3];
        /* 波形代码 */
        resp[5] = fmFile.fft_infor.brief.cid[wch];
        /* 起始节点号 */
        resp[6] = data[4];
        resp[7] = data[5];
        resp[8] = data[6];
        resp[9] = data[7];
        /* 传输数量 */
        resp[10] = data[8];
        resp[11] = data[9];
        
        tty_printf("正常传输波形数据.\n");
        memcpy((short*)&resp[12], &fmFile.fft_infor.wave.ft_cv[wch][widx], wnum <<1);

//              fs_wave_copy((short*)&resp[12], fidx, wch, widx, wnum);
        plen = 12 + (wnum << 1);
        return plen;
        //huangwh 传输参数数据
    default:
        err = 2;
        break;
    }

//        resp[0] = 234;
    resp[1] = err;
    return 2;
}


/**
 * @brief Transmit logs
 *
 * @param msrinf a pointer to the Modbus connection information
 *
 * @param data received data segment
 *
 * @param resp the buffer respond to client
 *
 * @param len the length of PDU(inlcude function code)
 *
 * @return the length of respond buffer
 */
int mbrs_translog(_mbser_inf *msrinf, unsigned char *data,
                  unsigned char *resp, unsigned short len)
{
    msrinf->sendlog_inf.resp[0] = data[0];
    msrinf->sendlog_inf.resp[1] = data[1];
    msrinf->sendlog_inf.resp[2] = data[2];
    msrinf->sendlog_inf.resp[3] = data[3];
    msrinf->sendlog_inf.resp[4] = data[6];
    //tty_printf("\t %d-%d-%d-%d-%d\n", data[0], data[1], data[2], data[3], data[6]);

    if(data[8] == 0)
    {
        msrinf->sendlog_inf.enflag = data[9];
        osSemaphoreWait(log_sem, osWaitForever);
        msrinf->sendlog_inf.tail = log_head;
        msrinf->sendlog_inf.num = log_cnt;
        osSemaphoreRelease(log_sem);
        tty_printf("收到使能标志:%d\n", msrinf->sendlog_inf.enflag);
    }
    else if(data[8] == 1)
    {
        msrinf->sendlog_inf.rnum = (data[9] << 8) + data[10];
        tty_printf("客户端已接收上次发送的%d条日志\n", msrinf->sendlog_inf.rnum);
    }

    return 0;
}

/**
 * @brief Transmit all the parameters(registers)
 *
 * @param msrinf a pointer to the Modbus connection information
 *
 * @param data received data segment
 *
 * @param resp the buffer respond to client
 *
 * @param len the length of PDU(inlcude function code)
 *
 * @return the length of respond buffer
 */
int mbrs_transallpara(_mbser_inf *msrinf, unsigned char *data,
                      unsigned char *resp, unsigned short len)
{
    msrinf->sendallp_inf.resp[0] = data[0];
    msrinf->sendallp_inf.resp[1] = data[1];
    msrinf->sendallp_inf.resp[2] = data[2];
    msrinf->sendallp_inf.resp[3] = data[3];
    msrinf->sendallp_inf.resp[4] = data[6];
    msrinf->sendallp_inf.inv = (data[8] << 8) + data[9];
    msrinf->sendallp_inf.tick = osKernelSysTick();
    tty_printf("time:%d\n", msrinf->sendallp_inf.tick);
    tty_printf("inv:%d\n", msrinf->sendallp_inf.inv);

    return 0;
}
/**
 * @brief Receiving firmware data, and upgrade firmware
 *
 * @param msrinf the point of the Modbus connection information
 *
 * @param data received data segment
 *
 * @param resp the buffer respond to client
 *
 * @param len the length of PDU(inlcude function code)
 *
 * @return the length of respond buffer
 */
int mbrs_update(_mbser_inf *msrinf, unsigned char *rbuf,				//POF_LOAD
                unsigned char *resp, unsigned short datalen)
{
    unsigned int codesize;
    unsigned int offset, len;
    unsigned char *data;

    data = &rbuf[0];

    switch(data[0])
    {
    case 0:
        tty_printf("客户端要求进行软件更新\n");
        resp[0] = 0x68;
        resp[1] = 0x0;
        codesize = ((data[1] << 24) & 0xff000000) |
                   ((data[2] << 16) & 0xff0000) |
                   ((data[3] << 8) & 0xff00) |
                   (data[4] & 0xff);

        if(update_inf.flag != 0)
        {
            tty_printf("未停机或正在更新\n");
            resp[2] = 1;
        }
        else if(((data[21] == 0) && (codesize > 0x10000)) ||
                ((data[21] == 1) && (codesize > 0x20000)) ||
                ((data[21] == 2) && (codesize > 0x300000)) ||
                ((data[21] == 3) && (codesize > POF_LENGTH)) ||
                (codesize == 0))
        {
            tty_printf("代码大小不符合要求\n");
            resp[2] = 2;
        }
        else
        {
            tty_printf("通知客户端可以进行更新操作\n");

            tty_printf("文件大小:%d\n", codesize);

            update_inf.flag = 1;
            memcpy(update_inf.md5, &data[5], 16);
            //test
            print_md5(update_inf.md5);
            update_inf.type = data[21];
            if(update_inf.type == 3)
            {
                update_inf.codesize = codesize - POF_HEAD_LENGTH - POF_TAIL_LENGTH;
            }
            else
            {
                update_inf.codesize = codesize;
            }
            update_inf.recved = 0;

            msrinf->update_ctrl.valid = 1;
            msrinf->update_ctrl.state = UPDATE_IDLE;
            firmware_update_timout_set(msrinf);

            resp[2] = 0;
        }

        resp[3] = (UPDATE_ONCE_SIZE >> 24) & 0xff;
        resp[4] = (UPDATE_ONCE_SIZE >> 16) & 0xff;
        resp[5] = (UPDATE_ONCE_SIZE >> 8) & 0xff;
        resp[6] = UPDATE_ONCE_SIZE & 0xff;
		firmware_check_delthreah();
        return 7;

    case 1:
        if(msrinf->update_ctrl.valid == 0)
        {
            tty_printf("未收到客户端请求，忽略更新命令\n");
            return 0;
        }

        resp[0] = 0x68;
        resp[1] = 0x1;

        offset = ((data[1] << 24) & 0xff000000) |
                 ((data[2] << 16) & 0xff0000) |
                 ((data[3] << 8) & 0xff00) |
                 ((data[4]) & 0xff);

        len = ((data[5] << 24) & 0xff000000) |
              ((data[6] << 16) & 0xff0000) |
              ((data[7] << 8) & 0xff00) |
              ((data[8]) & 0xff);

        if(update_inf.type == 3)
        {
            if(offset + len > POF_LENGTH)
            {
                tty_printf("请求的传输长度不正确，忽略更新命令\n");
                firmware_update_finish(msrinf);
                return 0;
            }
        }
        else
        {
            if(offset + len > update_inf.codesize)
            {
                tty_printf("请求的传输长度不正确，忽略更新命令\n");
                firmware_update_finish(msrinf);
                return 0;
            }
        }

        if(msrinf->update_ctrl.state == UPDATE_IDLE)
        {
            msrinf->update_ctrl.resp[0] = rbuf[0];
            msrinf->update_ctrl.resp[1] = rbuf[1];
            msrinf->update_ctrl.resp[2] = rbuf[2];
            msrinf->update_ctrl.resp[3] = rbuf[3];
            msrinf->update_ctrl.resp[4] = rbuf[6];
            update_inf.re_temp = len;

            if(update_inf.type == 3)
            {
                update_inf.offset = offset;
                if(update_inf.offset == 0)
                {
                    if(data[9] == 'P' && data[10] == 'O' && data[11] == 'F')
                    {
                        memcpy(pof_head, &data[9], POF_HEAD_LENGTH);
                        update_inf.recved = POF_HEAD_LENGTH;
                        msrinf->update_ctrl.state = UPDATE_PROGRAM_SUCCESS;
                        return 0;
                    }
					else
					{
						tty_printf("文件格式错误\n");
					}
                }
                else if(update_inf.offset + POF_TAIL_LENGTH >= POF_LENGTH)
                {
                    update_inf.recved += len;
                    msrinf->update_ctrl.state = UPDATE_PROGRAM_SUCCESS;
                    memcpy(pof_tail, &data[9], POF_TAIL_LENGTH);
                    return 0;
                }
                else
                {
                    update_inf.len = len;
                    update_inf.offset = offset - POF_HEAD_LENGTH;
                    memcpy(update_inf.buf, &data[9], update_inf.len);
                }
            }
            else if(update_inf.type == 2)
            {
                update_inf.offset = offset;
                update_inf.len = len;
                memcpy(update_inf.buf, &data[9], len);
            }

            if(firmware_update_start(msrinf) != 0)
            {
                tty_printf("启动固件更新失败\n");
                firmware_update_finish(msrinf);
                return 0;
            }
        }
        else
        {
            tty_printf("之前的更新操作尚未完成\n");
            return 0;
        }

        return 0;

    case 2:
        if(msrinf->update_ctrl.state == UPDATE_IDLE)
        {
            msrinf->update_ctrl.resp[0] = rbuf[0];
            msrinf->update_ctrl.resp[1] = rbuf[1];
            msrinf->update_ctrl.resp[2] = rbuf[2];
            msrinf->update_ctrl.resp[3] = rbuf[3];
            msrinf->update_ctrl.resp[4] = rbuf[6];

            if(firmware_check_start(msrinf) != 0)
            {
                tty_printf("启动固件校验失败\n");
                firmware_update_finish(msrinf);
                return 0;
            }
        }
        else
        {
            tty_printf("之前的更新操作尚未完成\n");
            return 0;
        }

        return 0;

    default:
        resp[0] = 0xe8;
        resp[1] = 1;
        return 2;
    }
}

static int is_msinf_invalid(_mbser_inf *msinf)
{
    if(msinf == NULL)
    {
        return 1;
    }

    if(msinf->conns == 0 ||
            msinf->pConn <= 0)
    {
        return 1;
    }

    return 0;
}

static int is_time2_send_paras(_mbser_inf *msinf)
{
    unsigned int elapse;
    unsigned int curtick;

    if(msinf->sendallp_inf.inv == 0)
    {
        return 0;
    }

    curtick = osKernelSysTick();

    if(msinf->sendallp_inf.tick <= curtick)
    {
        elapse = curtick - msinf->sendallp_inf.tick;
    }
    else
    {
        elapse = 0xffffffff - msinf->sendallp_inf.tick + 1 + curtick;
    }

    if(elapse >= MS2TICK(msinf->sendallp_inf.inv))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static int is_time2_send_waves(int wave_idx, unsigned int cur_tail,
                               _mbser_inf *msinf)
{
    int need_num, len;
    unsigned int last_tail;

    need_num = msinf->sendwave_inf.nps[wave_idx];
    last_tail = msinf->sendwave_inf.tail[wave_idx];
    len = (cur_tail >= last_tail) ? (cur_tail - last_tail) :
          (RTWQ_MAX_SIZE + cur_tail - last_tail);

    if(len >= need_num)
    {
        msinf->sendwave_inf.tail[wave_idx] =
            (last_tail + need_num >= RTWQ_MAX_SIZE) ?
            (last_tail + need_num - RTWQ_MAX_SIZE) :
            (last_tail + need_num);
        return 1;
    }

    return 0;
}

static int mb_recv(_mbser_inf *msinf, void *mem, size_t len)
{
    int err;
    size_t remain = len;
    char *buf = mem;

    while(remain)
    {
        err = recv(msinf->pConn, buf, len, 0);

        if(err == BSD_ERROR_TIMEOUT)
        {
            continue;
        }

        if(err < 0)
        {
            return err;
        }

        buf += err;
        remain -= err;
    }

    return len;
}

uint32_t mb_right = 0, mb_err = 0;

int mb_send (int sock, const char *buf, int len, int flags)
{
    int err;

    osSemaphoreWait(mb_send_sem, osWaitForever);

    if(len == 1110)
    {
        if(buf[7] == 0x67)
        {
            mb_right++;
        }
        else
        {
            mb_err++;
        }
    }

    err = send(sock, buf, len, flags);

    osSemaphoreRelease(mb_send_sem);

    return err;
}


/**
 * @brief Receiving firmware data, and upgrade firmware
 *
 * @param msrinf the point of the Modbus connection information
 *
 * @param data received data segment
 *
 * @param resp the buffer respond to client
 *
 * @param len the length of PDU(inlcude function code)
 *
 * @return the length of respond buffer
 */
static int mbhandle(_mbser_inf *mbsinf, unsigned char *rbuf,
                    unsigned char *resp, unsigned int rlen)
{
    unsigned short len;
    int resplen = 0;
	short state8;
	
    resp[0] = rbuf[0];
    resp[1] = rbuf[1];
    resp[2] = rbuf[2];
    resp[3] = rbuf[3];


    len = (rbuf[4] << 8) + rbuf[5];

    if(rlen < (len + 6) || rbuf[2] != 0 || rbuf[3] != 0)
    {
        return 0;
    }

    resp[6] = rbuf[6];
    tty_printf("MB功能码 is %d(0x%x) rlen(%d) rbuf[8-9]=(%d-%d)\n", rbuf[7], rbuf[7], rlen, resp[8],resp[9]);
    switch(rbuf[7])
    {
    case 0x03:
        resplen = mbrs_readholdingreg(&rbuf[8], &resp[7], len - 1);
        break;

    case 0x04:
        resplen = mbrs_readinputreg(&rbuf[8], &resp[7], len - 1);
        break;

    case 0x06:
        resplen = mbrs_writesinreg(&rbuf[8], &resp[7], len - 1);
        break;

    case 0x10:
        resplen = mbrs_writemulreg(&rbuf[8], &resp[7], len - 1);
        break;

    case 0x17:
        resplen = mbrs_rwmulreg(&rbuf[8], &resp[7], len - 1);
        break;

    case 0x64:
//		cmd1=fpga_read(Addr_Param111);
//		cmd1=RESETBIT(cmd1,15);
//		fpga_write(Addr_Param111,cmd1);
//		fpga_pdatabuf->set[Addr_Param111]=cmd1;
//		setpara(Addr_Param111,cmd1);
//		printf("cmd1 is %x\n",cmd1);
//		SETBIT(cmd1,15);
//		fpga_write(Addr_Param111,cmd1);
//		printf("cmd1 is %x\n",cmd1);
		resplen = mbrs_transcurwave(mbsinf, rbuf, resp, rlen);
		break;

    case 0x65:
        resplen = mbrs_transfaultinf(&rbuf[8], &resp[7], len - 1);
        break;

    case 0x66:
        resplen = mbrs_translog(mbsinf, rbuf, resp, rlen);
        break;

    case 0x67:
        resplen = mbrs_transallpara(mbsinf, rbuf, resp, rlen);
        break;

    case 0x68:
		//更新固件需停机操作20191027	
		state8 = fpga_read(STATE8_ADD);
		#if 0
		if(((state8 & 0xf)>FSM_IDLE)&&((state8 & 0xf)<FSM_FAULT))
		{
			set_pcs_shutdown();
			log_add(ET_OPSTATE,EST_PROCEDURE_UPDATE);	
		}
		#endif
        resplen = mbrs_update(mbsinf, &rbuf[8], &resp[7], len - 1);
        break;

    case 0x6A:
        resplen = mbrs_transfaultinf2(&rbuf[8], &resp[7], len - 1);
        break;
    case 0x6B:
        resplen = mbrs_transfmlog(&rbuf[8], &resp[7], len - 1);
        break;
    default:
        tty_printf("未定义的功能码: %d\n", rbuf[7]);
        break;
    }

    if((rbuf[7] == 0x64) || (rbuf[7] == 0x66) || (rbuf[7] == 0x67))
    {
        return 0;
    }

    if(resplen == 0)
    {
        return 0;
    }

    resp[4] = (resplen + 1) >> 8;
    resp[5] = (resplen + 1) & 0xff;

    return resplen + 7;
}

static int mb_recv_handle(_mbser_inf *mbsinf, unsigned char *recvbuf, int sock)
{
    unsigned char *respbuf;//*recvbuf;//rev[128]="";
    int leftlen;
    int resplen;
    int res;

    recvbuf = mbsinf->sendmb_inf.recvbuf;
    respbuf = mbsinf->sendmb_inf.respbuf;

    res = mb_recv(mbsinf, recvbuf, 6);

    if(res == BSD_ERROR_TIMEOUT)
    {
        return 0;
    }

    if(res <= 0)
    {
        tty_printf("res: %d\n", res);
        return 1;
    }

    leftlen = (recvbuf[4] << 8) | recvbuf[5];

    res = mb_recv(mbsinf, &recvbuf[6], leftlen);

    if(res == BSD_ERROR_TIMEOUT)
    {
        return 0;
    }

    if(res <= 0)
    {
        tty_printf("res: %d\n", res);
        return 1;
    }

    osSemaphoreWait(mb_send_sem, osWaitForever);
    resplen = mbhandle(mbsinf, (unsigned char *)recvbuf, respbuf, leftlen + 6);
    osSemaphoreRelease(mb_send_sem);

    if(resplen > 0)
    {
        int err;
        //osSemaphoreWait(mb_send_sem, osWaitForever);
        err = mb_send(sock, (const char *)respbuf, resplen, 0);
        //osSemaphoreRelease(mb_send_sem);
        if(err < 0)
        {
            tty_printf("网络发送失败！: %d\n", err);
            return 1;
        }

        resplen = 0;
    }

    return 0;
}

uint32_t buf_err = 0;

static uint8_t spec_buf[10000];

int send_paras(_mbser_inf *msinf)
{
    int err = 0;
    int nerr = 0;
    int sock;
    uint8_t *sendbuf;

    if(is_msinf_invalid(msinf))
    {
        err = 1;
        return err;
    }

    sock = msinf->pConn;
    sendbuf = spec_buf;

    /* 传输参数 */
    if(is_time2_send_paras(msinf))
    {
        sendbuf[0] = msinf->sendallp_inf.resp[0];
        sendbuf[1] = msinf->sendallp_inf.resp[1];
        sendbuf[2] = msinf->sendallp_inf.resp[2];
        sendbuf[3] = msinf->sendallp_inf.resp[3];
        sendbuf[4] = ((paranum << 2) + 4) >>
                     8;     //(+4 : contain unitid && command && len , Altogether 4 char)
        sendbuf[5] = ((paranum << 2) + 4) & 0xff;
        sendbuf[6] = msinf->sendallp_inf.resp[4];
        sendbuf[7] = 0x67;
        sendbuf[8] = paranum >> 8;
        sendbuf[9] = paranum & 0xff;
        memcpy(&sendbuf[10], para_list, paranum * 4);
        if(sendbuf[7] != 0x67)
        {
            buf_err++;
        }
        nerr = mb_send(sock, (const char *)sendbuf, paranum * 4 + 10, 0);
        if(nerr != paranum * 4 + 10)
        {
            err = 1;
        }

        msinf->sendallp_inf.tick = osKernelSysTick();
    }

    return err;
}

static int send_logs(_mbser_inf *msinf)
{
    int err = 0;
    int res;
    int i;
    int sock;
    unsigned char *sendbuf;
    unsigned int lognum;
    _log_data *loginf;

    loginf = &plog->logdata[0];

    if(is_msinf_invalid(msinf))
    {
        err = 1;
        return err;
    }

    sock = msinf->pConn;
    sendbuf = spec_buf;

    if(msinf->sendlog_inf.enflag == 0)
    {
        return err;
    }

    osSemaphoreWait(log_sem, osWaitForever);

    lognum = msinf->sendlog_inf.num;

    if(lognum > LOG_SEND_MAXNUM)
    {
        lognum = LOG_SEND_MAXNUM;
    }

    if(lognum == 0)
    {
        osSemaphoreRelease(log_sem);
        return err;
    }

    sendbuf[0] = msinf->sendlog_inf.resp[0];
    sendbuf[1] = msinf->sendlog_inf.resp[1];
    sendbuf[2] = msinf->sendlog_inf.resp[2];
    sendbuf[3] = msinf->sendlog_inf.resp[3];
    sendbuf[4] = (lognum * 10 + 4) >> 8;
    sendbuf[5] = (lognum * 10 + 4) & 0xff;
    sendbuf[6] = msinf->sendlog_inf.resp[4];
    sendbuf[7] = 0x66;
    sendbuf[8] = lognum >> 8;
    sendbuf[9] = lognum & 0xff;
    i = 10;

    while(lognum)
    {
        sendbuf[i++] = loginf[msinf->sendlog_inf.tail].type;
        sendbuf[i++] = loginf[msinf->sendlog_inf.tail].stype;
        sendbuf[i++] = loginf[msinf->sendlog_inf.tail].tm_year;
        sendbuf[i++] = loginf[msinf->sendlog_inf.tail].tm_mon;
        sendbuf[i++] = loginf[msinf->sendlog_inf.tail].tm_date;
        sendbuf[i++] = loginf[msinf->sendlog_inf.tail].tm_hour;
        sendbuf[i++] = loginf[msinf->sendlog_inf.tail].tm_min;
        sendbuf[i++] = loginf[msinf->sendlog_inf.tail].tm_sec;
        sendbuf[i++] = loginf[msinf->sendlog_inf.tail].tm_ms1;
        sendbuf[i++] = loginf[msinf->sendlog_inf.tail].tm_ms2;
        msinf->sendlog_inf.tail++;

        if(msinf->sendlog_inf.tail >= LOG_MAXNUM)
        {
            msinf->sendlog_inf.tail = 0;
        }

        lognum--;
        msinf->sendlog_inf.num--;
    }

    osSemaphoreRelease(log_sem);

    res = mb_send(sock, (const char *)sendbuf, i, 0);

    if(res < 0)
    {
        err = 1;
    }

    return err;
}

static int send_waves(_mbser_inf *msinf)
{
    int err = 0;
    int res;
    int i;
    int sock;
    unsigned char *sendbuf;
    unsigned int tail;

    if(is_msinf_invalid(msinf))
    {
        err = 1;
        return err;
    }

    sock = msinf->pConn;
    sendbuf = spec_buf;

    tail = rtwq_tail;

    for(i = 0; i < CHANNEL_MAX; i++)
    {
        if(msinf->sendwave_inf.nps[i] == 0)
        {
            continue;
        }

        if(is_time2_send_waves(i, tail, msinf))
        {
            copyrtwave((short *)&sendbuf[11], i, msinf->sendwave_inf.nps[i],
                       msinf->sendwave_inf.tail[i]);
            sendbuf[0] = msinf->sendwave_inf.resp[0];
            sendbuf[1] = msinf->sendwave_inf.resp[1];
            sendbuf[2] = msinf->sendwave_inf.resp[2];
            sendbuf[3] = msinf->sendwave_inf.resp[3];
            sendbuf[4] = ((msinf->sendwave_inf.nps[i] << 1) + 5) >> 8;
            sendbuf[5] = ((msinf->sendwave_inf.nps[i] << 1) + 5) & 0xff;
            sendbuf[6] = msinf->sendwave_inf.resp[4];
            sendbuf[7] = 0x64;
            sendbuf[8] = i;
            sendbuf[9] = (msinf->sendwave_inf.nps[i] >> 8) & 0xff;
            sendbuf[10] = msinf->sendwave_inf.nps[i] & 0xff;

            res = mb_send(sock, (const char *)sendbuf, (msinf->sendwave_inf.nps[i] << 1) + 11, 0);

            if(res < 0)
            {
                err = 1;
                break;
            }
        }
    }

    return err;
}

int firmware_update_handle(_mbser_inf *msinf)
{
    int sock;
    int res;
    int send_failed = 0;
    int pdu_len = 0;
    unsigned char *sendbuf;

    if(is_msinf_invalid(msinf))
    {
        send_failed = 1;
        return send_failed;
    }

    if(is_firmware_update_timeout(msinf))
    {
        tty_printf("固件更新超时\n");
        firmware_update_finish(msinf);
        return send_failed;
    }

    if(is_firmware_task_busy(msinf))
    {
        return send_failed;
    }

    sock = msinf->pConn;
    sendbuf = spec_buf;

    switch(msinf->update_ctrl.state)
    {
    case UPDATE_PROGRAM_SUCCESS:

        tty_printf("resp: %d\n", update_inf.recved);

        sendbuf[7] = 0x68;
        sendbuf[8] = 1;
        sendbuf[9] = (update_inf.recved >> 24) & 0xff;
        sendbuf[10] = (update_inf.recved >> 16) & 0xff;
        sendbuf[11] = (update_inf.recved >> 8) & 0xff;
        sendbuf[12] = update_inf.recved & 0xff;
        sendbuf[13] = 0;
        pdu_len = 7;

        msinf->update_ctrl.state = UPDATE_IDLE;
        break;

    case UPDATE_PROGRAM_FAILED:

        sendbuf[7] = 0x68;
        sendbuf[8] = 1;
        sendbuf[9] = (update_inf.recved >> 24) & 0xff;
        sendbuf[10] = (update_inf.recved >> 16) & 0xff;
        sendbuf[11] = (update_inf.recved >> 8) & 0xff;
        sendbuf[12] = update_inf.recved & 0xff;
        sendbuf[13] = 1;
        pdu_len = 7;

        msinf->update_ctrl.state = UPDATE_IDLE;
        break;

    case UPDATE_CHECK_SUCCESS:
        sendbuf[7] = 0x68;
        sendbuf[8] = 2;
        sendbuf[9] = 0;
        pdu_len = 3;

        firmware_update_finish(msinf);
        break;

    case UPDATE_CHECK_FAILED:
        sendbuf[7] = 0x68;
        sendbuf[8] = 2;
        sendbuf[9] = 1;
        pdu_len = 3;

        firmware_update_finish(msinf);
        break;

    default:
        return send_failed;
    }

    if(pdu_len)
    {
        sendbuf[0] = msinf->update_ctrl.resp[0];
        sendbuf[1] = msinf->update_ctrl.resp[1];
        sendbuf[2] = msinf->update_ctrl.resp[2];
        sendbuf[3] = msinf->update_ctrl.resp[3];
        sendbuf[4] = (1 + pdu_len) >> 8;
        sendbuf[5] = (1 + pdu_len) & 0xff;
        sendbuf[6] = msinf->update_ctrl.resp[4];

        res = mb_send(sock, (const char *)sendbuf, 7 + pdu_len, 0);

        if(res < 0)
        {
            send_failed = 1;
        }
    }

    return send_failed;
}

/* **************** 添加一个MODBUS客户端连接信息 ***************
参数:
    pNewConn:客户端的连接描述符
返回:
    成功添加返回_mbser_inf型指针,错误返回NULL
************************************************************* */
_mbser_inf *mbser_addconn(int pNewConn)
{
    int count;

    for(count = 0; count < MAX_CONN_NUM; count++)
    {
        if(mbser_inf[count].conns == 0)
        {
            memset(&mbser_inf[count], 0, sizeof(_mbser_inf));
            mbser_inf[count].conns = 1;
            mbser_inf[count].pConn = pNewConn;
            return &mbser_inf[count];
        }
    }

    return NULL;
}

void thread_delete_selfmb(void)
{
    osThreadId id;
    id = osThreadGetId();
    osThreadTerminate(id);
}

/* ********************** MODBUS处理线程 *********************
             分析MODBUS命令,对标准MODBUS命令进行操作,
          对自定义命令进行处理,提交给自定义命令处理线程
*********************************************************** */
void mbresp_task(const void *pdada)
{
    int  pNewConn;
    _mbser_inf *msinf;

    pNewConn = (int)pdada;
    msinf = mbser_addconn(pNewConn);

    while(1)
    {
        if(mb_recv_handle(msinf, msinf->sendmb_inf.recvbuf, pNewConn))
        {
            tty_printf("error recv\n");
            break;
        }
    }

    update_inf.flag = 0;
    msinf->update = 0;
    msinf->conns = 0;
    closesocket(pNewConn);
    thread_delete_selfmb();
}

void mbresp_spec_task(const void *pdata)
{
    int i;
    _mbser_inf *msinf;

    while(1)
    {
        for(i = 0; i < MAX_CONN_NUM; i++)
        {
            msinf = &mbser_inf[i];

            if(send_paras(msinf))
            {
                continue;
            }

            if(send_logs(msinf))
            {
                continue;
            }

            if(send_waves(msinf))
            {
                continue;
            }

            if(firmware_update_handle(msinf))
            {
                continue;
            }
        }

        osDelay(20);
    }
}

/* **********************  MODBUS主线程 *********************
                       负责监听,建立处理线程
*********************************************************** */
void MBsever_task(const void *pdata)
{
    SOCKADDR_IN addr;
    int mbsock, mbsd;

    mbsock = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_port = htons(1314);
    addr.sin_family = PF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(mbsock, (SOCKADDR *)&addr, sizeof(addr));
    listen(mbsock, 3);
    mb_send_sem = osSemaphoreCreate(osSemaphore(mb_send_sem), 1);

    while(1)
    {
        mbsd = accept(mbsock, NULL, NULL);//等待连接
        if(mbsd > 0)
        {
            if(osThreadCreate(osThread(mbresp_task), (void *)mbsd) == NULL)
            {
                closesocket(mbsd);
                tty_printf("close\n");
                continue;
            }
        }

        osDelay(2);
    }
}


