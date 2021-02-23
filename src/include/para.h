#ifndef PARA_H
#define PARA_H


enum
{
	Addr_Param0 = 0, //设置参数
	Addr_Param1,
	Addr_Param2,
	Addr_Param3,
	Addr_Param4,
	Addr_Param5,
	Addr_Param6,
	Addr_Param7,
	Addr_Param8,
	Addr_Param9,
	Addr_Param10,
	Addr_Param11,
	Addr_Param12,
	Addr_Param13,
	Addr_Param14,
	Addr_Param15,
	Addr_Param16,
	Addr_Param17,
	Addr_Param18,
	Addr_Param19,
	Addr_Param20,
	Addr_Param21,
	Addr_Param22,
	Addr_Param23,
	Addr_Param24,
	Addr_Param25,
	Addr_Param26,
	Addr_Param27,
	Addr_Param28,
	Addr_Param29,
	Addr_Param30,
	Addr_Param31,
	Addr_Param32,
	Addr_Param33,
	Addr_Param34,
	Addr_Param35,
	Addr_Param36,
	Addr_Param37,
	Addr_Param38,
	Addr_Param39,
	Addr_Param40,
	Addr_Param41,
	Addr_Param42,
	Addr_Param43,
	Addr_Param44,
	Addr_Param45,
	Addr_Param46,
	Addr_Param47,
	Addr_Param48,
	Addr_Param49,
	Addr_Param50,
	Addr_Param51,
	Addr_Param52,
	Addr_Param53,
	Addr_Param54,
	Addr_Param55,
	Addr_Param56,
	Addr_Param57,
	Addr_Param58,
	Addr_Param59,
	Addr_Param60,
	Addr_Param61,
	Addr_Param62,
	Addr_Param63,
	Addr_Param64,
	Addr_Param65,
	Addr_Param66,
	Addr_Param67,
	Addr_Param68,
	Addr_Param69,
	Addr_Param70,
	Addr_Param71,
	Addr_Param72,
	Addr_Param73,
	Addr_Param74,
	Addr_Param75,
	Addr_Param76,
	Addr_Param77,
	Addr_Param78,
	Addr_Param79,
	Addr_Param80,
	Addr_Param81,
	Addr_Param82,
	Addr_Param83,
	Addr_Param84,
	Addr_Param85,
	Addr_Param86,
	Addr_Param87,
	Addr_Param88,
	Addr_Param89,
	Addr_Param90,
	Addr_Param91,
	Addr_Param92,
	Addr_Param93,
	Addr_Param94,
	Addr_Param95,
	Addr_Param96,
	Addr_Param97,
	Addr_Param98,
	Addr_Param99,
	Addr_Param100,
	Addr_Param101,
	Addr_Param102,
	Addr_Param103,
	Addr_Param104,
	Addr_Param105,
	Addr_Param106,
	Addr_Param107,
	Addr_Param108,
	Addr_Param109,
	
	Addr_Param110 = 110, //设置命令
	Addr_Param111,
	Addr_Param112,
	Addr_Param113,
	Addr_Param114,
	Addr_Param115,
	Addr_Param116,
	Addr_Param117,
	Addr_Param118,
	Addr_Param119,
	
	Addr_Param120 = 120, //监控参数
	Addr_Param121,
	Addr_Param122,
	Addr_Param123,
	Addr_Param124,
	Addr_Param125,
	Addr_Param126,
	Addr_Param127,
	Addr_Param128,
	Addr_Param129,
	Addr_Param130,
	Addr_Param131,
	Addr_Param132,
	Addr_Param133,
	Addr_Param134,
	Addr_Param135,
	Addr_Param136,
	Addr_Param137,
	Addr_Param138,
	Addr_Param139,
	Addr_Param140,
	Addr_Param141,
	Addr_Param142,
	Addr_Param143,
	Addr_Param144,
	Addr_Param145,
	Addr_Param146,
	Addr_Param147,
	Addr_Param148,
	Addr_Param149,
	Addr_Param150,
	Addr_Param151,
	Addr_Param152,
	Addr_Param153,
	Addr_Param154,
	Addr_Param155,
	Addr_Param156,
	Addr_Param157,
	Addr_Param158,
	Addr_Param159,
	Addr_Param160,
	Addr_Param161,
	Addr_Param162,
	Addr_Param163,
	Addr_Param164,
	Addr_Param165,
	Addr_Param166,
	Addr_Param167,
	Addr_Param168,
	Addr_Param169,
	Addr_Param170,
	Addr_Param171,
	Addr_Param172,
	Addr_Param173,
	Addr_Param174,
	Addr_Param175,
	Addr_Param176,
	Addr_Param177,
	Addr_Param178,
	Addr_Param179,
	Addr_Param180,
	Addr_Param181,
	Addr_Param182,
	Addr_Param183,
	Addr_Param184,
	Addr_Param185,
	Addr_Param186,
	Addr_Param187,
	Addr_Param188,
	Addr_Param189,
	Addr_Param190,
	Addr_Param191,
	Addr_Param192,
	Addr_Param193,
	Addr_Param194,
	Addr_Param195,
	Addr_Param196,
	Addr_Param197,
	Addr_Param198,
	Addr_Param199,
	Addr_Param200 = 200,
	Addr_Param201,
	Addr_Param202,
	Addr_Param203,
	Addr_Param204,
	Addr_Param205,
	Addr_Param206,
	Addr_Param207,
	Addr_Param208,
	Addr_Param209,
	Addr_Param210,
	Addr_Param211,
	Addr_Param212,
	Addr_Param213,
	Addr_Param214,
	Addr_Param215,
	Addr_Param216,
	Addr_Param217,
	Addr_Param218,
	Addr_Param219,
	Addr_Param220,
	Addr_Param221,
	Addr_Param222,
	Addr_Param223,
	Addr_Param224,
	Addr_Param225,
	Addr_Param226,
	Addr_Param227,
	Addr_Param228,
	Addr_Param229,
	Addr_Param230,
	Addr_Param231,
	Addr_Param232,
	Addr_Param233,
	Addr_Param234,
	Addr_Param235,
	Addr_Param236,
	Addr_Param237,
	Addr_Param238,
	Addr_Param239,
	Addr_Param240,
	Addr_Param241,
	Addr_Param242,
	Addr_Param243,
	Addr_Param244,
	Addr_Param245,
	Addr_Param246,
	Addr_Param247,
	Addr_Param248,
	Addr_Param249,
	Addr_Param250,
	Addr_Param251,
	Addr_Param252,
	Addr_Param253,
	Addr_Param254,
	Addr_Param255,
	Addr_Param256,

	Addr_Param257,
	Addr_Param258,
	Addr_Param259,
	Addr_Param260,
	Addr_Param261,
	Addr_Param262,
	Addr_Param263,
	Addr_Param264,
	Addr_Param265,
	Addr_Param266,
	Addr_Param267,
	Addr_Param268,
	Addr_Param269,
	Addr_Param270,
	Addr_Param271,
	Addr_Param272,
	Addr_Param273,
	Addr_Param274,
	Addr_Param275,
	Addr_Param276,
	Addr_Param277,
	Addr_Param278,
	Addr_Param279,
	Addr_Param280,
	Addr_Param281,
	Addr_Param282,
	Addr_Param283,
	Addr_Param284,
	Addr_Param285,
	Addr_Param286,
	Addr_Param287,
	Addr_Param288,
	Addr_Param289,
	Addr_Param290,
	Addr_Param291,
	Addr_Param292,
	Addr_Param293,
	Addr_Param294,
	Addr_Param295,
	Addr_Param296,
	Addr_Param297,
	Addr_Param298,
	Addr_Param299,

	Addr_Param300 = 300,
	Addr_Param301,
	Addr_Param302,
	Addr_Param303,
	Addr_Param304,
	Addr_Param305,
	Addr_Param306,
	Addr_Param307,
	Addr_Param308,
	Addr_Param309,
	Addr_Param310,
	Addr_Param311,
	Addr_Param312,
	Addr_Param313,
	Addr_Param314,
	Addr_Param315,
	Addr_Param316,
	Addr_Param317,
	Addr_Param318,
	Addr_Param319,
	Addr_Param320,
	Addr_Param321,
	Addr_Param322,
	Addr_Param323,
	Addr_Param324,
	Addr_Param325,
	Addr_Param326,
	Addr_Param327,
	Addr_Param328,
	Addr_Param329,
	Addr_Param330,
	Addr_Param331,
	Addr_Param332,
	Addr_Param333,
	Addr_Param334,
	Addr_Param335,
	Addr_Param336,
	Addr_Param337,
	Addr_Param338,
	Addr_Param339,
	Addr_Param340,
	Addr_Param341,
	Addr_Param342,
	Addr_Param343,
	Addr_Param344,
	Addr_Param345,
	Addr_Param346,
	Addr_Param347,
	Addr_Param348,
	Addr_Param349,
	Addr_Param350,
	Addr_Param351,
	Addr_Param352,
	Addr_Param353,
	Addr_Param354,
	Addr_Param355,
	Addr_Param356,

	Addr_Param357,
	Addr_Param358,
	Addr_Param359,
	Addr_Param360,
	Addr_Param361,
	Addr_Param362,
	Addr_Param363,
	Addr_Param364,
	Addr_Param365,
	Addr_Param366,
	Addr_Param367,
	Addr_Param368,
	Addr_Param369,
	Addr_Param370,
	Addr_Param371,
	Addr_Param372,
	Addr_Param373,
	Addr_Param374,
	Addr_Param375,
	Addr_Param376,
	Addr_Param377,
	Addr_Param378,
	Addr_Param379,
	Addr_Param380,
	Addr_Param381,
	Addr_Param382,
	Addr_Param383,
	Addr_Param384,
	Addr_Param385,
	Addr_Param386,
	Addr_Param387,
	Addr_Param388,
	Addr_Param389,
	Addr_Param390,
	Addr_Param391,
	Addr_Param392,
	Addr_Param393,
	Addr_Param394,
	Addr_Param395,
	Addr_Param396,
	Addr_Param397,
	Addr_Param398,
	Addr_Param399,

	Addr_Param400 = 400,
	Addr_Param401,
	Addr_Param402,
	Addr_Param403,
	Addr_Param404,
	Addr_Param405,
	Addr_Param406,
	Addr_Param407,
	Addr_Param408,
	Addr_Param409,
	Addr_Param410,
	Addr_Param411,
	Addr_Param412,
	Addr_Param413,
	Addr_Param414,
	Addr_Param415,
	Addr_Param416,
	Addr_Param417,
	Addr_Param418,
	Addr_Param419,
	Addr_Param420,
	Addr_Param421,
	Addr_Param422,
	Addr_Param423,
	Addr_Param424,
	Addr_Param425,
	Addr_Param426,
	Addr_Param427,
	Addr_Param428,
	Addr_Param429,
	Addr_Param430,
	Addr_Param431,
	Addr_Param432,
	Addr_Param433,
	Addr_Param434,
	Addr_Param435,
	Addr_Param436,
	Addr_Param437,
	Addr_Param438,
	Addr_Param439,
	Addr_Param440,
	Addr_Param441,
	Addr_Param442,
	Addr_Param443,
	Addr_Param444,
	Addr_Param445,
	Addr_Param446,
	Addr_Param447,
	Addr_Param448,
	Addr_Param449,
	Addr_Param450,
	Addr_Param451,
	Addr_Param452,
	Addr_Param453,
	Addr_Param454,
	Addr_Param455,
	Addr_Param456,

	Addr_Param457,
	Addr_Param458,
	Addr_Param459,
	Addr_Param460,
	Addr_Param461,
	Addr_Param462,
	Addr_Param463,
	Addr_Param464,
	Addr_Param465,
	Addr_Param466,
	Addr_Param467,
	Addr_Param468,
	Addr_Param469,
	Addr_Param470,
	Addr_Param471,
	Addr_Param472,
	Addr_Param473,
	Addr_Param474,
	Addr_Param475,
	Addr_Param476,
	Addr_Param477,
	Addr_Param478,
	Addr_Param479,
	Addr_Param480,
	Addr_Param481,
	Addr_Param482,
	Addr_Param483,
	Addr_Param484,
	Addr_Param485,
	Addr_Param486,
	Addr_Param487,
	Addr_Param488,
	Addr_Param489,
	Addr_Param490,
	Addr_Param491,
	Addr_Param492,
	Addr_Param493,
	Addr_Param494,
	Addr_Param495,
	Addr_Param496,
	Addr_Param497,
	Addr_Param498,
	Addr_Param499,

	Addr_Param500 = 500,
	Addr_Param501,
	Addr_Param502,
	Addr_Param503,
	Addr_Param504,
	Addr_Param505,
	Addr_Param506,
	Addr_Param507,
	Addr_Param508,
	Addr_Param509,
	Addr_Param510,
	Addr_Param511,

	//密钥
	MB_DEVICE_CODE_YEAR   	= 550,
	MB_DEVICE_CODE_MD		= 551,
	MB_DEVICE_CODE_TYPE		= 552,
	MB_DEVICE_CODE_NUM		= 553,
	MB_DEVICE_WRITE_CODE	= 554,
	MB_LECENSE_KEY			= 555,

	/* 变比系数 */
	FD_UINX_ADD_H			= 600, //交流输入电压变比系数高位
	FD_UINX_ADD_L			= 601,
	FD_IINX_ADD_H			= 602, //交流输入电流变比系数高位
	FD_IINX_ADD_L			= 603,
	FD_UDC_ADD_H			= 604, //直流电压变比系数高位
	FD_UDC_ADD_L			= 605,
	FD_IDC_ADD_H			= 606, //直流电流变比系数高位
	FD_IDC_ADD_L			= 607,
	FD_UBTR_ADD_H			= 608, //电池电压变比系数高位
	FD_UBTR_ADD_L			= 609,	
	FD_UPOS_GND_ADD_H		= 610, //正极对地电压变比系数高位
	FD_UPOS_GND_ADD_L		= 611,
	FD_UNEG_GND_ADD_H		= 612, //负极对地电压变比系数高位
	FD_UNEG_GND_ADD_L		= 613,	
	FD_PAC_ADD_H			= 614, //交流功率变比系数高位
	FD_PAC_ADD_L			= 615,	
	FD_PDC_ADD_H			= 616, //直流功率变比系数高位
	FD_PDC_ADD_L			= 617,	
	FD_IAC_CT_ADD_H			= 618, //传感器交流电流变比高位
	FD_IAC_CT_ADD_L			= 619,
	FD_PAC_CT_ADD_H			= 620, //传感器交流功率变比高位
	FD_PAC_CT_ADD_L			= 621,
	FD_IAC_LKG_H			= 622, //系数漏电流变比高位
	FD_IAC_LKG_L			= 623, //漏电流变比系数低位

	RESET_FLAG				= 647, //复位标志位
	DEBUG_ONOFF				= 648, //调试打印开关
	Ctrl_Mode_SET			= 649,	//PCS控制模式设置
	Run_Mode_Set			= 650,	//运行模式选择	
	Communication_En		= 651,	//通讯保护使能	
	Strategy_En 			= 652,	//策略选择
	MP_INI_MONITOR_ADDR 	= 653, //监控机地址
	
	MP_CHRG_VOL_LIMIT		= 660, //充电限压值
	MP_DISC_VOL_LIMIT		= 661, //放电限压值
	MP_SVG_EN				= 662, //SVG使能
	MP_ISO_ISLAND			= 663, //孤岛保护
	MP_INI_RESIS_FD			= 664, //绝缘监测系数
	MP_INI_RESIS_REF		= 665, //绝缘判定门限
	MP_MAX_P_ADD			= 666, //最大交流有功功率值
	MP_MIN_P_ADD			= 667, //最小交流有功功率值
	MP_MAX_BA_C_ADD			= 668, //最大直流电流值
	MP_MIN_BA_C_ADD			= 669, //最小直流电流值
	MP_MAX_BA_U_ADD			= 670, //最大直流电压
	MP_MIN_BA_U_ADD			= 671, //最小直流电压
	MP_MAX_Q_ADD			= 672, //最大交流无功功率值
	MP_MIN_Q_ADD			= 673, //最小交流无功功率值
	MP_LOACAL_ASK			= 674, //本地请求
	MP_C_LIFE				= 675, //电容寿命值

	//增加保护值设定
	AC_FUN_START_TEMP		= 680,//交流柜风机启动温度
	AC_FUN_STOP_TEMP		= 681,//交流柜风机停止运行温度

	FILTER_CAPAC_ERR_TEMP   = 682,//滤波电容过温点
	TRANSFORMER_ERR_TEMP    = 683,//控制变压器过温点
	BUS_CAPAC_ERR_TEMP		= 684,//母线电容过温点
	ELEC_REACTOR_ERR_TEMP	= 685,//电抗器过温点
	AC_CABINET_ERR_TEMP		= 686,//交流柜过温点

	//削封填谷设置参数
	AMMETER1_A0				= 700, //电表1的地址A0
	AMMETER1_A1				= 701, //电表1的地址A1
	AMMETER1_A2				= 702, //电表1的地址A2
	AMMETER1_A3				= 703, //电表1的地址A3
	AMMETER1_A4				= 704, //电表1的地址A4
	AMMETER1_A5				= 705, //电表1的地址A5
	AMMETER1_VRC			= 706, //电表1变比系数
	AMMETER2_A0				= 707, //电表2的地址A0
	AMMETER2_A1				= 708, //电表2的地址A1
	AMMETER2_A2				= 709, //电表2的地址A2
	AMMETER2_A3				= 710, //电表2的地址A3
	AMMETER2_A4				= 711, //电表2的地址A4
	AMMETER2_A5				= 712, //电表2的地址A5
	AMMETER2_VRC			= 713, //电表2变比系数
	DIS_MIN_POWER			= 714,   //从电网获取最小功率
	COS_SETA_PC_SET         = 715,	//功率因素给定
	MB_XFTG_EN 				= 716, 
	MB_CAPACITY				= 717, 
	MB_START_TIME_0			= 718, 
	MB_STOP_TIME_0			= 719, 
	MB_SET_POWER_0			= 720, 
	MB_START_TIME_1			= 721, 
	MB_STOP_TIME_1			= 722, 
	MB_SET_POWER_1			= 723, 
	MB_START_TIME_2			= 724, 
	MB_STOP_TIME_2			= 725, 
	MB_SET_POWER_2			= 726, 
	MB_START_TIME_3			= 727, 
	MB_STOP_TIME_3			= 728, 
	MB_SET_POWER_3			= 729, 
	MB_START_TIME_4			= 730, 
	MB_STOP_TIME_4			= 731, 
	MB_SET_POWER_4			= 732, 
	MB_START_TIME_5			= 733, 
	MB_STOP_TIME_5			= 734, 
	MB_SET_POWER_5			= 735, 
	MB_START_TIME_6			= 736, 
	MB_STOP_TIME_6			= 737, 
	MB_SET_POWER_6			= 738, 
	MB_START_TIME_7			= 739, 
	MB_STOP_TIME_7			= 740, 
	MB_SET_POWER_7			= 741, 	

	//特殊设置
	MB_SB_EN				= 750, //软启使能
	MB_SB_TIME				= 751, //软启时间
	MB_SB_GOPWR				= 752, //软启目标功率
	MB_FR_EN				= 753, //故障恢复使能
	MB_FR_WTIME				= 754, //故障恢复等待时间
	MB_CO_EN                = 755, //通讯掉线停机使能
	MB_CO_JTIME             = 756, //通讯掉线判定时间
	MB_BOOTUP_TIME          = 757, //启动时间

	//调频参数
	MB_AGC_EN 				= 760, //调频使能
	MB_FRE_OVER				= 761,//频率适应性上限比较值
	MB_FRE_UNDER			= 762,//频率适应性下限比较值

	//频率保护
	MB_OF_Thr_1				= 790, //一级过频门限
	MB_OF_Thr_2				= 791, //二级过频门限
	MB_UF_Thr_1				= 792, //一级欠频门限
	MB_UF_Thr_2				= 793, //二级欠频门限
	MB_OF_Time_1			= 794, //一级过频时间
	MB_OF_Time_2			= 795, //二级过频时间
	MB_UF_Time_1			= 796, //一级欠频时间
	MB_UF_Time_2			= 797, //二级欠频时间

	/* 电量 */
	MP_POWER_AC_DAY_H		= 800, //AC当日发电量
	MP_POWER_AC_DAY_L		= 801,
	MP_POWER_AC_H			= 802, //AC总发电量
	MP_POWER_AC_L			= 803, 	
	MP_POWER_AC_C_DAY_H		= 804, //AC支路日输入电量
	MP_POWER_AC_C_DAY_L		= 805, 
	MP_POWER_AC_C_H			= 806, //AC支路总输入电量
	MP_POWER_AC_C_L			= 807, 
	MP_POWER_DC1_DAY_H		= 808, //DC1支路日输出电量
	MP_POWER_DC1_DAY_L		= 809, 
	MP_POWER_DC1_CON_H		= 810, //DC1支路总输出电量
	MP_POWER_DC1_CON_L		= 811, 
	MP_POWER_DC1_C_DAY_H	= 812, //DC1支路日输入电量
	MP_POWER_DC1_C_DAY_L	= 813, 
	MP_POWER_DC1_C_CON_H	= 814, //DC1支路总输入电量
	MP_POWER_DC1_C_CON_L	= 815, 
	MP_POWER_DC2_DAY_H		= 816, //DC2支路日输出电量
	MP_POWER_DC2_DAY_L		= 817, 
	MP_POWER_DC2_CON_H		= 818, //DC2支路总输出电量
	MP_POWER_DC2_CON_L		= 819, 
	MP_CO2_H           		= 820, //CO2减排量
	MP_CO2_L           		= 821, 
	MP_C_H           		= 822, //C减排量
	MP_C_L           		= 823, 
	MP_PVES_TIME_CON_H		= 824, //光储供电总运行时间
	MP_PVES_TIME_CON_L		= 825, 
	
	MP_SYS_STATUS			= 850, //系统运行状态: 0-停机 1-运行 2-故障
	MP_ARM_VISION			= 851, //当前系统ARM软件编号
	MP_PARA_VISION			= 852, //当前参数版本号
	
	MP_TEMP1 				= 862,
	MP_TEMP2 				= 863,
	MP_TEMP3 				= 864,
	MP_TEMP4 				= 865,
	MP_TEMP5 				= 866,
	MP_TEMP6 				= 867,

	W_H 					= 886,//频率高位
	W_L 					= 887,//频率低位


	UL_strtg_en_Addr		= 900,// 美标策略使能
	PF_x100_Addr, //	功率因数值
	VQ_V1_Addr, //	电压无功_电压值1
	VQ_V2_Addr, //	电压无功_电压值2
	VQ_V3_Addr, //	电压无功_电压值3
	VQ_V4_Addr, //	电压无功_电压值4
	VQ_Q1_Addr, //	电压无功_无功值1
	VQ_Q2_Addr, //	电压无功_无功值2
	VQ_Q3_Addr, //	电压无功_无功值3
	VQ_Q4_Addr, //	电压无功_无功值4 
	PQ_n_P3_Addr, //	有功无功_负轴_有功值3
	PQ_n_P2_Addr, //	有功无功_负轴_有功值2
	PQ_n_P1_Addr, //	有功无功_负轴_有功值1
	PQ_p_P1_Addr, //	有功无功_正轴_有功值1
	PQ_p_P2_Addr, //	有功无功_正轴_有功值2
	PQ_p_P3_Addr, //	有功无功_正轴_有功值3 
	PQ_n_Q3_Addr, //	有功无功_负轴_无功值3
	PQ_n_Q2_Addr, //	有功无功_负轴_无功值2
	PQ_n_Q1_Addr, //	有功无功_负轴_无功值1
	PQ_p_Q1_Addr, //	有功无功_正轴_无功值1
	PQ_p_Q2_Addr, //	有功无功_正轴_无功值2
	PQ_p_Q3_Addr, //	有功无功_正轴_无功值3 
	VP_V1_Addr, //	电压有功_电压值1
	VP_V2_Addr, //	电压有功_电压值2
	VP_P1_Addr, //	电压有功_有功值1
	VP_P2_Addr, //	电压有功_有功值2
	Uac_rate_Addr, //	交流额定电压
	P_rate_Addr, // 交流额定功率
	P_max_Addr, //	交流最大输出功率
	PF_min_x100_Addr, //允许最小的功率因数
	Lpf_times_Addr, //滤波点数
	Freq_rate_Addr, //频率额定值
	Pfr_dbUF_Addr, //一次调频欠频死区
	Pfr_kUF_Addr, //一次调频欠频不等率
    Pfr_dbOF_Addr, //一次调频过频死区
    Pfr_kOF_Addr, //一次调频过频不等率
    Pfr_Tresp_ms_Addr, //一次调频响应时间（单位ms）

	MAX_PARA_ID
};


/* 触摸屏部分使用寄存器地址(发电量信息) */
#define POWER_DATA_SIZE				(1288)		//所需要记录的发电量信息数量(根据NVROM确定的空间大小来给定此值，应小于2000)
#define POWER_BASE_ADDR				(3000 + 600)//记录电量信息基准地址
#define POWER_RECORD_HOUR			(24)
#define POWER_RECORD_DATE			(31)
#define POWER_RECORD_MONTH			(12)
#define POWER_AC_HOUR_BASE_ADDR		(3000 + 600 - POWER_BASE_ADDR)
#define POWER_AC_HOUR_ADDR_OFFSET	(48)		//7天同一小时时间段发电量地址等差数值
#define POWER_AC_DAY_BASE_ADDR		(3000 + 936 - POWER_BASE_ADDR)
#define POWER_AC_DAY_ADDR_OFFSET	(62)		//31天同一天时间段发电量地址等差数值
#define POWER_AC_MON_BASE_ADDR		(3000 + 1122 - POWER_BASE_ADDR)
#define POWER_AC_MON_ADDR_OFFSET	(24)		//12月同一月时间段发电量地址等差数值
#define POWER_DC2_HOUR_BASE_ADDR	(3000 + 1242 - POWER_BASE_ADDR)
#define POWER_DC2_HOUR_ADDR_OFFSET	(48)		//7天同一小时时间段发电量地址等差数值
#define POWER_DC2_DAY_BASE_ADDR		(3000 + 1578 - POWER_BASE_ADDR)
#define POWER_DC2_DAT_ADDR_OFFSET	(62)		//31天同一天时间段发电量地址等差数值
#define POWER_DC2_MON_BASE_ADDR		(3000 + 1764 - POWER_BASE_ADDR)
#define POWER_DC2_MON_ADDR_OFFSET	(24)		//12月同一月时间段发电量地址等差数值
#define POWER_AC_C_HOUR_BASE_ADDR	(3000 + 1884 - POWER_BASE_ADDR)
#define POWER_AC_C_HOUR_ADDR_OFFSET	(48)		//7天同一小时时间段充电量地址等差数值
#define POWER_AC_C_DAY_BASE_ADDR	(3000 + 2220 - POWER_BASE_ADDR)
#define POWER_AC_C_DAY_ADDR_OFFSET	(62)		//31天同一天时间段充电量地址等差数值
#define POWER_AC_C_MON_BASE_ADDR	(3000 + 2406 - POWER_BASE_ADDR)
#define POWER_AC_C_MON_ADDR_OFFSET	(24)		//12月同一月时间段充电量地址等差数值



typedef struct
{
	unsigned char *name;
	unsigned int id;
} para_information;


typedef struct
{
	short Sys_State1;
	short Sys_State2;
	short Alarm_State1;
	short Alarm_State2;
	short Sys_Err1;
	short Sys_Err2;
	short com_err;
	short Alarm_HMI;
	short PF;
} out_state_info_t;

typedef struct
{
	short p;
	short q;
} PQNonSVG_info_t;


typedef struct
{
	float Power_Con;					//AC总发电量

	/* AC放电电量 */
	float power_ac_day;					//AC支路日放电量
	float power_ac_con;					//AC支路总放电量
	float power_ac_yesterday;			//AC支路昨日放电量
	float power_ac_month;				//AC支路月放电量
	float power_ac_year;				//AC支路年放电量
	short power_ac_con_h;				//AC支路总放电量_高位(用于修改)
	short power_ac_con_l;				//AC支路总放电量_低位(用于修改)
	short power_ac_day_h;				//AC支路日放电量_高位(用于修改)
	short power_ac_day_l;				//AC支路日放电量_低位(用于修改)
	short power_ac_yesterday_h;			//AC支路昨日放电量_高位(用于修改)
	short power_ac_yesterday_l;			//AC支路昨日放电量_低位(用于修改)
	/* AC充电电量 */
	float power_ac_c_day;				//AC支路日充电量
	float power_ac_c_con;				//AC支路总充电量
	float power_ac_c_yesterday;			//AC支路昨日充电量
	float power_ac_c_month;				//AC支路月充电量
	float power_ac_c_year;				//AC支路年充电量
	/* DC1放电电量 */
	float power_dc1_day;				//DC1支路日放电量
	float power_dc1_con;				//DC1支路总放电量
	/* DC1充电电量 */
	float power_dc1_c_day;				//DC1支路日充电量
	float power_dc1_c_con;				//DC1支路总充电量
	/* DC2放电电量 */
	float power_dc2_day;				//DC2支路日放电量
	float power_dc2_con;				//DC2支路总放电量
	float power_dc2_yesterday;			//DC2支路昨日放电量
	float power_dc2_month;				//DC2支路月放电量
	/* DC2充电电量(保留) */
	
	float power_ac_day_cache;			//AC侧每小时发电量缓冲区
	float power_ac_c_day_cache;			//AC侧每小时充电量缓冲区
	float power_dc2_day_cache;			//DC2侧每小时发电量缓冲区

	float co2_dec;						//CO2减排量(根据总发电量折算)
	float C_dec;						//标煤节约量(根据总发电量折算)
	
	float run_time_total;				//总运行时间
	float run_time_old;					
	float run_time_now;					//连续运行时间
	
	float on_time_total;				//程序总运行时间
	float on_time_old;					
	float on_time_now;					//程序连续运行时间	
	
	float fre_adjust_count;				//一次调频动作总计数
	float fre_adjust_count_old;					
	float fre_adjust_count_now;			//一次调频		
	float fre_adjust_count_today;		//一次调频当天动作计数	

	float fre_lock_count;				//一次调频闭锁总计数
	float fre_lock_count_old;					
	float fre_lock_count_now;			//一次调频闭锁		
	float fre_lock_count_today;			//一次调频闭锁当天动作计数		
	
	float restart_tick;					//程序复位次数
	float crc_err_count;		
}_CountData;


typedef struct
{
	short time_year_get;
	short time_month_get;
	short time_date_get;
	short time_ap_get;
	short time_week_get;
	short time_hour_get;
	short time_minute_get;
	short time_second_get;	

	short time_year_set;
	short time_month_set;
	short time_date_set;
	short time_ap_set;
	short time_week_set;
	short time_hour_set;
	short time_minute_set;
	short time_second_set;
}_sys_time;

typedef struct
{
	unsigned int	power_time[POWER_DATA_SIZE];	
	float			power_value[POWER_DATA_SIZE];
}_power_data;

typedef struct
{

/*BMS长旺项目用*/
	unsigned short life_count_l; 
	unsigned short life_count_h;	
	
	unsigned short sysstate_h;
	unsigned short sysstate_l;
	
	unsigned short discharge_pre_power;
	unsigned short discharge_pre_power_h;
	unsigned short discharge_pre_power_l;
	
	unsigned short charge_pre_power;
	unsigned short charge_pre_power_h;
	unsigned short charge_pre_power_l;
	
	unsigned short q_discharge_h;
	unsigned short q_discharge_l;

	unsigned short q_charge_h;
	unsigned short q_charge_l;

	unsigned short bms_voltalge_h;
	unsigned short bms_voltalge_l;
	unsigned short bms_voltalge;
	
	unsigned short bms_current;	
	unsigned short bms_current_h;
	unsigned short bms_current_l;
	
	unsigned short bms_soc_h;
	unsigned short bms_soc_l;
	
	unsigned short battery1_waring;
	unsigned short battery1_err;
	
	unsigned short battery2_waring;
	unsigned short battery2_err;
	
	unsigned short battery3_waring;
	unsigned short battery3_err;
	
	unsigned short battery4_waring;
	unsigned short battery4_err;

	unsigned short battery5_waring;
	unsigned short battery5_err;
	
	unsigned short battery6_waring;
	unsigned short battery6_err;

	unsigned short battery7_waring;
	unsigned short battery7_err;

	unsigned short battery8_waring;
	unsigned short battery8_err;

	unsigned short battery9_waring;
	unsigned short battery9_err;
	
	unsigned short battery10_waring;
	unsigned short battery10_err;
		
}_MonitorData;


extern short para_list[];
extern para_information para_inf[];
extern const int paranum;
extern _sys_time Arm_time;
extern _CountData CountData;
extern out_state_info_t out_state_info;
extern PQNonSVG_info_t PQNonSVG;
extern short Upper_Param[];


int setpara(unsigned int id,short value);
int readpara(unsigned int id,short *value);
void para_init(void);
int power_clear(void);
int time_clear(void);

unsigned char set_power_data(unsigned int bcd_time,unsigned int id, float value);
unsigned char read_power_data(unsigned int id, float* value);
unsigned char read_power_time(unsigned int id, unsigned int* bcd_time);

#endif

