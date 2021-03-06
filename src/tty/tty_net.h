#ifndef _TTY_NET_H
#define _TTY_NET_H

#include "rl_net.h"
#include "cmsis_os.h"
/*网络接收函数结构体申明*/

struct telnet_recv_inf
{
    int socket;
};

int tty_telnet_printf_0(char const *format, ...);
int tty_telnet_printf_1(char const *format, ...);
int tty_telnet_printf_2(char const *format, ...);

#include <stdarg.h>
struct tty;
int tty_do_telnet_printf(struct tty * ptty, char const * format, va_list ap);

//struct tty;
extern int (*tty_telnet_printf[]) (char const *, ...);
extern osSemaphoreId tty_send_sem;
//extern int tty_netrecv(struct tty *ptty);
#endif
