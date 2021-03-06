
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "cmsis_os.h"
#include "rl_net.h"
#include "telnet.h"
#include "tty_net.h"
#include "shell.h"
#include "tty.h"
#include "fpga.h"

#define IAC             255 /* interpret as command: */
#define DONT            254 /* you are not to use option */
#define DO              253 /* please, you use option */
#define WONT            252 /* I won't use option */
#define WILL            251 /* I will use option */
#define SB              250 /* interpret as subnegotiation */
#define SE              240 /* end sub negotiation */
#define TELOPT_ECHO     1   /* echo */
#define TELOPT_SGA      3   /* suppress go ahead */
#define TELOPT_TTYPE    24  /* terminal type */
#define TELOPT_NAWS     31  /* window size */
#define TELOPT_LCOR     33  /* remote flow control */

static const char iacs_to_send[]  =
{
    IAC, WILL, TELOPT_SGA,
    IAC, WILL, TELOPT_ECHO
};

extern int mb_send (int sock, const char *buf, int len, int flags);

int telnet_flag;

osThreadDef(telnet_thread, osPriorityNormal, 3, 0);

//启动telnet任务
void telnet_task(const void *pdata)
{
    SOCKADDR_IN addr;
    int sock, sd;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_port = htons(23);
    addr.sin_family = PF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(sock, (SOCKADDR *)&addr, sizeof(addr));
    listen(sock, 3);

    while(1)
    {
        sd = accept(sock, NULL, NULL);//等待连接

        if(sd > 0)
        {
            if(osThreadCreate(osThread(telnet_thread), (void *)sd) == NULL)
            {
                closesocket(sd);
                printf("close\n");
                continue;
            }
        }

        osDelay(2);
    }
}


void thread_delete_self(void)
{
    osThreadId id;
    id = osThreadGetId();
    osThreadTerminate(id);
}

void telnet_thread(const void *pdada)
{
    int sd;
    struct tty *ptty;
    //unsigned char i;
    int revc_rt;
    int err = 0;
    int cmd_rt;
    char optval = 1;
#ifdef CONFIG_CMDLINE_EDITING
    struct telnet_recv_inf net_inf;
#endif

    sd = (int)pdada;
    setsockopt(sd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof (optval));

    ptty = tty_get(TTY_TYPE_NET);

    if(ptty == NULL)
    {
        closesocket(sd);
        thread_delete_self();
    }

    ptty->port = &sd;

#ifdef CONFIG_CMDLINE_EDITING
    net_inf.socket = sd;
    ptty->pnet_inf = &net_inf;
#endif

    err = mb_send(sd, iacs_to_send, sizeof(iacs_to_send), 0);
    if(err < 0) {
        return;
    }
    //send(sd, iacs_to_send, sizeof(iacs_to_send), 0);

    /*协商完毕可以打印信息*/
    ptty->flag = 1;
    ptty->printf("Telnet - Arm Soft Version: V%dC%dB%d\n", SOFT_VER_V, SOFT_VER_C, SOFT_VER_B);
    ptty->printf("Version Compile Time "  ", "__DATE__ " - " __TIME__"\n");

    while(1)
    {
        ptty->printf("clou>");
        revc_rt = tty_read_line(ptty);

        //  printf("revc_rt is %d \n",revc_rt);
        if(!revc_rt)
        {
            cmd_rt = tty_getcmd(ptty);

            if(!cmd_rt)
            {
                if((strcmp(ptty->cmd_buf[0], "quit") == 0)
                        || (strcmp(ptty->cmd_buf[0], "exit") == 0))
                {
                    tty_free(ptty);
                    closesocket(sd);
                    thread_delete_self();
                }

                //进入命令处理
                shell_run(ptty);
            }
            else
            {
                ptty->printf("参数数量超出限制\n");
            }
        }
        else
        {
            if(revc_rt == 3)
            {
                ptty->printf("buff 溢出\n");
                continue;
            }
            else if(revc_rt == -1)
            {
                printf("连接断开\n");
                tty_free(ptty);
                closesocket(sd);
                thread_delete_self();
            }
        }
    }
}

