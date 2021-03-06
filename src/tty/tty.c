#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "cmsis_os.h"
#include "tty_net.h"
#include "tty.h"
#include "telnet.h"
#include "tty_com.h"
#include "sys.h"
#include "fpga.h"

osSemaphoreId tty_sem = NULL;
osSemaphoreDef(tty_sem);

extern int tty_netrecv(struct tty *ptty);
extern int tty_telnet_getch(struct tty *ptty, char * ch );

int tty_read_line(struct tty *ptty);

#if TTY_NET_NUM != 0
struct tty nettty[TTY_NET_NUM];
#endif

#if TTY_COM_NUM != 0
struct tty comtty[TTY_COM_NUM];
#endif

int tty_printf(const char * format, ...)
{
    if(sys_ctl.dbg == 0) {
        return 0;
    }
#if TTY_NET_NUM != 0
    int i;
#endif
    va_list argptr;

#if TTY_COM_NUM != 0
    if(comtty[0].type == TTY_TYPE_COM)
    {
        va_start(argptr, format);
        tty_do_com_printf(&comtty[0], format, argptr);
        va_end(argptr);
    }
#endif

#if TTY_NET_NUM != 0
    for (i = 0; i < TTY_NET_NUM; i++)
    {
        if((nettty[i].type == TTY_TYPE_NET) && (nettty[i].flag == 1))
        {
            va_start(argptr,format);
            tty_do_telnet_printf(&nettty[i], format, argptr);
            va_end(argptr);
        }
    }
#endif

    return 0;
}



int tty_printf2(uint8_t level, const char * format, ...)
{
	if(GETBIT(sys_ctl.prtLevel, level)) {
#if TTY_NET_NUM != 0
	    int i;
#endif
		va_list argptr;

#if TTY_COM_NUM != 0
		if(comtty[0].type == TTY_TYPE_COM)
	    {
	        va_start(argptr, format); 
	        tty_do_com_printf(&comtty[0], format, argptr);
	        va_end(argptr);
		}
#endif

#if TTY_NET_NUM != 0
		for (i = 0; i < TTY_NET_NUM; i++)
	    {
			if((nettty[i].type == TTY_TYPE_NET) && (nettty[i].flag == 1))
	        {
	            va_start(argptr,format);
	            tty_do_telnet_printf(&nettty[i], format, argptr);
	            va_end(argptr);
			}
	    }
#endif
		return 0;
	}
	
	return 0;
}


void tty_init(struct tty *ptty, unsigned char tpye,
              int (*getc)(struct tty*, char *),
              int (*printf)(char const * ,...)
             )
{
    ptty->type = tpye;
    memset(&ptty->recv_buf,0,TTY_BUF_SIZE);
    memset(&ptty->cmd_buf,0,TTY_CMD_NUM*TTY_CMD_SIZE);
    ptty->getc = getc;
    ptty->printf = printf;

#ifdef CONFIG_CMDLINE_EDITING
    {
        int i;

        ptty->cmdline.hist_max = 0;
        ptty->cmdline.hist_add_idx = 0;
        ptty->cmdline.hist_cur = -1;
        ptty->cmdline.hist_num = 0;

        for (i = 0; i < HIST_MAX; i++) {
            ptty->cmdline.hist_list[i] = ptty->cmdline.hist_lines[i];
            ptty->cmdline.hist_list[i][0] = '\0';
        }
    }

#endif
}

struct tty * tty_get(unsigned char type)
{
#if TTY_NET_NUM != 0
    int i;
#endif

    if(tty_sem == NULL)
    {
        tty_sem = osSemaphoreCreate(osSemaphore(tty_sem), 1);
    }

#if TTY_COM_NUM != 0
    if (type == TTY_TYPE_COM ) {
        if (comtty[0].type == TTY_TYPE_IDLE)
        {
            tty_init(&comtty[0], TTY_TYPE_COM, tty_com_getch, tty_com_printf);

            return &comtty[0];
        }
    }
#endif

#if TTY_NET_NUM != 0
    if (type == TTY_TYPE_NET) {
        for( i = 0; i < TTY_NET_NUM; i++) {
            osSemaphoreWait(tty_sem, osWaitForever);
            if (nettty[i].type == TTY_TYPE_IDLE) {
                tty_init(&nettty[i], TTY_TYPE_NET, tty_telnet_getch, tty_telnet_printf[i]);
                osSemaphoreRelease(tty_sem);
                return &nettty[i];
            }
            osSemaphoreRelease(tty_sem);
        }
    }
#endif
    return NULL;
}

unsigned char tty_free(struct tty *ptty)
{
#if TTY_NET_NUM != 0
    char idx;
    idx = ptty - &nettty[0];
    osSemaphoreWait(tty_sem, osWaitForever);
    ptty->type = TTY_TYPE_IDLE;
    ptty->getc =  NULL;
    ptty->printf = NULL;
    ptty->port = NULL;
    ptty->flag = 0;
    osSemaphoreRelease(tty_sem);
    printf("\nfree tty is %d \n",idx);
#endif
    return 0;
}

/**
 * @brief:      parse_line(copy from uboot)
 * @details:    get argv and return argc
 * @param[in]   line command buffer
 * @param[out]  argv pointer array of argument
 * @param[out]  argc num of argument
 * @retval:     0 if success, 1 if failed
 */
int parse_line (char *line, char *argv[], int *argc)
{
    int nargs = 0;

    while (nargs < TTY_CMD_NUM) {

        /* skip any white space */
        while (isblank(*line))
            ++line;

        if (*line == '\0') {	/* end of line, no more args	*/
            argv[nargs] = NULL;
            *argc = nargs;
            return 0;
        }

        argv[nargs++] = line;	/* begin of argument string	*/

        /* find end of string */
        while (*line && !isblank(*line))
            ++line;

        if (*line == '\0') {	/* end of line, no more args	*/
            argv[nargs] = NULL;
            *argc = nargs;
            return 0;
        }

        *line++ = '\0';		/* terminate current arg	 */
    }

    *argc = nargs;

    return 1;
}

/**
 * @brief:      tty_getcmd
 * @details:    Parse command
 * @param[in]   pointer to tty
 * @param[out]  pointer to tty
 * @retval:     0 if success, 1 if failed
 */
int  tty_getcmd(struct tty *ptty)
{
    int err;

    err = parse_line(ptty->recv_buf, ptty->cmd_buf, &ptty->cmd_num);

    return err;
}

#ifdef CONFIG_CMDLINE_EDITING

#define putnstr(str,n)	do {			\
		ptty->printf("%.*s", (int)n, str);	\
	} while (0)

#define CTL_CH(c)		((c) - 'a' + 1)
#define CTL_BACKSPACE		('\b')
#define DEL			((char)255)
#define DEL7			((char)127)
#define CREAD_HIST_CHAR		('!')

#define getcmd_putch(ch)	ptty->printf("%c",ch)
#define getcmd_cbeep()		ptty->printf("\a")


#define BEGINNING_OF_LINE() {			\
	while (num) {				\
		ptty->printf("\b");	\
		num--;				\
	}					\
}

#define ERASE_TO_EOL() {				\
	if (num < eol_num) {				\
		ptty->printf("%*s", (int)(eol_num - num), ""); \
		do {					\
			ptty->printf("\b");	\
		} while (--eol_num > num);		\
	}						\
}

#define REFRESH_TO_EOL() {			\
	if (num < eol_num) {			\
		wlen = eol_num - num;		\
		putnstr(ptty->recv_buf + num, wlen);	\
		num = eol_num;			\
	}					\
}

static void cread_add_to_hist(struct tty *ptty ,char *line)
{
    strcpy(ptty->cmdline.hist_list[ptty->cmdline.hist_add_idx], line);

    if (++ptty->cmdline.hist_add_idx >= HIST_MAX)
        ptty->cmdline.hist_add_idx = 0;

    if (ptty->cmdline.hist_add_idx > ptty->cmdline.hist_max)
        ptty->cmdline.hist_max = ptty->cmdline.hist_add_idx;

    ptty->cmdline.hist_num++;
}
static char* hist_prev(struct tty *ptty )
{
    char *ret;
    int old_cur;

    if (ptty->cmdline.hist_cur < 0)
        return NULL;

    old_cur = ptty->cmdline.hist_cur;
    if (--ptty->cmdline.hist_cur < 0)
        ptty->cmdline.hist_cur = ptty->cmdline.hist_max;

    if (ptty->cmdline.hist_cur == ptty->cmdline.hist_add_idx) {
        ptty->cmdline.hist_cur = old_cur;
        ret = NULL;
    } else
        ret = ptty->cmdline.hist_list[ptty->cmdline.hist_cur];

    return (ret);
}

static char* hist_next(struct tty *ptty )
{
    char *ret;

    if (ptty->cmdline.hist_cur < 0)
        return NULL;

    if (ptty->cmdline.hist_cur == ptty->cmdline.hist_add_idx)
        return NULL;

    if (++ptty->cmdline.hist_cur > ptty->cmdline.hist_max)
        ptty->cmdline.hist_cur = 0;

    if (ptty->cmdline.hist_cur == ptty->cmdline.hist_add_idx) {
        ret = "";
    } else
        ret = ptty->cmdline.hist_list[ptty->cmdline.hist_cur];

    return (ret);
}



static void cread_add_char(struct tty *ptty,char ichar, int insert, unsigned long *num,
                           unsigned long *eol_num, char *buf, unsigned long len)
{
    unsigned long wlen;

    /* room ??? */
    if (insert || *num == *eol_num) {
        if (*eol_num > len - 1) {
            getcmd_cbeep();
            return;
        }
        (*eol_num)++;
    }

    if (insert) {
        wlen = *eol_num - *num;
        if (wlen > 1) {
            memmove(&buf[*num+1], &buf[*num], wlen-1);
        }

        buf[*num] = ichar;
        putnstr(buf + *num, wlen);
        (*num)++;
        while (--wlen) {
            getcmd_putch(CTL_BACKSPACE);
        }
    } else {
        /* echo the character */
        wlen = 1;
        buf[*num] = ichar;
        putnstr(buf + *num, wlen);
        (*num)++;
    }
}

static void cread_add_str(struct tty *ptty,char *str, int strsize, int insert, unsigned long *num,
                          unsigned long *eol_num, char *buf, unsigned long len)
{
    while (strsize--) {
        cread_add_char(ptty,*str, insert, num, eol_num, buf, len);
        str++;
    }
}

/**
 * @brief:      tty_deloneline
 * @details:    delete one line
 * @param[in]   ptty pointer to tty
 * @retval:     none
 */
void tty_deloneline(struct tty *ptty)
{
    int i,oneline = 50;

    for(i = 0; i < oneline; i++)
    {
        ptty->printf("\b");
    }

    for(i = 0; i < oneline; i++)
    {
        ptty->printf(" ");
    }

    for(i = 0; i < oneline; i++)
    {
        ptty->printf("\b");
    }
}

/**
 * @brief:      tty_getonechar
 * @details:    get a char, not echo
 * @param[in]   ptty pointer to tty
 * @param[out]  buf the buffer stored the char
 * @retval:     0 if success, -1 if failed, 1 if buffer is overflow
 */
int tty_getonechar(struct tty *ptty, char *c)
{
    if(ptty->getc(ptty, c))
    {
        return -1;
    }

    return 0;
}

/**
 * @brief:      tty_getstring
 * @details:    get a string
 * @param[in]   ptty pointer to tty
 *              buflen length of buffer
 * @param[out]  buf the buffer stored the string
 * @retval:     0 if success, -1 if failed, 1 if buffer is overflow
 */
int tty_getstring(struct tty *ptty, char *buf, int buflen)
{
    char *pstr;
    char c;
    int cnt;

    pstr = buf;
    cnt = 0;

    while(1)
    {
        if(ptty->getc(ptty, &c))
        {
            return -1;
        }

        if ((c >= ' ') && (c < 0x7e))
        {
            *pstr = c ;
            pstr++;
            cnt++;
            ptty->printf("%c", c);

            if(cnt >= buflen - 1)
            {
                buf[buflen - 1] = '\0';
                return 1;
            }
        }
        else if(c == '\b')
        {
            if (cnt > 0)
            {
                pstr--;
                cnt--;
                ptty->printf("\b \b");
            }
        }
        else if (c == '\r')
        {
            *pstr = '\0';
            ptty->printf("\n");
            return 0;
        }
    }
}

/**
 * ��һ��
 * @param  ptty [description]
 * @return      0 ���� ������ʶ�������  �����ն˴��� 1 buff ��� �����ϰ�  2 �ն��ж�
*
**/
int tty_read_line(struct tty *ptty)
{
    char ichar;

    unsigned long num = 0;
    unsigned long eol_num = 0;
    unsigned long wlen;
    int insert = 1;
    int esc_len = 0;
    char esc_save[8];

    while(1)
    {
        if (ptty->getc(ptty, &ichar)) return -1;

        /*��������*/
        if ((ichar == '\n') || (ichar == '\r')) {
            ptty->printf("\n");
            break;
        }
        if (esc_len != 0) {
            if (esc_len == 1) {
                if (ichar == '[') {
                    esc_save[esc_len] = ichar;
                    esc_len = 2;
                } else {
                    cread_add_str(ptty,esc_save, esc_len, insert,
                                  &num, &eol_num, ptty->recv_buf, TTY_BUF_SIZE);
                    esc_len = 0;
                }
                continue;
            }

            switch (ichar) {

            case 'D':	/* <- key */
                ichar = CTL_CH('b');
                esc_len = 0;
                break;
            case 'C':	/* -> key */
                ichar = CTL_CH('f');
                esc_len = 0;
                break;	/* pass off to ^F handler */
            case 'H':	/* Home key */
                ichar = CTL_CH('a');
                esc_len = 0;
                break;	/* pass off to ^A handler */
            case 'A':	/* up arrow */
                ichar = CTL_CH('p');
                esc_len = 0;
                break;	/* pass off to ^P handler */
            case 'B':	/* down arrow */
                ichar = CTL_CH('n');
                esc_len = 0;
                break;	/* pass off to ^N handler */
            default:
                esc_save[esc_len++] = ichar;
                cread_add_str(ptty,esc_save, esc_len, insert,
                              &num, &eol_num, ptty->recv_buf, TTY_BUF_SIZE);
                esc_len = 0;
                continue;
            }
        }

        switch (ichar) {
        case 0x1b:
            if (esc_len == 0) {
                esc_save[esc_len] = ichar;
                esc_len = 1;
            } else {
                ptty->printf("impossible condition #876\n");
                esc_len = 0;
            }
            break;

        case CTL_CH('a'):
            BEGINNING_OF_LINE();
            break;
        case CTL_CH('c'):	/* ^C - break */
            *ptty->recv_buf = '\0';	/* discard input */
            ptty->printf("\n");
            return (2);
        case CTL_CH('f'):
            if (num < eol_num) {
                getcmd_putch(ptty->recv_buf[num]);
                num++;
            }
            break;
        case CTL_CH('b'):
            if (num) {
                getcmd_putch(CTL_BACKSPACE);
                num--;
            }
            break;
        case CTL_CH('d'):
            if (num < eol_num) {
                wlen = eol_num - num - 1;
                if (wlen) {
                    memmove(&ptty->recv_buf[num], &ptty->recv_buf[num+1], wlen);
                    putnstr(ptty->recv_buf + num, wlen);
                }

                getcmd_putch(' ');
                do {
                    getcmd_putch(CTL_BACKSPACE);
                } while (wlen--);
                eol_num--;
            }
            break;
        case CTL_CH('k'):
            ERASE_TO_EOL();
            break;
        case CTL_CH('e'):
            REFRESH_TO_EOL();
            break;
        case CTL_CH('o'):
            insert = !insert;
            break;
        case CTL_CH('x'):
        case CTL_CH('u'):
            BEGINNING_OF_LINE();
            ERASE_TO_EOL();
            break;
            //	case DEL:
        case DEL7:
            if (num < eol_num) {
                wlen = eol_num - num - 1;
                if (wlen) {
                    memmove(&ptty->recv_buf[num], &ptty->recv_buf[num+1], wlen);
                    putnstr(ptty->recv_buf + num, wlen);
                }

                getcmd_putch(' ');
                do {
                    getcmd_putch(CTL_BACKSPACE);
                } while (wlen--);
                eol_num--;
            }

            break;
        case 8:
            if (num) {
                wlen = eol_num - num;
                num--;
                memmove(&ptty->recv_buf[num], &ptty->recv_buf[num+1], wlen);
                getcmd_putch(CTL_BACKSPACE);
                putnstr(ptty->recv_buf + num, wlen);
                getcmd_putch(' ');
                do {
                    getcmd_putch(CTL_BACKSPACE);
                } while (wlen--);
                eol_num--;
            }
            break;
            /*���� tab*/
        case '\t':
            continue;
        case CTL_CH('p'):
        case CTL_CH('n'):
        {
            char * hline;

            esc_len = 0;

            if (ichar == CTL_CH('p'))
                hline = hist_prev(ptty);
            else
                hline = hist_next(ptty);

            if (!hline) {
                getcmd_cbeep();
                continue;
            }

            /* nuke the current line */
            /* first, go home */
            BEGINNING_OF_LINE();

            /* erase to end of line */
            ERASE_TO_EOL();

            /* copy new line into place and display */
            strcpy(ptty->recv_buf, hline);
            eol_num = strlen(ptty->recv_buf);
            REFRESH_TO_EOL();
            continue;
        }
        default:
            cread_add_char(ptty,ichar, insert, &num, &eol_num, ptty->recv_buf, TTY_BUF_SIZE);
            break;
        }
    }
    //*len = eol_num;
    ptty->recv_buf[eol_num] = '\0';	/* lose the newline */

    if (ptty->recv_buf[0] && ptty->recv_buf[0] != CREAD_HIST_CHAR)
        cread_add_to_hist(ptty,ptty->recv_buf);
    ptty->cmdline.hist_cur = ptty->cmdline.hist_add_idx;

    return 0;
}

int tty_check_char(struct tty *ptty, int ch)
{
    int err;
    char cin;

    err = tty_getonechar(ptty, &cin);

    if(err || cin != ch)
    {
        return 0;
    }

    return 1;
}

#endif
