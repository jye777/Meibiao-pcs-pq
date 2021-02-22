#include "rl_net.h"
#include <cmsis_os.h>
#include "tty.h"
#include "string.h"
#include "nand_file.h"
#include "MBServer.h"
#include "epcs.h"

#define TFTP_DATA_LEN				512

uint8_t tftp_end = 0;
int32_t udp_sock;
static uint8_t tftpbuf[520];
static NET_ADDR4 serverip = {0};
static char *filename_to_wtrie;
extern uint8_t	pof_head[POF_HEAD_LENGTH];
extern uint8_t	pof_tail[POF_TAIL_LENGTH];

static uint8_t number_place(uint8_t num)
{
    uint8_t value = 1,i = 0;

    if(num >2)
    {
        return 0;
    }

    for(; i < num; i++)
    {
        value *=10;
    }

    return value;
}

//192.168.1.155
//static void ip_ascii_to_hex(char *ascii,uint8_t *ip)
//{
//    uint8_t i = 0,j = 0,m = 0,n = 0;

//    for(i = 0; i < NET_ADDR_IP4_LEN; i++)
//    {
//        m = 0;

//        for(; ascii[j] == '.'; j++)
//        {
//            m++;
//        }

//        for(n = 0 ; n <m ; n++)
//        {
//            ip[i] += (ascii[j-n-1] - '0') * number_place(n);
//        }
//    }
//}

uint8_t get_rrq(char *filename,uint8_t *buf)
{
//	const char rrq[23]	= {0x00,0x01,'g','e','n','e','r','i','c','a','r','m','.','b','i','n',0,'o','c','t','e','t',0x00};
    uint32_t filenamelen = 0,rrqlen = 0;

    filenamelen = strlen(filename);
    rrqlen = filenamelen + 9;

    memset(buf,0,rrqlen);

    buf[0] = 0;
    buf[1] = 1;

    strcpy((char *)buf+2,filename);
    strcpy((char *)buf+3+filenamelen,"octet");

    return rrqlen;
}

uint8_t get_ack(uint16_t blocknum,uint8_t *buf)
{
    memset(buf,0,4);

    buf[0] = 0;
    buf[1] = 4;
    buf[2] = (uint8_t)(blocknum >> 8);
    buf[3] = (uint8_t)(blocknum & 0x00ff);

    return 4;
}

uint8_t printf_tftp_err(void)
{
    switch(tftp_end)
    {
    case 0:
        tty_printf("文件加载中......\n");
        break;

    case 1:
        tty_printf("文件写入flash失败\n");
        break;

    case 2:
        tty_printf("文件未找到\n");
        break;

    case 3:
        tty_printf("拒绝访问\n");
        break;

    case 4:
        tty_printf("磁盘已满或者超出可分配空间\n");
        break;

    case 5:
        tty_printf("非法TFTP操作\n");
        break;

    case 6:
        tty_printf("传输ID未知\n");
        break;

    case 7:
        tty_printf("文件不存在\n");
        break;

    case 8:
        tty_printf("没有该用户\n");
        break;

    case 9:
        tty_printf("open file failed\n");
        break;

    case 10:
        tty_printf("服务器无应答\n");
        break;

    case 11:
        tty_printf("服务器非法操作码\n");
        break;

    case 12:
        tty_printf("文件加载完成\n");
        break;

    default:
        tty_printf("TFTP 无该错误类型\n");
        break;
    }
    return tftp_end;
}

uint8_t get_err(uint16_t errnum,uint8_t *buf)
{
    uint8_t errlen = 0;

    memset(buf,0,40);
    buf[0] = 0;
    buf[1] = 5;
    buf[2] = (uint8_t)(errnum >> 8);
    buf[3] = (uint8_t)(errnum & 0x00ff);

    switch(errnum)
    {
    case 0:
        strcpy((char *)buf+4,"Write flash failed");
//			tty_printf("文件写入flash失败\n");
        errlen = 23;
        break;

    case 1:
        strcpy((char *)buf+4,"File not found");
//			tty_printf("文件未找到\n");
        errlen = 19;
        break;

    case 2:
        strcpy((char *)buf+4,"Access violation");
//			tty_printf("拒绝访问\n");
        errlen = 21;
        break;

    case 3:
        strcpy((char *)buf+4,"Disk full or allocation exceeded");
//			tty_printf("磁盘已满或者超出可分配空间\n");
        errlen = 37;
        break;

    case 4:
        strcpy((char *)buf+4,"Illegal TFTP operation");
//			tty_printf("非法TFTP操作\n");
        errlen = 27;
        break;

    case 5:
        strcpy((char *)buf+4,"Unknown transfer ID");
//			tty_printf("传输ID未知\n");
        errlen = 24;
        break;

    case 6:
        strcpy((char *)buf+4,"File already exists");
//			tty_printf("文件不存在\n");
        errlen = 24;
        break;

    case 7:
        strcpy((char *)buf+4,"No such user");
//			tty_printf("没有该用户\n");
        errlen = 17;
        break;

    default:
//			tty_printf("TFTP 无该错误类型\n");
        break;
    }

    return errlen;
}

//const char rrq[23]	= {0x00,0x01,'g','e','n','e','r','i','c','a','r','m','.','b','i','n',0,'o','c','t','e','t',0x00};
NET_ADDR addr = { NET_ADDR_IP4, 69, 192, 168, 1, 155 };

void send_udp_data (NET_ADDR *serveraddr,char *buf,uint8_t len)
{
    uint8_t *sendbuf;
    netStatus status;

    if (udp_sock >= 0)
    {
        sendbuf = netUDP_GetBuffer (len);

        for(char i = 0; i < len; i++)
        {
            sendbuf[i] = buf[i];
        }
        status = netUDP_Send(udp_sock, serveraddr, sendbuf, len);
    }
    status = status;
}

static uint32_t tftp_firmware_update_epcs(uint16_t blok_num,uint8_t *buf,uint32_t len)
{
    uint32_t err = 0,offset = 0;


    if(blok_num == 1)
    {
        epcs_init();
        memcpy(pof_head,buf,POF_HEAD_LENGTH);
        buf += POF_HEAD_LENGTH;
        len = TFTP_DATA_LEN - POF_HEAD_LENGTH;
        offset = 0;
    }
    else if(blok_num <= 4096)
    {
        offset = (blok_num - 1) * TFTP_DATA_LEN - POF_HEAD_LENGTH;
    }
    else if(blok_num == 4097)
    {
        memcpy(pof_tail,buf + len - POF_TAIL_LENGTH , POF_TAIL_LENGTH);
        offset = (blok_num - 1) * TFTP_DATA_LEN - POF_HEAD_LENGTH;
        len = POF_HEAD_LENGTH;
    }
    else
    {
        return err;
    }
    epcs_transdata(buf, len);

    if(epcs_write(buf, offset, len) != 0)
    {
        err = 1;
    }

    return err;
}

static uint8_t tftp_firmware_update_file(char *name,uint8_t *buf, uint32_t len)
{
    FILE_STRUCT *f;
    int err = 0;

    f = getfileaddr(name);

    if(f == NULL)
    {
        err = 9;
//        tty_printf("open file failed\n");
        return err;
    }

    if(len == 0)
    {
        f->fsize = f->fwritennum +len;
        err = 12;
//			tty_printf("文件加载完成\n");
        return err;
    }
    else if(len == TFTP_DATA_LEN)
    {
        f->fsize = f->fwritennum + TFTP_DATA_LEN * 2;
    }
    else if(len < TFTP_DATA_LEN)
    {
        f->fsize = f->fwritennum +len;
        err = 12;
//				tty_printf("文件加载完成\n");
    }
    else
    {
        err = 5;
        return err;
    }

    osSemaphoreWait(nand_sem, osWaitForever);
    if((int)fwrite_w(f,buf,len))
    {
        err = 1;
    }
		osSemaphoreRelease(nand_sem);
    return err;
}

static void tftp_operating_end(void)
{
    netUDP_Close(udp_sock);
    netUDP_ReleaseSocket (udp_sock);
}

uint32_t udp_cb_func (int32_t socket, const  NET_ADDR *addr, const uint8_t *buf, uint32_t len)
{
    static uint16_t block_num = 0;
    uint8_t sendlen;

    for(uint8_t i = 0; i < 50; i++)
    {
        tftpbuf[i] = buf[i];
    }
    if(len <= 2)
    {
        tftp_end = 10;
//		tty_printf("服务器无应答\n");
    }
    else
    {
        switch(buf[1])
        {
        case 1:
            break;

        case 2:
            break;

        case 3:
            serverip.port = ((uint16_t)(*(buf-8)) << 8) + *(buf-7);
//				memcpy(tftpbuf,buf,520);

            if(block_num + 1 == (((uint16_t)buf[2] << 8) | ((uint16_t)buf[3])))
            {
                block_num++;

                tftp_end = tftp_firmware_update_file(filename_to_wtrie,(uint8_t *)buf+4,len-4);

								if(strcmp(filename_to_wtrie,"fpga_cur.pof") == 0)
								{
										if(tftp_firmware_update_epcs(block_num,(uint8_t *)tftpbuf + 4,len-4))
										{
												tftp_end = 1;
										}
								}

                if(tftp_end)
                {
                    send_udp_data((NET_ADDR *)&serverip,(char *)&tftpbuf,get_err(0,tftpbuf));
                }
                else
                {
                    sendlen = get_ack(block_num,tftpbuf);
                    send_udp_data((NET_ADDR *)&serverip,(char *)&tftpbuf,sendlen);
                }
            }
            else if(block_num == (((uint16_t)buf[2] << 8) |((uint16_t)buf[3])))
            {

            }
            else
            {
                send_udp_data((NET_ADDR *)&serverip,(char *)&tftpbuf,get_err(5,tftpbuf));
            }
            break;

        case 4:
            break;

        case 5:
            tftp_end = buf[3] + 1;
            block_num = 0;

            break;

        default:
            tftp_end = 11;
            break;
        }
    }

    if(tftp_end)
    {
        block_num = 0;
        tftp_operating_end();
    }
    return (0);
}

void tftp_getfile_start(NET_ADDR4 *addr, char *filename, char *lfn)
{
    uint8_t sendlen;

    serverip = *addr;
    serverip.port = 69;
    filename_to_wtrie = lfn;

    tftp_end = 0;
    udp_sock = netUDP_GetSocket(udp_cb_func);
    if (udp_sock >= 0)
    {
        netUDP_Open (udp_sock, 2000);
    }

    sendlen = get_rrq(filename,tftpbuf);
    send_udp_data((NET_ADDR *)&serverip,(char *)&tftpbuf,sendlen);
}
