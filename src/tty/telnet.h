#ifndef __TELNET_H__
#define __TELNET_H__

#include  "tty.h"

void telnet_thread(const void *pdata);
void telnet_task(const void * pdata );

#endif
