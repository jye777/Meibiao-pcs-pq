#ifndef LOG_DEF_H
#define LOG_DEF_H

//ÈÕÖ¾ - ¹ÊÕÏ
#define ET_FAULT			0		//¹ÊÕÏ

#define EST_Uina_OV1			0		//µçÍøµçÑ¹AÏà1¼¶¹ıÑ¹
#define EST_Uina_OV2			1		//µçÍøµçÑ¹AÏà2¼¶¹ıÑ¹
#define EST_Uina_OV3			2		//µçÍøµçÑ¹AÏà3¼¶¹ıÑ¹
#define EST_Uina_UV1			3		//µçÍøµçÑ¹AÏà1¼¶Ç·Ñ¹
#define EST_Uina_UV2			4		//µçÍøµçÑ¹AÏà2¼¶Ç·Ñ¹
#define EST_Uina_UV3			5		//µçÍøµçÑ¹AÏà3¼¶Ç·Ñ¹

#define EST_Uinb_OV1			6		//µçÍøµçÑ¹BÏà1¼¶¹ıÑ¹
#define EST_Uinb_OV2			7		//µçÍøµçÑ¹BÏà2¼¶¹ıÑ¹
#define EST_Uinb_OV3			8		//µçÍøµçÑ¹BÏà3¼¶¹ıÑ¹
#define EST_Uinb_UV1			9		//µçÍøµçÑ¹BÏà1¼¶Ç·Ñ¹
#define EST_Uinb_UV2			10		//µçÍøµçÑ¹BÏà2¼¶Ç·Ñ¹
#define EST_Uinb_UV3			11		//µçÍøµçÑ¹BÏà3¼¶Ç·Ñ¹

#define EST_Uinc_OV1			12		//µçÍøµçÑ¹CÏà1¼¶¹ıÑ¹
#define EST_Uinc_OV2			13		//µçÍøµçÑ¹CÏà2¼¶¹ıÑ¹
#define EST_Uinc_OV3			14		//µçÍøµçÑ¹CÏà3¼¶¹ıÑ¹
#define EST_Uinc_UV1			15		//µçÍøµçÑ¹CÏà1¼¶Ç·Ñ¹
#define EST_Uinc_UV2			16		//µçÍøµçÑ¹CÏà2¼¶Ç·Ñ¹
#define EST_Uinc_UV3			17		//µçÍøµçÑ¹CÏà3¼¶Ç·Ñ¹

#define EST_Iina_OI1			18		//µçÍøµçÁ÷AÏà1¼¶¹ıÁ÷
#define EST_Iina_OI2			19		//µçÍøµçÁ÷AÏà2¼¶¹ıÁ÷
#define EST_Iina_OI3			20		//µçÍøµçÁ÷AÏà3¼¶¹ıÁ÷

#define EST_Iinb_OI1			21		//µçÍøµçÁ÷BÏà1¼¶¹ıÁ÷
#define EST_Iinb_OI2			22		//µçÍøµçÁ÷BÏà2¼¶¹ıÁ÷
#define EST_Iinb_OI3			23		//µçÍøµçÁ÷BÏà3¼¶¹ıÁ÷

#define EST_Iinc_OI1			24		//µçÍøµçÁ÷CÏà1¼¶¹ıÁ÷
#define EST_Iinc_OI2			25		//µçÍøµçÁ÷CÏà2¼¶¹ıÁ÷
#define EST_Iinc_OI3			26		//µçÍøµçÁ÷CÏà3¼¶¹ıÁ÷

#define EST_Udc_OV1				27		//Ä¸Ïß1¼¶¹ıÑ¹
#define EST_Udc_OV2				28		//Ä¸Ïß2¼¶¹ıÑ¹
#define EST_Udc_OV3				29		//Ä¸Ïß3¼¶¹ıÑ¹
#define EST_Udc_UV1				30		//Ä¸Ïß1¼¶Ç·Ñ¹
#define EST_Udc_UV2				31		//Ä¸Ïß2¼¶Ç·Ñ¹
#define EST_Udc_UV3				32		//Ä¸Ïß3¼¶Ç·Ñ¹
#define EST_Idc_OV1				33		//Ä¸Ïß1¼¶¹ıÁ÷
#define EST_Idc_OV2				34		//Ä¸Ïß2¼¶¹ıÁ÷
#define EST_Idc_OV3				35		//Ä¸Ïß3¼¶¹ıÁ÷

#define EST_Ubtra_OV1			36      //¢ñºÅµç³Ø×é1¼¶¹ıÑ¹
#define EST_Ubtra_OV2			37      //¢ñºÅµç³Ø×é2¼¶¹ıÑ¹
#define EST_Ubtra_OV3			38      //¢ñºÅµç³Ø×é3¼¶¹ıÑ¹
#define EST_Ubtra_UV1			39      //¢ñºÅµç³Ø×é1¼¶Ç·Ñ¹
#define EST_Ubtra_UV2			40      //¢ñºÅµç³Ø×é2¼¶Ç·Ñ¹
#define EST_Ubtra_UV3			41      //¢ñºÅµç³Ø×é3¼¶Ç·Ñ¹

#define EST_UbtrIsltn_OV	    42		//µç³ØµçÑ¹¾øÔµ¼ì²â¹ÊÕÏ

#define EST_Uina_inst_OI		43		//AÏàµçÑ¹Ë²Ê±Öµ¹ıÑ¹
#define EST_Uinb_inst_OI		44		//BÏàµçÑ¹Ë²Ê±Öµ¹ıÑ¹
#define EST_Uinc_inst_OI		45		//CÏàµçÑ¹Ë²Ê±Öµ¹ıÑ¹
#define EST_Iina_inst_OI		46		//AÏàµçÁ÷Ë²Ê±Öµ¹ıÁ÷
#define EST_Iinb_inst_OI		47		//BÏàµçÁ÷Ë²Ê±Öµ¹ıÁ÷
#define EST_Iinc_inst_OI		48		//CÏàµçÁ÷Ë²Ê±Öµ¹ıÁ÷

#define EST_Udc_Inst_OV			49		//Ä¸ÏßµçÑ¹Ë²Ê±¹ıÑ¹
#define EST_Idc_Inst_OI			50		//Ä¸ÏßµçÁ÷Ë²Ê±¹ıÁ÷
#define EST_Ubtr_Inst_OV		51		//µç³ØµçÑ¹Ë²Ê±¹ıÑ¹

#define EST_IGBTA_Err0		   	52      //AÏàIGBT¹ÊÕÏ0
#define EST_IGBTA_Err1			53      //AÏàIGBT¹ÊÕÏ1
#define EST_IGBTB_Err0			54		//BÏàIGBT¹ÊÕÏ0
#define EST_IGBTB_Err1			55		//BÏàIGBT¹ÊÕÏ1
#define EST_IGBTC_Err0			56		//CÏàIGBT¹ÊÕÏ0
#define EST_IGBTC_Err1			57		//CÏàIGBT¹ÊÕÏ1

#define EST_Iina_Zero_Err		58		//µçÍøµçÁ÷AÏà»ØÁã¹ÊÕÏ
#define EST_Iinb_Zero_Err		59		//µçÍøµçÁ÷BÏà»ØÁã¹ÊÕÏ
#define EST_Iinc_Zero_Err 		60		//µçÍøµçÁ÷CÏà»ØÁã¹ÊÕÏ
#define EST_Ibtra_Zero_Err 		61		//Ö±Á÷µçÁ÷»ØÁã¹ÊÕÏ

#define EST_ProChargeFault_Err  62		//Ô¤³äµç¹ÊÕÏ
#define EST_Q1AndQ3_Err			63		//½»Ö±Á÷Ö÷¶ÏÂ·Æ÷¹ÊÕÏ

#define EST_F1AndF2_Err			64		//·ÀÀ×Æ÷¹ÊÕÏ
#define EST_T1_KT3_OT			65		//±äÑ¹Æ÷¹ıÎÂ
#define EST_KM1_Err				66		//Ö÷½Ó´¥Æ÷¹ÊÕÏ

#define EST_Frq_OF1				67		//1¼¶¹ıÆµ
#define EST_Frq_OF2				68		//2¼¶¹ıÆµ
#define EST_Frq_OF3				69		//3¼¶¹ıÆµ
#define EST_Frq_UF1				70		//1¼¶Ç·Æµ
#define EST_Frq_UF2				71		//2¼¶Ç·Æµ
#define EST_Frq_UF3				72		//3¼¶Ç·Æµ

#define EST_IGBT_Temp_OV		73		//IGBTÎÂ¶È¹ıÎÂ
#define EST_AD_Err				74      //ADÇı¶¯¹ÊÕÏ
#define EST_Em_In				75      //Èí¼ş¼±Í£
#define EST_CabinetHeatOV		76		//¹ñÄÚ¹ıºÄ
#define EST_Em_BUtton			77		//°´Å¥¼±Í£
#define EST_BMS_Em_Err			78		//BMS¼±Í£
#define EST_RESERVED_19			79		//±£Áô
#define EST_ERR_CONNCET			80		//·´½Ó
#define EST_IacLkg				81		//Â©µçÁ÷±£»¤
#define EST_Phase_Err			82		//ÏàĞò´íÎó


#define EST_Filter_Temp_Over	120     //ÂË²¨µçÈİ¹ıÎÂ¹ÊÕÏ

//ÈÕÖ¾ - °å¼äÍ¨ĞÅ
#define ET_CBB			1	//°å¼äÍ¨ĞÅ

#define EST_BMS_OUTLINE				0		//bmsµôÏß
#define EST_BMS_OUTLINE_REC			1		//bmsµôÏß»Ö¸´

#define EST_METER_OUTLINE   		2		//pcsÓëµçÄÜ±íµôÏß
#define EST_METER_OUTLINE_REC   	3		//pcsÓëµçÄÜ±íµôÏß»Ö¸´

#define EST_PLC_OUTLINE   			4		//pcsÓëPLC µôÏß
#define EST_PLC_OUTLINE_REC   		5		//pcsÓëPLC µôÏß»Ö¸´

#define EST_EMS_OUTLINE   			6		//pcsÓëEMS µôÏß
#define EST_EMS_OUTLINE_REC   		7		//pcsÓëEMS µôÏß»Ö¸´

//ÈÕÖ¾ - ÔËĞĞ×´Ì¬

#define ET_OPSTATE		2	//ÏµÍ³ÔËĞĞ×´Ì¬

#define EST_SYSBOOT		0	//ÏµÍ³Æô¶¯
#define EST_OPEN		1	//¹ÊÕÏ×Ô¿ª»ú
#define EST_CLOSE   	2	//¹ÊÕÏ×ÔÇå³ı
#define EST_EMS_OPEN		3	//AGC¿ª»ú
#define EST_EMS_CLOSE		4	//AGC¹Ø»ú
#define EST_HMI_OPEN		5	//´¥ÃşÆÁ¿ª»ú
#define EST_HMI_CLOSE   	6	//´¥ÃşÆÁ¹Ø»ú
#define EST_MB_OPEN			7	//ÉÏÎ»»ú¿ª»ú
#define EST_MB_CLOSE   		8	//ÉÏÎ»»ú¹Ø»ú
#define EST_BMS_CHARG_CHANGE		9	//bms³äµç¹¦ÂÊ±ä»¯
#define EST_BMS_DISCHARG_CHANGE   	10	//bms·Åµç¹¦ÂÊ±ä»¯

#define EST_OVERLOAD1_ERR        11  //Éè±¸¹ıÔØ±£»¤
#define EST_OVERLOAD2_ERR        12  //Éè±¸¹ıÔØ±£»¤
#define EST_OVERLOAD3_ERR        13  //Éè±¸¹ıÔØ±£»¤

#define EST_AUTO_OPEN			14	//×Ô¶¯¿ª»ú
#define EST_AUTO_CLOSE			15	//×Ô¶¯¹Ø»ú
#define EST_EMS_ESTOP			16 //ems¼±Í£

#define EST_BMS_ESTOP			17 //Bms¹ÊÕÏÍ£»ú
#define EST_DOOR_ESTOP			18 //¹ñÃÅ´ò¿ªpcsÍ£»ú
#define EST_FIRE_ESTOP			19 //Ïû·À¶¯×÷

#define EST_INPUT				20 //Ô­ÍøºÉ¶¯×÷
#define EST_INPUT_CLOSE			21 //Ô­ÍøºÉ¶¯×÷ÏûÊ§

#define EST_HTEP_ALM			22 //¸ßÎÂ¸æ¾¯
#define EST_HTEP_ALM_DIS		23 //¸ßÎÂ¸æ¾¯ÏûÊ§


#define EST_BMS_CHARG_500   25 //BMS¹¦ÂÊ±ä»¯Îª-500kw
#define EST_BMS_CHARG_250   26 //BMS¹¦ÂÊ±ä»¯Îª-250kw
#define EST_BMS_CHARG_0   	27 //BMS¹¦ÂÊ±ä»¯Îª0kw

#define EST_BMS_DISCHARG_500   28 //BMS¹¦ÂÊ±ä»¯Îª500kw
#define EST_BMS_DISCHARG_250   29 //BMS¹¦ÂÊ±ä»¯Îª250kw
#define EST_BMS_DISCHARG_0     30 //BMS¹¦ÂÊ±ä»¯Îª0kw
#define EST_BMS_POWER_CHANGE   31 //BMS¹¦ÂÊ±ä»¯
#define EST_BMS_POWER_CH_CHANGE   32 //BMS¹¦ÂÊ±ä»¯
#define EST_BMS_POWER_DIS_CHANGE   33 //BMS¹¦ÂÊ±ä»¯

#define EST_FRE_ADJUST_ABLE    	  34 //Ò»´Îµ÷Æµ¶¯×÷
#define EST_FRE_ADJUST_DISABLE    35 //Ò»´Îµ÷Æµ¶¯×÷½áÊø

#define EST_PCSRUNMODE_CHANGE     36 //pcsÔËĞĞÄ£Ê½ÇĞ»»
#define EST_PROCEDURE_UPDATE      37 //¹Ì¼ş¸üĞÂÍ£»ú

#define EST_FRE_ADJUST_LOCK    	  38 //Ò»´Îµ÷Æµ±ÕËø
#define EST_FRE_ADJUST_LOCKOVER   39 //Ò»´Îµ÷Æµ±ÕËø½áÊø

#define EST_CODE_INVALID   		  40 //ÃÜÔ¿¹ıÆÚ
#define EST_ERR_CODE   		      41 //ÃÜÔ¿´íÎó



//ÈÕÖ¾ -  ¿ª¹Ø¶¯×÷
#define ET_SWITCH				3	//¿ª¹Ø¶¯×÷

#define EST_Q1AndQ3_St_OPEN		0	//¶ÏÂ·Æ÷ÎüºÏ
#define EST_F1AndF2_St_OPEN		1	//·ÀÀ×Æ÷ÎüºÏ
#define EST_KM1_St_OPEN			2	//²¢Íø½Ó´¥Æ÷ºÏ
#define EST_T1_KT3_St_Err		3	//PCS¹ıÎÂ
#define EST_BMS_Em_St_Err		4	//BMS¼±Í£ĞÅºÅÊäÈë
#define EST_Em_St_Err			5	//¼±Í£ĞÅºÅÊäÈë
#define EST_OutSw3_St_Err		6	//Íâ²¿¿ª¹ØĞÅºÅÊäÈë
#define EST_Door_St_Open		7	//¹ñÃÅ´ò¿ª

#define EST_Q1AndQ3_St_CLOSED	16	//¶ÏÂ·Æ÷¶Ï¿ª
#define EST_F1AndF2_St_CLOSED	17	//·ÀÀ×Æ÷¶Ï¿ª
#define EST_KM1_St_CLOSED		18	//²¢Íø½Ó´¥Æ÷¶Ï
#define EST_T1_KT3_St_Normal	19	//PCSÎÂ¶ÈÕı³£
#define EST_BMS_Em_St_Normal	20	//BMS¼±Í£ĞÅºÅÊäÈëÕı³£
#define EST_Em_St_Normal		21	//¼±Í£ĞÅºÅÊäÈëÕı³£
#define EST_OutSw3_St_Normal	22	//Íâ²¿¿ª¹ØĞÅºÅÊäÈëÕı³£
#define EST_Door_St_Close		23	//¹ñÃÅ¹Ø±Õ

#define EST_First_VF			32	//µÚÒ»Ì¨VFÔ´Æô¶¯
#define EST_Lvrting				33	//µÍÑ¹´©Ô½ÖĞ
#define EST_UnBlc				34	//µçÑ¹²»Æ½ºâ
#define EST_PwrRdc_On			35	//¹ıÎÂÏŞÖÆ¹¦ÂÊÆô¶¯
#define EST_CtlLoop_On			36	//¿ØÖÆ»·Æô¶¯
#define EST_Uac_UV_En			37	//ACÇ·Ñ¹¼ì²âÊ¹ÄÜ
#define EST_VF_Mode				38	//½øÈëVFÄ£Ê½
#define EST_Excite_On			39	//ÕıÔÚÀø´Å

#define EST_First_VF_End		48	//µÚÒ»Ì¨VFÔ´Æô¶¯½áÊø
#define EST_Lvrting_End			49	//µÍÑ¹´©Ô½½áÊø
#define EST_UnBlc_End			50	//µçÑ¹²»Æ½ºâ½áÊø
#define EST_PwrRdc_End			51	//¹ıÎÂÏŞÖÆ¹¦ÂÊ½áÊø
#define EST_CtlLoop_End			52	//¿ØÖÆ»·Æô¶¯½áÊø
#define EST_Uac_UV_End			53	//ACÇ·Ñ¹¼ì²â²»Ê¹ÄÜ
#define EST_VF_Mode_End			54	//ÍË³öVFÄ£Ê½
#define EST_Excite_End			55	//Àø´Å½áÊø

#define EST_IGBT_HotOn_Drv		64	//IGBT¼ÓÈÈÆô¶¯
#define EST_KM1_Drv				65	//Ö÷½Ó´¥Æ÷Çı¶¯¿ª
#define EST_Fault_Drv			66	//¹ÊÕÏÖ¸Ê¾Êä³ö
#define EST_PIGBT_Cool_Drv		68	//IGBTÉ¢ÈÈ
#define EST_Charge_Drv			70	//½»Á÷Ô¤³äµç
#define EST_DCBrkTrip			71	//Ö±Á÷¶ÏÂ·Æ÷·ÖÀøÍÑ¿Û

#define EST_IGBT_HotOn_Drv_End	80	//IGBT¼ÓÈÈÍ£Ö¹
#define EST_KM1_Drv_End			81	//Ö÷½Ó´¥Æ÷Çı¶¯¹Ø
#define EST_Fault_Drv_End		82	//¹ÊÕÏÖ¸Ê¾Êä³öÍ£Ö¹
#define EST_PIGBT_Cool_Drv_End	84	//IGBTÉ¢ÈÈÍ£Ö¹
#define EST_Charge_Drv_End		86	//½»Á÷Ô¤³äµçÍ£Ö¹
#define EST_DCBrkTrip_End		87	//Ö±Á÷¶ÏÂ·Æ÷·ÖÀøÍÑ¿ÛÇı¶¯¹Ø

//ÈÕÖ¾ - Ö÷¿ØÍ¨ĞÅ
#define ET_MCCOMM	4	//Ö÷¿ØÍ¨ĞÅ

//ÈÕÖ¾ - ²Ù×÷ÌáÊ¾
#define ET_TIP		5
#define EST_OPEN_EN   			0	//¿ª»úÃüÁîÊ¹ÄÜ
#define EST_CLOSE_EN			1   //¹Ø»úÃüÁîÊ¹ÄÜ
#define EST_ZERO_CAL_EN    		3	//½ÌÁã
#define EST_ZERO_CAL_CLOSE_EN   4	//È¡Ïû½ÌÁ

#define EST_OPERN_DISABLE   	16	//¿ª»úÃüÁîÊ¹ÄÜ

#define EST_LVRT_EN				64  	//µÍ´©Ê¹ÄÜ

#define EST_LVRT_DISABLE		80  	//µÍ´©Ê¹ÄÜ

//ÈÕÖ¾ -Ä£Ê½ÇĞ»»
#define ET_MODE 6
#define EST_VF_MODE				0   //VFÄ£Ê½
#define EST_PQ_MODE				1	//PQÄ£Ê½
#define EST_IV_MODE				2	//VÄ£Ê½
#define EST_CP_MODE				3	
#define EST_CI_MODE				4	
#define EST_CU_MODE				5	
#define EST_VSG_MODE			6	

#define EST_EMS_CTR				10
#define EST_BMS_CTR				11
#define EST_HMI_CTR				12


//µ±Ç°¹ÊÕÏ 7

//ÈÕÖ¾-²ßÂÔÊ¹ÄÜÌáÊ¾
#define ET_STRATEGY 8

#endif

