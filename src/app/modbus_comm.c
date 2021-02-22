#define MODBUS_COMM_C
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
//#include "network.h"
#include "cmsis_os.h"
#include "modbus_comm.h"
#include "stm32f4xx_hal.h"
#include "Driver_USART.h"
#include "para.h"
#include "fpga.h"
#include "para.h"
#include "micro.h"
#include "log.h"
#include "crc.h"
#include "sdata_alloc.h"
#include "screen.h"

#define mb_dbg      printf

#if 0
static unsigned char mb_recv_buf[MB_RECVBUF_SIZE];
static unsigned char mb_send_buf[MB_SENDBUF_SIZE];

static int modbus_recv_packet(void);
static void modbus_resp(void);
static void modbus_read_hodingregs(void);
static void modbus_write_singlereg(void);
static void modbus_write_multipleregs(void);
static void modbus_illegal_func(void);
static void modbus_resp_err(unsigned char addr, unsigned char func,
                            unsigned char err);
static int modbus_crc_set(unsigned char *pdata, unsigned short cntlen);
static int modbus_crc_check(unsigned char *pdata, unsigned short cntlen);
static unsigned short modbus_read_reg(unsigned short addr, unsigned char *err);
static unsigned char modbus_write_reg(unsigned short addr, /*unsigned*/ short
                                      value);
extern ARM_DRIVER_USART Driver_USART6;
extern osThreadId tid_modbus_service;


void usart6_callback(uint32_t event)
{
    switch(event)
    {
    case ARM_USART_EVENT_RX_TIMEOUT:
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
        osSignalSet(tid_modbus_service, 0x01);
        break;
    default:
        break;
    }
}

void modbus_service_task(const void *pdata)
{
    int err;

    while(1)
    {
        err = modbus_recv_packet();

        if(err == 0)
        {
            modbus_resp();
        }

        osDelay(1);
    }
}

int modbus_recv_packet(void)
{
    unsigned int plen;

    ARM_DRIVER_USART *drv = &Driver_USART6;

    while(drv->GetStatus().tx_busy)
    {
        osDelay(1);
    }

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
    drv->Control(ARM_USART_ABORT_RECEIVE, 0);
    drv->Receive(mb_recv_buf, sizeof(mb_recv_buf));

    /* 等待数据包接收完成 */
    osSignalWait(1, osWaitForever);

    plen = drv->GetRxCount();

    if(plen >= MB_RECVBUF_SIZE)
    {
        return MB_ERR_RECVFAILED;
    }

    /*
        数据包长度过小则报错:
        地址(1BYTE)
        功能码(1BYTE)
        CRC(2BYTE)
        若数据只有1个字节，至少应有5个字节
    */
    if(plen < 5)
    {
        return MB_ERR_RECVFAILED;
    }

    /* 判断地址是否正确 */
#if MB_RECV_CHECKADDR

    if(mb_recv_buf[0] != arm_config_data_read(
                INI_MONITOR_ADDR_ADD)/*MB_MONITOR_ADDR*/)
    {
        printf("addr is error %d\n", arm_config_data_read(INI_MONITOR_ADDR_ADD));
        return MB_ERR_FORMATERR;
    }

#endif

    mb_inf.plen = plen;

    return MB_ERR_NONE;
}

int modbus_send_packet(uint8_t *buf, uint16_t cnt)
{
    ARM_DRIVER_USART *drv = &Driver_USART6;

    while(1)
    {
        if(drv->GetStatus().tx_busy)
        {
            osDelay(1);
            continue;
        }

        drv->Send(buf, cnt);
        break;
    }

    return MB_ERR_NONE;
}

void modbus_resp(void)
{
    unsigned char func;

    func = mb_recv_buf[1];

    switch(func)
    {
    case 0x03:
        //printf("功能码为03");
        modbus_read_hodingregs();
        break;

    case 0x04:
        //printf("功能码为04");
        modbus_read_hodingregs();
        break;

    case 0x06:
        //printf("功能码为06");
        modbus_write_singlereg();
        break;

    case 0x10:
        modbus_write_multipleregs();
        break;

    default:
        modbus_illegal_func();
        break;
    }
}

void modbus_read_hodingregs(void)
{
    unsigned char *rbuf;
    unsigned char *sbuf;
    unsigned short value;
    unsigned short addr;
    unsigned short num;
    unsigned short i;
    unsigned char err = 0;

    rbuf = mb_recv_buf;
    sbuf = mb_send_buf;

    //包大小应为8个字节
    if(mb_inf.plen < 8)
    {
        mb_inf.plen = 0;
        return;
    }

    //计算CRC校验值是否正确
    if(modbus_crc_check(rbuf, 6) != 0)
    {
        mb_inf.crc_err++;
        //  printf("err");
        return;
    }

    //起始地址
    addr = (rbuf[2] << 8) | rbuf[3];
    //读取数量
    num = (rbuf[4] << 8) | rbuf[5];

    //读寄存器值，填充到发送缓冲区
    for(i = 0; i < num; i++)
    {
        // printf("读寄存器\n");

        value = modbus_read_reg(addr + i, &err);

        //printf("%d",i+addr);
        //printf("%d",value);
        //printf("err is%d\n",err);
        if(err != 0)
        {
            modbus_resp_err(rbuf[0], rbuf[1], err);
            return;
        }

        sbuf[3 + i * 2] = value >> 8;
        sbuf[4 + i * 2] = value & 0xff;
    }

    //填充发送缓冲区除CRC段的内容
    sbuf[0] = rbuf[0];
    sbuf[1] = rbuf[1];
    sbuf[2] = num * 2;

    //填充CRC校验值
    modbus_crc_set(sbuf, num * 2 + 3);

    //发送回应
    modbus_send_packet(sbuf, num * 2 + 5);
}

void modbus_write_singlereg(void)
{
    unsigned char *rbuf;
    unsigned char *sbuf;
    //   unsigned short value;
    short   value;
    unsigned short addr;
    unsigned char err;

    rbuf = mb_recv_buf;
    sbuf = mb_send_buf;

    //包大小应为8个字节
    if(mb_inf.plen < 8)
    {
        mb_inf.plen = 0;
        return;
    }

    //计算CRC校验值是否正确
    if(modbus_crc_check(rbuf, 6) != 0)
    {
        mb_inf.crc_err++;
        return;
    }

    addr = (rbuf[2] << 8) | rbuf[3];
    value = (short)((rbuf[4] << 8) | rbuf[5]);
    //  printf("写寄存器\n");
    err = modbus_write_reg(addr, value);
    //  modbus_handle_delayed();

    if(err != 0)
    {
        modbus_resp_err(rbuf[0], rbuf[1], err);
        return;
    }

    //组包
    memcpy(sbuf, rbuf, 6);
    modbus_crc_set(sbuf, 6);

    //发送回应
    modbus_send_packet(sbuf, 8);
}

void modbus_write_multipleregs(void)
{
    unsigned char *rbuf;
    unsigned char *sbuf;
    unsigned short value;
    unsigned short addr;
    unsigned char err;
    unsigned short num;
    unsigned char bytes;
    int i;

    rbuf = mb_recv_buf;
    sbuf = mb_send_buf;

    addr = (rbuf[2] << 8) | rbuf[3];
    num = (rbuf[4] << 8) | rbuf[5];
    bytes = rbuf[6];

    if(bytes != (2 * num))
    {
        return;
    }

    //包大小应为8个字节
    if(mb_inf.plen < bytes + 9)
    {
        mb_inf.plen = 0;
        return;
    }

    //计算CRC校验值是否正确
    if(modbus_crc_check(rbuf, bytes + 7) != 0)
    {
        mb_inf.crc_err++;
        return;
    }

    for(i = 0; i < num; i++)
    {
        value = (rbuf[7 + i * 2] << 8) | rbuf[8 + i * 2];
        err = modbus_write_reg(addr + i, value);
        //  modbus_handle_delayed();

        if(err != 0)
        {
            modbus_resp_err(rbuf[0], rbuf[1], err);
            return;
        }
    }

    //组包
    memcpy(sbuf, rbuf, 6);
    modbus_crc_set(sbuf, 6);

    //发送回应
    modbus_send_packet(sbuf, 8);
}

void modbus_illegal_func(void)
{
    unsigned char addr;
    unsigned char func;

    addr = mb_recv_buf[0];
    func = mb_recv_buf[1];

    mb_dbg("未实现的功能码:0x%x\n", func);

    modbus_resp_err(addr, func, 1);
}

void modbus_resp_err(unsigned char addr, unsigned char func, unsigned char err)
{
    unsigned char *sbuf;

    sbuf = mb_send_buf;

    //功能码不能超出范围
    if(func >= 0x80)
    {
        return;
    }

    //组包
    sbuf[0] = addr;
    sbuf[1] = 0x80 + func;
    sbuf[2] = err;

    modbus_crc_set(sbuf, 3);

    //发送回应
    modbus_send_packet(sbuf, 5);
}

int modbus_crc_set(unsigned char *pdata, unsigned short cntlen)
{
    unsigned short crc;

    crc = crc16(pdata, cntlen);
    pdata[cntlen] = crc >> 8;
    pdata[cntlen + 1] = crc & 0xff;

    return 0;
}

int modbus_crc_check(unsigned char *pdata, unsigned short cntlen)
{
    unsigned short crc;
    unsigned short crcget;

    crc = crc16(pdata, cntlen);
    crcget = pdata[cntlen + 1] | (pdata[cntlen] << 8);

    if(crc == crcget)
    {
        return 0;
    }
    else
    {
        mb_dbg("CRC校验错误\n");
        return 1;
    }
}

unsigned short modbus_read_reg(unsigned short addr, unsigned char *err)
{
    unsigned short value = 0;

    switch(addr)
    {
    case 0:
        value = getting_data_handle(Addr_Param120, Upper_Param[Addr_Param120]);
        break;

    case 1:
        value = getting_data_handle(Addr_Param121, Upper_Param[Addr_Param121]);
        break;

    case 2:
        value = getting_data_handle(Addr_Param122, Upper_Param[Addr_Param122]);
        break;

    case 3:
        value = getting_data_handle(Addr_Param136, Upper_Param[Addr_Param136]);
        break;

    case 4:
        value = getting_data_handle(Addr_Param137, Upper_Param[Addr_Param137]);
        break;

    case 5:
        value = getting_data_handle(Addr_Param138, Upper_Param[Addr_Param138]);
        break;

    case 6:
        value = getting_data_handle(Addr_Param135, Upper_Param[Addr_Param135]);
        break;

    case 7:
        value = getting_data_handle(Addr_Param189, Upper_Param[Addr_Param189]);
        break;

    case 8:
        value = getting_data_handle(Addr_Param190, Upper_Param[Addr_Param190]);
        break;

    case 100:
        value = miro_write.Connect_State;
        break;

    default:
        value = 0;
        *err = 0;
        break;
    }
    return value;
}

unsigned char modbus_write_reg(unsigned short addr, /*unsigned*/ short value)
{
    unsigned char err = 0;

    switch(addr)
    {
    case 100:
        mb_data.micro_state = value;
        break;

    case 101:
        mb_data.u_grid_ever = value;

    case 102:
        mb_data.micro_sw_sta = value;

    default:
        err = 0;
        break;
    }

    return err;
}
#endif
