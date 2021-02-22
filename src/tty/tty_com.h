#ifndef _TTY_COM_H_
#define _TTY_COM_H_

int tty_com_recv(struct tty *ptty);
int tty_com_getch(struct tty * ptty,char * c);
int tty_com_printf(char const *format ,...);
int tty_com_open(void);

#include <stdarg.h>
struct tty;
int tty_do_com_printf(struct tty *ptty ,char const *format, va_list ap);

#endif
