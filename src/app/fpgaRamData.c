#include "stdint.h"
//#include "deviceType.h"
#include "fpgaramlist.h"
#include "fpgaramdata.h"
#include "para.h"
#include "sdata_alloc.h"
#include "storedata.h"
#include "fpga.h"
//#include "universalToolFunction.h"
//#define MODULE_NAME FPGARAMDATA /* 这个宏定义必须放在#include"debug.h"之前.且名字不能随便更改 */
//#include "debug.h"

static volatile uint16_t* fpga = (uint16_t *)0x6c000000;

uint16_t Tool_SumCheck_Uint16(uint16_t* pHead,
                                        uint32_t length,
                                        uint32_t exceptIndex,
                                        uint16_t toWriteData)
{
    uint16_t ret=0;
    if(exceptIndex>=length)
    {
        //ERROR("参数错误\n");
    }
    ret = toWriteData;
    for(uint32_t index=0;index<length;index++)
    {
        if(exceptIndex != index)
        {
            ret += pHead[index];
        }
    }
    return ret;
}


/* 异或校验所有fpga数据. */

int FpgaRamData_Write(uint32_t addr, uint16_t data)
{
    uint16_t sendCheck=0,recvCheck=0;
    int retryCnt=0;
	#if 0
    if((addr>=FPGARAM_INDEX_120) && addr != FPGARAM_INDEX_200 && addr != FPGARAM_INDEX_201 && 
		addr != FPGARAM_INDEX_202 && addr != FPGARAM_INDEX_203 && addr != FPGARAM_INDEX_204 &&
		addr != FPGARAM_INDEX_205 && addr != FPGARAM_INDEX_206 && addr != FPGARAM_INDEX_207 &&
		addr != FPGARAM_INDEX_208 && addr != FPGARAM_INDEX_209 && addr != FPGARAM_INDEX_210 && 
		addr != FPGARAM_INDEX_209 && addr != FPGARAM_INDEX_210 && addr != FPGARAM_INDEX_210 &&
		addr != FPGARAM_INDEX_211 && addr != FPGARAM_INDEX_212 && addr != FPGARAM_INDEX_213 &&
		addr != FPGARAM_INDEX_214 && addr != FPGARAM_INDEX_215 && addr != FPGARAM_INDEX_216 &&
		addr != FPGARAM_INDEX_217 && addr != FPGARAM_INDEX_218 && addr != FPGARAM_INDEX_219 &&
		addr != FPGARAM_INDEX_220 && addr != FPGARAM_INDEX_221 && addr != FPGARAM_INDEX_222)
	#endif
	
    if((addr>=FPGARAM_INDEX_252))
    {
        return (-1);
    }
	
    do
    {
		if(addr<FPGARAM_INDEX_120)
		{		
			if(++retryCnt>=3)
	        {
	            return (-2);
	        }
	        fpga[addr]=(uint16_t)(data);
	        /* 最后一个地址存放校验值. */
	        sendCheck=Tool_SumCheck_Uint16(fpga,FPGARAM_INDEX_120,addr,data);
	        /* 写253地址 */
	        fpga[FPGARAM_INDEX_253]=sendCheck;
	        /* 这一步是为了满足FPGA时序需要,写的值无所谓. */
	        fpga[FPGARAM_INDEX_254]=0;
	        fpga[FPGARAM_INDEX_255]=0;
	        /* 读254地址 */
	        recvCheck=fpga[FPGARAM_INDEX_254];
	        //DEBUG(3,"sendCheck=0x%04X,FPGA[253]=0x%04X,FPGA[254]=0x%04X\n",sendCheck,fpga[253],fpga[254]);
	        if(recvCheck != sendCheck)
			{
				CountData.crc_err_count++;			
				count_cal[CRC_ERR_COUNT] = CountData.crc_err_count;
			}

		}
		else
		{			
	        fpga[addr]=(uint16_t)(data);			
			return 0;
		}
		
    }
	while(fpga[FPGARAM_INDEX_254] != sendCheck);
	
    return 0;
}


uint16_t FpgaRamData_Read(uint32_t addr)
{
    /* 校验地址不能写,但是可以读 */
    return fpga[(addr)];
}

