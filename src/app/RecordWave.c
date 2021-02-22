#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "cmsis_os.h"
#include "fpga.h"
#include "systime.h"
#include "recordwave.h"
#include "record_wave.h"
#include "fmlog.h"
#include "pcs.h"
#include "para.h"
//------------------------------------------------------------------
extern pcs_manage_t pcs_manage;
extern short Upper_Param[1024]; //上位机参数

/* 采集波形函数 */
static uint32_t pointIndex=0;
static uint16_t wavePoint[FM_SAMPLE_POINT_QTY+FM_SAMPLE_FREQUENCY];/*  波形采样点存放区,定义得稍微偏大点 */

void RecordWave_RecordSamplePoint(uint8_t indexToZero)
{
	short i;
	if(indexToZero)
    {
        pointIndex=0;
    }
    else
    {
		
		uint16_t data=0;
        uint8_t hData=0;
        uint8_t lData=0;
        data= Upper_Param[Addr_Param135];/* 这里填写采集数据值 */
        hData=(uint8_t)(data>>8);
        lData=(uint8_t)(data);
        wavePoint[pointIndex]= (lData<<8)|hData;
        pointIndex++;
    }
}

uint16_t* RecordWave_GetSamplePoint(void)
{
    return wavePoint;
    
}
//------------------------------------------------------------------

osTimerId timer0_id;

osThreadId tid_RecordWave_Task;


//----------------------------------------------------------------

void timer0_callback_function(const void* pPara)
{
    osSignalSet(tid_RecordWave_Task,2);
}
osTimerDef(timer0,timer0_callback_function);


//--------------------------------------------------------
void RecordWave_Task(const void* pPara)
{
    int step=0;
    while(1)
    {
        switch(step)
        {
            case 0:
            {
                /* 等待采集开始信叿*/
                osSignalWait(1,osWaitForever);
                /* 开启一次性定时器 */
                osTimerStart( timer0_id,10000/* 定时时间(ms) */);
                /* 跳转至next step */
                step=1;
            }
            break;
            case 1:
            {
                /* 等待采集结束信号 */
                osEvent retValue;
                retValue=osSignalWait(0,10/*ms*/);
                if(retValue.value.signals==2)
                {
                    RecordWave_RecordSamplePoint(1);
                    /* 将波形存到nand */
                    RecordWave_StoreFmlog();
                    step=0;
                }
                else
                //10ms超时没有等到结束信号的话就采集波形?
                {
                     /* 采集波形,这里添加采集波形的函敿*/
                    RecordWave_RecordSamplePoint(0);
                }
            }
            break;
            default:
            {
            
            }
            break;
        }
		
        //osDelay(5);	
    }

}
osThreadDef(RecordWave_Task,osPriorityNormal,1,0);





//--------------------------(API)-----------------------------------
/* 要开始采集波形时通过此函数发送采集开始信叿*/
void RecordWave_Start(void)
{
    osSignalSet(tid_RecordWave_Task,1);
}

//--------------------------(API)-----------------------------------
/* 系统启动后在主函数中调用此函数创建线程和定时噿*/
void RecordWave_CreateTask(void)
{
    tid_RecordWave_Task = osThreadCreate(osThread(RecordWave_Task),NULL);
    timer0_id = osTimerCreate(osTimer(timer0), osTimerOnce, NULL); 
}



//***************************************************************************************



struct fmFileType fmFile;


/* 保存存储fmlog时的一些状态信息. */
struct fmFileInfor fmLogInfor;


osSemaphoreId savefmlog_sem = NULL;
osSemaphoreDef(savefmlog_sem);

void RecordWave_StoreFmlogThread(const void* pdata)
{

    fmFile.fft_infor.brief.wnum = FM_SAMPLE_POINT_QTY;
    fmFile.fft_infor.brief.widx = FM_SAMPLE_HEAD_DURATION*FM_SAMPLE_FREQUENCY;
    fmFile.fft_infor.brief.cnum = 1;
    fmFile.fft_infor.brief.cid[0] = 0;
   
    memcpy(&fmFile.fft_infor.wave.ft_cv[0][0],RecordWave_GetSamplePoint(),FM_SAMPLE_POINT_QTY*sizeof(uint16_t));
    fmLogInfor.flag = 1;

	FM_Write();
    fmLogInfor.record_doing = 0;
    fmLogInfor.event = 0;
}

osThreadDef(RecordWave_StoreFmlogThread,osPriorityNormal,1,0);


int RecordWave_StoreFmlog(void)
{
    unsigned int tail;
    struct tm tm;
    int ms;

    if(savefmlog_sem == NULL)
    {
        savefmlog_sem = osSemaphoreCreate(osSemaphore(savefmlog_sem), 1);
    }
    osSemaphoreWait(savefmlog_sem, osWaitForever);

    /* 如果正在写入FLASH，不进行记录 */
    if(fmLogInfor.record_doing == 1)
    {
        osSemaphoreRelease(savefmlog_sem);
        record_dbg("save fault: flash is busy.\n");
        return 1;
    }

    fmLogInfor.record_doing = 1;

    fmLogInfor.flag = 0;

    tail = rtwq_tail;

    get_time(&tm, &ms);


    fmFile.fft_infor.brief.did = FS_DEV_ID;

    fmFile.fft_infor.brief.fid = 101;
    fmFile.fft_infor.brief.year = (tm.tm_year + 1900) % 100;
    fmFile.fft_infor.brief.mon = tm.tm_mon + 1;
    fmFile.fft_infor.brief.date = tm.tm_mday;
    fmFile.fft_infor.brief.hour = tm.tm_hour;
    fmFile.fft_infor.brief.min = tm.tm_min;
    fmFile.fft_infor.brief.sec = tm.tm_sec;
    fmFile.fft_infor.brief.ms = ms;

    osThreadCreate(osThread(RecordWave_StoreFmlogThread), (void *)tail);
    
    osSemaphoreRelease(savefmlog_sem);
    return 0;
}


