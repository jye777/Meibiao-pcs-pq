#include <stdio.h>
#include <string.h>
#include "cmsis_os.h"
#include "log.h"
#include "fpga.h"
#include "record_wave.h"
#include "para.h"
#include "env.h"
#include "storedata.h"


#define BIGENDIAN(x)	((((((unsigned short)x)&0xff)<<8)|(((unsigned short)x)>>8)))
#define MPR_PARANUM     (sizeof(para_inf) / sizeof(para_information))


extern volatile float *nvrom_pw_info;
extern volatile unsigned int *nvrom_pw_time_info;
static osSemaphoreId power_sem;
static osSemaphoreDef(power_sem);
out_state_info_t out_state_info; //����״̬
_power_data power_data;
_CountData CountData;
_sys_time Arm_time;
PQNonSVG_info_t PQNonSVG; //P/Q��SVG���û���: [0]-P [1]-Q, �����ʵ��ֵ*MODULUS
short Upper_Param[1024]; //��λ������


para_information para_inf[] =
{
    {"����ĸ�ߵ�ѹ��ѹ�ο�",			Addr_Param0},
    {"����ĸ�ߵ�ѹǷѹ�ο�",			Addr_Param1},
    {"�����������޲ο�",				Addr_Param2},
    {"ֱ����ѹ���޲ο�",				Addr_Param3},
    {"ֱ��Ƿѹ���޲ο�",			Addr_Param4},
    {"ֱ���������޲ο�",			Addr_Param5},
    {"��ع�ѹ���޲ο�",			Addr_Param6},
    {"���Ƿѹ���޲ο�",			Addr_Param7},
    {"ĸ��ѹ���ѹ����",			Addr_Param8},
    {"����˲ʱ��ѹ����",			Addr_Param9},
    {"����˲ʱ��������",			Addr_Param10},
    {"ֱ��˲ʱ��ѹ����",			Addr_Param11},
    {"ֱ��˲ʱ��������",			Addr_Param12},
    {"���˲ʱ��ѹ����",			Addr_Param13},
    {"��ػ�������",		Addr_Param14},
    {"Ԥ�����ɵ�ѹ��ֵ����",		Addr_Param15},
    {"��Ƶ��ֵ",		Addr_Param16},
    {"ǷƵ��ֵ",				Addr_Param17},
    {"���ڹ��ı���ֵ",				Addr_Param18},
    {"��ģ����������ֵ",			Addr_Param19},
    {"����",			Addr_Param20},
    {"����",				Addr_Param21},
    {"����",		Addr_Param22},
    {"ֱ����ѹ����",		Addr_Param23},
    {"������ѹ��ֵ����",		Addr_Param24},
    {"������ѹ�����ֵ����",		Addr_Param25},
    {"�й����ʸ���",			Addr_Param26},
    {"�޹����ʸ���",			Addr_Param27},
    {"������������",			Addr_Param28},
    {"�����ŵ�����ֵ",				Addr_Param29},
    {"�����������ֵ",		Addr_Param30},
    {"ֱ����繦������ֵ",		Addr_Param31},
    {"ֱ���ŵ繦������ֵ",		Addr_Param32},
    {"ֱ������������ֵ",		Addr_Param33},
    {"ֱ���ŵ��������ֵ",			Addr_Param34},
    {"������ֱ���������",			Addr_Param35},
    {"г�������������",			Addr_Param36},
    {"г����ȡ�˲�Ƶ��",				Addr_Param37},
    {"����",		Addr_Param38},
    {"����",		Addr_Param39},
    {"����",		Addr_Param40},
    {"ֱ�����ʵ���������ת��ϵ��",		Addr_Param41},
    {"��ѹǰ��ϵ��",			Addr_Param42},
    {"�翹����",			Addr_Param43},
    {"�����޹�����ϵ��",			Addr_Param44},
    {"Ƶ�ʵ���ϵ��",				Addr_Param45},
    {"�������",		Addr_Param46},
    {"�������Kp",		Addr_Param47},
    {"�������Ki",		Addr_Param48},
    {"�������Kp",		Addr_Param49},
    {"�������Ki",			Addr_Param50},
    
    {"��ص�ѹ�����Ƶ��",			Addr_Param51},
    {"��ص��������Ƶ��",			Addr_Param52},
    {"���������Ƶ��",				Addr_Param53},
    {"ĸ�ߵ�ѹ�����Ƶ��",			Addr_Param54},
    {"��ص�ѹ2ĸ�ߵ�ѹת��ϵ��",	Addr_Param55},
    {"ĸ�߿��ƻ�Kp",				Addr_Param56},
    {"ĸ�߿��ƻ�Ki",				Addr_Param57},
    {"����⻷Kp",					Addr_Param58},
    {"����⻷Ki",					Addr_Param59},
    {"�ڻ�Kp",						Addr_Param60},
    {"�ڻ�Ki",						Addr_Param61},
    {"�ز�Ƶ��",					Addr_Param62},
    {"DC1֧·�ز�����",				Addr_Param63},
    {"DC2֧·�ز�����",				Addr_Param64},
    {"DC3֧·�ز�����",				Addr_Param65},
    {"DC4֧·�ز�����",				Addr_Param66},
    {"����ʱ��",					Addr_Param67},
    {"խ�������ʱ��",				Addr_Param68},
    {"IGBT���±�������",			Addr_Param69},
    {"����",						Addr_Param70},
    {"����",						Addr_Param71},
    {"����",						Addr_Param72},
    {"����",						Addr_Param73},
    {"����",						Addr_Param74},
    {"����",						Addr_Param75},
    {"����",						Addr_Param76},
    {"����",						Addr_Param77},
    {"����",						Addr_Param78},
    {"����",						Addr_Param79},
    {"����",						Addr_Param80},
    {"����",						Addr_Param81},
    {"����",						Addr_Param82},
    {"����",						Addr_Param83},
    {"����",						Addr_Param84},
    {"����",						Addr_Param85},
    {"����",						Addr_Param86},
    {"����",						Addr_Param87},
    {"����",						Addr_Param88},
    {"����",						Addr_Param89},
    {"����",						Addr_Param90},
    {"DC1֧·IGBT�¶�(to FPGA)",	Addr_Param91},
    {"DC2֧·IGBT�¶�(to FPGA)",	Addr_Param92},
    {"DC3֧·IGBT�¶�(to FPGA)",	Addr_Param93},
    {"DC4֧·IGBT�¶�(to FPGA)",	Addr_Param94},
    {"����",						Addr_Param95},
    {"����",						Addr_Param96},
    {"����",						Addr_Param97},
    {"����",						Addr_Param98},
    {"����",						Addr_Param99},
    {"���β���Ƶ������",			Addr_Param100},
    {"����ͨ��0����ѡ��",			Addr_Param101},
    {"����ͨ��1����ѡ��",			Addr_Param102},
    {"����ͨ��2����ѡ��",			Addr_Param103},
    {"����ͨ��3����ѡ��",			Addr_Param104},
    {"����ͨ��4����ѡ��",			Addr_Param105},
    {"����ͨ��5����ѡ��",			Addr_Param106},
    {"����ͨ��6����ѡ��",			Addr_Param107},
    {"����ͨ��7����ѡ��",			Addr_Param108},
    {"����ͨ��8����ѡ��",			Addr_Param109},
    {"CMD0",						Addr_Param110},
    {"CMD1",						Addr_Param111},
    {"CMD2",						Addr_Param112},
    {"CMD3",						Addr_Param113},
    {"CMD4",						Addr_Param114},
    {"CMD5",						Addr_Param115},
    {"CMD6",						Addr_Param116},
    {"CMD7",						Addr_Param117},
    {"CMD8",						Addr_Param118},
    {"CMD9",						Addr_Param119},
    {"ĸ�ߵ�ѹ",					Addr_Param120},
    {"DC1֧·����",					Addr_Param121},
    {"DC1֧·����",					Addr_Param122},
    {"DC1ֱ����ص�ѹ",				Addr_Param123},
    {"DC2֧·����",					Addr_Param124},
    {"DC2֧·����",					Addr_Param125},
    {"DC2ֱ����ص�ѹ",				Addr_Param126},
    {"DC3֧·����",					Addr_Param127},
    {"DC3֧·����",					Addr_Param128},
    {"DC3ֱ����ص�ѹ",				Addr_Param129},
    {"DC4֧·����",					Addr_Param130},
    {"DC4֧·����",					Addr_Param131},
    {"DC4ֱ����ص�ѹ",				Addr_Param132},
    {"ĸ�ߵ�ѹУ��ƽ��ֵ",			Addr_Param133},
    {"DC1֧·��ص�ѹУ��ƽ��ֵ",	Addr_Param134},
    {"DC2֧·��ص�ѹУ��ƽ��ֵ",	Addr_Param135},
    {"DC3֧·��ص�ѹУ��ƽ��ֵ",	Addr_Param136},
    {"DC4֧·��ص�ѹУ��ƽ��ֵ",	Addr_Param137},
    {"DC1֧·��ص���У��ƽ��ֵ",	Addr_Param138},
    {"DC2֧·��ص���У��ƽ��ֵ",	Addr_Param139},
    {"DC3֧·��ص���У��ƽ��ֵ",	Addr_Param140},
    {"DC4֧·��ص���У��ƽ��ֵ",	Addr_Param141},
    {"DC1֧·IGBT�¶�",				Addr_Param142},
    {"DC2֧·IGBT�¶�",				Addr_Param143},
    {"DC3֧·IGBT�¶�",				Addr_Param144},
    {"DC4֧·IGBT�¶�",				Addr_Param145},
    {"����",						Addr_Param146},
    {"����",						Addr_Param147},
    {"����",						Addr_Param148},
    {"����",						Addr_Param149},
    {"ϵͳ״̬�Ĵ���0",				Addr_Param150},
    {"ϵͳ״̬�Ĵ���1",				Addr_Param151},
    {"ϵͳ״̬�Ĵ���2",				Addr_Param152},
    {"ϵͳ״̬�Ĵ���3",				Addr_Param153},
    {"ϵͳ״̬�Ĵ���4",				Addr_Param154},
    {"ϵͳ״̬�Ĵ���5",				Addr_Param155},
    {"ϵͳ״̬�Ĵ���6",				Addr_Param156},
    {"ϵͳ״̬�Ĵ���7",				Addr_Param157},
    {"ϵͳ״̬�Ĵ���8",				Addr_Param158},
    {"ϵͳ״̬�Ĵ���9",				Addr_Param159},
    {"ϵͳ״̬�Ĵ���10",			Addr_Param160},
    {"ϵͳ״̬�Ĵ���11",			Addr_Param161},
    {"ϵͳ״̬�Ĵ���12",			Addr_Param162},
    {"ϵͳ״̬�Ĵ���13",			Addr_Param163},
    {"ϵͳ״̬�Ĵ���14",			Addr_Param164},
    {"ϵͳ״̬�Ĵ���15",			Addr_Param165},
    {"ϵͳ��������Ĵ���0",			Addr_Param166},
    {"ϵͳ��������Ĵ���1",			Addr_Param167},
    {"ϵͳ��������Ĵ���2",			Addr_Param168},
    {"ϵͳ��������Ĵ���3",			Addr_Param169},
    {"ϵͳ��������Ĵ���4",			Addr_Param170},
    {"ϵͳ��������Ĵ���5",			Addr_Param171},
    {"ϵͳ��������Ĵ���6",			Addr_Param172},
    {"ϵͳ��������Ĵ���7",			Addr_Param173},
    {"ϵͳ��������Ĵ���8",			Addr_Param174},
    {"ϵͳ��������Ĵ���9",			Addr_Param175},
    {"ϵͳ��������Ĵ���10",		Addr_Param176},
    {"ϵͳ��������Ĵ���11",		Addr_Param177},
    {"ϵͳ��������Ĵ���12",		Addr_Param178},
    {"ϵͳ��������Ĵ���13",		Addr_Param179},
    {"ϵͳ��������Ĵ���14",		Addr_Param180},
    {"ϵͳ��������Ĵ���15",		Addr_Param181},
    {"����",						Addr_Param182},
    {"����",						Addr_Param183},
    {"����",						Addr_Param184},
    {"����",						Addr_Param185},
    {"����",						Addr_Param186},
    {"����",						Addr_Param187},
    {"����",						Addr_Param188},
    {"����",						Addr_Param189},
    {"����",						Addr_Param190},
    {"����",						Addr_Param191},
    {"����",						Addr_Param192},
    {"����",						Addr_Param193},
    {"����",						Addr_Param194},
    {"����",						Addr_Param195},
    {"����",						Addr_Param196},
    {"����",						Addr_Param197},
    {"����",						Addr_Param198},
    {"FPGA_Version",				Addr_Param199},
    {"ĸ�ߵ�ѹУ��ֵ",				Addr_Param200},
    {"DC1֧·��ص�ѹУ��ֵ",		Addr_Param201},
    {"DC2֧·��ص�ѹУ��ֵ",		Addr_Param202},
    {"DC3֧·��ص�ѹУ��ֵ",		Addr_Param203},
    {"DC4֧·��ص�ѹУ��ֵ",		Addr_Param204},
    {"DC1֧·��ص���У��ֵ",		Addr_Param205},
    {"DC2֧·��ص���У��ֵ",		Addr_Param206},
    {"DC3֧·��ص���У��ֵ",		Addr_Param207},
    {"DC4֧·��ص���У��ֵ",		Addr_Param208},
    {"����",						Addr_Param209},
    {"����",						Addr_Param210},
    {"����",						Addr_Param211},
    {"����",						Addr_Param212},
    {"����",						Addr_Param213},
    {"����",						Addr_Param214},
    {"����",						Addr_Param215},
    {"����",						Addr_Param216},
    {"����",						Addr_Param217},
    {"����",						Addr_Param218},
    {"����",						Addr_Param219},
    {"����",						Addr_Param220},
    {"DC1֧·IGBT�¶�(to FPGA)",	Addr_Param221},
    {"DC2֧·IGBT�¶�(to FPGA)",	Addr_Param222},
    {"DC3֧·IGBT�¶�(to FPGA)",	Addr_Param223},
    {"DC4֧·IGBT�¶�(to FPGA)",	Addr_Param224},
    {"����",						Addr_Param225},
    {"����",						Addr_Param226},
    {"����",						Addr_Param227},
    {"����",						Addr_Param228},
    {"����",						Addr_Param229},
    {"����",						Addr_Param230},
    {"����",						Addr_Param231},
    {"����",						Addr_Param232},
    {"����",						Addr_Param233},
    {"����",						Addr_Param234},
    {"����",						Addr_Param235},
    {"����",						Addr_Param236},
    {"����",						Addr_Param237},
    {"����",						Addr_Param238},
    {"����",						Addr_Param239},
    {"����",						Addr_Param240},
    {"����",						Addr_Param241},
    {"����",						Addr_Param242},
    {"����",						Addr_Param243},
    {"����",						Addr_Param244},
    {"����",						Addr_Param245},
    {"����",						Addr_Param246},
    {"����ͨ��0����",				Addr_Param247},
    {"����ͨ��1����",				Addr_Param248},
    {"����ͨ��2����",				Addr_Param249},
    {"����ͨ��3����",				Addr_Param250},
    {"����ͨ��4����",				Addr_Param251},
    {"����ͨ��5����",				Addr_Param252},
    {"����ͨ��6����",				Addr_Param253},
    {"����ͨ��7����",				Addr_Param254},
    {"����ͨ��8����",				Addr_Param255},
    {"���β���Ƶ������",			Addr_Param256},
    {"����ͨ��0����ѡ��",			Addr_Param257},
    {"����ͨ��1����ѡ��",			Addr_Param258},
    {"����ͨ��2����ѡ��",			Addr_Param259},
    {"����ͨ��3����ѡ��",			Addr_Param260},
    {"����ͨ��4����ѡ��",			Addr_Param261},
    {"����ͨ��5����ѡ��",			Addr_Param262},
    {"����ͨ��6����ѡ��",			Addr_Param263},
    {"����ͨ��7����ѡ��",			Addr_Param264},
    {"����ͨ��8����ѡ��",			Addr_Param265},
    {"CMD0",						Addr_Param266},
    {"CMD1",						Addr_Param267},
    {"CMD2",						Addr_Param268},
    {"CMD3",						Addr_Param269},
    {"CMD4",						Addr_Param270},
    {"CMD5",						Addr_Param271},
    {"CMD6",						Addr_Param272},
    {"CMD7",						Addr_Param273},
    {"CMD8",						Addr_Param274},
    {"CMD9",						Addr_Param275},
    {"ĸ�ߵ�ѹ",					Addr_Param276},
    {"DC1֧·����",					Addr_Param277},
    {"DC1֧·����",					Addr_Param278},
    {"DC1ֱ����ص�ѹ",				Addr_Param279},
    {"DC2֧·����",					Addr_Param280},
    {"DC2֧·����",					Addr_Param281},
    {"DC2ֱ����ص�ѹ",				Addr_Param282},
    {"DC3֧·����",					Addr_Param283},
    {"DC3֧·����",					Addr_Param284},
    {"DC3ֱ����ص�ѹ",				Addr_Param285},
    {"DC4֧·����",					Addr_Param286},
    {"DC4֧·����",					Addr_Param287},
    {"DC4ֱ����ص�ѹ",				Addr_Param288},
    {"ĸ�ߵ�ѹУ��ƽ��ֵ",			Addr_Param289},
    {"DC1֧·��ص�ѹУ��ƽ��ֵ",	Addr_Param290},
    {"DC2֧·��ص�ѹУ��ƽ��ֵ",	Addr_Param291},
    {"DC3֧·��ص�ѹУ��ƽ��ֵ",	Addr_Param292},
    {"DC4֧·��ص�ѹУ��ƽ��ֵ",	Addr_Param293},
    {"DC1֧·��ص���У��ƽ��ֵ",	Addr_Param294},
    {"DC2֧·��ص���У��ƽ��ֵ",	Addr_Param295},
    {"DC3֧·��ص���У��ƽ��ֵ",	Addr_Param296},
    {"DC4֧·��ص���У��ƽ��ֵ",	Addr_Param297},
    {"DC1֧·IGBT�¶�",				Addr_Param298},
    {"DC2֧·IGBT�¶�",				Addr_Param299},
    {"DC3֧·IGBT�¶�",				Addr_Param300},

    {"DC1֧·��ص�ѹУ��ֵ",		Addr_Param301},
    {"DC2֧·��ص�ѹУ��ֵ",		Addr_Param302},
    {"DC3֧·��ص�ѹУ��ֵ",		Addr_Param303},
    {"DC4֧·��ص�ѹУ��ֵ",		Addr_Param304},
    {"DC1֧·��ص���У��ֵ",		Addr_Param305},
    {"DC2֧·��ص���У��ֵ",		Addr_Param306},
    {"DC3֧·��ص���У��ֵ",		Addr_Param307},
    {"DC4֧·��ص���У��ֵ",		Addr_Param308},
    {"����",						Addr_Param309},
    {"����",						Addr_Param310},
    {"����",						Addr_Param311},
    {"����",						Addr_Param312},
    {"����",						Addr_Param313},
    {"����",						Addr_Param314},
    {"����",						Addr_Param315},
    {"����",						Addr_Param316},
    {"����",						Addr_Param317},
    {"����",						Addr_Param318},
    {"����",						Addr_Param319},
    {"����",						Addr_Param320},
    {"DC1֧·IGBT�¶�(to FPGA)",	Addr_Param321},
    {"DC2֧·IGBT�¶�(to FPGA)",	Addr_Param322},
    {"DC3֧·IGBT�¶�(to FPGA)",	Addr_Param323},
    {"DC4֧·IGBT�¶�(to FPGA)",	Addr_Param324},
    {"����",						Addr_Param325},
    {"����",						Addr_Param326},
    {"����",						Addr_Param327},
    {"����",						Addr_Param328},
    {"����",						Addr_Param329},
    {"����",						Addr_Param330},
    {"����",						Addr_Param331},
    {"����",						Addr_Param332},
    {"����",						Addr_Param333},
    {"����",						Addr_Param334},
    {"����",						Addr_Param335},
    {"����",						Addr_Param336},
    {"����",						Addr_Param337},
    {"����",						Addr_Param338},
    {"����",						Addr_Param339},
    {"����",						Addr_Param340},
    {"����",						Addr_Param341},
    {"����",						Addr_Param342},
    {"����",						Addr_Param343},
    {"����",						Addr_Param344},
    {"����",						Addr_Param345},	
    {"����",						Addr_Param346},
    {"����",						Addr_Param347},
    {"����",						Addr_Param348},
    {"����",						Addr_Param349},
    {"ϵͳ״̬�Ĵ���0",				Addr_Param350},
    {"ϵͳ״̬�Ĵ���1",				Addr_Param351},
    {"ϵͳ״̬�Ĵ���2",				Addr_Param352},
    {"ϵͳ״̬�Ĵ���3",				Addr_Param353},
    {"ϵͳ״̬�Ĵ���4",				Addr_Param354},
    {"ϵͳ״̬�Ĵ���5",				Addr_Param355},
    {"ϵͳ״̬�Ĵ���6",				Addr_Param356},
    {"ϵͳ״̬�Ĵ���7",				Addr_Param357},
    {"ϵͳ״̬�Ĵ���8",				Addr_Param358},
    {"ϵͳ״̬�Ĵ���9",				Addr_Param359},
    {"ϵͳ״̬�Ĵ���10",			Addr_Param360},
    {"ϵͳ״̬�Ĵ���11",			Addr_Param361},
    {"ϵͳ״̬�Ĵ���12",			Addr_Param362},
    {"ϵͳ״̬�Ĵ���13",			Addr_Param363},
    {"ϵͳ״̬�Ĵ���14",			Addr_Param364},
    {"ϵͳ״̬�Ĵ���15",			Addr_Param365},
    {"ϵͳ��������Ĵ���0",			Addr_Param366},
    {"ϵͳ��������Ĵ���1",			Addr_Param367},
    {"ϵͳ��������Ĵ���2",			Addr_Param368},
    {"ϵͳ��������Ĵ���3",			Addr_Param369},
    {"ϵͳ��������Ĵ���4",			Addr_Param370},
    {"ϵͳ��������Ĵ���5",			Addr_Param371},
    {"ϵͳ��������Ĵ���6",			Addr_Param372},
    {"ϵͳ��������Ĵ���7",			Addr_Param373},
    {"ϵͳ��������Ĵ���8",			Addr_Param374},
    {"ϵͳ��������Ĵ���9",			Addr_Param375},
    {"ϵͳ��������Ĵ���10",		Addr_Param376},
    {"ϵͳ��������Ĵ���11",		Addr_Param377},
    {"ϵͳ��������Ĵ���12",		Addr_Param378},
    {"ϵͳ��������Ĵ���13",		Addr_Param379},
    {"ϵͳ��������Ĵ���14",		Addr_Param380},
    {"ϵͳ��������Ĵ���15",		Addr_Param381},
    {"����",						Addr_Param382},
    {"����",						Addr_Param383},
    {"����",						Addr_Param384},
    {"����",						Addr_Param385},
    {"����",						Addr_Param386},
    {"����",						Addr_Param387},
    {"����",						Addr_Param388},
    {"����",						Addr_Param389},
    {"����",						Addr_Param390},
    {"����",						Addr_Param391},
    {"����",						Addr_Param392},
    {"����",						Addr_Param393},
    {"����",						Addr_Param394},
    {"����",						Addr_Param395},
    {"����",						Addr_Param396},
    {"����",						Addr_Param397},
    {"����",						Addr_Param398},
    {"FPGA_Version",				Addr_Param399},
    {"ĸ�ߵ�ѹУ��ֵ",				Addr_Param400},
    {"DC1֧·��ص�ѹУ��ֵ",		Addr_Param401},
    {"DC2֧·��ص�ѹУ��ֵ",		Addr_Param402},
    {"DC3֧·��ص�ѹУ��ֵ",		Addr_Param403},
    {"DC4֧·��ص�ѹУ��ֵ",		Addr_Param404},
    {"DC1֧·��ص���У��ֵ",		Addr_Param405},
    {"DC2֧·��ص���У��ֵ",		Addr_Param406},
    {"DC3֧·��ص���У��ֵ",		Addr_Param407},
    {"DC4֧·��ص���У��ֵ",		Addr_Param408},
    {"����",						Addr_Param409},
    {"����",						Addr_Param410},
    {"����",						Addr_Param411},
    {"����",						Addr_Param412},
    {"����",						Addr_Param413},
    {"����",						Addr_Param414},
    {"����",						Addr_Param415},
    {"����",						Addr_Param416},
    {"����",						Addr_Param417},
    {"����",						Addr_Param418},
    {"����",						Addr_Param419},
    {"����",						Addr_Param420},
    {"DC1֧·IGBT�¶�(to FPGA)",	Addr_Param421},
    {"DC2֧·IGBT�¶�(to FPGA)",	Addr_Param422},
    {"DC3֧·IGBT�¶�(to FPGA)",	Addr_Param423},
    {"DC4֧·IGBT�¶�(to FPGA)",	Addr_Param424},
    {"����",						Addr_Param425},
    {"����",						Addr_Param426},
    {"����",						Addr_Param427},
    {"����",						Addr_Param428},
    {"����",						Addr_Param429},
    {"����",						Addr_Param430},
    {"����",						Addr_Param431},
    {"����",						Addr_Param432},
    {"����",						Addr_Param433},
    {"����",						Addr_Param434},
    {"����",						Addr_Param435},
    {"����",						Addr_Param436},
    {"����",						Addr_Param437},
    {"����",						Addr_Param438},
    {"����",						Addr_Param439},
    {"����",						Addr_Param440},
    {"����",						Addr_Param441},
    {"����",						Addr_Param442},
    {"����",						Addr_Param443},
    {"����",						Addr_Param444},
    {"����",						Addr_Param445},
    {"����",						Addr_Param446},
    {"����ͨ��0����",				Addr_Param447},
    {"����ͨ��1����",				Addr_Param448},
    {"����ͨ��2����",				Addr_Param449},
    {"����ͨ��3����",				Addr_Param450},
    {"����ͨ��4����",				Addr_Param451},
    {"����ͨ��5����",				Addr_Param452},
    {"����ͨ��6����",				Addr_Param453},
    {"����ͨ��7����",				Addr_Param454},
    {"����ͨ��8����",				Addr_Param455},
    {"ϵͳ״̬�Ĵ���6",				Addr_Param456},
    {"ϵͳ״̬�Ĵ���7",				Addr_Param457},
    {"ϵͳ״̬�Ĵ���8",				Addr_Param458},
    {"ϵͳ״̬�Ĵ���9",				Addr_Param459},
    {"ϵͳ״̬�Ĵ���10",			Addr_Param460},
    {"ϵͳ״̬�Ĵ���11",			Addr_Param461},
    {"ϵͳ״̬�Ĵ���12",			Addr_Param462},
    {"ϵͳ״̬�Ĵ���13",			Addr_Param463},
    {"ϵͳ״̬�Ĵ���14",			Addr_Param464},
    {"ϵͳ״̬�Ĵ���15",			Addr_Param465},
    {"ϵͳ��������Ĵ���0",			Addr_Param466},
    {"ϵͳ��������Ĵ���1",			Addr_Param467},
    {"ϵͳ��������Ĵ���2",			Addr_Param468},
    {"ϵͳ��������Ĵ���3",			Addr_Param469},
    {"ϵͳ��������Ĵ���4",			Addr_Param470},
    {"ϵͳ��������Ĵ���5",			Addr_Param471},
    {"ϵͳ��������Ĵ���6",			Addr_Param472},
    {"ϵͳ��������Ĵ���7",			Addr_Param473},
    {"ϵͳ��������Ĵ���8",			Addr_Param474},
    {"ϵͳ��������Ĵ���9",			Addr_Param475},
    {"ϵͳ��������Ĵ���10",		Addr_Param476},
    {"ϵͳ��������Ĵ���11",		Addr_Param477},
    {"ϵͳ��������Ĵ���12",		Addr_Param478},
    {"ϵͳ��������Ĵ���13",		Addr_Param479},
    {"ϵͳ��������Ĵ���14",		Addr_Param480},
    {"ϵͳ��������Ĵ���15",		Addr_Param481},
    {"����",						Addr_Param482},
    {"����",						Addr_Param483},
    {"����",						Addr_Param484},
    {"����",						Addr_Param485},
    {"����",						Addr_Param486},
    {"����",						Addr_Param487},
    {"����",						Addr_Param488},
    {"����",						Addr_Param489},
    {"����",						Addr_Param490},
    {"����",						Addr_Param491},
    {"����",						Addr_Param492},
    {"����",						Addr_Param493},
    {"����",						Addr_Param494},
    {"����",						Addr_Param495},
    {"����",						Addr_Param496},
    {"����",						Addr_Param497},
    {"����",						Addr_Param498},
    {"FPGA_Version",				Addr_Param499},
    {"ĸ�ߵ�ѹУ��ֵ",				Addr_Param500},
    {"DC1֧·��ص�ѹУ��ֵ",		Addr_Param501},
    {"DC2֧·��ص�ѹУ��ֵ",		Addr_Param502},
    {"DC3֧·��ص�ѹУ��ֵ",		Addr_Param503},
    {"DC4֧·��ص�ѹУ��ֵ",		Addr_Param504},
    {"DC1֧·��ص���У��ֵ",		Addr_Param505},
    {"DC2֧·��ص���У��ֵ",		Addr_Param506},
    {"DC3֧·��ص���У��ֵ",		Addr_Param507},
    {"DC4֧·��ص���У��ֵ",		Addr_Param508},
    {"����",						Addr_Param509},
    {"����",						Addr_Param510},
    {"����",						Addr_Param511},
    
	{"�豸�������",				MB_DEVICE_CODE_YEAR},	
	{"�豸��������",				MB_DEVICE_CODE_MD},
	{"�豸�ͺ�",					MB_DEVICE_CODE_TYPE},
	{"�豸�������",				MB_DEVICE_CODE_NUM},
	{"������Կ",					MB_DEVICE_WRITE_CODE},
	{"���������޸����",			MB_LECENSE_KEY},

    
    {"���������ѹ���ϵ��H",		FD_UINX_ADD_H},
    {"���������ѹ���ϵ��L",		FD_UINX_ADD_L},
    {"��������������ϵ��H",		FD_IINX_ADD_H},
    {"��������������ϵ��L",		FD_IINX_ADD_L},
    {"ֱ����ѹ���ϵ��H",			FD_UDC_ADD_H},
    {"ֱ����ѹ���ϵ��L",			FD_UDC_ADD_L},
    {"ֱ���������ϵ��H",			FD_IDC_ADD_H},
    {"ֱ���������ϵ��L",			FD_IDC_ADD_L},
    {"��ص�ѹ���ϵ��H",			FD_UBTR_ADD_H},
    {"��ص�ѹ���ϵ��L",			FD_UBTR_ADD_L},
    {"�����Եص�ѹ���ϵ��H",		FD_UPOS_GND_ADD_H},
    {"�����Եص�ѹ���ϵ��L",		FD_UPOS_GND_ADD_L},
    {"�����Եص�ѹ���ϵ��H",		FD_UNEG_GND_ADD_H},
    {"�����Եص�ѹ���ϵ��L",		FD_UNEG_GND_ADD_L},
    {"�������ʱ��ϵ��H",			FD_PAC_ADD_H},
    {"�������ʱ��ϵ��L",			FD_PAC_ADD_L},
    {"ֱ�����ʱ��ϵ��H",			FD_PDC_ADD_H},
    {"ֱ�����ʱ��ϵ��L",			FD_PDC_ADD_L},
    {"���е������ϵ��H",			FD_IAC_CT_ADD_H},
    {"���е������ϵ��L",			FD_IAC_CT_ADD_L},
    {"���й��ʱ��ϵ��H",			FD_PAC_CT_ADD_H},
    {"���й��ʱ��ϵ��L",			FD_PAC_CT_ADD_L},

    {"©�������ϵ��H",				FD_IAC_LKG_H},
    {"©�������ϵ��L",				FD_IAC_LKG_L},	
    
    {"����",						RESET_FLAG},
    {"����",						DEBUG_ONOFF},
	{"����ģʽ����",				Ctrl_Mode_SET}, 
	{"����ģʽѡ��",				Run_Mode_Set}, 
	{"ͨѶ��������ʹ��",			Communication_En}, 
	{"����ѡ��",					Strategy_En}, 	  
    {"����",						MP_INI_MONITOR_ADDR},
   
    {"�����ѹֵ",					MP_CHRG_VOL_LIMIT},
    {"�ŵ���ѹֵ",					MP_DISC_VOL_LIMIT},
    {"SVGʹ��",						MP_SVG_EN},
    {"�µ�����",					MP_ISO_ISLAND},

	
    {"��Ե���ϵ��",				MP_INI_RESIS_FD},
    {"��Ե�ж�����",				MP_INI_RESIS_REF},
    {"��ػ���ַ",					MP_INI_MONITOR_ADDR},
    {"����й�����",				MP_MAX_P_ADD},
    {"��С�й�����",				MP_MIN_P_ADD},
    {"���ֱ������",				MP_MAX_BA_C_ADD},
    {"��Сֱ������",				MP_MIN_BA_C_ADD},
    {"���ֱ����ѹ",				MP_MAX_BA_U_ADD},
    {"��Сֱ����ѹ",				MP_MIN_BA_U_ADD},
    {"����޹�����",				MP_MAX_Q_ADD},
    {"��С�޹�����",				MP_MIN_Q_ADD},
    {"��������",					MP_LOACAL_ASK},
    {"����ͨ��6����",				MP_C_LIFE},

    {"������������¶�",			AC_FUN_START_TEMP},
	{"�������ֹͣ�����¶�",		AC_FUN_STOP_TEMP},
	{"�˲����ݹ��µ��趨",			FILTER_CAPAC_ERR_TEMP},
	{"�������ֹͣ�����¶�",		AC_FUN_STOP_TEMP},
	{"���Ʊ�ѹ�����µ��¶�",		TRANSFORMER_ERR_TEMP},
	{"ĸ�ߵ��ݹ��µ��¶�",			BUS_CAPAC_ERR_TEMP},
	{"�翹�����µ��¶�",			ELEC_REACTOR_ERR_TEMP},
	{"��������µ��¶�",			AC_CABINET_ERR_TEMP},

	{"���1��ַA0",			AMMETER1_A0},
	{"���1��ַA1",			AMMETER1_A1},
	{"���1��ַA2",			AMMETER1_A2},
	{"���1��ַA3",			AMMETER1_A3},
	{"���1��ַA4",			AMMETER1_A4},	
	{"���1��ַA5",			AMMETER1_A5},	
	{"���1���ϵ��",		AMMETER1_VRC}, 
	{"���2��ַA0",			AMMETER2_A0},
	{"���2��ַA1",			AMMETER2_A1},
	{"���2��ַA2",			AMMETER2_A2},
	{"���2��ַA3",			AMMETER2_A3},
	{"���2��ַA4",			AMMETER2_A4},	
	{"���2��ַA5",			AMMETER2_A5},	
	{"���2���ϵ��",		AMMETER2_VRC},	
	{"������С���繦��", 	DIS_MIN_POWER},		
	{"������������", 		COS_SETA_PC_SET},
    {"�������ʹ�ܿ���",			MB_XFTG_EN},    
    {"��ѹ������",					MB_CAPACITY},     
    {"ʱ��1��ʼʱ��1",				MB_START_TIME_0},
	{"ʱ��1����ʱ��1",				MB_STOP_TIME_0},
	{"ʱ��1ʱ���1��������",		MB_SET_POWER_0},

    {"ʱ��1��ʼʱ��2",				MB_START_TIME_1},
	{"ʱ��1����ʱ��2",				MB_STOP_TIME_1},
	{"ʱ��1ʱ���2��������",		MB_SET_POWER_1},    

	{"ʱ��1��ʼʱ��3",				MB_START_TIME_2},
	{"ʱ��1����ʱ��3",				MB_STOP_TIME_2},
	{"ʱ��1ʱ���3��������",		MB_SET_POWER_2},	

	{"ʱ��1��ʼʱ��4",				MB_START_TIME_3},
	{"ʱ��1����ʱ��4",				MB_STOP_TIME_3},
	{"ʱ��1ʱ���4��������",		MB_SET_POWER_3},
	
	{"ʱ��1��ʼʱ��5",				MB_START_TIME_4},
	{"ʱ��1����ʱ��5",				MB_STOP_TIME_4},
	{"ʱ��1ʱ���5��������",		MB_SET_POWER_4},
	
	{"ʱ��1��ʼʱ��6",				MB_START_TIME_5},
	{"ʱ��1����ʱ��6",				MB_STOP_TIME_5},
	{"ʱ��1ʱ���6��������",		MB_SET_POWER_5},
	
	{"ʱ��1��ʼʱ��7",				MB_START_TIME_6},
	{"ʱ��1����ʱ��7",				MB_STOP_TIME_6},
	{"ʱ��1ʱ���7��������",		MB_SET_POWER_6},
	
	{"ʱ��1��ʼʱ��8",				MB_START_TIME_7},
	{"ʱ��1����ʱ��8",				MB_STOP_TIME_7},
	{"ʱ��1ʱ���8��������",		MB_SET_POWER_7},

    {"", MB_SB_EN},
    {"", MB_SB_TIME},
	{"", MB_SB_GOPWR},
	{"", MB_FR_EN},
	{"", MB_FR_WTIME},
    {"", MB_CO_EN},
    {"", MB_CO_JTIME},
    {"", MB_BOOTUP_TIME},
    
	{"", MB_OF_Thr_1},
	{"", MB_OF_Thr_2},
	{"", MB_UF_Thr_1},
	{"", MB_UF_Thr_2},
	{"", MB_OF_Time_1},
	{"", MB_OF_Time_2},
	{"", MB_UF_Time_1},
	{"", MB_UF_Time_2},

	{"AC���շ�����H",				MP_POWER_AC_DAY_H},
    {"AC���շ�����L",				MP_POWER_AC_DAY_L},
	{"AC�ܷ�����H",					MP_POWER_AC_H},
	{"AC�ܷ�����L",					MP_POWER_AC_L},

	{"�豸״̬",			 		MP_SYS_STATUS},
	{"ARM����汾��",				MP_ARM_VISION},
	{"�����汾��",					MP_PARA_VISION},
	
   	{"w_H", 						W_H},
	{"w_L", 						W_L},	

	{"w_L", 						MP_TEMP1},	
	{"w_L", 						MP_TEMP2},	
	{"w_L", 						MP_TEMP3},	
	{"w_L", 						MP_TEMP4},	
	{"w_L", 						MP_TEMP5},	
	{"w_L", 						MP_TEMP6},	
	
    {"������ѹAB(Upper)",			870},
    {"������ѹBC(Upper)",			871},
    {"������ѹCA(Upper)",			872},
    {"��������A(Upper)",			873},
    {"��������B(Upper)",			874},
    {"��������C(Upper)",			875},
    {"ĸ�ߵ�ѹ(Upper)",				876},
    {"ĸ�ߵ���(Upper)",				877},
    {"��ص�ѹ(Upper)",				878},
    {"������Եص�ѹ(Upper)",		879},
    {"��ظ��Եص�ѹ(Upper)",		880},
    {"�й�����(Upper)",				881},
    {"�޹�����(Upper)",				882},
    {"ֱ������(Upper)",				883},
    {"��Ե��ֵ(Upper)",				884},
    {"Ƶ��(Upper)",					885},

	{"",				900},
	{"",				901},
	{"",				902},
	{"",				903},
	{"",				904},
	{"",				905},
	{"",				906},
	{"",				907},
	{"",				908},
	{"",				909},
	{"",				910},
	{"",				911},
	{"",				912},
	{"",				913},
	{"",				914},
	{"",				915},
	{"",				916},
	{"",				917},
	{"",				918},
	{"",				919},
	{"",				920},
	{"",				921},
	{"",				922},
	{"",				923},
	{"",				924},
	{"",				925},
	{"",				926},
	{"",				927},
	{"",				928},
	{"",				929},
};
static unsigned short para_idx[MAX_PARA_ID];
short para_list[MPR_PARANUM*2];
const int paranum = sizeof(para_inf) / sizeof(para_information);


unsigned char set_power_data(unsigned int bcd_time,unsigned int id, float value)
{
    if(id > POWER_DATA_SIZE) {
        return 1;
    }

    power_data.power_value[id] = value;
    power_data.power_time[id]  = bcd_time;

    /*����RAM*/
    nvrom_pw_info[id] = power_data.power_value[id];
    nvrom_pw_time_info[id] = power_data.power_time[id];

    return 0;
}

unsigned char read_power_data(unsigned int id, float* value)
{
    if(id > POWER_DATA_SIZE) {
        return 1;
    }

    *value = power_data.power_value[id];

    return 0;
}

int time_clear(void)
{
	count_cal_write(RUN_TIME,0);	
	CountData.run_time_total= 0;
	CountData.run_time_now = 0;
	CountData.run_time_old= 0;
	
	count_cal_write(ON_TIME,0);	
	CountData.on_time_total = 0;
	CountData.on_time_now = 0;
	CountData.on_time_old = 0;

	count_cal_write(FREADJUST_COUNT,0);	
	CountData.fre_adjust_count = 0;
	CountData.fre_adjust_count_now= 0;
	CountData.fre_adjust_count_old= 0;

	count_cal_write(FREADJUST_COUNT_TODAY,0);	
	CountData.fre_adjust_count_today = 0;

	count_cal_write(FRELOCK_COUNT,0);	
	CountData.fre_lock_count = 0;
	CountData.fre_lock_count_now= 0;
	CountData.fre_lock_count_old= 0;

	count_cal_write(FRELOCK_COUNT_TODAY,0);	
	CountData.fre_lock_count_today = 0;

	
	count_cal_write(RESTART_COUNT,0);
	CountData.restart_tick = 0;
	
	count_cal_write(CRC_ERR_COUNT,0);
	CountData.crc_err_count =0;

	//��Կ�������������ٴ�������Կʹ����Ч	
	arm_config_data_write(CODE_RELEASE_FLAG,0); 							
	arm_config_data_write(CODE_DEBUG_FLAG,0); 						
	arm_config_data_write(CODE_TEMPORARY1_FLAG,0); 						
	arm_config_data_write(CODE_TEMPORARY2_FLAG,0); 						
	arm_config_data_write(CODE_TEMPORARY3_FLAG,0); 	
	arm_config_data_write(CODE_PERMANENT_FLAG,0); 	
	
	return 0;
}

int power_clear(void)
{
    int i;

    osSemaphoreWait(power_sem, osWaitForever);
    for(i = 0; i < COUNT_SIZE ; i++) {
        count_cal_write(i, 0);
    }
    CountData.Power_Con         		= count_cal[POWER_CON_ADD];
    CountData.power_ac_con       		= count_cal[POWER_AC_CON_ADD];
    CountData.power_ac_day   			= count_cal[POWER_AC_DAY_ADD];
    CountData.power_ac_yesterday 		= count_cal[POWER_AC_Y_ADD];
    CountData.power_ac_month 			= count_cal[POWER_AC_MONTH_ADD];
    CountData.power_ac_year 			= count_cal[POWER_AC_YEAR_ADD];
	
    CountData.power_ac_c_con    		= count_cal[POWER_AC_C_CON_ADD];	
    CountData.power_ac_c_day    		= count_cal[POWER_AC_C_DAY_ADD];
    CountData.power_ac_c_yesterday		= count_cal[POWER_AC_C_Y_ADD];
    CountData.power_ac_c_month			= count_cal[POWER_AC_C_MONTH_ADD];
    CountData.power_ac_c_year			= count_cal[POWER_AC_C_YEAR_ADD];
	
    CountData.power_dc1_day     		= count_cal[POWER_DC1_DAY_ADD];
    CountData.power_dc1_con     		= count_cal[POWER_DC1_CON_ADD];
    CountData.power_dc1_c_day   		= count_cal[POWER_DC1_C_DAY_ADD];
    CountData.power_dc1_c_con   		= count_cal[POWER_DC1_C_CON_ADD];
    CountData.power_dc2_day     		= count_cal[POWER_DC2_DAY_ADD];
    CountData.power_dc2_con     		= count_cal[POWER_DC2_CON_ADD];
    CountData.power_dc2_yesterday 		= count_cal[POWER_DC2_Y_ADD];
    CountData.power_dc2_month 			= count_cal[POWER_DC2_MON_ADD];
    CountData.power_ac_day_cache = 0;
    CountData.power_ac_c_day_cache = 0;
    CountData.power_dc2_day_cache = 0;

    for(i = 0; i <= POWER_DATA_SIZE; i++) 
	{
        set_power_data(0, i, 0);
    }
    osSemaphoreRelease(power_sem);

    return 0;
}

unsigned char read_power_time(unsigned int id, unsigned int* bcd_time)
{
    if(id > POWER_DATA_SIZE) {
        return 1;
    }

    *bcd_time = power_data.power_time[id];

    return 0;
}

unsigned char set_power_time(unsigned int id, unsigned int* bcd_time)
{
    if(id > POWER_DATA_SIZE) {
        return 1;
    }

    power_data.power_time[id] = *bcd_time;

    return 0;
}


int setpara(unsigned int id, short value)
{
    unsigned int addr;

    if(id > MAX_PARA_ID) {
        return 1;
    }

    addr = para_idx[id];
#if 1
    if(addr==0) {
        return 1;
    }
#endif
    para_list[addr] = BIGENDIAN(value);

    return 0;
}


int readpara(unsigned int id, short* value)
{
    unsigned int addr;

    if(id > MAX_PARA_ID) {
        return 1;
    }

    addr = para_idx[id];

    *value = BIGENDIAN(para_list[addr]);

    return 0;
}

void para_init(void)
{
    int len, i;
    para_information* pinf;

    len = sizeof(para_inf) / sizeof(para_information);
    pinf = &para_inf[0];

    memset(para_list, 0, sizeof(para_list));

    for(i = 0; i < len; i++) {
        para_list[i * 2] = BIGENDIAN(pinf->id);
        para_idx[pinf->id] = i * 2 + 1;
        pinf++;
    }

    for(i = 0; i < POWER_DATA_SIZE; i++) {
        power_data.power_time[i]  = nvrom_pw_time_info[i];
        power_data.power_value[i] = nvrom_pw_info[i];
    }
}

