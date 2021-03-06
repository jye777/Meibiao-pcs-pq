#ifndef LOG_DEF_H
#define LOG_DEF_H

//日志 - 故障
#define ET_FAULT			0		//故障

#define EST_Uina_OV1			0		//电网电压A相1级过压
#define EST_Uina_OV2			1		//电网电压A相2级过压
#define EST_Uina_OV3			2		//电网电压A相3级过压
#define EST_Uina_UV1			3		//电网电压A相1级欠压
#define EST_Uina_UV2			4		//电网电压A相2级欠压
#define EST_Uina_UV3			5		//电网电压A相3级欠压

#define EST_Uinb_OV1			6		//电网电压B相1级过压
#define EST_Uinb_OV2			7		//电网电压B相2级过压
#define EST_Uinb_OV3			8		//电网电压B相3级过压
#define EST_Uinb_UV1			9		//电网电压B相1级欠压
#define EST_Uinb_UV2			10		//电网电压B相2级欠压
#define EST_Uinb_UV3			11		//电网电压B相3级欠压

#define EST_Uinc_OV1			12		//电网电压C相1级过压
#define EST_Uinc_OV2			13		//电网电压C相2级过压
#define EST_Uinc_OV3			14		//电网电压C相3级过压
#define EST_Uinc_UV1			15		//电网电压C相1级欠压
#define EST_Uinc_UV2			16		//电网电压C相2级欠压
#define EST_Uinc_UV3			17		//电网电压C相3级欠压

#define EST_Iina_OI1			18		//电网电流A相1级过流
#define EST_Iina_OI2			19		//电网电流A相2级过流
#define EST_Iina_OI3			20		//电网电流A相3级过流

#define EST_Iinb_OI1			21		//电网电流B相1级过流
#define EST_Iinb_OI2			22		//电网电流B相2级过流
#define EST_Iinb_OI3			23		//电网电流B相3级过流

#define EST_Iinc_OI1			24		//电网电流C相1级过流
#define EST_Iinc_OI2			25		//电网电流C相2级过流
#define EST_Iinc_OI3			26		//电网电流C相3级过流

#define EST_Udc_OV1				27		//母线1级过压
#define EST_Udc_OV2				28		//母线2级过压
#define EST_Udc_OV3				29		//母线3级过压
#define EST_Udc_UV1				30		//母线1级欠压
#define EST_Udc_UV2				31		//母线2级欠压
#define EST_Udc_UV3				32		//母线3级欠压
#define EST_Idc_OV1				33		//母线1级过流
#define EST_Idc_OV2				34		//母线2级过流
#define EST_Idc_OV3				35		//母线3级过流

#define EST_Ubtra_OV1			36      //Ⅰ号电池组1级过压
#define EST_Ubtra_OV2			37      //Ⅰ号电池组2级过压
#define EST_Ubtra_OV3			38      //Ⅰ号电池组3级过压
#define EST_Ubtra_UV1			39      //Ⅰ号电池组1级欠压
#define EST_Ubtra_UV2			40      //Ⅰ号电池组2级欠压
#define EST_Ubtra_UV3			41      //Ⅰ号电池组3级欠压

#define EST_UbtrIsltn_OV	    42		//电池电压绝缘检测故障

#define EST_Uina_inst_OI		43		//A相电压瞬时值过压
#define EST_Uinb_inst_OI		44		//B相电压瞬时值过压
#define EST_Uinc_inst_OI		45		//C相电压瞬时值过压
#define EST_Iina_inst_OI		46		//A相电流瞬时值过流
#define EST_Iinb_inst_OI		47		//B相电流瞬时值过流
#define EST_Iinc_inst_OI		48		//C相电流瞬时值过流

#define EST_Udc_Inst_OV			49		//母线电压瞬时过压
#define EST_Idc_Inst_OI			50		//母线电流瞬时过流
#define EST_Ubtr_Inst_OV		51		//电池电压瞬时过压

#define EST_IGBTA_Err0		   	52      //A相IGBT故障0
#define EST_IGBTA_Err1			53      //A相IGBT故障1
#define EST_IGBTB_Err0			54		//B相IGBT故障0
#define EST_IGBTB_Err1			55		//B相IGBT故障1
#define EST_IGBTC_Err0			56		//C相IGBT故障0
#define EST_IGBTC_Err1			57		//C相IGBT故障1

#define EST_Iina_Zero_Err		58		//电网电流A相回零故障
#define EST_Iinb_Zero_Err		59		//电网电流B相回零故障
#define EST_Iinc_Zero_Err 		60		//电网电流C相回零故障
#define EST_Ibtra_Zero_Err 		61		//直流电流回零故障

#define EST_ProChargeFault_Err  62		//预充电故障
#define EST_Q1AndQ3_Err			63		//交直流主断路器故障

#define EST_F1AndF2_Err			64		//防雷器故障
#define EST_T1_KT3_OT			65		//变压器过温
#define EST_KM1_Err				66		//主接触器故障

#define EST_Frq_OF1				67		//1级过频
#define EST_Frq_OF2				68		//2级过频
#define EST_Frq_OF3				69		//3级过频
#define EST_Frq_UF1				70		//1级欠频
#define EST_Frq_UF2				71		//2级欠频
#define EST_Frq_UF3				72		//3级欠频

#define EST_IGBT_Temp_OV		73		//IGBT温度过温
#define EST_AD_Err				74      //AD驱动故障
#define EST_Em_In				75      //软件急停
#define EST_CabinetHeatOV		76		//柜内过耗
#define EST_Em_BUtton			77		//按钮急停
#define EST_BMS_Em_Err			78		//BMS急停
#define EST_RESERVED_19			79		//保留
#define EST_ERR_CONNCET			80		//反接
#define EST_IacLkg				81		//漏电流保护
#define EST_Phase_Err			82		//相序错误


#define EST_Filter_Temp_Over	120     //滤波电容过温故障

//日志 - 板间通信
#define ET_CBB			1	//板间通信

#define EST_BMS_OUTLINE				0		//bms掉线
#define EST_BMS_OUTLINE_REC			1		//bms掉线恢复

#define EST_METER_OUTLINE   		2		//pcs与电能表掉线
#define EST_METER_OUTLINE_REC   	3		//pcs与电能表掉线恢复

#define EST_PLC_OUTLINE   			4		//pcs与PLC 掉线
#define EST_PLC_OUTLINE_REC   		5		//pcs与PLC 掉线恢复

#define EST_EMS_OUTLINE   			6		//pcs与EMS 掉线
#define EST_EMS_OUTLINE_REC   		7		//pcs与EMS 掉线恢复

//日志 - 运行状态

#define ET_OPSTATE		2	//系统运行状态

#define EST_SYSBOOT		0	//系统启动
#define EST_OPEN		1	//故障自开机
#define EST_CLOSE   	2	//故障自清除
#define EST_EMS_OPEN		3	//AGC开机
#define EST_EMS_CLOSE		4	//AGC关机
#define EST_HMI_OPEN		5	//触摸屏开机
#define EST_HMI_CLOSE   	6	//触摸屏关机
#define EST_MB_OPEN			7	//上位机开机
#define EST_MB_CLOSE   		8	//上位机关机
#define EST_BMS_CHARG_CHANGE		9	//bms充电功率变化
#define EST_BMS_DISCHARG_CHANGE   	10	//bms放电功率变化

#define EST_OVERLOAD1_ERR        11  //设备过载保护
#define EST_OVERLOAD2_ERR        12  //设备过载保护
#define EST_OVERLOAD3_ERR        13  //设备过载保护

#define EST_AUTO_OPEN			14	//自动开机
#define EST_AUTO_CLOSE			15	//自动关机
#define EST_EMS_ESTOP			16 //ems急停

#define EST_BMS_ESTOP			17 //Bms故障停机
#define EST_DOOR_ESTOP			18 //柜门打开pcs停机
#define EST_FIRE_ESTOP			19 //消防动作

#define EST_INPUT				20 //原网荷动作
#define EST_INPUT_CLOSE			21 //原网荷动作消失

#define EST_HTEP_ALM			22 //高温告警
#define EST_HTEP_ALM_DIS		23 //高温告警消失


#define EST_BMS_CHARG_500   25 //BMS功率变化为-500kw
#define EST_BMS_CHARG_250   26 //BMS功率变化为-250kw
#define EST_BMS_CHARG_0   	27 //BMS功率变化为0kw

#define EST_BMS_DISCHARG_500   28 //BMS功率变化为500kw
#define EST_BMS_DISCHARG_250   29 //BMS功率变化为250kw
#define EST_BMS_DISCHARG_0     30 //BMS功率变化为0kw
#define EST_BMS_POWER_CHANGE   31 //BMS功率变化
#define EST_BMS_POWER_CH_CHANGE   32 //BMS功率变化
#define EST_BMS_POWER_DIS_CHANGE   33 //BMS功率变化

#define EST_FRE_ADJUST_ABLE    	  34 //一次调频动作
#define EST_FRE_ADJUST_DISABLE    35 //一次调频动作结束

#define EST_PCSRUNMODE_CHANGE     36 //pcs运行模式切换
#define EST_PROCEDURE_UPDATE      37 //固件更新停机

#define EST_FRE_ADJUST_LOCK    	  38 //一次调频闭锁
#define EST_FRE_ADJUST_LOCKOVER   39 //一次调频闭锁结束

#define EST_CODE_INVALID   		  40 //密钥过期
#define EST_ERR_CODE   		      41 //密钥错误



//日志 -  开关动作
#define ET_SWITCH				3	//开关动作

#define EST_Q1AndQ3_St_OPEN		0	//断路器吸合
#define EST_F1AndF2_St_OPEN		1	//防雷器吸合
#define EST_KM1_St_OPEN			2	//并网接触器合
#define EST_T1_KT3_St_Err		3	//PCS过温
#define EST_BMS_Em_St_Err		4	//BMS急停信号输入
#define EST_Em_St_Err			5	//急停信号输入
#define EST_OutSw3_St_Err		6	//外部开关信号输入
#define EST_Door_St_Open		7	//柜门打开

#define EST_Q1AndQ3_St_CLOSED	16	//断路器断开
#define EST_F1AndF2_St_CLOSED	17	//防雷器断开
#define EST_KM1_St_CLOSED		18	//并网接触器断
#define EST_T1_KT3_St_Normal	19	//PCS温度正常
#define EST_BMS_Em_St_Normal	20	//BMS急停信号输入正常
#define EST_Em_St_Normal		21	//急停信号输入正常
#define EST_OutSw3_St_Normal	22	//外部开关信号输入正常
#define EST_Door_St_Close		23	//柜门关闭

#define EST_First_VF			32	//第一台VF源启动
#define EST_Lvrting				33	//低压穿越中
#define EST_UnBlc				34	//电压不平衡
#define EST_PwrRdc_On			35	//过温限制功率启动
#define EST_CtlLoop_On			36	//控制环启动
#define EST_Uac_UV_En			37	//AC欠压检测使能
#define EST_VF_Mode				38	//进入VF模式
#define EST_Excite_On			39	//正在励磁

#define EST_First_VF_End		48	//第一台VF源启动结束
#define EST_Lvrting_End			49	//低压穿越结束
#define EST_UnBlc_End			50	//电压不平衡结束
#define EST_PwrRdc_End			51	//过温限制功率结束
#define EST_CtlLoop_End			52	//控制环启动结束
#define EST_Uac_UV_End			53	//AC欠压检测不使能
#define EST_VF_Mode_End			54	//退出VF模式
#define EST_Excite_End			55	//励磁结束

#define EST_IGBT_HotOn_Drv		64	//IGBT加热启动
#define EST_KM1_Drv				65	//主接触器驱动开
#define EST_Fault_Drv			66	//故障指示输出
#define EST_PIGBT_Cool_Drv		68	//IGBT散热
#define EST_Charge_Drv			70	//交流预充电
#define EST_DCBrkTrip			71	//直流断路器分励脱扣

#define EST_IGBT_HotOn_Drv_End	80	//IGBT加热停止
#define EST_KM1_Drv_End			81	//主接触器驱动关
#define EST_Fault_Drv_End		82	//故障指示输出停止
#define EST_PIGBT_Cool_Drv_End	84	//IGBT散热停止
#define EST_Charge_Drv_End		86	//交流预充电停止
#define EST_DCBrkTrip_End		87	//直流断路器分励脱扣驱动关

//日志 - 主控通信
#define ET_MCCOMM	4	//主控通信

//日志 - 操作提示
#define ET_TIP		5
#define EST_OPEN_EN   			0	//开机命令使能
#define EST_CLOSE_EN			1   //关机命令使能
#define EST_ZERO_CAL_EN    		3	//教零
#define EST_ZERO_CAL_CLOSE_EN   4	//取消教�

#define EST_OPERN_DISABLE   	16	//开机命令使能

#define EST_LVRT_EN				64  	//低穿使能

#define EST_LVRT_DISABLE		80  	//低穿使能

//日志 -模式切换
#define ET_MODE 6
#define EST_VF_MODE				0   //VF模式
#define EST_PQ_MODE				1	//PQ模式
#define EST_IV_MODE				2	//V模式
#define EST_CP_MODE				3	
#define EST_CI_MODE				4	
#define EST_CU_MODE				5	
#define EST_VSG_MODE			6	

#define EST_EMS_CTR				10
#define EST_BMS_CTR				11
#define EST_HMI_CTR				12


//当前故障 7

//日志-策略使能提示
#define ET_STRATEGY 8

#endif

