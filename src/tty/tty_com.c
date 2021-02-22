#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "cmsis_os.h"
#include "tty.h"
#include "shell.h"

int tty_com_open(void)
{
    tty_get(TTY_TYPE_COM);
    return 0;
}

int tty_com_getch(struct tty *ptty, char *c)
{
    *c = fgetc(stdin);
    return 0;
}

#define COM_PRINTF_BUF_MAX 256

int tty_do_com_printf(struct tty *ptty ,char const *format, va_list ap)
{
    int len;
    int i = 0;
    char tty_com_buf[COM_PRINTF_BUF_MAX];

    len = vsnprintf(tty_com_buf, sizeof(tty_com_buf), format, ap);

    if(len > -1 && len < COM_PRINTF_BUF_MAX)
    {
        while((tty_com_buf[i] != '\0') && (i < len))
        {
            fputc(tty_com_buf[i], stdout);
            i++;
        }
    }

    return len;
}

int tty_com_printf(char const *format , ...)
{
    int len;
    va_list argptr;

    va_start(argptr, format);

    len = tty_do_com_printf(&comtty[0], format, argptr);

    va_end(argptr);

    return len;
}

/*
* 0 ���� 1 buff���
*/
int tty_com_recv(struct tty *ptty)
{
    char *pstr;
    unsigned char pos;
    char c;
    //int err;
    pstr  = ptty->recv_buf;
    while(1)
    {
        // uart_read_data(e_dbgu, (unsigned char *)&c, 1);
        tty_com_getch(ptty, &c);
        //ptty->printf("%d \n",c);
        if ((c >= ' ')&&(c < 0x7e)) {
            *pstr = c ;
            pstr++;
            ptty->printf("%c",c);
            pos = pstr - ptty->recv_buf;
            if (pos >= TTY_BUF_SIZE)  return 1;
        } else if(c == '\b') {
            if (pstr > ptty->recv_buf) {
                pstr--;
                ptty->printf("\b \b");
            }
        } else if (c == '\r') {
            *pstr = '\0';
            ptty->printf("\n");
            return 0;
        }
    }
}

