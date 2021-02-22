#ifndef __FMLOG_H__
#define __FMLOG_H__

uint8_t FM_FileInit(void);
void FM_Write(void);
uint8_t FM_ReadBriefInfor(uint16_t rd_num);
uint8_t FM_ReadFMLog(uint16_t rd_num);
uint16_t FM_GetFMLogQty(void);

#endif
