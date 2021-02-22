#ifndef __DEBUG_H__
#define __DEBUG_H__
#include "stdint.h"
#include "stdio.h"
#include "stdbool.h"

#define HIGHTLIGHT  "\033[1m"
#define UNDERLINE   "\033[4m"
#define TWINKLE     "\033[5m"

/* background color code. */
#define BG_BLACK     "40"
#define BG_RED       "41"
#define BG_GREEN     "42"
#define BG_YELLOW    "43"
#define BG_BLUE      "44"
#define BG_PURPLE    "45"
#define BG_DEEPGREEN "46"
#define BG_WHITE     "47"

/* front color code. */
#define FR_BLACK       "30"
#define FR_RED         "31"
#define FR_GREEN       "32"
#define FR_YELLOW      "33"
#define FR_BLUE        "34"
#define FR_PURPLE      "35"
#define FR_DEEPGREEN   "36"
#define FR_WHITE       "37"

#define ENDTAG   "\033[0m"

//#define SET_COLOR(B,F) "\033["##B##";"##F##"m"
#define SET_STRING(a,B,F,c,d)   "\n"a"\033["B";"F"m"c d"\n"
#define SET_STRING2(a,B,F,c,d)  a"\033["B";"F"m"c d
#define SET_STRING3(a,B,F,c,d)  a"\033["B";"F"m"c d"\n"

#define RETURN_AND_SHIFT_RIGHT(step) "\r\033[" #step "C\033[0m"

#define PARA_COLOR(c)  SET_STRING(HIGHTLIGHT,BG_GREEN,FR_RED,c,ENDTAG)
#define LOG_COLOR(c)   SET_STRING(HIGHTLIGHT,BG_YELLOW,FR_BLACK,c,ENDTAG)
#define WAVE_COLOR(c)  SET_STRING(HIGHTLIGHT,BG_BLUE,FR_WHITE,c,ENDTAG)
#define DEBUG_COLOR(c) SET_STRING(HIGHTLIGHT,BG_PURPLE,FR_WHITE,c,ENDTAG)
#define SEND_COLOR(c)  SET_STRING(HIGHTLIGHT,BG_BLACK,FR_WHITE,c,ENDTAG)
#define RECV_COLOR(c)  SET_STRING(HIGHTLIGHT,BG_WHITE,FR_BLACK,c,ENDTAG)
#define WARNING_COLOR(c)  SET_STRING(HIGHTLIGHT,BG_WHITE,FR_RED,c,ENDTAG)
#define WARNING2_COLOR(c)  SET_STRING2(HIGHTLIGHT,BG_WHITE,FR_RED,c,ENDTAG)

#define DEBUG1_COLOR(c) SET_STRING3(HIGHTLIGHT,BG_WHITE,FR_BLACK,c,ENDTAG)
#define DEBUG2_COLOR(c) SET_STRING3(HIGHTLIGHT,BG_BLACK,FR_WHITE,c,ENDTAG)


enum debugModeType
{
    DEBUG_MODE_0,
    DEBUG_MODE_1,
    DEBUG_MODE_2,
    DEBUG_MODE_3,
};

enum debugGradeType
{
    DEBUG_GRADE_0,/* ��ʾ�������󼶱�Ĵ�ӡ */
    DEBUG_GRADE_1,/* ��ʾ��ͨ���󼶱�Ĵ�ӡ */
    DEBUG_GRADE_2,/* ��ʾ���漶��Ĵ�ӡ */
    DEBUG_GRADE_3,/* ��ʾ��Ҫ���Լ���Ĵ�ӡ */
    DEBUG_GRADE_4,/* ��ʾ�еȵ��Լ���Ĵ�ӡ */
    DEBUG_GRADE_5,/* ��ʾһ����Լ���Ĵ�ӡ */
};

void Debug_HardInit(void);

void Debug_SysInit(void);

void Debug_Init(void);

bool Debug_GetGlobalOnOff(void);
bool Debug_GetGradeOnOff(uint32_t index,uint8_t thisGrade);

void Debug_SetGlobalDebugOnOff(uint8_t onOrOff);
void Debug_SetModuleDebugInfor(uint32_t index,uint8_t onOrOff,uint8_t mode,uint8_t grade,int (*printfunction)(char const * ,...));
void Debug_DisplayDebugInformation(int (*pPrintf)(char const * ,...));

#if 0
/* This is module debug switch. */
/* ����debug�ı��뿪��,����debug�����Ƿ�Ҫ��������̼� */
/************************************(Y)*****(N)**/
#define SAMPLE_DEBUG_ENABLE           1
#define VOLTAGEDETECTION_DEBUG_ENABLE       1
#define INSDETECT_DEBUG_ENABLE        1
#define RS485_2APP_DEBUG_ENABLE       1
#define TEMPERATURE_DEBUG_ENABLE      1
/* ÿ���һ��ģ���debug,����Ҫ���������ʹ�� */
#endif

#if 1
enum debugModule
/* ÿ���һ��ģ���debug,����Ҫ���������ʹ������ */
{
    INSDETECT_DEBUG_ENABLE_INDEX,
    SAMPLE_DEBUG_ENABLE_INDEX,
    VOLTAGEDETECTION_DEBUG_ENABLE_INDEX,
    RS485_2APP_DEBUG_ENABLE_INDEX,
    TEMPERATURE_DEBUG_ENABLE_INDEX,
    /* ��������� */
    DEBUGMODULE_QTY,
};
#endif

#define CONCATE_A_B(A,B)   A B
#define CONCATE_A_B_C(A,B,C)   A #B C

#define DEBUG_PRINT   printf//Debug_Printf
#define IF_STATEMENT(INDEX,GRADE)  if(Debug_GetGradeOnOff(INDEX,GRADE))

/* ���ڴ�ӡ��debug��ӡ���ƿ��ƵĴ�ӡ */
#define DEBUG_STATEMENT(INDEX,GRADE,...)   IF_STATEMENT(INDEX,GRADE)DEBUG_PRINT(CONCATE_A_B_C("[DEBUG_GRADE_",GRADE,"]")__VA_ARGS__)
#define DEBUG_STATEMENT_A(INDEX,GRADE,...)   IF_STATEMENT(INDEX,GRADE)DEBUG_PRINT(__VA_ARGS__)
/* ���ڴ�ӡ��ȫ��debug���ؿ��ƵĴ�ӡ */
#define GLOBAL_DEBUG          if(Debug_GetGlobalOnOff())DEBUG_PRINT

#define _MODULE_DEBUG_ENABLE(x)         x##_DEBUG_ENABLE
#define _MODULE_DEBUG_ENABLE_INDEX(x)   x##_DEBUG_ENABLE_INDEX
#define MODULE_DEBUG_ENABLE(x)          _MODULE_DEBUG_ENABLE(x)
#define MODULE_DEBUG_ENABLE_INDEX(x)    _MODULE_DEBUG_ENABLE_INDEX(x)

#if MODULE_DEBUG_ENABLE(MODULE_NAME)
#define DEBUG(GRADE,...)  DEBUG_STATEMENT(MODULE_DEBUG_ENABLE_INDEX(MODULE_NAME),GRADE,__VA_ARGS__)
#define DEBUG_A(GRADE,...)  DEBUG_STATEMENT_A(MODULE_DEBUG_ENABLE_INDEX(MODULE_NAME),GRADE,__VA_ARGS__)
#else
#define DEBUG(...)
#define DEBUG_A(GRADE,...)
#endif


/* ����������ӡ���ܴ�ӡ���ƿ��� */
/* ��������ӡ    (���һ���������Դ�ӡ)*/
#define EMERGENCY_PRINT          DEBUG_PRINT
/* ��������ӡ    (���һ��������������ʱ������ӡ)*/
#define ERROR(...)   DEBUG_PRINT("[ERROR]"__VA_ARGS__)


#endif
