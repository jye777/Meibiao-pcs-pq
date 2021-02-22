#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "epcs.h"
#include "cmd.h"

extern uint8_t epcs_databuf[0x10000];

int do_epcs_init(struct tty *ptty)
{
    epcs_init();
    ptty->printf("��ʼ�����\n");

    return 0;
}

int do_epcs_close(struct tty *ptty)
{
    epcs_close();
    ptty->printf("�ѹر�\n");

    return 0;
}

int do_epcs_read_status(struct tty *ptty)
{
    int err;
    unsigned char status;

    err = epcs_read_status(&status);

    if(err != 0)
    {
        ptty->printf("��״̬����\n");
    }
    else
    {
        ptty->printf("epcs status: 0x%x\n", status);
    }

    return 0;
}

int do_epcs_write_status(struct tty *ptty)
{
    int err;
    unsigned int status;

    if(ptty->cmd_num < 2)
    {
        ptty->printf("�����������\n");
        return 1;
    }

    sscanf(ptty->cmd_buf[1], "%x", &status);

    ptty->printf("����״ֵ̬Ϊ: 0x%x\n", status);

    err = epcs_write_status((unsigned char)status);

    if(err != 0)
    {
        ptty->printf("д״̬����\n");
    }
    else
    {
        ptty->printf("д״̬�ɹ�\n");
    }

    return 0;
}

int do_epcs_read_bytes(struct tty *ptty)
{
    int err;
    unsigned int addr;
    unsigned int size;
    unsigned int onesize;
    unsigned int offset;

    if(ptty->cmd_num < 3)
    {
        ptty->printf("�����������\n");
        return 1;
    }

    sscanf(ptty->cmd_buf[1], "%x", &addr);
    sscanf(ptty->cmd_buf[2], "%x", &size);

    offset = 0;

    while(size)
    {
        onesize = size > 0xffff ? 0xffff : size;

        ptty->printf("�ӵ�ַ0x%x��ȡ0x%x�ֽ�����: \n", addr + offset, onesize);

        err = epcs_read_bytes(&epcs_databuf[offset], addr + offset, onesize);

        if(err != 0)
        {
            ptty->printf("��ȡʧ��\n");
            return 0;
        }

        offset += onesize;
        size -= onesize;
    }

    ptty->printf("��ȡ�ɹ�\n");

    return 0;
}

int do_epcs_read_silicon_id(struct tty *ptty)
{
    int err;
    unsigned char id;

    err = epcs_read_silicon_id(&id);

    if(err != 0)
    {
        ptty->printf("��ȡʧ��\n");
    }
    else
    {
        ptty->printf("ID: 0x%x\n", id);
    }

    return 0;
}

int do_epcs_display_buf(struct tty *ptty)
{
    int i, j;
    unsigned int addr;
    unsigned int size;
    unsigned char data;
    unsigned char temp;

    if(ptty->cmd_num < 3)
    {
        ptty->printf("�����������\n");
        return 1;
    }

    sscanf(ptty->cmd_buf[1], "%x", &addr);
    sscanf(ptty->cmd_buf[2], "%x", &size);

    if(size > sizeof(epcs_databuf))
    {
        ptty->printf("�ֽ�������������\n");
        return 0;
    }

    for(i = 0, j = 0; i < size; i++)
    {
        temp = epcs_databuf[addr + i];

        data = ((temp & 0x1) << 7) |
               ((temp & 0x2) << 5) |
               ((temp & 0x4) << 3) |
               ((temp & 0x8) << 1) |
               ((temp & 0x10) >> 1) |
               ((temp & 0x20) >> 3) |
               ((temp & 0x40) >> 5) |
               ((temp & 0x80) >> 7);

        ptty->printf("%02x", data);

        j++;

        if(j == 0x10)
        {
            ptty->printf("\n");
            j = 0;
        }
        else
        {
            ptty->printf(" ");
        }
    }

    return 0;
}

int do_epcs_display2_buf(struct tty *ptty)
{
    int i;
    unsigned int addr;
    unsigned int size;
    unsigned char data;
    unsigned char temp;

    if(ptty->cmd_num < 3)
    {
        ptty->printf("�����������\n");
        return 1;
    }

    sscanf(ptty->cmd_buf[1], "%x", &addr);
    sscanf(ptty->cmd_buf[2], "%x", &size);

    if(size > sizeof(epcs_databuf))
    {
        ptty->printf("�ֽ�������������\n");
        return 0;
    }

    for(i = 0; i < size; i++)
    {
        temp = epcs_databuf[addr + i];

        data = ((temp & 0x1) << 7) |
               ((temp & 0x2) << 5) |
               ((temp & 0x4) << 3) |
               ((temp & 0x8) << 1) |
               ((temp & 0x10) >> 1) |
               ((temp & 0x20) >> 3) |
               ((temp & 0x40) >> 5) |
               ((temp & 0x80) >> 7);

        ptty->printf("%c", data);
    }

    return 0;
}

int do_epcs_erase(struct tty *ptty)
{
    unsigned char status;
    unsigned int addr;
    int flag;

    if(ptty->cmd_num < 2)
    {
        flag = 0;
    }
    else
    {
        flag = 1;
        sscanf(ptty->cmd_buf[1], "%x", &addr);
    }

    epcs_write_enable();

    if(flag)
    {
        epcs_erase_sector(addr);
    }
    else
    {
        epcs_erase_bulk();
    }

    while(1)
    {
        epcs_read_status(&status);

        if((status & 1) == 0)
        {
            break;
        }
    }

    epcs_write_disable();

    ptty->printf("�������\n");

    return 0;
}

int do_epcs_write(struct tty *ptty)
{
    int err;
    unsigned int addr;
    unsigned int size;
    unsigned int onesize;
    unsigned int offset;
    unsigned char status;

    if(ptty->cmd_num < 3)
    {
        ptty->printf("�����������\n");
        return 1;
    }

    sscanf(ptty->cmd_buf[1], "%x", &addr);
    sscanf(ptty->cmd_buf[2], "%x", &size);

    if(size > sizeof(epcs_databuf))
    {
        ptty->printf("�ֽ�������������\n");
        return 0;
    }

    offset = 0;

    while(size)
    {
        onesize = size > 256 ? 256 : size;

        epcs_write_enable();

        err = epcs_write_bytes(&epcs_databuf[offset], addr + offset, onesize);

        if(err != 0)
        {
            ptty->printf("д��ʧ��\n");
            epcs_write_disable();
            return 0;
        }

        offset += onesize;
        size -= onesize;

        while(1)
        {
            epcs_read_status(&status);
            if((status & 1) == 0)
            {
                break;
            }
        }
    }

    epcs_write_disable();

    ptty->printf("д�������\n");

    return 0;
}

int do_epcs_copy(struct tty *ptty)
{
    unsigned int addr;
    unsigned int size;
    unsigned int i;
    unsigned char temp, *pdata;

    if(ptty->cmd_num < 3)
    {
        ptty->printf("�����������\n");
        return 1;
    }

    sscanf(ptty->cmd_buf[1], "%x", &addr);
    sscanf(ptty->cmd_buf[2], "%x", &size);

    if(size > sizeof(epcs_databuf))
    {
        ptty->printf("�ֽ�������������\n");
        return 0;
    }

    ptty->printf("�ӵ�ַ0x%x����0x%x�ֽ����ݵ�������\n", addr, size);

    pdata = (unsigned char *)addr;

    for(i = 0; i < size; i++)
    {
        temp = pdata[i];

        epcs_databuf[i] = ((temp & 0x1) << 7) |
                          ((temp & 0x2) << 5) |
                          ((temp & 0x4) << 3) |
                          ((temp & 0x8) << 1) |
                          ((temp & 0x10) >> 1) |
                          ((temp & 0x20) >> 3) |
                          ((temp & 0x40) >> 5) |
                          ((temp & 0x80) >> 7);
    }

    ptty->printf("����ɸߵ�λת��\n");

    return 0;
}

int do_epcs(struct tty *ptty)
{
    if(ptty->cmd_num < 2)
    {
        ptty->printf("����Ĳ�������ȷ\n");
    }

    if(strcmp(ptty->cmd_buf[1], "init") == 0)
    {
        do_epcs_init(ptty);
    }
    else if(strcmp(ptty->cmd_buf[1], "close") == 0)
    {
        do_epcs_close(ptty);
    }
    else if(strcmp(ptty->cmd_buf[1], "rs") == 0)
    {
        do_epcs_read_status(ptty);
    }
    else if(strcmp(ptty->cmd_buf[1], "ws") == 0)
    {
        do_epcs_write_status(ptty);
    }
    else if(strcmp(ptty->cmd_buf[1], "rb") == 0)
    {
        do_epcs_read_bytes(ptty);
    }
    else if(strcmp(ptty->cmd_buf[1], "db") == 0)
    {
        do_epcs_display_buf(ptty);
    }
    else if(strcmp(ptty->cmd_buf[1], "db2") == 0)
    {
        do_epcs_display2_buf(ptty);
    }
    else if(strcmp(ptty->cmd_buf[1], "rid") == 0)
    {
        do_epcs_read_silicon_id(ptty);
    }
    else if(strcmp(ptty->cmd_buf[1], "erase") == 0)
    {
        do_epcs_erase(ptty);
    }
    else if(strcmp(ptty->cmd_buf[1], "write") == 0)
    {
        do_epcs_write(ptty);
    }
    else if(strcmp(ptty->cmd_buf[1], "cp") == 0)
    {
        do_epcs_copy(ptty);
    }
    else
    {
        ptty->printf("δ���������\n");
    }

    return 0;
}

cmdt cmd_epcs = {"epcs", do_epcs, "����EPCS16���ʹ���",
                 "epcs ����EPCS16���ʹ���\n"
                };

