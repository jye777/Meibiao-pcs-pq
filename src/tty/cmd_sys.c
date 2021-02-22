#include <string.h>
#include <cmsis_os.h>
#include "bsp.h"
#include "cmd.h"
#include "sys.h"
#include "para.h"
#include "fpga.h"
#include "micro.h"



/*ϵͳ�й�*/
int do_reset(struct tty *ptty)
{
    miro_write.manShutdownFlag = 1;
    set_pcs_shutdown(); //���⿪�������λ��Σ��
    //watchdog_reset();
    osDelay(1000);
    SCB->AIRCR = 0x05FA0004;
    while(1);
}


int do_print_ctrl(struct tty *ptty)
{
    unsigned short prtLevel;
    sscanf(ptty->cmd_buf[1], "%d", (int *)&prtLevel);
	sys_ctl.prtLevel = prtLevel;
    //setpara(Addr_Print_Level, sys_ctl.prtLevel);
    return 0;
}



#if 0
int do_cpustat(struct tty *ptty)
{
    int i;
    int cpu_usage_cur;
    int cpu_usage_max;
    int cpu_usage_avg;
    int cpu_usage_arraysum;
    int cpu_usage_arraycnt;

    cpu_usage_cur = OSCPUUsage;
    cpu_usage_max = OSCPUUsageMax;

    cpu_usage_arraysum = 0;
    cpu_usage_arraycnt = OSCPUUsageArrayCnt;

    for(i = 0; i < cpu_usage_arraycnt; i++)
    {
        cpu_usage_arraysum += OSCPUUsageArray[i];
    }

    cpu_usage_avg = cpu_usage_arraysum / cpu_usage_arraycnt;

    ptty->printf(
        "cpu��ǰʹ����: %d%%, ���ʹ����: %d%%, ����ƽ��ʹ����: %d%%\n",
        cpu_usage_cur, cpu_usage_max, cpu_usage_avg);

    return 0;
}
#endif

#if 0
int do_getversion(struct tty *ptty)
{
    short year, mon, day, hour, min, sec;

    get_build_date(&year, &mon, &day);
    get_build_time(&hour, &min, &sec);

    ptty->printf("\n�������ʱ��: %d/%d/%d %d:%d:%d\n\n",
                 year, mon, day, hour, min, sec);
    //ptty->printf("����汾:%d\n", BUILD_SELECT);

    return 0;
}
#endif

#if 0
int do_showtask(struct tty *ptty)
{
    int prio;
    INT8U err;
    OS_TCB tcb;

    for(prio = 0; prio <= OS_TASK_IDLE_PRIO; prio++)
    {
        err = OSTaskQuery(prio, &tcb);

        if(err != OS_ERR_NONE)
        {
            continue;
        }

        ptty->printf("prio: %d, ��ʹ�õ�ջ��С: ", prio);

        if((tcb.OSTCBOpt & OS_TASK_OPT_STK_CHK) == 0)
        {
            ptty->printf("�����\n");
        }
        else
        {
            ptty->printf("%d\n", tcb.OSTCBStkUsed);
        }
    }

    return 0;
}
#endif

cmdt cmd_reset = {"reset", do_reset, "��λARM",
                  "reset\n"
                  "��λARM\n"
                 };


cmdt cmd_dbg = {"dbg", do_print_ctrl, "��ӡ��Ϣ����",
                "dbg\n"
                "��ӡ��Ϣ����\n"
               };



