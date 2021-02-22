#include "stdint.h"
#include "cmsis_os.h"
//#include "env.h"
#include "debug.h"

/*
��ģ����Ҫ������Ŀ�Ĵ�ӡ����.
(һ)�����ģ��ʱ��Ҫע��ĵط�:
1.��debug.h�ļ��ж���ģ���ӡʹ��:
    ��#define ģ����_DEBUG_ENABLE          1
2.��debug.h�ļ���enum debugModuleö�������ģ���ӡʹ������(�������붨����DEBUGMODULE_QTY֮ǰ):
    ��:ģ����_DEBUG_ENABLE_INDEX,
3.��debug.c�ļ��е�debugModuleInfor���������ģ���ӡʹ����������Ӧ��ģ���ļ���:
    ��:{ģ����_DEBUG_ENABLE_INDEX          ,"xxx.c"},
4.��ģ���ļ�xxx.c�а���debug.h�ļ�,�ұ����ڰ���֮ǰ����ģ����(���밴�����˳��):
    ��:#define MODULE_NAME ģ����
       #include "debug.h"
5.�ڸ�ģ��ʹ��1`4������֮��,��ģ����ֻ��Ҫʹ��DEBUG()����е��Դ�ӡ����.

(��)��ӡ���Ƽ��
1.������ӡ���ư�����������,����һ�����عرն��ᵼ�²���ӡ.
2.ȫ�ִ�ӡ����:debugGlobalOnOff.��ֵ�ɻ�����������.����ͨ��shell�ն˵�dbg��������ȫ�ֿ��ص�״̬
3.ģ���ӡ����:ͨ��shell�ն˵�setmoduledbg�����������.
4.ģ���ӡ����:��ĳһ��DEBUG��Ĵ�ӡ��������ģ���ӡ����ʱ,ģ���ӡ���𿪹عر�.Ҳ��ͨ��setmoduledbg��������.
*/


void Debug_HardInit(void)
{
    //�ڴ���Ŀ�о���uart1�ĳ�ʼ�����ض���.
}
//===========================================================================================


uint8_t debugGlobalOnOff=0;/* debugȫ�ֿ��� */
struct debugType
{
    //uint32_t dt_moduleIndex;
    /* ģ���ӡ���� */
    uint8_t dt_onOff;
    /* ָ��debug��ģʽ:0,1,2,3 */
    uint8_t dt_mode;
    /* ��ӡ����() */
    uint8_t dt_grade;
};

/* �����ģ���DEBUG����,ģʽ,������Ϣ. */
static struct debugType debugInfor[DEBUGMODULE_QTY];


struct debugModuleInforType
{
    uint32_t dmit_moduleIndex;
    char*    dmit_moduleName;
};

const static struct debugModuleInforType debugModuleInfor[DEBUGMODULE_QTY+1]=
/* ����ṹ���ڲ�ģ������˳����Ҫ,���������Ⱥ�����. */
/* ÿ���һ��ģ���debug,����Ҫ���������ʹ�������Լ���Ӧ��ģ���ļ��� */
/* ��δ�ڴ˽��еǼǵ�ģ���Ϊδ֪ģ��. */
{
    {SAMPLE_DEBUG_ENABLE_INDEX,           "Sample.c"},
    {VOLTAGEDETECTION_DEBUG_ENABLE_INDEX ,"VoltageDetection.c"},
    {RS485_2APP_DEBUG_ENABLE_INDEX       ,"RS485_2_App.c"},
    
    [DEBUGMODULE_QTY]={DEBUGMODULE_QTY   ,"Unknown Module"}/* ���ڴ�����δ�Ǽǵ�ģ��. */
};

uint32_t moDebug_GetModuleInforIndex(uint32_t moduleIndex)
/* �˺�����Ҫ�����Ժ��ȡģ�������. */
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
    /* ���û�ҵ��ͷ���δ�ҵ� */
    return (index);
}

void Debug_Init(void)
/* ������pnfs��env��ʼ��֮����ܵ��ô˺���. */
{
    uint8_t onoff=1;
    debugGlobalOnOff=onoff;
    /* �������������в�����GLOBALDEBUGʱ,GLOBALDEBUGĬ���ǹرյ� */

#if 1
    /* �ڵ���ģ��ʱ,������������ģ���Ĭ��debug���� */
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
/* NEW STYLE.Ŀǰ�����õķ�ʽ. */
/* ��ȡ�ּ����Ծ���֮���OnOff */
{
    
    if(index>=DEBUGMODULE_QTY)
    {
        return false;
    }
    bool onoff=false;
    bool gradeOnoff=false;
    /* ���ж�ȫ��debug���غ�ģ��debug���� */
    onoff=(debugInfor[index].dt_onOff)&&debugGlobalOnOff;
    if(true == onoff)
    {
        switch(debugInfor[index].dt_mode)
        {
            case DEBUG_MODE_0:
            /* ģʽ0:��ӡ��ȼ������Ϣ. */
            {
                gradeOnoff=(debugInfor[index].dt_grade == thisGrade);
            }
            break;
            case DEBUG_MODE_1:
            /* ģʽ1:��ӡС����ָ���������Ϣ */
            {
                gradeOnoff=(debugInfor[index].dt_grade >= thisGrade);
            }
            break;
            case DEBUG_MODE_2:
            /* ģʽ2:��ӡ�����ָ���������Ϣ */
            {
                gradeOnoff=(debugInfor[index].dt_grade <= thisGrade);
            }
            break;
            case DEBUG_MODE_3:
            /* ģʽ3:��ӡ��ָ�����������Ϣ */
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
        printfunction("ģ��:%s debugOnOff:%u,mode:%u,grade:%u.\n",debugModuleInfor[moDebug_GetModuleInforIndex(index)].dmit_moduleName,onoff,mode,grade);
    }
}

void Debug_DisplayDebugInformation(int (*pPrintf)(char const * ,...))
{
    uint32_t moduleIndex=0;
    pPrintf("\t+-------------------------------------------------------\n");
    pPrintf("\t|----��ʾϵͳ����ģ��Ĵ�ӡ����----\n");
    pPrintf("\t+-------------------------------------------------------\n");
    pPrintf("\t|Index:ģ������ֵ.\n");
    pPrintf("\t|OnOff:ģ���ӡ����.\n");
    pPrintf("\t|Mode :ģ���ӡģʽ.\n");
    pPrintf("\t|\t->Mode0:��ӡ(����)ָ���������Ϣ.\n");
    pPrintf("\t|\t->Mode1:��ӡ(С����)ָ���������Ϣ.\n");
    pPrintf("\t|\t->Mode2:��ӡ(�����)ָ���������Ϣ.\n");
    pPrintf("\t|\t->Mode3:��ӡ(��ָ��������)����Ϣ.\n");
    pPrintf("\t|Grade:ģ���ӡ����.\n");
    pPrintf("\t|\t->Grade0:�������󼶱�Ĵ�ӡ.\n");
    pPrintf("\t|\t->Grade1:��ͨ���󼶱�Ĵ�ӡ.\n");
    pPrintf("\t|\t->Grade2:���漶��Ĵ�ӡ.\n");
    pPrintf("\t|\t->Grade3:��Ҫ���Լ���Ĵ�ӡ.\n");
    pPrintf("\t|\t->Grade4:�еȵ��Լ���Ĵ�ӡ.\n");
    pPrintf("\t|\t->Grade5:��Ҫ���Լ���Ĵ�ӡ.\n");
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


