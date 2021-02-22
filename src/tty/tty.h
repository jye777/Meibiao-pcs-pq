#ifndef _TTY_H
#define _TTY_H

#include "tty_net.h"

#define TTY_BUF_SIZE	100
#define TTY_CMD_NUM	10
#define TTY_CMD_SIZE      25

#define TTY_TYPE_IDLE	0
#define TTY_TYPE_COM	1
#define TTY_TYPE_NET	2

#define TTY_NET_NUM 3
#define TTY_COM_NUM 1

#define isblank(c)	(c == ' ' || c == '\t')

//开启行编辑
#define CONFIG_CMDLINE_EDITING

#define HIST_MAX		20
#define HIST_SIZE		TTY_BUF_SIZE

#define ctrlc()       tty_check_char(ptty, 3)

typedef enum
{
	PRT_ERROR = 0,	//错误
	PRT_WARM,		//告警
	PRT_INFO,		//通知
	PRT_DEBUG,		//调试
	PRT_DEBUG2,		//调试
	PRT_DEBUG3,		//调试
	PRT_DEBUG4,		//调试
	PRT_DEBUG5,		//调试
	PRT_DEBUG6,		//调试
	PRT_DEBUG7,		//调试
	PRT_DEBUG8,		//调试
	PRT_DEBUG9,		//调试
	PRT_MAX
} prt_level_t;


#if 0
#define printf_error(fmt...)
#define printf_warm(fmt...)	
#define printf_info(fmt...)	
#define printf_debug(fmt...)
#define printf_debug2(fmt...)
#define printf_debug3(fmt...)
#define printf_debug4(fmt...)
#define printf_debug5(fmt...)
#define printf_debug6(fmt...)
#define printf_debug7(fmt...)
#define printf_debug8(fmt...)
#define printf_debug9(fmt...)
#else
#define printf_error(fmt...)		tty_printf2(PRT_ERROR, fmt)		// 1
#define printf_warm(fmt...)			tty_printf2(PRT_WARM, fmt)		// 2
#define printf_info(fmt...)			tty_printf2(PRT_INFO, fmt)		// 4
#define printf_debug(fmt...)		tty_printf2(PRT_DEBUG, fmt)		// 8
#define printf_debug2(fmt...)		tty_printf2(PRT_DEBUG2, fmt)	// 16		EMU
#define printf_debug3(fmt...)		tty_printf2(PRT_DEBUG3, fmt)	// 32		BMS
#define printf_debug4(fmt...)		tty_printf2(PRT_DEBUG4, fmt)	// 64		OTHER-HANDLER3
#define printf_debug5(fmt...)		tty_printf2(PRT_DEBUG5, fmt)	// 128		HMI/UPPER
#define printf_debug6(fmt...)		tty_printf2(PRT_DEBUG6, fmt)	// 256		EMU-EXTRAL
#define printf_debug7(fmt...)		tty_printf2(PRT_DEBUG7, fmt)	// 512		OTHER-HANDLER
#define printf_debug8(fmt...)		tty_printf2(PRT_DEBUG8, fmt)	// 1024		OTHER-HANDLER2
#define printf_debug9(fmt...)		tty_printf2(PRT_DEBUG9, fmt)	// 2048
#endif


#define printf_error_ex(fmt...)		do { \
										printf_error("[ERROR] #%-4d %s(): ", __LINE__, __FUNCTION__); \
										printf_error(fmt); \
									} while(0)
									
#define printf_warm_ex(fmt...)		do { \
										printf_warm("[WARM] #%-4d %s(): ", __LINE__, __FUNCTION__); \
										printf_warm(fmt); \
									} while(0)
									
#define printf_info_ex(fmt...)		do { \
										printf_info("[INFO] #%-4d %s(): ", __LINE__, __FUNCTION__); \
										printf_info(fmt); \
									} while(0)
									
#define printf_debug_ex(fmt...)		do { \
										printf_debug("[DBG] #%-4d %s(): ", __LINE__, __FUNCTION__); \
										printf_debug(fmt); \
									} while(0)
									
#define printf_debug2_ex(fmt...)	do { \
										printf_debug2("[DBG2] #%-4d %s(): ", __LINE__, __FUNCTION__); \
										printf_debug2(fmt); \
									} while(0)
									
#define printf_debug3_ex(fmt...)	do { \
										printf_debug3("[DBG3] #%-4d %s(): ", __LINE__, __FUNCTION__); \
										printf_debug3(fmt); \
									} while(0)
									
#define printf_debug4_ex(fmt...)	do { \
										printf_debug4("[DBG4] #%-4d %s(): ", __LINE__, __FUNCTION__); \
										printf_debug4(fmt); \
									} while(0)
									
#define printf_debug5_ex(fmt...)	do { \
										printf_debug5("[DBG5] #%-4d %s(): ", __LINE__, __FUNCTION__); \
										printf_debug5(fmt); \
									} while(0)
									
#define printf_debug6_ex(fmt...)	do { \
										printf_debug6("[DBG6] #%-4d %s(): ", __LINE__, __FUNCTION__); \
										printf_debug6(fmt); \
									} while(0)

#define printf_debug7_ex(fmt...)	do { \
										printf_debug7("[DBG7] #%-4d %s(): ", __LINE__, __FUNCTION__); \
										printf_debug7(fmt); \
									} while(0)
									
#define printf_debug8_ex(fmt...)	do { \
										printf_debug8("[DBG8] #%-4d %s(): ", __LINE__, __FUNCTION__); \
										printf_debug8(fmt); \
									} while(0)
									
#define printf_debug9_ex(fmt...)	do { \
										printf_debug9("[DBG9] #%-4d %s(): ", __LINE__, __FUNCTION__); \
										printf_debug9(fmt); \
									} while(0)



//struct telnet_recv_inf;
struct tty_cmdline_edit
{

    int hist_max;
    int hist_add_idx;
    int hist_cur;
    unsigned hist_num;
    char* hist_list[HIST_MAX];
    char hist_lines[HIST_MAX][HIST_SIZE + 1];
};

struct tty
{
    /* TTY类型 0:空闲 1: 串口 2:网络 FF:紧急处理 */
    unsigned char type;

    /* 接收buff */
    char  recv_buf[TTY_BUF_SIZE];

    /* 命令buff */
    char *cmd_buf[TTY_CMD_NUM];

    /* 命令个数 */
    int cmd_num;

    /* 接收函数 */
    int (*getc)(struct tty*, char *);

    /* 打印函数 */
    int (*printf)(char const * ,...);

    /* 接口指针 */
    void * port;

    /* 发送使能标志 */
    char  flag;

#ifdef CONFIG_CMDLINE_EDITING

    /* 行编辑所需结构体 */
    struct tty_cmdline_edit cmdline;

    /* 模拟套接字 */
    struct telnet_recv_inf *pnet_inf;

#endif
};

#if TTY_NET_NUM != 0
extern struct tty nettty[TTY_NET_NUM];
#endif

#if TTY_COM_NUM != 0
extern struct tty comtty[TTY_COM_NUM];
#endif

int tty_read_line(struct tty * ptty);
int  tty_getcmd(struct tty *ptty);
void tty_deloneline(struct tty *ptty);
int tty_getonechar(struct tty *ptty, char *c);
int tty_getstring(struct tty *ptty, char *buf, int buflen);
struct tty * tty_get(unsigned char type);
unsigned char tty_free(struct tty *ptty);
int tty_printf(const char * format, ...);
int tty_printf2(uint8_t level, const char * format, ...);
int tty_check_char(struct tty *ptty, int ch);

#endif
