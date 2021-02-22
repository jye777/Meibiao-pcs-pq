#ifndef MBSERVER_H_
#define MBSERVER_H_

#include "cmsis_os.h"
#include "rl_net.h"
#include "record_wave.h"

#define LOG_SEND_MAXNUM	130
#define UPDATE_ONCE_SIZE		(2048)
#define MAX_CONN_NUM		3	//MODBUS������������֧��������
#define PIDV_SET_LHFLAG_HIGH	(0x0)
#define PIDV_SET_LHFLAG_LOW		(0x1)
#define BIT32_LOW		1
#define BIT32_HIGH		2

#define CTOS_16(a,b) (((unsigned short) a) << 16)|(b&0xffff)

/* ************** ʵʱ���������Ϣ ************** */
typedef struct 
{
	volatile unsigned char num;			//�贫���ͨ������
	volatile unsigned char resp[5];		//01-mbid,23-mbpro,4-unitid
	volatile unsigned short nps[CHANNEL_MAX];	//ÿ�봫����ٽ��
	volatile unsigned short inv[CHANNEL_MAX];	//������ٺ���
	volatile unsigned int tail[CHANNEL_MAX];	//���λ�����ָ��
	volatile unsigned char enable;	    //״̬��ʶ
}_sendwave_inf;

/* *************** ��־�����Ϣ ************** */


typedef struct
{
	volatile unsigned char resp[5];	//01-mbid,23-mbpro,4-unitid
	volatile unsigned char enflag;	//ʹ�ܱ�־
	volatile unsigned short rnum;	//�ѽ�����Ŀ
	volatile unsigned int tail;		//����ָ��
	volatile unsigned int num;		//Ӧ������Ŀ
}_sendlog_inf;

/* *************** ���������Ϣ ************** */
typedef struct
{
	volatile unsigned char resp[5];		//01-mbid,23-mbpro,4-unitid
	volatile unsigned short inv;		//������ٺ���
	volatile unsigned int tick;			//��ʱ���������ж��Ƿ�������
}_sendallp_inf;

/* ************** ��׼Э�������Ϣ ************** */
typedef struct
{
	volatile unsigned char resp[5]; 	//01-mbid,23-mbpro,4-unitid
    unsigned char recvbuf[50000];       //��ͨ���ݽ��ջ�����
    unsigned char respbuf[50000];       //��ͨ���ݷ��ͻ�����
}_sendmb_inf;

#define FIRMWARE_TIMEOUT_MS     20000
typedef struct
{
    /* �̼�������Ч��־ */
    int valid;
    /* �̼�����״̬ */
    int state;
    /* ��ʱ����־ */
    int timeout_check;
    /* ��ʱ�������� */
    unsigned int timeout_counter;
    /* 01-mbid, 23-mbpro, 4-unitid */
    volatile unsigned char resp[5];
}_update_control;

#define UPDATE_ONCE_SIZE		(2048)
#define UPDATE_ALL_SIZE			(0x200000)

typedef struct
{
	unsigned char flag;				/* �̼����±�־ */
	unsigned char type;				/* �̼���� */
	unsigned int codesize;			/* �̼����� */
	unsigned int recved;			/* �ѽ��ճ��� */
	unsigned int re_temp;
	unsigned int offset;			/* ���δ������ */
	unsigned int len;				/* ���δ��䳤�� */
	unsigned char md5[16];			/* У���� */
	unsigned char buf[UPDATE_ONCE_SIZE];	/* �������ݻ����� */
}_update_inf;

/* ************** MODBUS�ͻ��������Ϣ ************** */
typedef struct
{
	_sendwave_inf sendwave_inf;	//ʵʱ���δ��������Ϣ
	_sendallp_inf sendallp_inf;	//���������Ϣ
	_sendlog_inf sendlog_inf;	//��־�����Ϣ
	_sendmb_inf sendmb_inf;		//��׼Э��͹��ϲ��δ��������Ϣ
	_update_control update_ctrl;   //�̼����¿���
	uint8_t conns;				//����״̬: 0-δ���ӻ����Ӵ���,1-��������
	int pConn;		        //�ͻ�������������
	uint8_t update;				//�̼����±�־����ǰ�������ڽ��й̼�����ʱ��1�����³ɹ���ʧ����0
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
	unsigned int gaddr;			/* mb_pv�е�������� */
	unsigned int addr;			/* mb_pv�����еĳ�Ա��� */
	unsigned int faddr;			/* ��Ӧ��FPGA��ַ */
	unsigned short mbid;		/* ��Ӧ��MODBUS�Ĵ���ID */
	unsigned short value;		/* ����ֵ */
	unsigned char lhflag;		/* �ߵ�λ��ʶ */
	unsigned short *paraset;	/* ��Ӧctlparaʵ�ʵ�ַ */
}MB_PIDV_SET;

extern _mbser_inf mbser_inf[MAX_CONN_NUM];
extern int longdataex_remain;
extern unsigned char mb_writereg(unsigned short saddr,unsigned short value,short *rt);
extern void firmware_check_delthreah(void);
extern void firmware_check_createthreah(void);

void MBsever_task(const void* pdata);
void mbresp_spec_task(const void *pdata);

#endif 

