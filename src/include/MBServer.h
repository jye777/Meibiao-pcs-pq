#ifndef MBSERVER_H_
#define MBSERVER_H_

#include "cmsis_os.h"
#include "rl_net.h"
#include "record_wave.h"

#define LOG_SEND_MAXNUM	130
#define UPDATE_ONCE_SIZE		(2048)
#define MAX_CONN_NUM		3	//MODBUS网络服务器最大支持连接数
#define PIDV_SET_LHFLAG_HIGH	(0x0)
#define PIDV_SET_LHFLAG_LOW		(0x1)
#define BIT32_LOW		1
#define BIT32_HIGH		2

#define CTOS_16(a,b) (((unsigned short) a) << 16)|(b&0xffff)

/* ************** 实时波形相关信息 ************** */
typedef struct 
{
	volatile unsigned char num;			//需传输的通道数量
	volatile unsigned char resp[5];		//01-mbid,23-mbpro,4-unitid
	volatile unsigned short nps[CHANNEL_MAX];	//每秒传输多少结点
	volatile unsigned short inv[CHANNEL_MAX];	//间隔多少毫秒
	volatile unsigned int tail[CHANNEL_MAX];	//波形缓冲区指针
	volatile unsigned char enable;	    //状态标识
}_sendwave_inf;

/* *************** 日志相关信息 ************** */


typedef struct
{
	volatile unsigned char resp[5];	//01-mbid,23-mbpro,4-unitid
	volatile unsigned char enflag;	//使能标志
	volatile unsigned short rnum;	//已接收数目
	volatile unsigned int tail;		//发送指针
	volatile unsigned int num;		//应发送数目
}_sendlog_inf;

/* *************** 参数相关信息 ************** */
typedef struct
{
	volatile unsigned char resp[5];		//01-mbid,23-mbpro,4-unitid
	volatile unsigned short inv;		//间隔多少毫秒
	volatile unsigned int tick;			//计时器，用于判断是否发送数据
}_sendallp_inf;

/* ************** 标准协议相关信息 ************** */
typedef struct
{
	volatile unsigned char resp[5]; 	//01-mbid,23-mbpro,4-unitid
    unsigned char recvbuf[50000];       //普通数据接收缓冲区
    unsigned char respbuf[50000];       //普通数据发送缓冲区
}_sendmb_inf;

#define FIRMWARE_TIMEOUT_MS     20000
typedef struct
{
    /* 固件更新有效标志 */
    int valid;
    /* 固件更新状态 */
    int state;
    /* 超时检测标志 */
    int timeout_check;
    /* 超时检测计数器 */
    unsigned int timeout_counter;
    /* 01-mbid, 23-mbpro, 4-unitid */
    volatile unsigned char resp[5];
}_update_control;

#define UPDATE_ONCE_SIZE		(2048)
#define UPDATE_ALL_SIZE			(0x200000)

typedef struct
{
	unsigned char flag;				/* 固件更新标志 */
	unsigned char type;				/* 固件类别 */
	unsigned int codesize;			/* 固件长度 */
	unsigned int recved;			/* 已接收长度 */
	unsigned int re_temp;
	unsigned int offset;			/* 本次传输起点 */
	unsigned int len;				/* 本次传输长度 */
	unsigned char md5[16];			/* 校验码 */
	unsigned char buf[UPDATE_ONCE_SIZE];	/* 输入数据缓冲区 */
}_update_inf;

/* ************** MODBUS客户端相关信息 ************** */
typedef struct
{
	_sendwave_inf sendwave_inf;	//实时波形传输相关信息
	_sendallp_inf sendallp_inf;	//参数相关信息
	_sendlog_inf sendlog_inf;	//日志相关信息
	_sendmb_inf sendmb_inf;		//标准协议和故障波形传输相关信息
	_update_control update_ctrl;   //固件更新控制
	uint8_t conns;				//连接状态: 0-未连接或连接错误,1-连接正常
	int pConn;		        //客户端连接描述符
	uint8_t update;				//固件更新标志，当前连接正在进行固件更新时置1，更新成功或失败置0
}_mbser_inf;

typedef enum 
{
    UPDATE_IDLE = 0,
    UPDATE_PROGRAM_DOING,
    UPDATE_PROGRAM_FAILED,
    UPDATE_PROGRAM_SUCCESS,
    UPDATE_CHECK_DOING,
    UPDATE_CHECK_FAILED,
    UPDATE_CHECK_SUCCESS,
}firmware_update_state;

typedef struct _MB_PIDV_SET
{
	unsigned int gaddr;			/* mb_pv中的数组序号 */
	unsigned int addr;			/* mb_pv数组中的成员序号 */
	unsigned int faddr;			/* 对应的FPGA地址 */
	unsigned short mbid;		/* 对应的MODBUS寄存器ID */
	unsigned short value;		/* 配置值 */
	unsigned char lhflag;		/* 高低位标识 */
	unsigned short *paraset;	/* 对应ctlpara实际地址 */
}MB_PIDV_SET;

extern _mbser_inf mbser_inf[MAX_CONN_NUM];
extern int longdataex_remain;
extern unsigned char mb_writereg(unsigned short saddr,unsigned short value,short *rt);
extern void firmware_check_delthreah(void);
extern void firmware_check_createthreah(void);

void MBsever_task(const void* pdata);
void mbresp_spec_task(const void *pdata);

#endif 

