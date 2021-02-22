#ifndef LOG_DEF_H
#define LOG_DEF_H

//��־ - ����
#define ET_FAULT			0		//����

#define EST_Uina_OV1			0		//������ѹA��1����ѹ
#define EST_Uina_OV2			1		//������ѹA��2����ѹ
#define EST_Uina_OV3			2		//������ѹA��3����ѹ
#define EST_Uina_UV1			3		//������ѹA��1��Ƿѹ
#define EST_Uina_UV2			4		//������ѹA��2��Ƿѹ
#define EST_Uina_UV3			5		//������ѹA��3��Ƿѹ

#define EST_Uinb_OV1			6		//������ѹB��1����ѹ
#define EST_Uinb_OV2			7		//������ѹB��2����ѹ
#define EST_Uinb_OV3			8		//������ѹB��3����ѹ
#define EST_Uinb_UV1			9		//������ѹB��1��Ƿѹ
#define EST_Uinb_UV2			10		//������ѹB��2��Ƿѹ
#define EST_Uinb_UV3			11		//������ѹB��3��Ƿѹ

#define EST_Uinc_OV1			12		//������ѹC��1����ѹ
#define EST_Uinc_OV2			13		//������ѹC��2����ѹ
#define EST_Uinc_OV3			14		//������ѹC��3����ѹ
#define EST_Uinc_UV1			15		//������ѹC��1��Ƿѹ
#define EST_Uinc_UV2			16		//������ѹC��2��Ƿѹ
#define EST_Uinc_UV3			17		//������ѹC��3��Ƿѹ

#define EST_Iina_OI1			18		//��������A��1������
#define EST_Iina_OI2			19		//��������A��2������
#define EST_Iina_OI3			20		//��������A��3������

#define EST_Iinb_OI1			21		//��������B��1������
#define EST_Iinb_OI2			22		//��������B��2������
#define EST_Iinb_OI3			23		//��������B��3������

#define EST_Iinc_OI1			24		//��������C��1������
#define EST_Iinc_OI2			25		//��������C��2������
#define EST_Iinc_OI3			26		//��������C��3������

#define EST_Udc_OV1				27		//ĸ��1����ѹ
#define EST_Udc_OV2				28		//ĸ��2����ѹ
#define EST_Udc_OV3				29		//ĸ��3����ѹ
#define EST_Udc_UV1				30		//ĸ��1��Ƿѹ
#define EST_Udc_UV2				31		//ĸ��2��Ƿѹ
#define EST_Udc_UV3				32		//ĸ��3��Ƿѹ
#define EST_Idc_OV1				33		//ĸ��1������
#define EST_Idc_OV2				34		//ĸ��2������
#define EST_Idc_OV3				35		//ĸ��3������

#define EST_Ubtra_OV1			36      //��ŵ����1����ѹ
#define EST_Ubtra_OV2			37      //��ŵ����2����ѹ
#define EST_Ubtra_OV3			38      //��ŵ����3����ѹ
#define EST_Ubtra_UV1			39      //��ŵ����1��Ƿѹ
#define EST_Ubtra_UV2			40      //��ŵ����2��Ƿѹ
#define EST_Ubtra_UV3			41      //��ŵ����3��Ƿѹ

#define EST_UbtrIsltn_OV	    42		//��ص�ѹ��Ե������

#define EST_Uina_inst_OI		43		//A���ѹ˲ʱֵ��ѹ
#define EST_Uinb_inst_OI		44		//B���ѹ˲ʱֵ��ѹ
#define EST_Uinc_inst_OI		45		//C���ѹ˲ʱֵ��ѹ
#define EST_Iina_inst_OI		46		//A�����˲ʱֵ����
#define EST_Iinb_inst_OI		47		//B�����˲ʱֵ����
#define EST_Iinc_inst_OI		48		//C�����˲ʱֵ����

#define EST_Udc_Inst_OV			49		//ĸ�ߵ�ѹ˲ʱ��ѹ
#define EST_Idc_Inst_OI			50		//ĸ�ߵ���˲ʱ����
#define EST_Ubtr_Inst_OV		51		//��ص�ѹ˲ʱ��ѹ

#define EST_IGBTA_Err0		   	52      //A��IGBT����0
#define EST_IGBTA_Err1			53      //A��IGBT����1
#define EST_IGBTB_Err0			54		//B��IGBT����0
#define EST_IGBTB_Err1			55		//B��IGBT����1
#define EST_IGBTC_Err0			56		//C��IGBT����0
#define EST_IGBTC_Err1			57		//C��IGBT����1

#define EST_Iina_Zero_Err		58		//��������A��������
#define EST_Iinb_Zero_Err		59		//��������B��������
#define EST_Iinc_Zero_Err 		60		//��������C��������
#define EST_Ibtra_Zero_Err 		61		//ֱ�������������

#define EST_ProChargeFault_Err  62		//Ԥ������
#define EST_Q1AndQ3_Err			63		//��ֱ������·������

#define EST_F1AndF2_Err			64		//����������
#define EST_T1_KT3_OT			65		//��ѹ������
#define EST_KM1_Err				66		//���Ӵ�������

#define EST_Frq_OF1				67		//1����Ƶ
#define EST_Frq_OF2				68		//2����Ƶ
#define EST_Frq_OF3				69		//3����Ƶ
#define EST_Frq_UF1				70		//1��ǷƵ
#define EST_Frq_UF2				71		//2��ǷƵ
#define EST_Frq_UF3				72		//3��ǷƵ

#define EST_IGBT_Temp_OV		73		//IGBT�¶ȹ���
#define EST_AD_Err				74      //AD��������
#define EST_Em_In				75      //�����ͣ
#define EST_CabinetHeatOV		76		//���ڹ���
#define EST_Em_BUtton			77		//��ť��ͣ
#define EST_BMS_Em_Err			78		//BMS��ͣ
#define EST_RESERVED_19			79		//����
#define EST_ERR_CONNCET			80		//����
#define EST_IacLkg				81		//©��������
#define EST_Phase_Err			82		//�������


#define EST_Filter_Temp_Over	120     //�˲����ݹ��¹���

//��־ - ���ͨ��
#define ET_CBB			1	//���ͨ��

#define EST_BMS_OUTLINE				0		//bms����
#define EST_BMS_OUTLINE_REC			1		//bms���߻ָ�

#define EST_METER_OUTLINE   		2		//pcs����ܱ����
#define EST_METER_OUTLINE_REC   	3		//pcs����ܱ���߻ָ�

#define EST_PLC_OUTLINE   			4		//pcs��PLC ����
#define EST_PLC_OUTLINE_REC   		5		//pcs��PLC ���߻ָ�

#define EST_EMS_OUTLINE   			6		//pcs��EMS ����
#define EST_EMS_OUTLINE_REC   		7		//pcs��EMS ���߻ָ�

//��־ - ����״̬

#define ET_OPSTATE		2	//ϵͳ����״̬

#define EST_SYSBOOT		0	//ϵͳ����
#define EST_OPEN		1	//�����Կ���
#define EST_CLOSE   	2	//���������
#define EST_EMS_OPEN		3	//AGC����
#define EST_EMS_CLOSE		4	//AGC�ػ�
#define EST_HMI_OPEN		5	//����������
#define EST_HMI_CLOSE   	6	//�������ػ�
#define EST_MB_OPEN			7	//��λ������
#define EST_MB_CLOSE   		8	//��λ���ػ�
#define EST_BMS_CHARG_CHANGE		9	//bms��繦�ʱ仯
#define EST_BMS_DISCHARG_CHANGE   	10	//bms�ŵ繦�ʱ仯

#define EST_OVERLOAD1_ERR        11  //�豸���ر���
#define EST_OVERLOAD2_ERR        12  //�豸���ر���
#define EST_OVERLOAD3_ERR        13  //�豸���ر���

#define EST_AUTO_OPEN			14	//�Զ�����
#define EST_AUTO_CLOSE			15	//�Զ��ػ�
#define EST_EMS_ESTOP			16 //ems��ͣ

#define EST_BMS_ESTOP			17 //Bms����ͣ��
#define EST_DOOR_ESTOP			18 //���Ŵ�pcsͣ��
#define EST_FIRE_ESTOP			19 //��������

#define EST_INPUT				20 //ԭ���ɶ���
#define EST_INPUT_CLOSE			21 //ԭ���ɶ�����ʧ

#define EST_HTEP_ALM			22 //���¸澯
#define EST_HTEP_ALM_DIS		23 //���¸澯��ʧ


#define EST_BMS_CHARG_500   25 //BMS���ʱ仯Ϊ-500kw
#define EST_BMS_CHARG_250   26 //BMS���ʱ仯Ϊ-250kw
#define EST_BMS_CHARG_0   	27 //BMS���ʱ仯Ϊ0kw

#define EST_BMS_DISCHARG_500   28 //BMS���ʱ仯Ϊ500kw
#define EST_BMS_DISCHARG_250   29 //BMS���ʱ仯Ϊ250kw
#define EST_BMS_DISCHARG_0     30 //BMS���ʱ仯Ϊ0kw
#define EST_BMS_POWER_CHANGE   31 //BMS���ʱ仯
#define EST_BMS_POWER_CH_CHANGE   32 //BMS���ʱ仯
#define EST_BMS_POWER_DIS_CHANGE   33 //BMS���ʱ仯

#define EST_FRE_ADJUST_ABLE    	  34 //һ�ε�Ƶ����
#define EST_FRE_ADJUST_DISABLE    35 //һ�ε�Ƶ��������

#define EST_PCSRUNMODE_CHANGE     36 //pcs����ģʽ�л�
#define EST_PROCEDURE_UPDATE      37 //�̼�����ͣ��

#define EST_FRE_ADJUST_LOCK    	  38 //һ�ε�Ƶ����
#define EST_FRE_ADJUST_LOCKOVER   39 //һ�ε�Ƶ��������

#define EST_CODE_INVALID   		  40 //��Կ����
#define EST_ERR_CODE   		      41 //��Կ����



//��־ -  ���ض���
#define ET_SWITCH				3	//���ض���

#define EST_Q1AndQ3_St_OPEN		0	//��·������
#define EST_F1AndF2_St_OPEN		1	//����������
#define EST_KM1_St_OPEN			2	//�����Ӵ�����
#define EST_T1_KT3_St_Err		3	//PCS����
#define EST_BMS_Em_St_Err		4	//BMS��ͣ�ź�����
#define EST_Em_St_Err			5	//��ͣ�ź�����
#define EST_OutSw3_St_Err		6	//�ⲿ�����ź�����
#define EST_Door_St_Open		7	//���Ŵ�

#define EST_Q1AndQ3_St_CLOSED	16	//��·���Ͽ�
#define EST_F1AndF2_St_CLOSED	17	//�������Ͽ�
#define EST_KM1_St_CLOSED		18	//�����Ӵ�����
#define EST_T1_KT3_St_Normal	19	//PCS�¶�����
#define EST_BMS_Em_St_Normal	20	//BMS��ͣ�ź���������
#define EST_Em_St_Normal		21	//��ͣ�ź���������
#define EST_OutSw3_St_Normal	22	//�ⲿ�����ź���������
#define EST_Door_St_Close		23	//���Źر�

#define EST_First_VF			32	//��һ̨VFԴ����
#define EST_Lvrting				33	//��ѹ��Խ��
#define EST_UnBlc				34	//��ѹ��ƽ��
#define EST_PwrRdc_On			35	//�������ƹ�������
#define EST_CtlLoop_On			36	//���ƻ�����
#define EST_Uac_UV_En			37	//ACǷѹ���ʹ��
#define EST_VF_Mode				38	//����VFģʽ
#define EST_Excite_On			39	//��������

#define EST_First_VF_End		48	//��һ̨VFԴ��������
#define EST_Lvrting_End			49	//��ѹ��Խ����
#define EST_UnBlc_End			50	//��ѹ��ƽ�����
#define EST_PwrRdc_End			51	//�������ƹ��ʽ���
#define EST_CtlLoop_End			52	//���ƻ���������
#define EST_Uac_UV_End			53	//ACǷѹ��ⲻʹ��
#define EST_VF_Mode_End			54	//�˳�VFģʽ
#define EST_Excite_End			55	//���Ž���

#define EST_IGBT_HotOn_Drv		64	//IGBT��������
#define EST_KM1_Drv				65	//���Ӵ���������
#define EST_Fault_Drv			66	//����ָʾ���
#define EST_PIGBT_Cool_Drv		68	//IGBTɢ��
#define EST_Charge_Drv			70	//����Ԥ���
#define EST_DCBrkTrip			71	//ֱ����·�������ѿ�

#define EST_IGBT_HotOn_Drv_End	80	//IGBT����ֹͣ
#define EST_KM1_Drv_End			81	//���Ӵ���������
#define EST_Fault_Drv_End		82	//����ָʾ���ֹͣ
#define EST_PIGBT_Cool_Drv_End	84	//IGBTɢ��ֹͣ
#define EST_Charge_Drv_End		86	//����Ԥ���ֹͣ
#define EST_DCBrkTrip_End		87	//ֱ����·�������ѿ�������

//��־ - ����ͨ��
#define ET_MCCOMM	4	//����ͨ��

//��־ - ������ʾ
#define ET_TIP		5
#define EST_OPEN_EN   			0	//��������ʹ��
#define EST_CLOSE_EN			1   //�ػ�����ʹ��
#define EST_ZERO_CAL_EN    		3	//����
#define EST_ZERO_CAL_CLOSE_EN   4	//ȡ�����

#define EST_OPERN_DISABLE   	16	//��������ʹ��

#define EST_LVRT_EN				64  	//�ʹ�ʹ��

#define EST_LVRT_DISABLE		80  	//�ʹ�ʹ��

//��־ -ģʽ�л�
#define ET_MODE 6
#define EST_VF_MODE				0   //VFģʽ
#define EST_PQ_MODE				1	//PQģʽ
#define EST_IV_MODE				2	//Vģʽ
#define EST_CP_MODE				3	
#define EST_CI_MODE				4	
#define EST_CU_MODE				5	
#define EST_VSG_MODE			6	

#define EST_EMS_CTR				10
#define EST_BMS_CTR				11
#define EST_HMI_CTR				12


//��ǰ���� 7

//��־-����ʹ����ʾ
#define ET_STRATEGY 8

#endif

