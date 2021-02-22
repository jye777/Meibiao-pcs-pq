#ifndef __RECORDWAVE_H__
#define __RECORDWAVE_H__

#include "stdint.h"

#define FM_CHANNEL_MAX 1

#define FM_SAMPLE_FREQUENCY  (100) /* 每秒钟100次采样 */
#define FM_SAMPLE_HEAD_DURATION  (4)
#define FM_SAMPLE_TAIL_DURATION  (6)
#define FM_SAMPLE_DURATION   (FM_SAMPLE_HEAD_DURATION+FM_SAMPLE_TAIL_DURATION)
#define FM_SAMPLE_POINT_QTY     FM_SAMPLE_FREQUENCY*FM_SAMPLE_DURATION

struct fmWaveType
{
	unsigned int head;		//0xC0C0
	unsigned int len;		//CRC之后的长度
	unsigned int crc;		//CRC32校验值
    short ft_cv[FM_CHANNEL_MAX][FM_SAMPLE_POINT_QTY];
};

struct fmInforType
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
    unsigned short cid[FM_CHANNEL_MAX];	//通道标识
};


struct fmAllInforType
{
	struct fmInforType brief;
	struct fmWaveType wave;
};

#define FMLOG_MAX 50
struct fmFileType
{
		unsigned int curnum;
		unsigned int addr[FMLOG_MAX];
		struct fmAllInforType  fft_infor;
};


struct fmFileInfor
{
    unsigned char flag;
    unsigned char *event;
	unsigned char record_doing;
};




void RecordWave_Start(void);
void RecordWave_CreateTask(void);
uint16_t* RecordWave_GetSamplePoint(void);


int RecordWave_StoreFmlog(void);

#endif
