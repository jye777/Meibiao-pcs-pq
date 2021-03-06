#include <stdio.h>
#include <string.h>

#include "cmd.h"
#include "fpga.h"
#include "para.h"

static unsigned short fpga_shell_buf[256];
/****************************************

 *@ ?FPGA??
 *
 * @???????+??+????
 *
 * @return 1:??0:??


******************************************/
extern volatile unsigned short *fpga;
extern short fpga_pdatabuf_write(uint16_t id, short value);
int do_fread(struct tty *ptty)
{
    //unsigned short *data;
    int i, addr, size, maxnum;

    if(ptty->cmd_num < 2)
    {
        ptty->printf("��������ȷ����\n");
        return 1;
    }

    sscanf(ptty->cmd_buf[1], "%d", &addr);

    if(ptty->cmd_num > 2)
    {
        sscanf(ptty->cmd_buf[2], "%d", &size);
    }
    else
    {
        size = 1;
    }

    maxnum = sizeof(fpga_shell_buf) / sizeof(fpga_shell_buf[0]);

    if(size > maxnum)
    {
        ptty->printf( "����ȡ%d���Ĵ���\n", maxnum);
        size = maxnum;
    }
    for(i=0; i<size; i++)
    {
        fpga_shell_buf[i] = fpga_read(addr+i);
    }
    //fpga_read(data, size, addr);

    for(i = 0; i < size; i++)
    {
        ptty->printf("fpga[0x%x] = 0x%x(%d %d 0x%x)\n",
                     addr + i, fpga_shell_buf[i], fpga_shell_buf[i], (short)fpga_shell_buf[i], (uint32_t)&fpga[addr + i]);
    }
    return 0;
}


int do_fwrite(struct tty *ptty)
{
    unsigned short data;
    int addr;

    if(ptty->cmd_num < 3)
    {
        ptty->printf("????????\n");
        return 1;
    }

    sscanf(ptty->cmd_buf[1], "%d", &addr);
    sscanf(ptty->cmd_buf[2], "%hx", &data);

    fpga_write(addr,data);
    if(addr<120)
    {
        fpga_pdatabuf_write(addr,data);
    }
    ptty->printf( "fpga write [0x%x] = 0x%x\n", addr, data);

    return 0;
}

int do_readparas(struct tty *ptty)
{
    int i;
    para_information* pinf;
    short v;

    pinf = &para_inf[0];

    for(i = 0; i < paranum; i++)
    {
        //ptty->printf("%s:0x%x\n", pinf->name, (unsigned short)v);
        readpara(pinf->id, &v);
        ptty->printf("%s:0x%x\n", pinf->name, (unsigned short)v);
        pinf++;
    }

    return 0;
}

int do_readpara(struct tty *ptty)
{
    int addr;
    short v;

    if(ptty->cmd_num < 2)
    {
        ptty->printf("��������\n");
        return 1;
    }

    sscanf(ptty->cmd_buf[1], "%d", &addr);
    readpara(addr,&v);
    ptty->printf("addr value is %d\n",v);
    return 0;
}

cmdt cmd_fread = {"fr", do_fread, "��FPGA�Ĵ���",
                  "fr 1\n"
                  "����ַ1�ļĴ���ֵ\n"
                 };
cmdt cmd_fwrite = {"fw", do_fwrite, "��FPGA�Ĵ���",
                   "fr 1\n"
                   "����ַ1�ļĴ���ֵ\n"
                  };
cmdt cmd_readparas = {"readparas", do_readparas, "����λ���Ĵ���",
                      "readparas\n"
                      "���Ĵ���ֵ\n"
                     };
cmdt cmd_readpara = {"rp", do_readpara, "����λ���Ĵ���",
                     "readparas\n"
                     "���Ĵ���ֵ\n"
                    };
