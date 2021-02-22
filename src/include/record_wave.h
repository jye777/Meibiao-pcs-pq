#ifndef RECORD_WAVE_H_
#define RECORD_WAVE_H_

typedef struct
{
    unsigned char flag;
    unsigned char *event;
	unsigned char record_doing;
}_fault_inf;

#ifdef RECORD_WAVE_C
#define RECORD_WAVE_EXTERN
#else
#define RECORD_WAVE_EXTERN      extern
extern unsigned int rtwq_tail,rtwq_head,ftwq_tail,ftwq_head;
#endif

#define record_dbg	printf

/* 波形记录 */

#define VOLT_FREQ       (3000)
#define CUR_FREQ        (3000)
#define TEMPER_FREQ     (16)

#define RT_BUF_SEC      (20)//实时波形保存时间
#define FT_SBF          (5)//故障前波形保存时间
#define FT_SAF          (15)//故障后波形保存时间
#define FT_BUF_SEC      (FT_SBF + FT_SAF)//故障前后波形保存总时间

#define RTWQ_MAX_SIZE (VOLT_FREQ*RT_BUF_SEC)
#define FTWQ_MAX_SIZE (VOLT_FREQ*FT_BUF_SEC)
#define RTWQ_HSIZE  RTWQ_MAX_SIZE
#define RTWQ_LSIZE  (TEMPER_FREQ*RT_BUF_SEC)
#define FTWQ_HSIZE  FTWQ_MAX_SIZE
#define FTWQ_LSIZE  (TEMPER_FREQ*FT_BUF_SEC)

#define CHANNEL_MAX 	(9)  //要记录的最大通道数

#define FIFOSEL_CHANNEL_6		0x43
#define FIFOSEL_CHANNEL_7		0x44
#define FIFOSEL_CHANNEL_8		0x45

#define WAVEBUF RECORD_WAVE_EXTERN short

//实时波形数据缓冲区
WAVEBUF rt_cv[CHANNEL_MAX][VOLT_FREQ*RT_BUF_SEC];//电压电流
WAVEBUF rt_tem[3][TEMPER_FREQ*RT_BUF_SEC];//温度
//故障前后波形数据缓冲区
//WAVEBUF ft_cv[9][VOLT_FREQ*FT_BUF_SEC];//电压电流
//WAVEBUF ft_tem[3][TEMPER_FREQ*FT_BUF_SEC];//温度

#define FT_DATA_NUM_MAX					6
#define FT_ONEDATA_SIZE					0x110000

//AM29LV320DT
#define FT_DATA_NUM_AM29LV320DT			2
#define FT_FIFOINF_SEC_AM29LV320DT			27
#define FT_FIFOINF_BAK_SEC_AM29LV320DT		28
#define FT_DATAS_SEC_AM29LV320DT		29
#define FT_DATAE_SEC_AM29LV320DT		62

//AM29LV320DB
#define FT_DATA_NUM_AM29LV320DB			2
#define FT_FIFOINF_SEC_AM29LV320DB			33
#define FT_FIFOINF_BAK_SEC_AM29LV320DB		34
#define FT_DATAS_SEC_AM29LV320DB		35
#define FT_DATAE_SEC_AM29LV320DB		68

//S29GL064_04
#define FT_DATA_NUM_S29GL064_04			6
#define FT_FIFOINF_SEC_S29GL064_04			29
#define FT_FIFOINF_BAK_SEC_S29GL064_04		30
#define FT_DATAS_SEC_S29GL064_04		31
#define FT_DATAE_SEC_S29GL064_04		132

//故障存储信息
typedef struct
{
	unsigned char flash_enable;
	unsigned int onedata_size;
	unsigned int data_num;
	unsigned int fifoinf_sec;
	unsigned int fifoinf_bak_sec;
	unsigned int datas_sec;
	unsigned int datae_sec;
	unsigned int data_sec[FT_DATA_NUM_MAX];
}_fault_save_inf;

/* 故障数据队列 */

#define FS_FIFO_HEAD	0x5050

typedef struct
{
	unsigned int head;	//0x5050
	unsigned int len;	//crc之后的长度
	unsigned int crc;	//crc32校验值
	unsigned int size;	//可保存故障数
	unsigned int onedata_size;	//每个故障数据的容量(字节)
	unsigned int fifo_head;	//队列头(最老的故障)
	unsigned int fifo_tail;	//队列尾(最新的故障)
	unsigned int fifo_cnt;	//已保存故障数
}_fault_save_fifo;

typedef struct
{
	unsigned int len;	//len之后的长度
	unsigned int size;	//可保存故障数
	unsigned int onedata_size;	//每个故障数据的容量(字节)
	unsigned int fifo_head;	//队列头(最老的故障)
	unsigned int fifo_tail;	//队列尾(最新的故障)
	unsigned int fifo_cnt;	//已保存故障数
}_fault_save_fifoinf;

/* 故障概要 */

#define FS_BRIEF_HEAD		0xA0A0
#define FS_DEV_ID			1

typedef struct
{
	unsigned int head;		//0xA0A0
	unsigned int len;		//CRC之后的长度
	unsigned int crc;		//CRC32校验值
	unsigned short did;		//设备ID
	unsigned short fid;		//故障ID
	unsigned char year;
	unsigned char mon;
	unsigned char date;
	unsigned char hour;
	unsigned char min;
	unsigned char sec;
	unsigned short ms;
	unsigned int num;		//参数个数
    unsigned int wnum;		//波形点数量
    unsigned int widx;		//故障点索引
    unsigned int cnum;		//通道数量
    unsigned short cid[CHANNEL_MAX];	//通道标识
}_fault_save_brief;

typedef struct
{	
	unsigned int len;		//len之后的长度
	unsigned short did;		//设备ID
	unsigned short fid;		//故障ID
	unsigned char year;
	unsigned char mon;
	unsigned char date;
	unsigned char hour;
	unsigned char min;
	unsigned char sec;
	unsigned short ms;
	unsigned int num;		//参数个数
    unsigned int wnum;		//波形点数量
    unsigned int widx;		//故障点索引
    unsigned int cnum;		//通道数量
    unsigned short cid[CHANNEL_MAX];	//通道标识
}_fault_save_briefinf;

/* 故障参数 */

#define FS_PARA_HEAD		0xB0B0
#define PARA_NUM_MAX		1000

typedef struct
{
	unsigned int head;		//0xB0B0
	unsigned int len;		//CRC之后的长度
	unsigned int crc;		//CRC32校验值
	unsigned short para[PARA_NUM_MAX * 2];
}_fault_save_para;

typedef struct
{
	unsigned int len;		//len之后的长度
	unsigned short *para;
}_fault_save_parainf;

/* 故障波形 */

#define FS_WAVE_HEAD		0xC0C0

typedef struct
{
	unsigned int head;		//0xC0C0
	unsigned int len;		//CRC之后的长度
	unsigned int crc;		//CRC32校验值
    short ft_cv[CHANNEL_MAX][VOLT_FREQ*FT_BUF_SEC];
}_fault_save_wave;

typedef struct
{
	_fault_save_brief brief;
	_fault_save_para para;
	_fault_save_wave wave;
}_fault_save_data;

#define FAULT_MAX 5
typedef struct 
{
		unsigned int curnum;
		unsigned int addr[FAULT_MAX];
		_fault_save_data  fault;
}WAVEFILE;

RECORD_WAVE_EXTERN _fault_inf faultinf;
RECORD_WAVE_EXTERN _fault_save_inf fault_save;
//RECORD_WAVE_EXTERN _fault_save_data wave.fault;
RECORD_WAVE_EXTERN WAVEFILE wave;

RECORD_WAVE_EXTERN osSemaphoreId savefinf_sem;

void record_wave_init(void);
void record_wave_task(const void * pdata);

RECORD_WAVE_EXTERN unsigned int copyrtwave(short *sbuf,unsigned short channel,unsigned int len,int tail);
RECORD_WAVE_EXTERN unsigned int copyftwave(short *sbuf,unsigned short channel,unsigned int num,unsigned int saddr);
//保存故障波形到缓冲区
RECORD_WAVE_EXTERN int savefinf(int failtype,unsigned char *event);
//清除故障波形
RECORD_WAVE_EXTERN void clearftwave(unsigned char *event);
RECORD_WAVE_EXTERN void clearftwave_any(void);
RECORD_WAVE_EXTERN int fs_fifo_valid(unsigned char sec);
RECORD_WAVE_EXTERN int fs_fifoinf_read(_fault_save_fifoinf *pfifo);
RECORD_WAVE_EXTERN int fs_brief_valid(int idx);
RECORD_WAVE_EXTERN int fs_briefinf_read(int idx, _fault_save_briefinf *pbrief);
RECORD_WAVE_EXTERN int fs_briefinf_copy(int idx, _fault_save_briefinf *dst);
RECORD_WAVE_EXTERN int fs_para_valid(int idx);
RECORD_WAVE_EXTERN int fs_para_copy(short *dst, int idx, int num);
RECORD_WAVE_EXTERN int fs_wave_valid(int idx);
RECORD_WAVE_EXTERN int fs_wave_copy(short *dst, int idx, int cidx, 
						int widx, int wnum);
RECORD_WAVE_EXTERN int fs_delete(void);
RECORD_WAVE_EXTERN void clearftwave(unsigned char* event);
int savefinf2(int failtype, unsigned char* event);


#endif /*RECORD_WAVE_H_*/

