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
out_state_info_t out_state_info; //对外状态
_power_data power_data;
_CountData CountData;
_sys_time Arm_time;
PQNonSVG_info_t PQNonSVG; //P/Q非SVG设置缓存: [0]-P [1]-Q, 存的是实际值*MODULUS
short Upper_Param[1024]; //上位机参数


para_information para_inf[] =
{
    {"交流母线电压过压参考",			Addr_Param0},
    {"交流母线电压欠压参考",			Addr_Param1},
    {"交流过流门限参考",				Addr_Param2},
    {"直流过压门限参考",				Addr_Param3},
    {"直流欠压门限参考",			Addr_Param4},
    {"直流过流门限参考",			Addr_Param5},
    {"电池过压门限参考",			Addr_Param6},
    {"电池欠压门限参考",			Addr_Param7},
    {"母线压差过压门限",			Addr_Param8},
    {"交流瞬时过压门限",			Addr_Param9},
    {"交流瞬时过流门限",			Addr_Param10},
    {"直流瞬时过压门限",			Addr_Param11},
    {"直流瞬时过流门限",			Addr_Param12},
    {"电池瞬时过压门限",			Addr_Param13},
    {"电池回零门限",		Addr_Param14},
    {"预充电完成电压差值门限",		Addr_Param15},
    {"过频阈值",		Addr_Param16},
    {"欠频阈值",				Addr_Param17},
    {"柜内过耗保护值",				Addr_Param18},
    {"共模电流过流阈值",			Addr_Param19},
    {"备用",			Addr_Param20},
    {"备用",				Addr_Param21},
    {"备用",		Addr_Param22},
    {"直流电压给定",		Addr_Param23},
    {"交流电压幅值给定",		Addr_Param24},
    {"交流电压负序幅值给定",		Addr_Param25},
    {"有功功率给定",			Addr_Param26},
    {"无功功率给定",			Addr_Param27},
    {"交流电流限制",			Addr_Param28},
    {"交流放电限制值",				Addr_Param29},
    {"交流充电限制值",		Addr_Param30},
    {"直流充电功率限制值",		Addr_Param31},
    {"直流放电功率限制值",		Addr_Param32},
    {"直流充电电流限制值",		Addr_Param33},
    {"直流放电电流限制值",			Addr_Param34},
    {"交流到直流功率损耗",			Addr_Param35},
    {"谐振抑制虚拟电阻",			Addr_Param36},
    {"谐波提取滤波频率",				Addr_Param37},
    {"备用",		Addr_Param38},
    {"备用",		Addr_Param39},
    {"备用",		Addr_Param40},
    {"直流功率到交流功率转换系数",		Addr_Param41},
    {"电压前馈系数",			Addr_Param42},
    {"电抗参数",			Addr_Param43},
    {"电容无功抑制系数",			Addr_Param44},
    {"频率调整系数",				Addr_Param45},
    {"虚拟电阻",		Addr_Param46},
    {"虚拟电阻Kp",		Addr_Param47},
    {"虚拟电阻Ki",		Addr_Param48},
    {"虚拟惯量Kp",		Addr_Param49},
    {"虚拟电阻Ki",			Addr_Param50},
    
    {"电池电压软给定频率",			Addr_Param51},
    {"电池电流软给定频率",			Addr_Param52},
    {"功率软给定频率",				Addr_Param53},
    {"母线电压软给定频率",			Addr_Param54},
    {"电池电压2母线电压转换系数",	Addr_Param55},
    {"母线控制环Kp",				Addr_Param56},
    {"母线控制环Ki",				Addr_Param57},
    {"电池外环Kp",					Addr_Param58},
    {"电池外环Ki",					Addr_Param59},
    {"内环Kp",						Addr_Param60},
    {"内环Ki",						Addr_Param61},
    {"载波频率",					Addr_Param62},
    {"DC1支路载波相移",				Addr_Param63},
    {"DC2支路载波相移",				Addr_Param64},
    {"DC3支路载波相移",				Addr_Param65},
    {"DC4支路载波相移",				Addr_Param66},
    {"死区时间",					Addr_Param67},
    {"窄脉冲封锁时间",				Addr_Param68},
    {"IGBT过温保护门限",			Addr_Param69},
    {"备用",						Addr_Param70},
    {"备用",						Addr_Param71},
    {"备用",						Addr_Param72},
    {"备用",						Addr_Param73},
    {"备用",						Addr_Param74},
    {"备用",						Addr_Param75},
    {"备用",						Addr_Param76},
    {"备用",						Addr_Param77},
    {"备用",						Addr_Param78},
    {"备用",						Addr_Param79},
    {"备用",						Addr_Param80},
    {"备用",						Addr_Param81},
    {"备用",						Addr_Param82},
    {"备用",						Addr_Param83},
    {"备用",						Addr_Param84},
    {"备用",						Addr_Param85},
    {"备用",						Addr_Param86},
    {"备用",						Addr_Param87},
    {"备用",						Addr_Param88},
    {"备用",						Addr_Param89},
    {"备用",						Addr_Param90},
    {"DC1支路IGBT温度(to FPGA)",	Addr_Param91},
    {"DC2支路IGBT温度(to FPGA)",	Addr_Param92},
    {"DC3支路IGBT温度(to FPGA)",	Addr_Param93},
    {"DC4支路IGBT温度(to FPGA)",	Addr_Param94},
    {"备用",						Addr_Param95},
    {"备用",						Addr_Param96},
    {"备用",						Addr_Param97},
    {"备用",						Addr_Param98},
    {"备用",						Addr_Param99},
    {"波形采样频率设置",			Addr_Param100},
    {"波形通道0内容选择",			Addr_Param101},
    {"波形通道1内容选择",			Addr_Param102},
    {"波形通道2内容选择",			Addr_Param103},
    {"波形通道3内容选择",			Addr_Param104},
    {"波形通道4内容选择",			Addr_Param105},
    {"波形通道5内容选择",			Addr_Param106},
    {"波形通道6内容选择",			Addr_Param107},
    {"波形通道7内容选择",			Addr_Param108},
    {"波形通道8内容选择",			Addr_Param109},
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
    {"母线电压",					Addr_Param120},
    {"DC1支路功率",					Addr_Param121},
    {"DC1支路电流",					Addr_Param122},
    {"DC1直流电池电压",				Addr_Param123},
    {"DC2支路功率",					Addr_Param124},
    {"DC2支路电流",					Addr_Param125},
    {"DC2直流电池电压",				Addr_Param126},
    {"DC3支路功率",					Addr_Param127},
    {"DC3支路电流",					Addr_Param128},
    {"DC3直流电池电压",				Addr_Param129},
    {"DC4支路功率",					Addr_Param130},
    {"DC4支路电流",					Addr_Param131},
    {"DC4直流电池电压",				Addr_Param132},
    {"母线电压校零平均值",			Addr_Param133},
    {"DC1支路电池电压校零平均值",	Addr_Param134},
    {"DC2支路电池电压校零平均值",	Addr_Param135},
    {"DC3支路电池电压校零平均值",	Addr_Param136},
    {"DC4支路电池电压校零平均值",	Addr_Param137},
    {"DC1支路电池电流校零平均值",	Addr_Param138},
    {"DC2支路电池电流校零平均值",	Addr_Param139},
    {"DC3支路电池电流校零平均值",	Addr_Param140},
    {"DC4支路电池电流校零平均值",	Addr_Param141},
    {"DC1支路IGBT温度",				Addr_Param142},
    {"DC2支路IGBT温度",				Addr_Param143},
    {"DC3支路IGBT温度",				Addr_Param144},
    {"DC4支路IGBT温度",				Addr_Param145},
    {"备用",						Addr_Param146},
    {"备用",						Addr_Param147},
    {"备用",						Addr_Param148},
    {"备用",						Addr_Param149},
    {"系统状态寄存器0",				Addr_Param150},
    {"系统状态寄存器1",				Addr_Param151},
    {"系统状态寄存器2",				Addr_Param152},
    {"系统状态寄存器3",				Addr_Param153},
    {"系统状态寄存器4",				Addr_Param154},
    {"系统状态寄存器5",				Addr_Param155},
    {"系统状态寄存器6",				Addr_Param156},
    {"系统状态寄存器7",				Addr_Param157},
    {"系统状态寄存器8",				Addr_Param158},
    {"系统状态寄存器9",				Addr_Param159},
    {"系统状态寄存器10",			Addr_Param160},
    {"系统状态寄存器11",			Addr_Param161},
    {"系统状态寄存器12",			Addr_Param162},
    {"系统状态寄存器13",			Addr_Param163},
    {"系统状态寄存器14",			Addr_Param164},
    {"系统状态寄存器15",			Addr_Param165},
    {"系统故障锁存寄存器0",			Addr_Param166},
    {"系统故障锁存寄存器1",			Addr_Param167},
    {"系统故障锁存寄存器2",			Addr_Param168},
    {"系统故障锁存寄存器3",			Addr_Param169},
    {"系统故障锁存寄存器4",			Addr_Param170},
    {"系统故障锁存寄存器5",			Addr_Param171},
    {"系统故障锁存寄存器6",			Addr_Param172},
    {"系统故障锁存寄存器7",			Addr_Param173},
    {"系统故障锁存寄存器8",			Addr_Param174},
    {"系统故障锁存寄存器9",			Addr_Param175},
    {"系统故障锁存寄存器10",		Addr_Param176},
    {"系统故障锁存寄存器11",		Addr_Param177},
    {"系统故障锁存寄存器12",		Addr_Param178},
    {"系统故障锁存寄存器13",		Addr_Param179},
    {"系统故障锁存寄存器14",		Addr_Param180},
    {"系统故障锁存寄存器15",		Addr_Param181},
    {"备用",						Addr_Param182},
    {"备用",						Addr_Param183},
    {"备用",						Addr_Param184},
    {"备用",						Addr_Param185},
    {"备用",						Addr_Param186},
    {"备用",						Addr_Param187},
    {"备用",						Addr_Param188},
    {"备用",						Addr_Param189},
    {"备用",						Addr_Param190},
    {"备用",						Addr_Param191},
    {"备用",						Addr_Param192},
    {"备用",						Addr_Param193},
    {"备用",						Addr_Param194},
    {"备用",						Addr_Param195},
    {"备用",						Addr_Param196},
    {"备用",						Addr_Param197},
    {"备用",						Addr_Param198},
    {"FPGA_Version",				Addr_Param199},
    {"母线电压校零值",				Addr_Param200},
    {"DC1支路电池电压校零值",		Addr_Param201},
    {"DC2支路电池电压校零值",		Addr_Param202},
    {"DC3支路电池电压校零值",		Addr_Param203},
    {"DC4支路电池电压校零值",		Addr_Param204},
    {"DC1支路电池电流校零值",		Addr_Param205},
    {"DC2支路电池电流校零值",		Addr_Param206},
    {"DC3支路电池电流校零值",		Addr_Param207},
    {"DC4支路电池电流校零值",		Addr_Param208},
    {"备用",						Addr_Param209},
    {"备用",						Addr_Param210},
    {"备用",						Addr_Param211},
    {"备用",						Addr_Param212},
    {"备用",						Addr_Param213},
    {"备用",						Addr_Param214},
    {"备用",						Addr_Param215},
    {"备用",						Addr_Param216},
    {"备用",						Addr_Param217},
    {"备用",						Addr_Param218},
    {"备用",						Addr_Param219},
    {"备用",						Addr_Param220},
    {"DC1支路IGBT温度(to FPGA)",	Addr_Param221},
    {"DC2支路IGBT温度(to FPGA)",	Addr_Param222},
    {"DC3支路IGBT温度(to FPGA)",	Addr_Param223},
    {"DC4支路IGBT温度(to FPGA)",	Addr_Param224},
    {"备用",						Addr_Param225},
    {"备用",						Addr_Param226},
    {"备用",						Addr_Param227},
    {"备用",						Addr_Param228},
    {"备用",						Addr_Param229},
    {"备用",						Addr_Param230},
    {"备用",						Addr_Param231},
    {"备用",						Addr_Param232},
    {"备用",						Addr_Param233},
    {"备用",						Addr_Param234},
    {"备用",						Addr_Param235},
    {"备用",						Addr_Param236},
    {"备用",						Addr_Param237},
    {"备用",						Addr_Param238},
    {"备用",						Addr_Param239},
    {"备用",						Addr_Param240},
    {"备用",						Addr_Param241},
    {"备用",						Addr_Param242},
    {"备用",						Addr_Param243},
    {"备用",						Addr_Param244},
    {"备用",						Addr_Param245},
    {"备用",						Addr_Param246},
    {"波形通道0数据",				Addr_Param247},
    {"波形通道1数据",				Addr_Param248},
    {"波形通道2数据",				Addr_Param249},
    {"波形通道3数据",				Addr_Param250},
    {"波形通道4数据",				Addr_Param251},
    {"波形通道5数据",				Addr_Param252},
    {"波形通道6数据",				Addr_Param253},
    {"波形通道7数据",				Addr_Param254},
    {"波形通道8数据",				Addr_Param255},
    {"波形采样频率设置",			Addr_Param256},
    {"波形通道0内容选择",			Addr_Param257},
    {"波形通道1内容选择",			Addr_Param258},
    {"波形通道2内容选择",			Addr_Param259},
    {"波形通道3内容选择",			Addr_Param260},
    {"波形通道4内容选择",			Addr_Param261},
    {"波形通道5内容选择",			Addr_Param262},
    {"波形通道6内容选择",			Addr_Param263},
    {"波形通道7内容选择",			Addr_Param264},
    {"波形通道8内容选择",			Addr_Param265},
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
    {"母线电压",					Addr_Param276},
    {"DC1支路功率",					Addr_Param277},
    {"DC1支路电流",					Addr_Param278},
    {"DC1直流电池电压",				Addr_Param279},
    {"DC2支路功率",					Addr_Param280},
    {"DC2支路电流",					Addr_Param281},
    {"DC2直流电池电压",				Addr_Param282},
    {"DC3支路功率",					Addr_Param283},
    {"DC3支路电流",					Addr_Param284},
    {"DC3直流电池电压",				Addr_Param285},
    {"DC4支路功率",					Addr_Param286},
    {"DC4支路电流",					Addr_Param287},
    {"DC4直流电池电压",				Addr_Param288},
    {"母线电压校零平均值",			Addr_Param289},
    {"DC1支路电池电压校零平均值",	Addr_Param290},
    {"DC2支路电池电压校零平均值",	Addr_Param291},
    {"DC3支路电池电压校零平均值",	Addr_Param292},
    {"DC4支路电池电压校零平均值",	Addr_Param293},
    {"DC1支路电池电流校零平均值",	Addr_Param294},
    {"DC2支路电池电流校零平均值",	Addr_Param295},
    {"DC3支路电池电流校零平均值",	Addr_Param296},
    {"DC4支路电池电流校零平均值",	Addr_Param297},
    {"DC1支路IGBT温度",				Addr_Param298},
    {"DC2支路IGBT温度",				Addr_Param299},
    {"DC3支路IGBT温度",				Addr_Param300},

    {"DC1支路电池电压校零值",		Addr_Param301},
    {"DC2支路电池电压校零值",		Addr_Param302},
    {"DC3支路电池电压校零值",		Addr_Param303},
    {"DC4支路电池电压校零值",		Addr_Param304},
    {"DC1支路电池电流校零值",		Addr_Param305},
    {"DC2支路电池电流校零值",		Addr_Param306},
    {"DC3支路电池电流校零值",		Addr_Param307},
    {"DC4支路电池电流校零值",		Addr_Param308},
    {"备用",						Addr_Param309},
    {"备用",						Addr_Param310},
    {"备用",						Addr_Param311},
    {"备用",						Addr_Param312},
    {"备用",						Addr_Param313},
    {"备用",						Addr_Param314},
    {"备用",						Addr_Param315},
    {"备用",						Addr_Param316},
    {"备用",						Addr_Param317},
    {"备用",						Addr_Param318},
    {"备用",						Addr_Param319},
    {"备用",						Addr_Param320},
    {"DC1支路IGBT温度(to FPGA)",	Addr_Param321},
    {"DC2支路IGBT温度(to FPGA)",	Addr_Param322},
    {"DC3支路IGBT温度(to FPGA)",	Addr_Param323},
    {"DC4支路IGBT温度(to FPGA)",	Addr_Param324},
    {"备用",						Addr_Param325},
    {"备用",						Addr_Param326},
    {"备用",						Addr_Param327},
    {"备用",						Addr_Param328},
    {"备用",						Addr_Param329},
    {"备用",						Addr_Param330},
    {"备用",						Addr_Param331},
    {"备用",						Addr_Param332},
    {"备用",						Addr_Param333},
    {"备用",						Addr_Param334},
    {"备用",						Addr_Param335},
    {"备用",						Addr_Param336},
    {"备用",						Addr_Param337},
    {"备用",						Addr_Param338},
    {"备用",						Addr_Param339},
    {"备用",						Addr_Param340},
    {"备用",						Addr_Param341},
    {"备用",						Addr_Param342},
    {"备用",						Addr_Param343},
    {"备用",						Addr_Param344},
    {"备用",						Addr_Param345},	
    {"备用",						Addr_Param346},
    {"备用",						Addr_Param347},
    {"备用",						Addr_Param348},
    {"备用",						Addr_Param349},
    {"系统状态寄存器0",				Addr_Param350},
    {"系统状态寄存器1",				Addr_Param351},
    {"系统状态寄存器2",				Addr_Param352},
    {"系统状态寄存器3",				Addr_Param353},
    {"系统状态寄存器4",				Addr_Param354},
    {"系统状态寄存器5",				Addr_Param355},
    {"系统状态寄存器6",				Addr_Param356},
    {"系统状态寄存器7",				Addr_Param357},
    {"系统状态寄存器8",				Addr_Param358},
    {"系统状态寄存器9",				Addr_Param359},
    {"系统状态寄存器10",			Addr_Param360},
    {"系统状态寄存器11",			Addr_Param361},
    {"系统状态寄存器12",			Addr_Param362},
    {"系统状态寄存器13",			Addr_Param363},
    {"系统状态寄存器14",			Addr_Param364},
    {"系统状态寄存器15",			Addr_Param365},
    {"系统故障锁存寄存器0",			Addr_Param366},
    {"系统故障锁存寄存器1",			Addr_Param367},
    {"系统故障锁存寄存器2",			Addr_Param368},
    {"系统故障锁存寄存器3",			Addr_Param369},
    {"系统故障锁存寄存器4",			Addr_Param370},
    {"系统故障锁存寄存器5",			Addr_Param371},
    {"系统故障锁存寄存器6",			Addr_Param372},
    {"系统故障锁存寄存器7",			Addr_Param373},
    {"系统故障锁存寄存器8",			Addr_Param374},
    {"系统故障锁存寄存器9",			Addr_Param375},
    {"系统故障锁存寄存器10",		Addr_Param376},
    {"系统故障锁存寄存器11",		Addr_Param377},
    {"系统故障锁存寄存器12",		Addr_Param378},
    {"系统故障锁存寄存器13",		Addr_Param379},
    {"系统故障锁存寄存器14",		Addr_Param380},
    {"系统故障锁存寄存器15",		Addr_Param381},
    {"备用",						Addr_Param382},
    {"备用",						Addr_Param383},
    {"备用",						Addr_Param384},
    {"备用",						Addr_Param385},
    {"备用",						Addr_Param386},
    {"备用",						Addr_Param387},
    {"备用",						Addr_Param388},
    {"备用",						Addr_Param389},
    {"备用",						Addr_Param390},
    {"备用",						Addr_Param391},
    {"备用",						Addr_Param392},
    {"备用",						Addr_Param393},
    {"备用",						Addr_Param394},
    {"备用",						Addr_Param395},
    {"备用",						Addr_Param396},
    {"备用",						Addr_Param397},
    {"备用",						Addr_Param398},
    {"FPGA_Version",				Addr_Param399},
    {"母线电压校零值",				Addr_Param400},
    {"DC1支路电池电压校零值",		Addr_Param401},
    {"DC2支路电池电压校零值",		Addr_Param402},
    {"DC3支路电池电压校零值",		Addr_Param403},
    {"DC4支路电池电压校零值",		Addr_Param404},
    {"DC1支路电池电流校零值",		Addr_Param405},
    {"DC2支路电池电流校零值",		Addr_Param406},
    {"DC3支路电池电流校零值",		Addr_Param407},
    {"DC4支路电池电流校零值",		Addr_Param408},
    {"备用",						Addr_Param409},
    {"备用",						Addr_Param410},
    {"备用",						Addr_Param411},
    {"备用",						Addr_Param412},
    {"备用",						Addr_Param413},
    {"备用",						Addr_Param414},
    {"备用",						Addr_Param415},
    {"备用",						Addr_Param416},
    {"备用",						Addr_Param417},
    {"备用",						Addr_Param418},
    {"备用",						Addr_Param419},
    {"备用",						Addr_Param420},
    {"DC1支路IGBT温度(to FPGA)",	Addr_Param421},
    {"DC2支路IGBT温度(to FPGA)",	Addr_Param422},
    {"DC3支路IGBT温度(to FPGA)",	Addr_Param423},
    {"DC4支路IGBT温度(to FPGA)",	Addr_Param424},
    {"备用",						Addr_Param425},
    {"备用",						Addr_Param426},
    {"备用",						Addr_Param427},
    {"备用",						Addr_Param428},
    {"备用",						Addr_Param429},
    {"备用",						Addr_Param430},
    {"备用",						Addr_Param431},
    {"备用",						Addr_Param432},
    {"备用",						Addr_Param433},
    {"备用",						Addr_Param434},
    {"备用",						Addr_Param435},
    {"备用",						Addr_Param436},
    {"备用",						Addr_Param437},
    {"备用",						Addr_Param438},
    {"备用",						Addr_Param439},
    {"备用",						Addr_Param440},
    {"备用",						Addr_Param441},
    {"备用",						Addr_Param442},
    {"备用",						Addr_Param443},
    {"备用",						Addr_Param444},
    {"备用",						Addr_Param445},
    {"备用",						Addr_Param446},
    {"波形通道0数据",				Addr_Param447},
    {"波形通道1数据",				Addr_Param448},
    {"波形通道2数据",				Addr_Param449},
    {"波形通道3数据",				Addr_Param450},
    {"波形通道4数据",				Addr_Param451},
    {"波形通道5数据",				Addr_Param452},
    {"波形通道6数据",				Addr_Param453},
    {"波形通道7数据",				Addr_Param454},
    {"波形通道8数据",				Addr_Param455},
    {"系统状态寄存器6",				Addr_Param456},
    {"系统状态寄存器7",				Addr_Param457},
    {"系统状态寄存器8",				Addr_Param458},
    {"系统状态寄存器9",				Addr_Param459},
    {"系统状态寄存器10",			Addr_Param460},
    {"系统状态寄存器11",			Addr_Param461},
    {"系统状态寄存器12",			Addr_Param462},
    {"系统状态寄存器13",			Addr_Param463},
    {"系统状态寄存器14",			Addr_Param464},
    {"系统状态寄存器15",			Addr_Param465},
    {"系统故障锁存寄存器0",			Addr_Param466},
    {"系统故障锁存寄存器1",			Addr_Param467},
    {"系统故障锁存寄存器2",			Addr_Param468},
    {"系统故障锁存寄存器3",			Addr_Param469},
    {"系统故障锁存寄存器4",			Addr_Param470},
    {"系统故障锁存寄存器5",			Addr_Param471},
    {"系统故障锁存寄存器6",			Addr_Param472},
    {"系统故障锁存寄存器7",			Addr_Param473},
    {"系统故障锁存寄存器8",			Addr_Param474},
    {"系统故障锁存寄存器9",			Addr_Param475},
    {"系统故障锁存寄存器10",		Addr_Param476},
    {"系统故障锁存寄存器11",		Addr_Param477},
    {"系统故障锁存寄存器12",		Addr_Param478},
    {"系统故障锁存寄存器13",		Addr_Param479},
    {"系统故障锁存寄存器14",		Addr_Param480},
    {"系统故障锁存寄存器15",		Addr_Param481},
    {"备用",						Addr_Param482},
    {"备用",						Addr_Param483},
    {"备用",						Addr_Param484},
    {"备用",						Addr_Param485},
    {"备用",						Addr_Param486},
    {"备用",						Addr_Param487},
    {"备用",						Addr_Param488},
    {"备用",						Addr_Param489},
    {"备用",						Addr_Param490},
    {"备用",						Addr_Param491},
    {"备用",						Addr_Param492},
    {"备用",						Addr_Param493},
    {"备用",						Addr_Param494},
    {"备用",						Addr_Param495},
    {"备用",						Addr_Param496},
    {"备用",						Addr_Param497},
    {"备用",						Addr_Param498},
    {"FPGA_Version",				Addr_Param499},
    {"母线电压校零值",				Addr_Param500},
    {"DC1支路电池电压校零值",		Addr_Param501},
    {"DC2支路电池电压校零值",		Addr_Param502},
    {"DC3支路电池电压校零值",		Addr_Param503},
    {"DC4支路电池电压校零值",		Addr_Param504},
    {"DC1支路电池电流校零值",		Addr_Param505},
    {"DC2支路电池电流校零值",		Addr_Param506},
    {"DC3支路电池电流校零值",		Addr_Param507},
    {"DC4支路电池电流校零值",		Addr_Param508},
    {"备用",						Addr_Param509},
    {"备用",						Addr_Param510},
    {"备用",						Addr_Param511},
    
	{"设备出厂年份",				MB_DEVICE_CODE_YEAR},	
	{"设备出厂月日",				MB_DEVICE_CODE_MD},
	{"设备型号",					MB_DEVICE_CODE_TYPE},
	{"设备出厂编号",				MB_DEVICE_CODE_NUM},
	{"输入密钥",					MB_DEVICE_WRITE_CODE},
	{"出厂编码修改许可",			MB_LECENSE_KEY},

    
    {"交流输入电压变比系数H",		FD_UINX_ADD_H},
    {"交流输入电压变比系数L",		FD_UINX_ADD_L},
    {"交流输入电流变比系数H",		FD_IINX_ADD_H},
    {"交流输入电流变比系数L",		FD_IINX_ADD_L},
    {"直流电压变比系数H",			FD_UDC_ADD_H},
    {"直流电压变比系数L",			FD_UDC_ADD_L},
    {"直流电流变比系数H",			FD_IDC_ADD_H},
    {"直流电流变比系数L",			FD_IDC_ADD_L},
    {"电池电压变比系数H",			FD_UBTR_ADD_H},
    {"电池电压变比系数L",			FD_UBTR_ADD_L},
    {"正极对地电压变比系数H",		FD_UPOS_GND_ADD_H},
    {"正极对地电压变比系数L",		FD_UPOS_GND_ADD_L},
    {"负极对地电压变比系数H",		FD_UNEG_GND_ADD_H},
    {"负极对地电压变比系数L",		FD_UNEG_GND_ADD_L},
    {"交流功率变比系数H",			FD_PAC_ADD_H},
    {"交流功率变比系数L",			FD_PAC_ADD_L},
    {"直流功率变比系数H",			FD_PDC_ADD_H},
    {"直流功率变比系数L",			FD_PDC_ADD_L},
    {"互感电流变比系数H",			FD_IAC_CT_ADD_H},
    {"互感电流变比系数L",			FD_IAC_CT_ADD_L},
    {"互感功率变比系数H",			FD_PAC_CT_ADD_H},
    {"互感功率变比系数L",			FD_PAC_CT_ADD_L},

    {"漏电流变比系数H",				FD_IAC_LKG_H},
    {"漏电流变比系数L",				FD_IAC_LKG_L},	
    
    {"备用",						RESET_FLAG},
    {"备用",						DEBUG_ONOFF},
	{"控制模式设置",				Ctrl_Mode_SET}, 
	{"运行模式选择",				Run_Mode_Set}, 
	{"通讯保护策略使能",			Communication_En}, 
	{"策略选择",					Strategy_En}, 	  
    {"备用",						MP_INI_MONITOR_ADDR},
   
    {"充电限压值",					MP_CHRG_VOL_LIMIT},
    {"放电限压值",					MP_DISC_VOL_LIMIT},
    {"SVG使能",						MP_SVG_EN},
    {"孤岛保护",					MP_ISO_ISLAND},

	
    {"绝缘监测系数",				MP_INI_RESIS_FD},
    {"绝缘判定门限",				MP_INI_RESIS_REF},
    {"监控机地址",					MP_INI_MONITOR_ADDR},
    {"最大有功门限",				MP_MAX_P_ADD},
    {"最小有功门限",				MP_MIN_P_ADD},
    {"最大直流电流",				MP_MAX_BA_C_ADD},
    {"最小直流电流",				MP_MIN_BA_C_ADD},
    {"最大直流电压",				MP_MAX_BA_U_ADD},
    {"最小直流电压",				MP_MIN_BA_U_ADD},
    {"最大无功门限",				MP_MAX_Q_ADD},
    {"最小无功门限",				MP_MIN_Q_ADD},
    {"本地请求",					MP_LOACAL_ASK},
    {"波形通道6数据",				MP_C_LIFE},

    {"交流风机启动温度",			AC_FUN_START_TEMP},
	{"交流风机停止运行温度",		AC_FUN_STOP_TEMP},
	{"滤波电容过温点设定",			FILTER_CAPAC_ERR_TEMP},
	{"交流风机停止运行温度",		AC_FUN_STOP_TEMP},
	{"控制变压器过温点温度",		TRANSFORMER_ERR_TEMP},
	{"母线电容过温点温度",			BUS_CAPAC_ERR_TEMP},
	{"电抗器过温点温度",			ELEC_REACTOR_ERR_TEMP},
	{"交流柜过温点温度",			AC_CABINET_ERR_TEMP},

	{"电表1地址A0",			AMMETER1_A0},
	{"电表1地址A1",			AMMETER1_A1},
	{"电表1地址A2",			AMMETER1_A2},
	{"电表1地址A3",			AMMETER1_A3},
	{"电表1地址A4",			AMMETER1_A4},	
	{"电表1地址A5",			AMMETER1_A5},	
	{"电表1变比系数",		AMMETER1_VRC}, 
	{"电表2地址A0",			AMMETER2_A0},
	{"电表2地址A1",			AMMETER2_A1},
	{"电表2地址A2",			AMMETER2_A2},
	{"电表2地址A3",			AMMETER2_A3},
	{"电表2地址A4",			AMMETER2_A4},	
	{"电表2地址A5",			AMMETER2_A5},	
	{"电表2变比系数",		AMMETER2_VRC},	
	{"电网最小供电功率", 	DIS_MIN_POWER},		
	{"电网功率因素", 		COS_SETA_PC_SET},
    {"削封填谷使能开关",			MB_XFTG_EN},    
    {"变压器容量",					MB_CAPACITY},     
    {"时区1开始时间1",				MB_START_TIME_0},
	{"时区1结束时间1",				MB_STOP_TIME_0},
	{"时区1时间段1功率设置",		MB_SET_POWER_0},

    {"时区1开始时间2",				MB_START_TIME_1},
	{"时区1结束时间2",				MB_STOP_TIME_1},
	{"时区1时间段2功率设置",		MB_SET_POWER_1},    

	{"时区1开始时间3",				MB_START_TIME_2},
	{"时区1结束时间3",				MB_STOP_TIME_2},
	{"时区1时间段3功率设置",		MB_SET_POWER_2},	

	{"时区1开始时间4",				MB_START_TIME_3},
	{"时区1结束时间4",				MB_STOP_TIME_3},
	{"时区1时间段4功率设置",		MB_SET_POWER_3},
	
	{"时区1开始时间5",				MB_START_TIME_4},
	{"时区1结束时间5",				MB_STOP_TIME_4},
	{"时区1时间段5功率设置",		MB_SET_POWER_4},
	
	{"时区1开始时间6",				MB_START_TIME_5},
	{"时区1结束时间6",				MB_STOP_TIME_5},
	{"时区1时间段6功率设置",		MB_SET_POWER_5},
	
	{"时区1开始时间7",				MB_START_TIME_6},
	{"时区1结束时间7",				MB_STOP_TIME_6},
	{"时区1时间段7功率设置",		MB_SET_POWER_6},
	
	{"时区1开始时间8",				MB_START_TIME_7},
	{"时区1结束时间8",				MB_STOP_TIME_7},
	{"时区1时间段8功率设置",		MB_SET_POWER_7},

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

	{"AC当日发电量H",				MP_POWER_AC_DAY_H},
    {"AC当日发电量L",				MP_POWER_AC_DAY_L},
	{"AC总发电量H",					MP_POWER_AC_H},
	{"AC总发电量L",					MP_POWER_AC_L},

	{"设备状态",			 		MP_SYS_STATUS},
	{"ARM程序版本号",				MP_ARM_VISION},
	{"参数版本号",					MP_PARA_VISION},
	
   	{"w_H", 						W_H},
	{"w_L", 						W_L},	

	{"w_L", 						MP_TEMP1},	
	{"w_L", 						MP_TEMP2},	
	{"w_L", 						MP_TEMP3},	
	{"w_L", 						MP_TEMP4},	
	{"w_L", 						MP_TEMP5},	
	{"w_L", 						MP_TEMP6},	
	
    {"交流电压AB(Upper)",			870},
    {"交流电压BC(Upper)",			871},
    {"交流电压CA(Upper)",			872},
    {"交流电流A(Upper)",			873},
    {"交流电流B(Upper)",			874},
    {"交流电流C(Upper)",			875},
    {"母线电压(Upper)",				876},
    {"母线电流(Upper)",				877},
    {"电池电压(Upper)",				878},
    {"电池正对地电压(Upper)",		879},
    {"电池负对地电压(Upper)",		880},
    {"有功功率(Upper)",				881},
    {"无功功率(Upper)",				882},
    {"直流功率(Upper)",				883},
    {"绝缘差值(Upper)",				884},
    {"频率(Upper)",					885},

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

    /*存入RAM*/
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

	//密钥处理，清理后可以再次输入密钥使其有效	
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

