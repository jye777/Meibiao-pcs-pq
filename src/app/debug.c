#include "stdint.h"
#include "cmsis_os.h"
//#include "env.h"
#include "debug.h"

/*
此模块主要负责项目的打印机制.
(一)添加新模块时需要注意的地方:
1.在debug.h文件中定义模块打印使能:
    如#define 模块名_DEBUG_ENABLE          1
2.在debug.h文件中enum debugModule枚举中添加模块打印使能索引(索引必须定义在DEBUGMODULE_QTY之前):
    如:模块名_DEBUG_ENABLE_INDEX,
3.在debug.c文件中的debugModuleInfor数组中添加模块打印使能索引及对应的模块文件名:
    如:{模块名_DEBUG_ENABLE_INDEX          ,"xxx.c"},
4.在模块文件xxx.c中包含debug.h文件,且必须在包含之前定义模块名(必须按照这个顺序):
    如:#define MODULE_NAME 模块名
       #include "debug.h"
5.在各模块使用1`4步操作之后,在模块中只需要使用DEBUG()宏进行调试打印即可.

(二)打印机制简介
1.整个打印机制包含三个开关,任意一个开关关闭都会导致不打印.
2.全局打印开关:debugGlobalOnOff.其值由环境变量决定.可以通过shell终端的dbg命令设置全局开关的状态
3.模块打印开关:通过shell终端的setmoduledbg命令进行设置.
4.模块打印级别:当某一个DEBUG宏的打印级别不满足模块打印级别时,模块打印级别开关关闭.也是通过setmoduledbg命令设置.
*/


void Debug_HardInit(void)
{
    //在此项目中就是uart1的初始化及重定向.
}
//===========================================================================================


uint8_t debugGlobalOnOff=0;/* debug全局开关 */
struct debugType
{
    //uint32_t dt_moduleIndex;
    /* 模块打印开关 */
    uint8_t dt_onOff;
    /* 指定debug的模式:0,1,2,3 */
    uint8_t dt_mode;
    /* 打印级别() */
    uint8_t dt_grade;
};

/* 保存各模块的DEBUG开关,模式,级别信息. */
static struct debugType debugInfor[DEBUGMODULE_QTY];


struct debugModuleInforType
{
    uint32_t dmit_moduleIndex;
    char*    dmit_moduleName;
};

const static struct debugModuleInforType debugModuleInfor[DEBUGMODULE_QTY+1]=
/* 这个结构体内部模块排列顺序不重要,可以随意先后排列. */
/* 每添加一个模块的debug,都需要在这里添加使能索引以及对应的模块文件名 */
/* 尚未在此进行登记的模块称为未知模块. */
{
    {SAMPLE_DEBUG_ENABLE_INDEX,           "Sample.c"},
    {VOLTAGEDETECTION_DEBUG_ENABLE_INDEX ,"VoltageDetection.c"},
    {RS485_2APP_DEBUG_ENABLE_INDEX       ,"RS485_2_App.c"},
    
    [DEBUGMODULE_QTY]={DEBUGMODULE_QTY   ,"Unknown Module"}/* 用于代表尚未登记的模块. */
};

uint32_t moDebug_GetModuleInforIndex(uint32_t moduleIndex)
/* 此函数主要用于稍后获取模块的名字. */
{
    if(moduleIndex>=DEBUGMODULE_QTY)
    {
        return DEBUGMODULE_QTY;
    }
    
    uint32_t index=0;
    for(index=0;index<DEBUGMODULE_QTY;index++)
    {
        if(debugModuleInfor[index].dmit_moduleIndex == moduleIndex)
        {
            break;
        }
    }
    /* 如果没找到就返回未找到 */
    return (index);
}

void Debug_Init(void)
/* 必须在pnfs和env初始化之后才能调用此函数. */
{
    uint8_t onoff=1;
    debugGlobalOnOff=onoff;
    /* 倘若环境变量中不存在GLOBALDEBUG时,GLOBALDEBUG默认是关闭的 */

#if 1
    /* 在调试模块时,这里用来控制模块的默认debug开关 */
    //Debug_SetModuleDebugInfor(INSDETECT_DEBUG_ENABLE_INDEX, 1, DEBUG_MODE_2,DEBUG_GRADE_0,NULL);
    //Debug_SetModuleDebugInfor(TEMPERATURE_DEBUG_ENABLE_INDEX, 1, DEBUG_MODE_2,DEBUG_GRADE_0,NULL);
    //Debug_SetModuleDebugInfor(CLOUDUART_DEBUG_ENABLE_INDEX, 1, DEBUG_MODE_2,DEBUG_GRADE_0,NULL);
    //Debug_SetModuleDebugInfor(LOCALNET_DEBUG_ENABLE_INDEX, 1, DEBUG_MODE_2,DEBUG_GRADE_0,NULL);
    //Debug_SetModuleDebugInfor(INITW5XXX_DEBUG_ENABLE_INDEX, 1, DEBUG_MODE_2,DEBUG_GRADE_0,NULL);
    //Debug_SetModuleDebugInfor(LOGINTERFACE_DEBUG_ENABLE_INDEX, 1, DEBUG_MODE_2,DEBUG_GRADE_0,NULL);
    //Debug_SetModuleDebugInfor(FPGARAMDATA_DEBUG_ENABLE_INDEX, 1, DEBUG_MODE_2,DEBUG_GRADE_0,NULL);
    //Debug_SetModuleDebugInfor(WAVEFORMSAMPLE_DEBUG_ENABLE_INDEX, 1, DEBUG_MODE_2,DEBUG_GRADE_0,NULL);
    //Debug_SetModuleDebugInfor(SAMPLE_DEBUG_ENABLE_INDEX, 1, DEBUG_MODE_2,DEBUG_GRADE_0,NULL);
    //Debug_SetModuleDebugInfor(VOLTAGEDETECTION_DEBUG_ENABLE_INDEX, 1, DEBUG_MODE_2,DEBUG_GRADE_0,NULL);
#endif
}



bool Debug_GetGlobalOnOff(void)
{
    uint8_t onoff=0;
    onoff=debugGlobalOnOff;
    return (bool)onoff;
}


bool Debug_GetGradeOnOff(uint32_t index,uint8_t thisGrade)
/* NEW STYLE.目前正采用的方式. */
/* 获取分级策略决定之后的OnOff */
{
    
    if(index>=DEBUGMODULE_QTY)
    {
        return false;
    }
    bool onoff=false;
    bool gradeOnoff=false;
    /* 先判断全局debug开关和模块debug开关 */
    onoff=(debugInfor[index].dt_onOff)&&debugGlobalOnOff;
    if(true == onoff)
    {
        switch(debugInfor[index].dt_mode)
        {
            case DEBUG_MODE_0:
            /* 模式0:打印相等级别的信息. */
            {
                gradeOnoff=(debugInfor[index].dt_grade == thisGrade);
            }
            break;
            case DEBUG_MODE_1:
            /* 模式1:打印小等于指定级别的信息 */
            {
                gradeOnoff=(debugInfor[index].dt_grade >= thisGrade);
            }
            break;
            case DEBUG_MODE_2:
            /* 模式2:打印大等于指定级别的信息 */
            {
                gradeOnoff=(debugInfor[index].dt_grade <= thisGrade);
            }
            break;
            case DEBUG_MODE_3:
            /* 模式3:打印除指定级别外的信息 */
            {
                gradeOnoff=(debugInfor[index].dt_grade != thisGrade);
            }
            break;
        }

    }
    return (onoff&&gradeOnoff);
}


void Debug_SetGlobalDebugOnOff(uint8_t onOrOff)
{
    debugGlobalOnOff=onOrOff;
    if(debugGlobalOnOff)
    {
        //setenv(GLOBALDEBUG,"1");
    }
    else
    {
        //setenv(GLOBALDEBUG,"0");
    }
}


void Debug_SetModuleDebugInfor(uint32_t index,uint8_t onOrOff,uint8_t mode,uint8_t grade,int (*printfunction)(char const * ,...))
{
    if(index>=DEBUGMODULE_QTY)
    {
        return;
    }
    uint8_t onoff=0;
    onoff=onOrOff?(1):(0);

    debugInfor[index].dt_onOff = onoff;
    debugInfor[index].dt_mode = mode;
    debugInfor[index].dt_grade = grade;
    if(printfunction)
    {
        printfunction("模块:%s debugOnOff:%u,mode:%u,grade:%u.\n",debugModuleInfor[moDebug_GetModuleInforIndex(index)].dmit_moduleName,onoff,mode,grade);
    }
}

void Debug_DisplayDebugInformation(int (*pPrintf)(char const * ,...))
{
    uint32_t moduleIndex=0;
    pPrintf("\t+-------------------------------------------------------\n");
    pPrintf("\t|----显示系统所有模块的打印配置----\n");
    pPrintf("\t+-------------------------------------------------------\n");
    pPrintf("\t|Index:模块索引值.\n");
    pPrintf("\t|OnOff:模块打印开关.\n");
    pPrintf("\t|Mode :模块打印模式.\n");
    pPrintf("\t|\t->Mode0:打印(等于)指定级别的信息.\n");
    pPrintf("\t|\t->Mode1:打印(小等于)指定级别的信息.\n");
    pPrintf("\t|\t->Mode2:打印(大等于)指定级别的信息.\n");
    pPrintf("\t|\t->Mode3:打印(除指定级别外)的信息.\n");
    pPrintf("\t|Grade:模块打印级别.\n");
    pPrintf("\t|\t->Grade0:致命错误级别的打印.\n");
    pPrintf("\t|\t->Grade1:普通错误级别的打印.\n");
    pPrintf("\t|\t->Grade2:警告级别的打印.\n");
    pPrintf("\t|\t->Grade3:重要调试级别的打印.\n");
    pPrintf("\t|\t->Grade4:中等调试级别的打印.\n");
    pPrintf("\t|\t->Grade5:次要调试级别的打印.\n");
    pPrintf("\t+-------------------------------------------------------\n");

    pPrintf("\tIndex\tOnOff\tMode\tGrade\tModuleName\n");
    for(moduleIndex=0;moduleIndex<DEBUGMODULE_QTY;moduleIndex++)
    {
        if(moduleIndex%2)
        {
            pPrintf("\t" DEBUG2_COLOR("%u  \t%u \t%u  \t%u \t%s"),moduleIndex,debugInfor[moduleIndex].dt_onOff,
                                                    debugInfor[moduleIndex].dt_mode,
                                                    debugInfor[moduleIndex].dt_grade,
                                                    debugModuleInfor[moDebug_GetModuleInforIndex(moduleIndex)].dmit_moduleName);
        }
        else
        {
            pPrintf("\t" DEBUG1_COLOR("%u  \t%u \t%u  \t%u \t%s"),moduleIndex,debugInfor[moduleIndex].dt_onOff,
                                                    debugInfor[moduleIndex].dt_mode,
                                                    debugInfor[moduleIndex].dt_grade,
                                                    debugModuleInfor[moDebug_GetModuleInforIndex(moduleIndex)].dmit_moduleName);
        }
    }
}


