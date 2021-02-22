#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "tty.h"
#include "cmsis_os.h"
#include "cpu.h"
#include "stm32f4xx_hal.h"
#include "Driver_USART.h"
#include "sys.h"
#include "para.h"
#include "fpga.h"
#include "micro.h"
#include "log.h"
#include "crc.h"
#include "systime.h"
#include "modbus_master.h"
#include "modbus_comm.h"
#include "MBServer.h"
#include "pcs.h"
#include "storedata.h"
#include "screen.h"
#include "custom.h"
#include "sdata_alloc.h"


#define SET_PARAM_NUM	(110)
#define mb_dbg      printf

enum {
    METHOD_0 = 0,
    METHOD_1,
    METHOD_2,
    METHOD_3,
    METHOD_4,
    METHOD_5,
    METHOD_6,    
    METHOD_7
};

unsigned char mb_data_auto_update_u0 = 1;
modbus_config mod_config;
static unsigned char run_mode_hmi = 0;
static unsigned char cp_mode_flag = 0, ci_mode_flag = 0;
static unsigned char mb_recv_buf_u0[MB_RECVBUF_SIZE];
static unsigned char mb_send_buf_u0[MB_SENDBUF_SIZE];
static int modbus_recv_packet_u0(void);
static void modbus_resp_u0(void);
static void modbus_read_hodingregs_u0(void);
static void modbus_write_singlereg_u0(void);
static void modbus_write_multipleregs_u0(void);
static void modbus_illegal_func_u0(void);
static void modbus_resp_err_u0(unsigned char addr, unsigned char func, unsigned char err);
static int modbus_crc_set_u0(unsigned char* pdata, unsigned short cntlen);
static int modbus_crc_check_u0(unsigned char* pdata, unsigned short cntlen);
unsigned short modbus_read_reg_u0(unsigned short addr, unsigned char* err);
unsigned char modbus_write_reg_u0(unsigned short addr, /*unsigned*/ short value);
extern ARM_DRIVER_USART Driver_USART3;
extern osThreadId tid_modbus_screen;
extern uint8_t ip4_addr[];
extern uint8_t mac_addr[];
extern struct tm tm;
extern _CountData CountData;
static screen_protect_t screen_protect = {1,1,1,1,1};
handle_data_info_t data_handle[]=
{
    {METHOD_1, FD_UINX_ADD}, //0
    {METHOD_1, FD_UINX_ADD}, //1
    {METHOD_1, FD_IINX_ADD}, //2
    {METHOD_1, FD_UDC_ADD}, //3
    {METHOD_1, FD_UDC_ADD}, //4
    {METHOD_1, FD_IDC_ADD}, //5
    {METHOD_1, FD_UBTR_ADD}, //6
    {METHOD_1, FD_UBTR_ADD}, //7
    {METHOD_1, FD_UDC_ADD}, //8
    {METHOD_2, FD_UINX_ADD}, //9
    {METHOD_2, FD_IINX_ADD}, //10
    {METHOD_3, FD_UDC_ADD}, //11
    {METHOD_3, FD_IDC_ADD}, //12
    {METHOD_3, FD_UBTR_ADD}, //13
    {METHOD_0, 0}, //14
    {METHOD_1, FD_UDC_ADD}, //15
    {METHOD_5, 0}, //16
    {METHOD_5, 0}, //17
    {METHOD_1, FD_PAC_ADD}, //18
    {METHOD_7, 0}, //19
    {METHOD_0, 0}, //20
    {METHOD_0, 0}, //21
    {METHOD_0, 0}, //22
    {METHOD_1, FD_UDC_ADD}, //23
    {METHOD_4, FD_UINX_ADD}, //24
    {METHOD_1, FD_UINX_ADD}, //25
    {METHOD_1, FD_PAC_ADD}, //26
    {METHOD_1, FD_PAC_ADD}, //27
    {METHOD_6, FD_IINX_ADD}, //28
    {METHOD_1, FD_PAC_ADD}, //29
    {METHOD_1, FD_PAC_ADD}, //30
    {METHOD_1, FD_PDC_ADD}, //31
    {METHOD_1, FD_PDC_ADD}, //32
    {METHOD_1, FD_IDC_ADD}, //33
    {METHOD_1, FD_IDC_ADD}, //34
    {METHOD_0, 0}, //35
    {METHOD_0, 0}, //36
    {METHOD_0, 0}, //37
    {METHOD_0, 0}, //38
    {METHOD_0, 0}, //39
    {METHOD_0, 0}, //40
    {METHOD_0, 0}, //41
    {METHOD_0, 0}, //42
    {METHOD_0, 0}, //43
    {METHOD_0, 0}, //44
    {METHOD_0, 0}, //45
    {METHOD_0, 0}, //46
    {METHOD_0, 0}, //47
    {METHOD_0, 0}, //48
    {METHOD_0, 0}, //49
    {METHOD_0, 0}, //50
    {METHOD_0, 0}, //51
    {METHOD_0, 0}, //52
    {METHOD_0, 0}, //53
    {METHOD_0, 0}, //54
    {METHOD_0, 0}, //55
    {METHOD_0, 0}, //56
    {METHOD_0, 0}, //57
    {METHOD_0, 0}, //58
    {METHOD_0, 0}, //59
    {METHOD_0, 0}, //60
    {METHOD_0, 0}, //61
    {METHOD_0, 0}, //62
    {METHOD_0, 0}, //63
    {METHOD_0, 0}, //64
    {METHOD_0, 0}, //65
    {METHOD_0, 0}, //66
    {METHOD_0, 0}, //67
    {METHOD_0, 0}, //68
    {METHOD_0, 0}, //69
    {METHOD_0, 0}, //70
    {METHOD_0, 0}, //71
    {METHOD_0, 0}, //72
    {METHOD_0, 0}, //73
    {METHOD_0, 0}, //74
    {METHOD_0, 0}, //75
    {METHOD_0, 0}, //76
    {METHOD_0, 0}, //77
    {METHOD_0, 0}, //78
    {METHOD_0, 0}, //79
    {METHOD_0, 0}, //80
    {METHOD_0, 0}, //81
    {METHOD_0, 0}, //82
    {METHOD_0, 0}, //83
    {METHOD_0, 0}, //84
    {METHOD_0, 0}, //85
    {METHOD_0, 0}, //86
    {METHOD_0, 0}, //87
    {METHOD_0, 0}, //88
    {METHOD_0, 0}, //89
    {METHOD_0, 0}, //90
    {METHOD_0, 0}, //91
    {METHOD_1, FD_PAC_ADD}, //92
    {METHOD_1, FD_PAC_ADD}, //93
    {METHOD_0, 0}, //94
    {METHOD_0, 0}, //95
    {METHOD_0, 0}, //96
    {METHOD_0, 0}, //97
    {METHOD_0, 0}, //98
    {METHOD_0, 0}, //99
    {METHOD_0, 0}, //100
    {METHOD_0, 0}, //101
    {METHOD_0, 0}, //102
    {METHOD_0, 0}, //103
    {METHOD_0, 0}, //104
    {METHOD_0, 0}, //105
    {METHOD_0, 0}, //106
    {METHOD_0, 0}, //107
    {METHOD_0, 0}, //108
    {METHOD_0, 0}, //109
    {METHOD_0, 0}, //110
    {METHOD_0, 0}, //111
    {METHOD_0, 0}, //112
    {METHOD_0, 0}, //113
    {METHOD_0, 0}, //114
    {METHOD_0, 0}, //115
    {METHOD_0, 0}, //116
    {METHOD_0, 0}, //117
    {METHOD_0, 0}, //118
    {METHOD_0, 0}, //119

    {METHOD_1, FD_UINX_ADD}, //120
    {METHOD_1, FD_UINX_ADD}, //121
    {METHOD_1, FD_UINX_ADD}, //122
    {METHOD_1, FD_IINX_ADD}, //123
    {METHOD_1, FD_IINX_ADD}, //124
    {METHOD_1, FD_IINX_ADD}, //125
    {METHOD_1, FD_UDC_ADD}, //126
    {METHOD_1, FD_IDC_ADD}, //127
    {METHOD_1, FD_UBTR_ADD}, //128
    {METHOD_1, FD_UPOS_GND_ADD}, //129
    {METHOD_1, FD_UNEG_GND_ADD}, //130
    {METHOD_1, FD_PAC_ADD}, //131
    {METHOD_1, FD_PAC_ADD}, //132
    {METHOD_1, FD_PDC_ADD}, //133
    {METHOD_1, FD_UDC_ADD}, //134
    {METHOD_5, 0}, //135
    {METHOD_1, FD_IAC_LKG_ADD}, //136
    {METHOD_1, FD_IAC_CT_ADD}, //137
    {METHOD_1, FD_IAC_CT_ADD}, //138
    {METHOD_0, 0}, //139
    {METHOD_0, 0}, //140
    {METHOD_0, 0}, //141
    {METHOD_0, 0}, //142
    {METHOD_0, 0}, //143
    {METHOD_0, 0}, //144
    {METHOD_0, 0}, //145
    {METHOD_0, 0}, //146
    {METHOD_0, 0}, //147
    {METHOD_0, 0}, //148
    {METHOD_0, 0}, //149
    {METHOD_0, 0}, //150
    {METHOD_0, 0}, //151
    {METHOD_0, 0}, //152
    {METHOD_0, 0}, //153
    {METHOD_0, 0}, //154
    {METHOD_0, 0}, //155
    {METHOD_0, 0}, //156
    {METHOD_0, 0}, //157
    {METHOD_0, 0}, //158
    {METHOD_0, 0}, //159
    {METHOD_0, 0}, //160
    {METHOD_0, 0}, //161
    {METHOD_0, 0}, //162
    {METHOD_0, 0}, //163
    {METHOD_0, 0}, //164
    {METHOD_0, 0}, //165
    {METHOD_0, 0}, //166
    {METHOD_0, 0}, //167
    {METHOD_0, 0}, //168
    {METHOD_0, 0}, //169
    {METHOD_0, 0}, //170
    {METHOD_0, 0}, //171
    {METHOD_0, 0}, //172
    {METHOD_0, 0}, //173
    {METHOD_0, 0}, //174
    {METHOD_0, 0}, //175
    {METHOD_0, 0}, //176
    {METHOD_0, 0}, //177
    {METHOD_0, 0}, //178
    {METHOD_0, 0}, //179
    {METHOD_0, 0}, //180
    {METHOD_0, 0}, //181
    {METHOD_0, 0}, //182
    {METHOD_0, 0}, //183
    {METHOD_0, 0}, //184
    {METHOD_0, 0}, //185
    {METHOD_0, 0}, //186
    {METHOD_0, 0}, //187
    {METHOD_0, 0}, //188
    {METHOD_1, FD_PAC_CT_ADD}, //189
    {METHOD_1, FD_PAC_CT_ADD}, //190
    {METHOD_0, 0}, //191
    {METHOD_1, FD_PAC_ADD}, //192
    {METHOD_0, 0}, //193
    {METHOD_0, 0}, //194
    {METHOD_0, 0}, //195
    {METHOD_0, 0}, //196
    {METHOD_0, 0}, //197
    {METHOD_0, 0}, //198
    {METHOD_0, 0}, //199
    {METHOD_0, 0}, //200
    {METHOD_0, 0}, //201
    {METHOD_0, 0}, //202
    {METHOD_0, 0}, //203
    {METHOD_0, 0}, //204
    {METHOD_0, 0}, //205
    {METHOD_0, 0}, //206
    {METHOD_0, 0}, //207
    {METHOD_0, 0}, //208
    {METHOD_0, 0}, //209
    {METHOD_0, 0}, //210
    {METHOD_0, 0}, //211
    {METHOD_0, 0}, //212
    {METHOD_0, 0}, //213
    {METHOD_0, 0}, //214
    {METHOD_0, 0}, //215
    {METHOD_0, 0}, //216
    {METHOD_0, 0}, //217
    {METHOD_0, 0}, //218
    {METHOD_0, 0}, //219
    {METHOD_0, 0}, //220
    {METHOD_0, 0}, //221
    {METHOD_0, 0}, //222
    {METHOD_0, 0}, //223
    {METHOD_0, 0}, //224
    {METHOD_0, 0}, //225
    {METHOD_0, 0}, //226
    {METHOD_0, 0}, //227
    {METHOD_0, 0}, //228
    {METHOD_0, 0}, //229
    {METHOD_0, 0}, //230
    {METHOD_0, 0}, //231
    {METHOD_0, 0}, //232
    {METHOD_0, 0}, //233
    {METHOD_0, 0}, //234
    {METHOD_0, 0}, //235
    {METHOD_0, 0}, //236
    {METHOD_0, 0}, //237
    {METHOD_0, 0}, //238
    {METHOD_0, 0}, //239
    {METHOD_0, 0}, //240
    {METHOD_0, 0}, //241
    {METHOD_0, 0}, //242
    {METHOD_0, 0}, //243
    {METHOD_0, 0}, //244
    {METHOD_0, 0}, //245
    {METHOD_0, 0}, //246
    {METHOD_0, 0}, //247
    {METHOD_0, 0}, //248
    {METHOD_0, 0}, //249
    {METHOD_0, 0}, //250
    {METHOD_0, 0}, //251
    {METHOD_0, 0}, //252
    {METHOD_0, 0}, //253
    {METHOD_0, 0}, //254
    {METHOD_0, 0}, //255

	{METHOD_1, FD_UINX_ADD}, //256
	{METHOD_1, FD_UINX_ADD}, //257
	{METHOD_1, FD_UINX_ADD}, //258
	{METHOD_1, FD_IINX_ADD}, //259
	{METHOD_1, FD_IINX_ADD}, //260
	{METHOD_1, FD_IINX_ADD}, //261
	{METHOD_1, FD_UDC_ADD}, //261
	{METHOD_1, FD_IDC_ADD}, //263
	{METHOD_1, FD_UBTR_ADD}, //264
	{METHOD_1, FD_UPOS_GND_ADD}, //265
	{METHOD_1, FD_UNEG_GND_ADD}, //266
	
	{METHOD_1, FD_PAC_ADD}, //267
	{METHOD_1, FD_PAC_ADD}, //268
	{METHOD_1, FD_PDC_ADD}, //269
	{METHOD_1, FD_UDC_ADD}, //270
	{METHOD_5, 0}, //271

	{METHOD_1, FD_UINX_ADD}, //272
	{METHOD_1, FD_UINX_ADD}, //273
	{METHOD_1, FD_UINX_ADD}, //274
	
	{METHOD_0, 0}, //275 系统状态寄存器
	{METHOD_0, 0}, //276
	{METHOD_0, 0}, //277
	{METHOD_0, 0}, //278
	{METHOD_0, 0}, //279
	{METHOD_0, 0}, //280
	{METHOD_0, 0}, //281
	{METHOD_0, 0}, //282
	{METHOD_0, 0}, //283
	{METHOD_0, 0}, //284
	{METHOD_0, 0}, //285
	{METHOD_0, 0}, //286
	{METHOD_0, 0}, //287
	{METHOD_0, 0}, //288
	{METHOD_0, 0}, //289
	{METHOD_0, 0}, //290
	{METHOD_0, 0}, //291系统故障锁存寄存器
	{METHOD_0, 0}, //292
	{METHOD_0, 0}, //293
	{METHOD_0, 0}, //294
	{METHOD_0, 0}, //295
	{METHOD_0, 0}, //296
	{METHOD_0, 0}, //297
	{METHOD_0, 0}, //298
	{METHOD_0, 0}, //299
	{METHOD_0, 0}, //300
	{METHOD_0, 0}, //301
	{METHOD_0, 0}, //302
	{METHOD_0, 0}, //303
	{METHOD_0, 0}, //304
	{METHOD_0, 0}, //305
	{METHOD_0, 0}, //306
	
	{METHOD_0, 0}, //307
	{METHOD_0, 0}, //308
	{METHOD_0, 0}, //309
	{METHOD_0, 0}, //310
	{METHOD_0, 0}, //311
	{METHOD_0, 0}, //312
	{METHOD_0, 0}, //313
	{METHOD_0, 0}, //314
	{METHOD_0, 0}, //315
	{METHOD_0, 0}, //316
	{METHOD_0, 0}, //317
	{METHOD_0, 0}, //318
	{METHOD_0, 0}, //319	
	{METHOD_0, 0}, //320
	{METHOD_0, 0}, //321
	{METHOD_0, 0}, //322
	
	{METHOD_0, 0}, //323
	{METHOD_0, 0}, //324
	{METHOD_0, 0}, //325
	{METHOD_0, 0}, //326
	{METHOD_0, 0}, //327
	{METHOD_1, FD_IAC_LKG_ADD}, //328
	{METHOD_1, FD_IAC_LKG_ADD}, //329
	
	{METHOD_0, 0}, //330
	{METHOD_0, 0}, //331
	{METHOD_0, 0}, //332
	{METHOD_0, 0}, //333
	{METHOD_0, 0}, //334
	{METHOD_0, 0}, //335 fpga版本
	
	{METHOD_0, 0}, //336
	{METHOD_0, 0}, //337
	{METHOD_0, 0}, //338
	{METHOD_0, 0}, //339
	{METHOD_0, 0}, //340
	{METHOD_0, 0}, //341
	{METHOD_0, 0}, //342
	{METHOD_0, 0}, //343
	{METHOD_0, 0}, //344
	{METHOD_0, 0}, //345
	{METHOD_0, 0}, //346
	{METHOD_0, 0}, //347
	{METHOD_0, 0}, //348
	{METHOD_0, 0}, //349
	{METHOD_0, 0}, //350
	{METHOD_0, 0}, //351
	{METHOD_0, 0}, //352
	{METHOD_0, 0}, //353
	{METHOD_0, 0}, //354
	{METHOD_0, 0}, //355
	{METHOD_0, 0}, //356
	{METHOD_0, 0}, //357
	{METHOD_0, 0}, //358
	{METHOD_0, 0}, //359
	{METHOD_0, 0}, //360
	{METHOD_0, 0}, //361
	{METHOD_0, 0}, //362
	{METHOD_0, 0}, //363
	{METHOD_0, 0}, //364
	{METHOD_0, 0}, //365
	{METHOD_0, 0}, //366
	{METHOD_0, 0}, //367
	{METHOD_0, 0}, //368
	{METHOD_0, 0}, //369
	{METHOD_0, 0}, //370
	{METHOD_0, 0}, //371
	{METHOD_0, 0}, //372
	{METHOD_0, 0}, //373
	{METHOD_0, 0}, //374
	{METHOD_0, 0}, //375
	{METHOD_0, 0}, //376
	{METHOD_0, 0}, //377
	{METHOD_0, 0}, //378
	{METHOD_0, 0}, //379
	{METHOD_0, 0}, //380
	{METHOD_0, 0}, //381
	{METHOD_0, 0}, //382
	{METHOD_0, 0}, //383
	{METHOD_0, 0}, //384
	{METHOD_0, 0}, //385
	{METHOD_0, 0}, //386
	{METHOD_0, 0}, //387
	{METHOD_0, 0}, //388
	{METHOD_0, 0}, //389
	{METHOD_0, 0}, //390
	{METHOD_0, 0}, //391
	{METHOD_0, 0}, //392
	{METHOD_0, 0}, //393
	{METHOD_0, 0}, //394
	{METHOD_0, 0}, //395
	{METHOD_0, 0}, //396
	{METHOD_0, 0}, //397
	{METHOD_0, 0}, //398
	{METHOD_0, 0}, //399
	{METHOD_0, 0}, //400
	
};


short setting_data_handle(short addr, short value)
{
    float val_ret;
		float valuef;
    
		valuef = (float)value;
		switch(data_handle[addr].method)
    {
    case METHOD_0:
        val_ret = valuef;
        break;

    case METHOD_1:
        val_ret = (valuef*arm_config_data_read32(data_handle[addr].coef_id)/100000/MODULUS);
        break;

    case METHOD_2:
        val_ret = (valuef*arm_config_data_read32(data_handle[addr].coef_id)/100000*1.414f/MODULUS + 100);
        break;

    case METHOD_3:
        val_ret = (valuef*arm_config_data_read32(data_handle[addr].coef_id)/100000/MODULUS + 100);
        break;

    case METHOD_4:
        val_ret = (valuef*arm_config_data_read32(data_handle[addr].coef_id)/100000*1.414f/1.732f/MODULUS);
        break;

    case METHOD_5:
        val_ret = (valuef*343.6f/MODULUS);
        break;

    case METHOD_6:
        val_ret = (valuef*arm_config_data_read32(data_handle[addr].coef_id)/100000*1.414f/MODULUS);
        break;

	case METHOD_7:		
        val_ret = valuef*3.0f;		
        break;

    default:
        val_ret = valuef;
        break;
    }

    return (short)val_ret;
}


short getting_data_handle(short addr, short value)
{
    float val_ret,valuef;
	
		valuef = (float)value;

    switch(data_handle[addr].method)
    {
    case METHOD_0:
        val_ret = valuef;
        break;

    case METHOD_1:
        val_ret = (valuef*MODULUS/arm_config_data_read32(data_handle[addr].coef_id)*100000);
        break;

    case METHOD_2:
        val_ret = ((valuef-100)*MODULUS/1.414f/arm_config_data_read32(data_handle[addr].coef_id)*100000);
        break;

    case METHOD_3:
        val_ret = ((valuef-100)*MODULUS/arm_config_data_read32(data_handle[addr].coef_id)*100000);
        break;

    case METHOD_4:
        val_ret = (valuef*MODULUS*1.732f/1.414f/arm_config_data_read32(data_handle[addr].coef_id)*100000);
        break;

    case METHOD_5:
        val_ret = (valuef*MODULUS/343.6f);
        break;

    case METHOD_6:
        val_ret = (valuef*MODULUS/1.414f/arm_config_data_read32(data_handle[addr].coef_id)*100000);
        break;
		
	case METHOD_7:		
		val_ret = valuef/3.0f;		
		break;

    default:
        val_ret = valuef;
        break;
    }

    return (short)val_ret;
}


unsigned short modbus_read_reg_u0(unsigned short addr, unsigned char* err)
{
    int value = 0;
	//short value_pare;
    float power_count = 0;
    unsigned short addr_tmp;

    /* FPGA直通数据 */
    if(addr <= 255) {
        value = fpga_pdatabuf_get[addr];
    }

    /* 时间 */
    else if(addr >= 300 && addr <= 309) 
	{
        switch(addr)
        {
        case 300:
            value = screen_protect.time_set_en;
            break;
        case 301:
            value = Arm_time.time_year_get;
            break;
        case 302:
            value = Arm_time.time_month_get;
            break;
        case 303:
            value = Arm_time.time_date_get;
            break;
        case 306:
            value = Arm_time.time_hour_get;
            break;
        case 307:
            value = Arm_time.time_minute_get;
            break;
        case 308:
            value = Arm_time.time_second_get;
            break;
        default:
            return value;
        }
    }

    /* 开关机 */
    else if(addr >= 310 && addr <= 319) 
	{
        switch(addr)
        {
        case 310:
            value = Pcs_Ctrl_Mode_Get();
            break;
			
        case 311:
            value = Pcs_Run_Mode_Get();
            break;
			
        case 312:
            value = sys_ctl.onoff;
            break;
			
        case 313:
            if(GETBIT(Upper_Param[Addr_Param112],0) == 1) {
                SETBIT(value,0);
            } else {
                value = RESETBIT(value,0);
            }
            if(arm_config_data_read(ISO_ISLAND_ADD) == 1) {
                SETBIT(value,1);
            } else {
                value = RESETBIT(value,1);
            }
            if(GETBIT(Upper_Param[Addr_Param112],2) == 1) {
                SETBIT(value,2);
            } else {
                value = RESETBIT(value,2);
            }
            if(arm_config_data_read(SVG_EN_ADD) == 1) {
                SETBIT(value,3);
            } else {
                value = RESETBIT(value,3);
            }
            break;

		case 314:
			break;
		
		case 315:
			break;
			
		case 316:
			value = miro_write.communication_en;
			break;
			
		case 317:
			value = miro_write.strategy_en;
			break;

        default:
            return value;
        }
    }

    /* 以太网 */
    else if(addr >= 320 && addr <= 329) 
	{
        switch(addr)
        {
        case 320:
            value = screen_protect.eth_set_en;
            break;
        case 321:
            value = ip4_addr[0];
            break;
        case 322:
            value = ip4_addr[1];
            break;
        case 323:
            value = ip4_addr[2];
            break;
        case 324:
            value = ip4_addr[3];
            break;
        case 325:
            value = mac_addr[0];
            break;
        case 326:
            value = mac_addr[1];
            break;
        case 327:
            value = mac_addr[2];
            break;
        case 328:
            value = mac_addr[3];
            break;
        default:
            return value;
        }
    }

    /* 485 */
    else if(addr >= 330 && addr <= 339) 
	{
        switch(addr)
        {
        case 330:
            value = screen_protect.rs485_set_en;
            break;
        case 331:
            mod_config.addr = arm_config_data_read(INI_MONITOR_ADDR_ADD);
            value = mod_config.addr;
            break;
        case 332:
            mod_config.baud = arm_config_data_read(UART_CONFIG_BAUD);
            value = mod_config.baud;
            break;
        case 333:
            mod_config.data_bit = arm_config_data_read(UART_CONFIG_DATABIT);
            value = mod_config.data_bit;
            break;
        case 334:
            mod_config.stop = arm_config_data_read(UART_CONFIG_STOP);
            value = mod_config.stop;
            break;
        case 335:
            mod_config.parity = arm_config_data_read(UART_CONFIG_PARITY);
            value = mod_config.parity;
            break;
        default:
            return value;
        }
    }

    /* AC */
    else if(addr >= 400 && addr <= 419) 
	{
        switch(addr)
        {
        case 400:
            addr_tmp = Addr_Param0;
            break;
        case 403:
            addr_tmp = Addr_Param9;
            break;
        case 404:
            addr_tmp = Addr_Param1;
            break;
        case 408:
            addr_tmp = Addr_Param2;
            break;
        case 411:
            addr_tmp = Addr_Param10;
            break;
        case 412:
            addr_tmp = Addr_Param28;
            break;
        case 413:
            addr_tmp = Addr_Param28;
            break;
        case 414:
            addr_tmp = Addr_Param30;
            break;
        case 415:
            addr_tmp = Addr_Param29;
            break;
        case 416:
            addr_tmp = Addr_Param24;
            break;
        case 417:
            addr_tmp = Addr_Param26;
            break;
        case 418:
            addr_tmp = Addr_Param27;
            break;
        case 419:
            addr_tmp = Addr_Param23;
            break;
        default:
            return value;
        }
        value=getting_data_handle(addr_tmp, fpga_pdatabuf_get[addr_tmp]);
    }

    /* BUS */
    else if(addr >= 420 && addr <= 439) 
	{
        switch(addr)
        {
        case 420:
            addr_tmp = Addr_Param3;
            break;
        case 423:
            addr_tmp = Addr_Param11;
            break;
        case 424:
            addr_tmp = Addr_Param4;
            break;
        case 428:
            addr_tmp = Addr_Param5;
            break;
        case 431:
            addr_tmp = Addr_Param12;
            break;
        case 432:
            addr_tmp = Addr_Param34;
            break;
        case 433:
            addr_tmp = Addr_Param33;
            break;
        case 434:
            addr_tmp = Addr_Param32;
            break;
        case 435:
            addr_tmp = Addr_Param31;
            break;
        case 436:
            addr_tmp = Addr_Param23;
            break;
        default:
            if(addr == 437) {
                cp_mode_flag = arm_config_data_read(CP_MODE_FLAG_ADD);
                if(cp_mode_flag  == 2) {
                    addr_tmp = Addr_Param31;
                    value=getting_data_handle(addr_tmp, fpga_pdatabuf_get[addr_tmp]);
                } else if(cp_mode_flag == 1) {
                    addr_tmp = Addr_Param32;
                    value=getting_data_handle(addr_tmp, fpga_pdatabuf_get[addr_tmp]);
                } else {
                    value = 0;
                }
            }
            if(addr == 438) {
                ci_mode_flag = arm_config_data_read(CI_MODE_FLAG_ADD);
                if(ci_mode_flag  == 2) {
                    addr_tmp = Addr_Param33;
                    value=getting_data_handle(addr_tmp, fpga_pdatabuf_get[addr_tmp]);
                } else if(ci_mode_flag  == 1) {
                    addr_tmp = Addr_Param34;
                    value=getting_data_handle(addr_tmp, fpga_pdatabuf_get[addr_tmp]);
                } else {
                    value = 0;
                }
            }
            return value;
        }
        value=getting_data_handle(addr_tmp, fpga_pdatabuf_get[addr_tmp]);
    }

    /* DC1 */
    else if(addr >= 440 && addr <= 459) 
	{
        switch(addr)
        {
        case 440:
            addr_tmp = Addr_Param6;
            break;
        case 443:
            addr_tmp = Addr_Param13;
            break;
        case 444:
            addr_tmp = Addr_Param7;
            break;
        default:
            return value;
        }
        value=getting_data_handle(addr_tmp, fpga_pdatabuf_get[addr_tmp]);
    }
	
    /* 一次调频设定 */
    else if(addr >= 500 && addr <= 502) 
	{
        switch(addr)
        {
        case 500:
            value = miro_write.pfm_able;
            break;
			
        case 501:
			value = miro_write.dead_zone;
            break;
			
        case 502:
			value = miro_write.diversity_factor;
            break;
			
        default:
            return value;
        }
    }

    /* 保护时间 */
    else if(addr >= 600 && addr <= 699) 
	{
        switch(addr)
        {
        case 600:
            addr_tmp = Addr_Param16;
            break;
        case 603:
            addr_tmp = Addr_Param17;
            break;
        default:
            return value;
        }
        value=getting_data_handle(addr_tmp, fpga_pdatabuf_get[addr_tmp]);
    }

    /* 硬件保护 */
    else if(addr >= 700 && addr <= 799) 
	{
        switch(addr)
        {
        case 701:
            addr_tmp = Addr_Param8;
            break;
			
		case 704:
            addr_tmp = Addr_Param18;
            break;
			
		//漏电流	保护值显示
		case 705:
            addr_tmp = Addr_Param19;
            break;			
        default:
            return value;
        }
        value=getting_data_handle(addr_tmp, fpga_pdatabuf_get[addr_tmp]);
    }

    /* 电池充放电限压值 */
    else if(addr >= 800 && addr <= 899) 
	{
        switch(addr)
        {
        case 810:
            value = arm_config_data_read(CHRG_VOL_LIMIT_ADD);
            break;
        case 811:
            value = arm_config_data_read(DISC_VOL_LIMIT_ADD);
            break;
        case 812:
            value = arm_config_data_read(END_SWITCH_EN_ADD);
            break;
        default:
            return value;
        }
    }

  
  /* 尖峰平谷时间 */
	  else if(addr >= 1200 && addr <= 1300) 
	  {
		  switch(addr)
		  {
			  case 1200:
				  value=arm_config_data_read(START_TIME_0);
				  break;
			  case 1201:
				  value=arm_config_data_read(STOP_TIME_0);
				  break;
			  case 1202:
				  value=arm_config_data_read(SET_POWER_0);
				  break;
			  case 1203:
				  value=arm_config_data_read(CAPACITY);
				  break;
				  
			  case 1205:
				  value=arm_config_data_read(START_TIME_1);
				  break;
			  case 1206:
				  value=arm_config_data_read(STOP_TIME_1);
				  break;
			  case 1207:
				  value=arm_config_data_read(SET_POWER_1);
				  break;
			  //case 1208:
			  //  value=arm_config_data_read(CAPACITY);
			  //  break;
			  case 1210:
				  value=arm_config_data_read(START_TIME_2);
				  break;
			  case 1211:
				  value=arm_config_data_read(STOP_TIME_2);
				  break;
			  case 1212:
				  value=arm_config_data_read(SET_POWER_2);
				  break;
			  //case 1213:
			  //  value=arm_config_data_read(CAPACITY);
			  //  break;
			  case 1215:
				  value=arm_config_data_read(START_TIME_3);
				  break;
			  case 1216:
				  value=arm_config_data_read(STOP_TIME_3);
				  break;
			  case 1217:
				  value=arm_config_data_read(SET_POWER_3);
				  break;
			  //case 1218:
			  //  value=arm_config_data_read(CAPACITY);
			  //  break;
			  case 1220:
				  value=arm_config_data_read(START_TIME_4);
				  break;
			  case 1221:
				  value=arm_config_data_read(STOP_TIME_4);
				  break;
			  case 1222:
				  value=arm_config_data_read(SET_POWER_4);
				  break;
			  //case 1223:
			  //  value=arm_config_data_read(CAPACITY);
			  //  break;
  
			  case 1225:
				  value=arm_config_data_read(START_TIME_5);
				  break;
			  case 1226:
				  value=arm_config_data_read(STOP_TIME_5);
				  break;
			  case 1227:
				  value=arm_config_data_read(SET_POWER_5);
				  break;
			  //case 1228:
			  //  value=arm_config_data_read(CAPACITY);
			  //  break;
				  
			  case 1230:
				  value=arm_config_data_read(START_TIME_6);
				  break;
			  case 1231:
				  value=arm_config_data_read(STOP_TIME_6);
				  break;
			  case 1232:
				  value=arm_config_data_read(SET_POWER_6);
				  break;
			  //case 1233:
			  //  value=arm_config_data_read(CAPACITY);
			  //  break;
				  
			  case 1235:
				  value=arm_config_data_read(START_TIME_7);
				  break;
			  case 1236:
				  value=arm_config_data_read(STOP_TIME_7);
				  break;
			  case 1237:
				  value=arm_config_data_read(SET_POWER_7);
				  break;
			  //case 1238:
			  //  value=arm_config_data_read(CAPACITY);
			  //  break;
			  
			  case 1297:
				  value = arm_config_data_read(DISCHARGE_POWER_LMT);
				  break;
				  
			  case 1298:
				  value = arm_config_data_read(CHARGE_POWER_LMT);
				  break;
				  
			  case 1299:
				  value = arm_config_data_read(XFTG_EN);
				  break;
				  
			  case 1300:
				  value = arm_config_data_read(cos_seta);
				  break;	
				  
			  default:
				  return value;
		  }
		  //tty_printf("addr[%d] value[%d]\n", addr, value);
	  }   

	#if 1
	 //cmd控制命令显示
  	  else if(addr >= 1301 && addr <= 1310) 
	  {
	  	switch(addr)
		{
			case 1301:
				  value = Upper_Param[Addr_Param110];
				  break;
				  
			case 1302:
				  value = Upper_Param[Addr_Param111];
				  break;
				  
			case 1303:
				  value = Upper_Param[Addr_Param112];
				  break;
				  
			case 1304:
				  value = Upper_Param[Addr_Param113];
				  break;
				  
			case 1305:
				  value = Upper_Param[Addr_Param114];
				  break;
				  
			case 1306:
				  value = Upper_Param[Addr_Param115];
				  break;
				  
			case 1307:
				  value = Upper_Param[Addr_Param116];
				  break;
				  
			case 1308:
				  value = Upper_Param[Addr_Param117];
				  break;
				  
			case 1309:
				  value = Upper_Param[Addr_Param118];
				  break;
				  
			case 1310:
				  value = Upper_Param[Addr_Param119];
				  break;
				  
			default:
				  return value;

		}

  	 }
#endif
		 
	  /* 版本 */
	  else if(addr >= 3000 && addr <= 3019) 
	  {
		  switch(addr)
		  {			  

			  case 3006:
				  value = miro_write.device_year;
				  break;

			  case 3007:
				  value = miro_write.device_md;
				  break;

			  case 3008:
				  value = miro_write.device_type;
				  break;

			  case 3009:
				  value = miro_write.device_num;
				  break;				 

			  case 3010:
				  value = SOFT_VER;
				  break;
				  
			  case 3011:
				  value = Upper_Param[Addr_Param335];
				  break;

			  case 3016:
				  value = miro_write.code_type;
				  break;

			  case 3017:
				  value = miro_write.code_remain_date;
				  break;

			  case 3018:
				value = miro_write.code_err_tick;
				break;

				  
			  default:
				  return value;
		  }
	  }
	  
	  /* 实时重要参数 */
	  else if(addr >= 3020 && addr <= 3029) 
	  {
		  switch(addr)
		  {
			  case 3020:
				  value = out_state_info.Sys_State1 & 0x3;
				  break;
			  case 3021:
				  value = mb_data.efficiency;
				  break;
			  case 3022:
				  value = mb_data.cur_soc;
				  break;
			  case 3023:
				  value = mb_data.last_onoff_mark;
				  break;
			  default:
				  return value;
		  }
	  }
		  
	  /* AC交流 */
	  else if(addr >= 3030 && addr <= 3039) 
	  {
		  switch(addr)
		  {
			  case 3030:
				  value = getting_data_handle(Addr_Param256, Upper_Param[Addr_Param256]);
				  break;
			  case 3031:
				  value = getting_data_handle(Addr_Param257, Upper_Param[Addr_Param257]);
				  break;
			  case 3032:
				  value = getting_data_handle(Addr_Param258, Upper_Param[Addr_Param258]);
				  break;
			  case 3033:
#ifdef HAS_CT //互感器显示
				  value = getting_data_handle(Addr_Param136, Upper_Param[Addr_Param136]);
#else //传感器显示
				  value = getting_data_handle(Addr_Param259, Upper_Param[Addr_Param259]);
#endif
				  break;
			  case 3034:
#ifdef HAS_CT //互感器显示
				  value = getting_data_handle(Addr_Param137, Upper_Param[Addr_Param137]);
#else //传感器显示
				  value = getting_data_handle(Addr_Param260, Upper_Param[Addr_Param260]);
#endif
				  break;
			  case 3035:
#ifdef HAS_CT //互感器显示
				  value = getting_data_handle(Addr_Param138, Upper_Param[Addr_Param138]);
#else //传感器显示
				  value = getting_data_handle(Addr_Param261, Upper_Param[Addr_Param261]);
#endif
				  break;
			  case 3036:
#ifdef HAS_CT //互感器显示
				  value = getting_data_handle(Addr_Param189, Upper_Param[Addr_Param189]);
#else //传感器显示
				  value = getting_data_handle(Addr_Param267, Upper_Param[Addr_Param267]);
#endif
				  break;
			  case 3037:
#ifdef HAS_CT //互感器显示
				  value = getting_data_handle(Addr_Param190, Upper_Param[Addr_Param190]);
#else //传感器显示
				  value = getting_data_handle(Addr_Param268, Upper_Param[Addr_Param268]);
#endif
				  break;
			  case 3038:
				  value = mb_data.pfactor;
				  break;
			  case 3039:
				  value = getting_data_handle(Addr_Param271, Upper_Param[Addr_Param271]);
				  break;
			  default:
				  return value;
		  }
	  }
  
	  /* BUS母线 */
	  else if(addr >= 3040 && addr <= 3049) 
	  {
		  switch(addr)
		  {
			  case 3040:
				  value = getting_data_handle(Addr_Param262, Upper_Param[Addr_Param262]);
				  break;
			  case 3043:
				  value = getting_data_handle(Addr_Param263, Upper_Param[Addr_Param263]);
				  break;
			  case 3046:
				  value = getting_data_handle(Addr_Param269, Upper_Param[Addr_Param269]);
				  break;
			  default:
				  return value;
		  }
	  }
  
	  /* DC1电池 */
	  else if(addr >= 3050 && addr <= 3059) 
	  {
		  switch(addr)
		  {
			  case 3050:
				  value = getting_data_handle(Addr_Param264, Upper_Param[Addr_Param264]);
				  break;
			  case 3051:
				  value = getting_data_handle(Addr_Param265, Upper_Param[Addr_Param265]);
				  break;
			  case 3052:
				  value = getting_data_handle(Addr_Param266, Upper_Param[Addr_Param266]);
				  break;
			  case 3053:
				  value = getting_data_handle(Addr_Param263, Upper_Param[Addr_Param263]);
				  break;
			  case 3054:
				  value = getting_data_handle(Addr_Param270, Upper_Param[Addr_Param270]);
				  break;
			//漏电流显示	  
			 case 3055:
				  value = getting_data_handle(Addr_Param328, Upper_Param[Addr_Param328]);
				  break;
				  
			  case 3056:
				  value = getting_data_handle(Addr_Param269, Upper_Param[Addr_Param269]);
				  break;  
			  default:
				  return value;
		  }
	  } 		  

	  /* 一次调频相关数据 */
	  else if(addr >= 3080 && addr <= 3089) 
	  {
		  switch(addr)
		  {
			  case 3080:
				  value = 0;//out_state_info.fre_adjust_flag;
				  break;
				  
			  case 3081:
				  value = (short)(getting_data_handle(Addr_Param192, Upper_Param[Addr_Param192]));
				  break;
				  
			  case 3082:
				  value = (short)(getting_data_handle(Addr_Param192, Upper_Param[Addr_Param192]) + getting_data_handle(Addr_Param26, fpga_pdatabuf_get[Addr_Param26]));
				  break;

			  case 3083:
				value = (unsigned int)CountData.fre_adjust_count_today >> 16;				
				break;
				
			  case 3084:
				value = (unsigned int)CountData.fre_adjust_count_today & 0xffff;				
				break;

			  case 3085:
				value = (unsigned int)CountData.fre_adjust_count >> 16;				
				break;
				
			  case 3086:
				value = (unsigned int)CountData.fre_adjust_count & 0xffff;				
				break;	
				
			  default:
				  return value;
		  }
	  } 
	  
	  /* 绝缘检测信息 */
	  else if(addr >= 3090 && addr <= 3109) 
	  {
		  switch(addr)
		  {
			  case 3090:
				  value = isoInfo.temp[0];
				  break;
			  case 3091:
				  value = isoInfo.temp[1];
				  break;
			  case 3092:
				  value = isoInfo.temp[2];
				  break;
			  case 3093:
				  value = isoInfo.temp[3];
				  break;
			  case 3094:
				  value = isoInfo.temp[4];
				  break;
			  case 3095:
				  value = isoInfo.temp[5];
				  break;
			  case 3096:
				  value = isoInfo.temp[6];
				  break;
			  case 3097:
				  value = isoInfo.temp[7];
				  break;
			  case 3098:
				  value = isoInfo.volGridAB;
				  break;
			  case 3099:
				  value = isoInfo.volGridBC;
				  break;
			  case 3100:
				  value = isoInfo.volGridCA;
				  break;
			  case 3101:
				  value = isoInfo.isoResPos;
				  break;
			  case 3102:
				  value = isoInfo.isoResNeg;
				  break;
			  case 3103:
				  value = isoInfo.curGridA;
				  break;
			  case 3104:
				  value = isoInfo.curGridB;
				  break;
			  case 3105:
				  value = isoInfo.curGridC;
				  break;
			  case 3106:
				  value = isoInfo.curLeak;
				  break;
			  case 3107:
				  value = isoInfo.isoVolPos;
				  break;
			  case 3108:
				  value = isoInfo.isoVolNeg;
				  break;
			  default:
				  return 0;
		  }
	  } 
  
	  /* IGBT温度 */
	  else if(addr >= 3130 && addr <= 3139) 
	  {
		  switch(addr)
		  {
			  case 3130:
				  value = pcs_manage.temp_a_max;//Upper_Param[Addr_Param196];
				  break;
				  
			  case 3131:
				  value = pcs_manage.temp_b_max;//Upper_Param[Addr_Param197];
				  break;
				  
			  case 3132:
				  value = pcs_manage.temp_c_max;//Upper_Param[Addr_Param198];
				  break;
				  
			  default:
				  return value;
		  }
	  } 			  
  
	  /* 系统状态 */
	  else if(addr >= 3140 && addr <= 3199) 
	  {
		  switch(addr)
		  {
			  case 3140:
				  value = Upper_Param[Addr_Param275];
				  break;
			  case 3141:
				  value = Upper_Param[Addr_Param276];
				  break;
			  case 3142:
				  value = Upper_Param[Addr_Param277];
				  break;
			  case 3143:
				  value = Upper_Param[Addr_Param278];
				  break;
			  case 3144:
				  value = Upper_Param[Addr_Param279];
				  break;
			  case 3145:
				  value = Upper_Param[Addr_Param280];
				  break;
			  case 3146:
				  value = Upper_Param[Addr_Param281];
				  break;
			  case 3147:
				  value = Upper_Param[Addr_Param282];
				  break;
			  case 3148:
				  value = Upper_Param[Addr_Param283];
				  //tty_printf("state is %d\n",(value & 0xf));
				  break;
			  case 3149:
				  value = Upper_Param[Addr_Param284];
				  break;
			  case 3150:
				  value = Upper_Param[Addr_Param285];
				  break;
			  case 3151:
				  value = Upper_Param[Addr_Param286];
				  break;
			  case 3152:
				  value = Upper_Param[Addr_Param287];
				  break;
			  case 3153:
				  value = Upper_Param[Addr_Param288];
				  break;
			  case 3154:
				  value = Upper_Param[Addr_Param289];
				  break;
			  case 3155:
				  value = Upper_Param[Addr_Param290];
				  break;
			  case 3156:
				  value = Upper_Param[Addr_Param291];
				  break;
			  case 3157:
				  value = Upper_Param[Addr_Param292];
				  break;
			  case 3158:
				  value = Upper_Param[Addr_Param293];
				  break;
			  case 3159:
				  value = Upper_Param[Addr_Param294];
				  break;
			  case 3160:
				  value = Upper_Param[Addr_Param295];
				  break;
			  case 3161:
				  value = Upper_Param[Addr_Param296];
				  break;
			  case 3162:
				  value = Upper_Param[Addr_Param297];
				  break;
			  case 3163:
				  value = Upper_Param[Addr_Param298];
				  break;
			  case 3164:
				  value = Upper_Param[Addr_Param299];
				  break;
			  case 3165:
				  value = Upper_Param[Addr_Param300];
				  break;
			  case 3166:
				  value = Upper_Param[Addr_Param301];
				  break;
			  case 3167:
				  value = Upper_Param[Addr_Param302];
				  break;
			  case 3168:
				  value = Upper_Param[Addr_Param303];
				  break;
			  case 3169:
				  value = Upper_Param[Addr_Param304];
				  break;
			  case 3170:
				  value = Upper_Param[Addr_Param305];
				  break;
			  case 3171:
				  value = Upper_Param[Addr_Param306];
				  break;  
			  default:
				  return value;
		  }
	  } 			  
  
	  /* 发电量 */
	  else if(addr >= 3400 && addr <= 3599) 
	  {
		  //tty_printf("[HMI GET] addr:value=%d:%d\n", addr, value);
		  switch(addr)
		  {
			  case 3400:
				  //CountData.power_ac_day = count_cal_read(POWER_AC_DAY_ADD);	//20171122test
				  //CountData.power_ac_day=count_cal[POWER_AC_DAY_ADD];
				  power_count = CountData.power_ac_day * MODULUS;
				  value=(unsigned int)power_count >> 16;
				  break;
				  
			  case 3401:
				  //CountData.power_ac_day=count_cal[POWER_AC_DAY_ADD];
				  power_count = CountData.power_ac_day * MODULUS;
				  value=(unsigned int)power_count & 0xffff;
				  break;
			  case 3402:
				  //CountData.power_ac_c_day=count_cal[POWER_AC_C_DAY_ADD];
				  power_count = CountData.power_ac_c_day * MODULUS;
				  value=(unsigned int)power_count >> 16;
				  break;
			  case 3403:
				  //CountData.power_ac_c_day=count_cal[POWER_AC_C_DAY_ADD];
				  power_count = CountData.power_ac_c_day * MODULUS;
				  value=(unsigned int)power_count & 0xffff;
				  break;
			  case 3408:
				  //CountData.power_dc1_day=count_cal[POWER_DC1_DAY_ADD];
				  power_count = CountData.power_dc1_day * MODULUS;
				  value=(unsigned int)power_count >> 16;
				  break;
			  case 3409:
				  //CountData.power_dc1_day=count_cal[POWER_DC1_DAY_ADD];
				  power_count = CountData.power_dc1_day * MODULUS;
				  value=(unsigned int)power_count & 0xffff;
				  break;
			  case 3410:
				  //CountData.power_dc1_c_day=count_cal[POWER_DC1_C_DAY_ADD];
				  power_count = CountData.power_dc1_c_day * MODULUS;
				  value=(unsigned int)power_count >> 16;
				  break;
			  case 3411:
				  //CountData.power_dc1_c_day=count_cal[POWER_DC1_C_DAY_ADD];
				  power_count = CountData.power_dc1_c_day * MODULUS;
				  value=(unsigned int)power_count & 0xffff;
				  break;
			  case 3412:
				  //CountData.power_dc2_day=count_cal[POWER_DC2_DAY_ADD];
				  power_count = CountData.power_dc2_day * MODULUS;
				  value=(unsigned int)power_count >> 16;
				  break;
			  case 3413:
				  //CountData.power_dc2_day=count_cal[POWER_DC2_DAY_ADD];
				  power_count = CountData.power_dc2_day * MODULUS;
				  value=(unsigned int)power_count & 0xffff;
				  break;
			  case 3414:
				  value=0;
				  break;
			  case 3415:
				  value=0;
				  break;
			  case 3440:
				  //CountData.power_ac_con=count_cal[POWER_AC_CON_ADD];
				  power_count = CountData.power_ac_con * MODULUS;
				  value=(unsigned int)power_count >> 16;
				  break;
			  case 3441:
				  //CountData.power_ac_con=count_cal[POWER_AC_CON_ADD];
				  power_count = CountData.power_ac_con * MODULUS;
				  value=(unsigned int)power_count & 0xffff;
				  break;
			  case 3442:
				  //CountData.power_ac_c_con=count_cal[POWER_AC_C_CON_ADD];
				  power_count = CountData.power_ac_c_con * MODULUS;
				  value=(unsigned int)power_count >> 16;
				  break;
			  case 3443:
				  //CountData.power_ac_c_con=count_cal[POWER_AC_C_CON_ADD];
				  power_count = CountData.power_ac_c_con * MODULUS;
				  value=(unsigned int)power_count & 0xffff;
				  break;
			  case 3448:
				  //CountData.power_dc1_con=count_cal[POWER_DC1_CON_ADD];
				  power_count = CountData.power_dc1_con * MODULUS;
				  value=(unsigned int)power_count >> 16;
				  break;
			  case 3449:
				  //CountData.power_dc1_day=count_cal[POWER_DC1_CON_ADD];
				  power_count = CountData.power_dc1_day * MODULUS;
				  value=(unsigned int)power_count & 0xffff;
				  break;
			  case 3450:
				  //CountData.power_dc1_c_con=count_cal[POWER_DC1_C_CON_ADD];
				  power_count = CountData.power_dc1_c_con * MODULUS;
				  value=(unsigned int)power_count >> 16;
				  break;
			  case 3451:
				  //CountData.power_dc1_c_day=count_cal[POWER_DC1_C_CON_ADD];
				  power_count = CountData.power_dc1_c_day * MODULUS;
				  value=(unsigned int)power_count & 0xffff;
				  break;
			  case 3452:
				  //CountData.power_dc2_con=count_cal[POWER_DC2_CON_ADD];
				  power_count = CountData.power_dc2_con * MODULUS;
				  value=(unsigned int)power_count >> 16;
				  break;
			  case 3453:
				  //CountData.power_dc2_con=count_cal[POWER_DC2_CON_ADD];
				  power_count = CountData.power_dc2_con * MODULUS;
				  value=(unsigned int)power_count & 0xffff;
				  break;
			  case 3454:
				  value=0;
				  break;
			  case 3455:
				  value=0;				  
				  break;
				  
			  case 3480:
				//CountData.run_time_total = count_cal[RUN_TIME];
				value = (unsigned int)CountData.run_time_total >> 16;				
				break;
				
			  case 3481:
				//CountData.run_time_total = count_cal[RUN_TIME];
				value = (unsigned int)CountData.run_time_total & 0xffff;				
				break;
				
			  case 3490:
				  //CountData.power_ac_yesterday=count_cal[POWER_AC_Y_ADD];
				  power_count = CountData.power_ac_yesterday * MODULUS;
				  value=(unsigned int)power_count >> 16;
				  break;
			  case 3491:
				  //CountData.power_ac_yesterday=count_cal[POWER_AC_Y_ADD];
				  power_count = CountData.power_ac_yesterday * MODULUS;
				  value=(unsigned int)power_count & 0xffff;
				  break;
			  case 3492:
				  //CountData.power_ac_c_yesterday=count_cal[POWER_AC_C_Y_ADD];
				  power_count = CountData.power_ac_c_yesterday * MODULUS;
				  value=(unsigned int)power_count >> 16;
				  break;
			  case 3493:
				  //CountData.power_ac_c_yesterday=count_cal[POWER_AC_C_Y_ADD];
				  power_count = CountData.power_ac_c_yesterday * MODULUS;
				  value=(unsigned int)power_count & 0xffff;
				  break;
			  case 3494:
				  //CountData.power_ac_year=count_cal[POWER_AC_YEAR_ADD];
				  power_count = CountData.power_ac_year * MODULUS;
				  value=(unsigned int)power_count >> 16;
				  break;
			  case 3495:
				  //CountData.power_ac_year=count_cal[POWER_AC_YEAR_ADD];
				  power_count = CountData.power_ac_year * MODULUS;
				  value=(unsigned int)power_count & 0xffff;
				  break;
				  
			  case 3496:
				  //CountData.power_ac_c_yesterday=count_cal[POWER_AC_C_Y_ADD];
				  power_count = CountData.power_ac_c_yesterday * MODULUS;
				  value=(unsigned int)power_count >> 16;
				  break;
			  case 3497:
				  //CountData.power_ac_c_yesterday=count_cal[POWER_AC_C_Y_ADD];
				  power_count = CountData.power_ac_c_yesterday * MODULUS;
				  value=(unsigned int)power_count & 0xffff;
				  break;
			  case 3498:
				  //CountData.power_ac_c_month=count_cal[POWER_AC_C_MONTH_ADD];
				  power_count = CountData.power_ac_c_month * MODULUS;
				  value=(unsigned int)power_count >> 16;
				  break;
			  case 3499:
				  //CountData.power_ac_c_month=count_cal[POWER_AC_C_MONTH_ADD];
				  power_count = CountData.power_ac_c_month * MODULUS;
				  value=(unsigned int)power_count & 0xffff;
				  break;
			  case 3500:
				  //CountData.power_ac_c_year=count_cal[POWER_AC_C_YEAR_ADD];
				  power_count = CountData.power_ac_c_year * MODULUS;
				  value=(unsigned int)power_count >> 16;
				  break;
			  case 3501:
				  //CountData.power_ac_c_year=count_cal[POWER_AC_C_YEAR_ADD];
				  power_count = CountData.power_ac_c_year * MODULUS;
				  value=(unsigned int)power_count & 0xffff;
				  break;
				  
			  //连续运行时间  
			  case 3502:
				value = (unsigned int)CountData.run_time_now >> 16;				
				break;
				
			  case 3503:
				value = (unsigned int)CountData.run_time_now & 0xffff;				
				break;
				
		      //程序总工作时长
			  case 3504:
				//CountData.on_time_total = count_cal[ON_TIME];
				value = (unsigned int)CountData.on_time_total >> 16;				
				break;
				
			  case 3505:
				//CountData.on_time_total = count_cal[ON_TIME];
				value = (unsigned int)CountData.on_time_total & 0xffff;				
				break;	
				
			  //程序连续工作时长
			  case 3506:
				value = (unsigned int)CountData.on_time_now >> 16;				
				break;
				
			  case 3507:
				value = (unsigned int)CountData.on_time_now & 0xffff;				
				break;
				
			  //复位次数
			  case 3508:
				//CountData.restart_tick = count_cal[RESTART_COUNT];
				value = (unsigned int)CountData.restart_tick >> 16;				
				break;
				
			  case 3509:			  	
				//CountData.restart_tick = count_cal[RESTART_COUNT];
				value = (unsigned int)CountData.restart_tick & 0xffff;				
				break;				
																  
			  default:
				  return value;
		  }   
	  }
  
	  /* 发电量历史 */
	  else if(addr >= 3600) 
	  {
		  //tty_printf("[HMI GET more] addr:value=%d:%d\n", addr, value);
		  if((POWER_BASE_ADDR <= addr) && ((POWER_BASE_ADDR + POWER_DATA_SIZE) >= addr)) {
			  if(addr % 2 ==0) {
				  read_power_data(addr - POWER_BASE_ADDR, &power_count);
				  power_count *= MODULUS;
				  value = (unsigned int)power_count >> 16;
			  } else {
				  read_power_data(addr - 1 - POWER_BASE_ADDR, &power_count);
				  power_count *= MODULUS;
				  value = (unsigned int)power_count & 0xffff;
			  }
		  } else {
			  value=0;
		  }
	  }
	  
	  //if(addr >= 3140 && addr < 3200) {
		//  return value;
	  //}
	  //tty_printf("addr[%d] value[%d]\n", addr, value);
	  return value;
  }
  
unsigned char modbus_write_reg_u0(unsigned short addr, short value)
{
    CPU_SR     cpu_sr;
    int err = 0;
    unsigned char ctrl_flag=0;
    int count=0;
    //short cmd0=0;
    unsigned short addr_tmp;
	
	short base_fre;
	float charge_power_lim_set;	

    sys_ctl.dev_opt_mode = fpga_read(Addr_Param111) & 0x1;

    for(count = 0; count < MAX_CONN_NUM; count++) 
	{
        if( mbser_inf[count].conns == 1)
		{
            if(sys_ctl.dev_opt_mode==1) 
			{
                ctrl_flag = CTRL_SCREEN;
                break;
            } 
			else 
			{
                ctrl_flag = CTRL_UPPER;
                break;
            }
        } 
		else 
		{
            ctrl_flag = CTRL_SCREEN;
        }
    }
    //tty_printf("[SCREEN SET] addr:value=(%d:%d) ctrl_flag=%d\n", addr, value, ctrl_flag);
	printf_debug5("[HMI SET]: addr:value=%d:%d  ctrlMode=%d\n", addr, value, Pcs_Ctrl_Mode_Get());
    //if(ctrl_flag == CTRL_SCREEN && Pcs_Ctrl_Mode_Get() == HMI_CTRL) 		
    if(Pcs_Ctrl_Mode_Get() == HMI_CTRL || Pcs_Ctrl_Mode_Get() == EMS_CTRL || Pcs_Ctrl_Mode_Get() == 0 || Pcs_Ctrl_Mode_Get() == BMS_CTRL) 
	{
        /* 设置参数 */
        if(addr <= 255) 
		{
            fpga_pdatabuf_get[addr]=setting_data_handle(addr, value);
            setpara(addr,fpga_pdatabuf_get[addr]);
            CPU_CRITICAL_ENTER();
            fpga_write(addr,fpga_pdatabuf_get[addr]);
            fpga_pdatabuf_write(addr,fpga_pdatabuf_get[addr]);
            CPU_CRITICAL_EXIT();
        }

        /* 时间 */
        else if(addr >= 300 && addr <= 309) {
            switch(addr)
            {
            case 300:
                screen_protect.time_set_en = value;
                arm_config_data_write(SCREEN_PROTECT_TIME, screen_protect.time_set_en);
                break;
            case 301:
                if(screen_protect.time_set_en == 1) {
                    timeset.years = 1;
                    timeset.year = value;
                    timeset_delay(100);
                }
                break;
            case 302:
                if(screen_protect.time_set_en == 1) {
                    timeset.mons = 1;
                    timeset.mon = value;
                    timeset_delay(100);
                }
                break;
            case 303:
                if(screen_protect.time_set_en == 1) {
                    timeset.dates = 1;
                    timeset.date = value;
                    timeset_delay(100);
                }
                break;
            case 304:
                break;
            case 305:
                break;
            case 306:
                if(screen_protect.time_set_en == 1) {
                    timeset.hours = 1;
                    timeset.hour = value;
                    timeset_delay(100);
                }
                break;
            case 307:
                if(screen_protect.time_set_en == 1) {
                    timeset.mins = 1;
                    timeset.min = value;
                    timeset_delay(100);
                }
                break;
            case 308:
                if(screen_protect.time_set_en == 1) {
                    timeset.secs = 1;
                    timeset.sec = value;
                    timeset_delay(100);
                }
                break;
            default:
                return err;
            }
        }

        /* 开关机 */
        else if(addr >= 310 && addr <= 319)
		{
           switch(addr)
           {
           	case 310:
                Pcs_Ctrl_Mode_Set(value);
                //tty_printf("[HMI SET]: ctr_mode[%d]\n", value);
              break;
				
            case 311:
                Pcs_Run_Mode_Set(value);
                run_mode_hmi = value;
                break;
				
            case 312:
                sys_ctl.onoff = value;
                if(0 == sys_ctl.onoff) 
				{ //off
					miro_write.manShutdownFlag = 1;
					set_pcs_shutdown();
					log_add(ET_OPSTATE,EST_HMI_CLOSE);
					miro_write.bootup_flag = 0;
                    mb_data.last_onoff_mark = 16;
                } 
				else if(1 == sys_ctl.onoff) 
				{ //on
				    miro_write.manShutdownFlag = 0;
					set_pcs_bootup();                    
					log_add(ET_OPSTATE, EST_HMI_OPEN);		
					miro_write.bootup_flag = 1;
                    mb_data.last_onoff_mark = 32;
                }
                break;
				
            case 313:
                if(GETBIT(value,0) == 1) //低穿使能
				{
                    SETBIT(Upper_Param[Addr_Param112],0);
                } 
				else 
				{
                    Upper_Param[Addr_Param112] = RESETBIT(Upper_Param[Addr_Param112],0);
                }
				
                if(GETBIT(value,1) == 1) 
				{
                    setpara(MP_ISO_ISLAND,1);
                    arm_config_data_write(ISO_ISLAND_ADD,1);
                } 
				else 
				{
                    setpara(MP_ISO_ISLAND,0);
                    arm_config_data_write(ISO_ISLAND_ADD,0);
                }
				
                if(GETBIT(value,2) == 1) //虚拟同步
				{
                    Pcs_Run_Mode_Set(VF_MODE);
                    SETBIT(Upper_Param[Addr_Param112],2);
                } 
				else 
				{
                    Upper_Param[Addr_Param112] = RESETBIT(Upper_Param[Addr_Param112],2);
                    Pcs_Run_Mode_Set(PQ_MODE);
                }
				
                if(GETBIT(value,3) == 1)	//无功补偿
				{
                    svg_info.svg_en = 1;
                    setpara(MP_SVG_EN,1);
                    arm_config_data_write(SVG_EN_ADD,1);
                } 
				else 
				{
                    svg_info.svg_en = 0;
                    setpara(MP_SVG_EN,0);
                    arm_config_data_write(SVG_EN_ADD,0);
                }

                if(GETBIT(value,4) == 1) //载波同步
				{
                } 
				else 
				{
                }
				
                if(GETBIT(value,5) == 1) //低穿无功补偿
				{
                } 
				else 
				{
                }	
				
				if(GETBIT(value,6) == 1) //高穿使能
				{
                } 
				else 
				{
                }

				if(GETBIT(value,7) == 1) //高穿使能
				{
                } 
				else 
				{
                }
				
                setpara(Addr_Param112,Upper_Param[Addr_Param112]);
                fpga_write(Addr_Param112, Upper_Param[Addr_Param112]);
                fpga_pdatabuf_write(Addr_Param112,value);
                break;
				
			case 314:
				if(GETBIT(value,1) == 1) //交流预充电使能
				{
                    SETBIT(Upper_Param[Addr_Param113],6);
                } 
				else 
				{
                    Upper_Param[Addr_Param113] = RESETBIT(Upper_Param[Addr_Param113],6);
                }				
                setpara(Addr_Param113,Upper_Param[Addr_Param113]);
                fpga_write(Addr_Param113, Upper_Param[Addr_Param113]);
                fpga_pdatabuf_write(Addr_Param113,value);
				break;

			case 315:				
				if(GETBIT(value,6) == 1) //时间清零
				{
					time_clear();
                } 
				else 
				{
                }	
				if(GETBIT(value,7) == 1) //发电量清零
				{					
					power_clear();
                } 
				else 
				{
                }					
				break;
				
			case 316:	//通讯保护使能
				miro_write.communication_en	= value;
				arm_config_data_write(COMMUICATION_PRO_EN,miro_write.communication_en);				
				break;	

			case 317:
				miro_write.strategy_en	= value;
				arm_config_data_write(STRATEGY_EN,miro_write.strategy_en);						
				break;	
				
            default:
                return err;
            }
        }

        /* 以太网 */
        else if(addr >= 320 && addr <= 329) {
            switch(addr)
            {
            case 320:
                screen_protect.eth_set_en = value;
                arm_config_data_write(SCREEN_PROTECT_ETH, screen_protect.eth_set_en);
                break;
            case 321:
                //if(screen_protect.eth_set_en == 1) 
				//{
                    ip4_addr[0] = value;				
					//netIF_SetOption (NET_IF_CLASS_ETH | 0, netIF_OptionIP4_Address, ip4_addr, NET_ADDR_IP4_LEN);
               // }
                break;
            case 322:
                //if(screen_protect.eth_set_en == 1) 
				//{
                    ip4_addr[1] = value;					
					//netIF_SetOption (NET_IF_CLASS_ETH | 0, netIF_OptionIP4_Address, ip4_addr, NET_ADDR_IP4_LEN);
                //}
                break;
            case 323:
                //if(screen_protect.eth_set_en == 1) 
				//{
                    ip4_addr[2] = value;					
					//netIF_SetOption (NET_IF_CLASS_ETH | 0, netIF_OptionIP4_Address, ip4_addr, NET_ADDR_IP4_LEN);
                //}
                break;
            case 324:
                //if(screen_protect.eth_set_en == 1) 
				//{
                    ip4_addr[3] = value;
					//netIF_SetOption (NET_IF_CLASS_ETH | 0, netIF_OptionIP4_Address, ip4_addr, NET_ADDR_IP4_LEN);	
                //}
                break;
            case 325:
                if(screen_protect.eth_set_en == 1) 
				{
                    mac_addr[0] = value;
                }
                break;
            case 326:
                if(screen_protect.eth_set_en == 1) 
				{
                    mac_addr[1] = value;
                }
                break;
            case 327:
                if(screen_protect.eth_set_en == 1) 
				{
                    mac_addr[2] = value;
                }
                break;
            case 328:
                if(screen_protect.eth_set_en == 1) 
				{
                    mac_addr[3] = value;
                }
                break;
            default:
                return err;
            }
        }

        /* 485 */
        else if(addr >= 330 && addr <= 339) {
            switch(addr)
            {
            case 330:
                screen_protect.rs485_set_en = value;
                arm_config_data_write(SCREEN_PROTECT_RS485, screen_protect.rs485_set_en);
                break;
				
            case 331:
                //if(screen_protect.rs485_set_en == 1) {
                    mod_config.addr=value;
                    arm_config_data_write(INI_MONITOR_ADDR_ADD, mod_config.addr);
                    setpara(MP_INI_MONITOR_ADDR, mod_config.addr);
                //}
                break;
				
            case 332:
                if(screen_protect.rs485_set_en == 1) {
                    mod_config.baud=value;
                    arm_config_data_write(UART_CONFIG_BAUD, mod_config.baud);
                }
                break;
				
            case 333:
                if(screen_protect.rs485_set_en == 1) {
                    mod_config.data_bit=value;
                    arm_config_data_write(UART_CONFIG_DATABIT, mod_config.data_bit);
                }
                break;
				
            case 334:
                if(screen_protect.rs485_set_en == 1) {
                    mod_config.stop=value;
                    arm_config_data_write(UART_CONFIG_STOP, mod_config.stop);
                }
                break;
				
            case 335:
                if(screen_protect.rs485_set_en == 1) {
                    mod_config.parity=value;
                    arm_config_data_write(UART_CONFIG_PARITY, mod_config.parity);
                }
                break;
				
            default:
                return err;
            }
        }

		//密钥设定
        else if(addr >= 350 && addr <= 360) 
		{
            switch(addr)
            {
            
            case 350:
				
				#if 1  //密钥验证				
                miro_write.code_input  = value;
				arm_config_data_write(DEVICE_WRITE_CODE,miro_write.code_input);
				if(miro_write.code_input == miro_write.code_release)
				{
					miro_write.code_type = 1;
					
					if(arm_config_data_read(CODE_RELEASE_FLAG)==0) //第一次写正确密钥后计算有效时间
					{
						miro_write.code_remain_date = 7;
						arm_config_data_write(CODE_REMAIN_DATE,miro_write.code_remain_date);   
						arm_config_data_write(CODE_RELEASE_FLAG,1);							
						miro_write.code_err_tick = 0;
					}
					
				}
				
				else if (miro_write.code_input == miro_write.code_debug)
				{
					miro_write.code_type = 2;					
					if(arm_config_data_read(CODE_DEBUG_FLAG)==0) //第一次写正确密钥后计算有效时间
					{
						miro_write.code_remain_date = 30;
						arm_config_data_write(CODE_REMAIN_DATE,miro_write.code_remain_date);   
						arm_config_data_write(CODE_DEBUG_FLAG,1);							
						miro_write.code_err_tick = 0;
					}
				}
					
				else if (miro_write.code_input == miro_write.code_temporary1)
				{
					miro_write.code_type = 4;
					if(arm_config_data_read(CODE_TEMPORARY1_FLAG)==0) //第一次写正确密钥后计算有效时间
					{
						miro_write.code_remain_date = 7;
						arm_config_data_write(CODE_REMAIN_DATE,miro_write.code_remain_date);   
						arm_config_data_write(CODE_TEMPORARY1_FLAG,1);						
						miro_write.code_err_tick = 0;
					}					
				}
				
				else if (miro_write.code_input == miro_write.code_temporary2)
				{
					miro_write.code_type = 8;
					if(arm_config_data_read(CODE_TEMPORARY2_FLAG)==0) //第一次写正确密钥后计算有效时间
					{
						miro_write.code_remain_date = 30;
						arm_config_data_write(CODE_REMAIN_DATE,miro_write.code_remain_date);   
						arm_config_data_write(CODE_TEMPORARY2_FLAG,1);						
						miro_write.code_err_tick = 0;
					}						
				}
				
				else if (miro_write.code_input == miro_write.code_temporary3)
				{
					miro_write.code_type = 16;
					if(arm_config_data_read(CODE_TEMPORARY3_FLAG)==0) //第一次写正确密钥后计算有效时间
					{
						miro_write.code_remain_date = 90;
						arm_config_data_write(CODE_REMAIN_DATE,miro_write.code_remain_date);   
						arm_config_data_write(CODE_TEMPORARY3_FLAG,1);						
						miro_write.code_err_tick = 0;
					}						
				}
				
				else if (miro_write.code_input == miro_write.code_permanent)
				{
					miro_write.code_type = 32;
					if(arm_config_data_read(CODE_PERMANENT_FLAG)==0) //第一次写正确密钥后计算有效时间
					{
						miro_write.code_remain_date = 9999;
						arm_config_data_write(CODE_REMAIN_DATE,miro_write.code_remain_date);   
						arm_config_data_write(CODE_PERMANENT_FLAG,1);						
						miro_write.code_err_tick = 0;
					}					
				}
				
				else
				{
					miro_write.code_type = 0;
					//miro_write.code_remain_date = 0;
					//arm_config_data_write(CODE_REMAIN_DATE,0);  					
					log_add(ET_OPSTATE,EST_ERR_CODE);	//密钥错误
					miro_write.code_err_tick = 1;
				}
				arm_config_data_write(CODE_TPYE,miro_write.code_type);						
								
				#endif
				break;
				
            case 351:
				break;  
				
			case 352:
				break;	
				
			default:
				break;	
            }
        }


		//vf模式参数设定
        else if(addr >= 397 && addr <= 399) 
		{
            switch(addr)
            {
            
            case 397:
				break;
				
            case 398:
				break;  
				
			case 399:
				break;	
				
			default:
				return err;
            }
        }
				
        /* AC */
        else if(addr >= 400 && addr <= 419) {
            switch(addr)
            {
            case 400:
                addr_tmp = Addr_Param0;
                break;
            case 403:
                addr_tmp = Addr_Param9;
                break;
            case 404:
                addr_tmp = Addr_Param1;
                break;
            case 408:
                addr_tmp = Addr_Param2;
                break;
            case 411:
                addr_tmp = Addr_Param10;
                break;
            case 412:
                addr_tmp = Addr_Param28;
                break;
            case 413:
                addr_tmp = Addr_Param28;
                break;
            case 414:
                addr_tmp = Addr_Param30;
                break;
            case 415:
                addr_tmp = Addr_Param29;
                break;
            case 416:
                addr_tmp = Addr_Param24;
                break;
            case 417:
                addr_tmp = Addr_Param26;
                break;
            case 418:
                addr_tmp = Addr_Param27;
                break;
            case 419:
                addr_tmp = Addr_Param23;
                break;
            default:
                return err;
            }
            fpga_pdatabuf_get[addr_tmp]=setting_data_handle(addr_tmp, value);
            setpara(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
            if((addr != 417) && (addr != 418)) {
                CPU_CRITICAL_ENTER();
                fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                CPU_CRITICAL_EXIT();
            } else if(addr == 417) {
                if(svg_info.svg_en == 0) {
                    CPU_CRITICAL_ENTER();
                    fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_EXIT();
                }
                PQNonSVG.p=value;
            } else if(addr == 418) {
                if(svg_info.svg_en == 0) {
                    CPU_CRITICAL_ENTER();
                    fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_EXIT();
                }
                PQNonSVG.q=value;
            }
        }

        /* BUS */
        else if(addr >= 420 && addr <= 439) 
		{
            switch(addr)
            {
            case 420:
                addr_tmp = Addr_Param3;
                break;
            case 423:
                addr_tmp = Addr_Param11;
                break;
            case 424:
                addr_tmp = Addr_Param4;
                break;
            case 428:
                addr_tmp = Addr_Param5;
                break;
            case 431:
                addr_tmp = Addr_Param12;
                break;
            case 432:
                addr_tmp = Addr_Param34;
                break;
            case 433:
                addr_tmp = Addr_Param33;
                break;
            case 434:
                addr_tmp = Addr_Param32;
                break;
            case 435:
                addr_tmp = Addr_Param31;
                break;
            default:
				
            if(addr == 436) 
			{ //直流电压给定 CU_MODE
               addr_tmp = Addr_Param23;
               fpga_pdatabuf_get[addr_tmp] = setting_data_handle(addr_tmp, value);
               setpara(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
               CPU_CRITICAL_ENTER();
               fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
               fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
               CPU_CRITICAL_EXIT();
					
               if(run_mode_hmi == CU_MODE) 
			   {
               		addr_tmp = Addr_Param31;
                    fpga_pdatabuf_get[addr_tmp]=setting_data_handle(addr_tmp, (-1)*650*MODULUS);
                    setpara(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_ENTER();
                    fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_EXIT();
                    addr_tmp = Addr_Param32;
                    fpga_pdatabuf_get[addr_tmp]=setting_data_handle(addr_tmp, 650*MODULUS);
                    setpara(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_ENTER();
                    fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_EXIT();
                    addr_tmp = Addr_Param33;
                    fpga_pdatabuf_get[addr_tmp]=setting_data_handle(addr_tmp, (-1)*1320*MODULUS);
                    setpara(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_ENTER();
                    fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_EXIT();
                    addr_tmp = Addr_Param34;
                    fpga_pdatabuf_get[addr_tmp]=setting_data_handle(addr_tmp, 1320*MODULUS);
                    setpara(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_ENTER();
                    fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_EXIT();
               }
           }
				
          if(addr == 437) 
		  { //直流功率给定 CP_MODE
             if(value < 0) 
		     { //充电	
             	addr_tmp = Addr_Param31;
                fpga_pdatabuf_get[addr_tmp]=setting_data_handle(addr_tmp, value);
                setpara(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                CPU_CRITICAL_ENTER();
                fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                CPU_CRITICAL_EXIT();
                addr_tmp = Addr_Param32;
                fpga_pdatabuf_get[addr_tmp]=setting_data_handle(addr_tmp, (-1)*value);
                setpara(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                CPU_CRITICAL_ENTER();
                fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                CPU_CRITICAL_EXIT();
             } 
			 else 
			 { //放电
              	addr_tmp = Addr_Param31;
                fpga_pdatabuf_get[addr_tmp]=setting_data_handle(addr_tmp, (-1)*value);
                setpara(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                CPU_CRITICAL_ENTER();
                fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                CPU_CRITICAL_EXIT();
                addr_tmp = Addr_Param32;
                fpga_pdatabuf_get[addr_tmp]=setting_data_handle(addr_tmp, value);
                setpara(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                CPU_CRITICAL_ENTER();
                fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                CPU_CRITICAL_EXIT();
             }
                    addr_tmp = Addr_Param33;
                    fpga_pdatabuf_get[addr_tmp]=setting_data_handle(addr_tmp, (-1)*1320*MODULUS);
                    setpara(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_ENTER();
                    fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_EXIT();
                    addr_tmp = Addr_Param34;
                    fpga_pdatabuf_get[addr_tmp]=setting_data_handle(addr_tmp, 1320*MODULUS);
                    setpara(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_ENTER();
                    fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_EXIT();

                    addr_tmp = Addr_Param23;
                    if(value < 0) 
					{ //充电
                        cp_mode_flag = 2;
                        fpga_pdatabuf_get[addr_tmp]=setting_data_handle(addr_tmp, 650*MODULUS);
                    } 
					else 
					{ //放电
                        cp_mode_flag = 1;
                        fpga_pdatabuf_get[addr_tmp]=setting_data_handle(addr_tmp, 380*MODULUS);
                    }
                    arm_config_data_write(CP_MODE_FLAG_ADD,cp_mode_flag);
                    setpara(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_ENTER();
                    fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_EXIT();
                } 

				else if(addr == 438) 
				{ //直流电流给定 CI_MODE
                    if(value < 0) 
					{ //充电
                        addr_tmp = Addr_Param33;
                        fpga_pdatabuf_get[addr_tmp]=setting_data_handle(addr_tmp, value);
                        setpara(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                        CPU_CRITICAL_ENTER();
                        fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                        fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                        CPU_CRITICAL_EXIT();
                        addr_tmp = Addr_Param34;
                        fpga_pdatabuf_get[addr_tmp]=setting_data_handle(addr_tmp, (-1)*value);
                        setpara(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                        CPU_CRITICAL_ENTER();
                        fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                        fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                        CPU_CRITICAL_EXIT();
                    } 
					else 
					{ //放电
                        addr_tmp = Addr_Param33;
                        fpga_pdatabuf_get[addr_tmp]=setting_data_handle(addr_tmp, (-1)*value);
                        setpara(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                        CPU_CRITICAL_ENTER();
                        fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                        fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                        CPU_CRITICAL_EXIT();
                        addr_tmp = Addr_Param34;
                        fpga_pdatabuf_get[addr_tmp]=setting_data_handle(addr_tmp, value);
                        setpara(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                        CPU_CRITICAL_ENTER();
                        fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                        fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                        CPU_CRITICAL_EXIT();
                    }
                    addr_tmp = Addr_Param31;
                    fpga_pdatabuf_get[addr_tmp]=setting_data_handle(addr_tmp, (-1)*650*MODULUS);
                    setpara(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_ENTER();
                    fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_EXIT();
                    addr_tmp = Addr_Param32;
                    fpga_pdatabuf_get[addr_tmp]=setting_data_handle(addr_tmp, 650*MODULUS);
                    setpara(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_ENTER();
                    fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_EXIT();

                    addr_tmp = Addr_Param23;
                    if(value < 0) 
					{ //充电
                        ci_mode_flag = 2;
                        fpga_pdatabuf_get[addr_tmp]=setting_data_handle(addr_tmp, 650*MODULUS);
                    }
					else 
					{ //放电
                        ci_mode_flag = 1;
                        fpga_pdatabuf_get[addr_tmp]=setting_data_handle(addr_tmp, 380*MODULUS);
                    }
                    arm_config_data_write(CI_MODE_FLAG_ADD,ci_mode_flag);
                    setpara(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_ENTER();
                    fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
                    CPU_CRITICAL_EXIT();
                }
				
                return err;
            }
            fpga_pdatabuf_get[addr_tmp]=setting_data_handle(addr_tmp, value);
            setpara(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
            CPU_CRITICAL_ENTER();
            fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
            fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
            CPU_CRITICAL_EXIT();
        }

        /* DC1 */
        else if(addr >= 440 && addr <= 459) 
		{
            switch(addr)
            {
            case 440:
                addr_tmp = Addr_Param6;
                break;
            case 443:
                addr_tmp = Addr_Param13;
                break;
            case 444:
                addr_tmp = Addr_Param7;
                break;
            default:
                return err;
            }
            CPU_CRITICAL_ENTER();
            fpga_pdatabuf_get[addr_tmp]=setting_data_handle(addr_tmp, value);
            setpara(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
            fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
            fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
            CPU_CRITICAL_EXIT();
        }


        /* 一次调频 */
        else if(addr >= 500 && addr <= 502) 
		{
            switch(addr)
            {
            case 500:
				miro_write.pfm_able = value;
				if(Pcs_Ctrl_Mode_Get()== EMS_CTRL)
				{
					if(miro_write.pfm_able==1)
					{
						SETBIT(miro_write.strategy_en,14);
					}
					else if(miro_write.pfm_able==0)
					{
						miro_write.strategy_en = RESETBIT(miro_write.strategy_en,14);

					}
					setpara(Strategy_En,miro_write.strategy_en); 		
					arm_config_data_write(STRATEGY_EN,miro_write.strategy_en);	
				}				
                break;
				
            case 501:
				miro_write.dead_zone = value;
				if(Pcs_Ctrl_Mode_Get()== EMS_CTRL)
				{
					miro_write.Freq_Dead_Zone =(short)(miro_write.dead_zone*0.1377438f);
					fpga_write(Addr_Param91, miro_write.Freq_Dead_Zone);
					fpga_pdatabuf_write(Addr_Param91, miro_write.Freq_Dead_Zone);
				}
                break;
				
            case 502:
				miro_write.diversity_factor = value;
				if(Pcs_Ctrl_Mode_Get()== EMS_CTRL)
				{
					charge_power_lim_set = setting_data_handle(Addr_Param26, arm_config_data_read(MAX_P_ADD))*10.0f; //fpga_read(Addr_Param30); 		
					base_fre = fpga_read(Addr_Param90);
					miro_write.Diversity_Factor1 = (short)(10000.0f/base_fre/miro_write.diversity_factor*charge_power_lim_set);
					fpga_write(Addr_Param94, miro_write.Diversity_Factor1);
					fpga_pdatabuf_write(Addr_Param94, miro_write.Diversity_Factor1); 
				}
                break;	
				
            default:
                return err;
            }
        }

        /* 保护时间 */
        else if(addr >= 600 && addr <= 699) 
		{
            switch(addr)
            {
            case 600:
                addr_tmp = Addr_Param16;
                break;
            case 603:
                addr_tmp = Addr_Param17;
                break;
            default:
                return err;
            }
            CPU_CRITICAL_ENTER();
            fpga_pdatabuf_get[addr_tmp]=setting_data_handle(addr_tmp, value);
            setpara(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
            fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
            fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
            CPU_CRITICAL_EXIT();
        }

        /* 硬件保护 */
        else if(addr >= 700 && addr <= 799) 
		{
            switch(addr)
            {
            case 701:
                addr_tmp = Addr_Param8;
                break;
				
			case 704:
                addr_tmp = Addr_Param18;
                break;
				
			case 705:
                addr_tmp = Addr_Param19;  //漏电流保护值
                break;	
            default:
                return err;
            }
            CPU_CRITICAL_ENTER();
            fpga_pdatabuf_get[addr_tmp]=setting_data_handle(addr_tmp, value);
            setpara(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
            fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
            fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
            CPU_CRITICAL_EXIT();
        }

        /* 电池充放电限压值 */
        else if(addr >= 800 && addr <= 899) 
		{
            switch(addr)
            {
            case 810:
                setpara(MP_CHRG_VOL_LIMIT,value);
                arm_config_data_write(CHRG_VOL_LIMIT_ADD,(short)value);
                break;
            case 811:
                setpara(MP_DISC_VOL_LIMIT,value);
                arm_config_data_write(DISC_VOL_LIMIT_ADD,(short)value);
                break;
            case 812:
                //setpara(MP_END_SWITCH_EN,value);
                arm_config_data_write(END_SWITCH_EN_ADD,(short)value);
                break;
            default:
                return err;
            }
        }

 /* 尖峰平谷 */
		else if(addr >= 1200 && addr <= 1300) 
		{
			switch(addr)
			{
				case 1200:
			        pcs_manage.period_info[0].start_time_h = value / 100;
			        pcs_manage.period_info[0].start_time_m = value % 100;
			        arm_config_data_write(START_TIME_0, value);				
					//tty_printf("START_TIME_0 is %d\n", value);
			        break;
					
			    case 1201:
			        pcs_manage.period_info[0].stop_time_h = value / 100;
			        pcs_manage.period_info[0].stop_time_m = value % 100;
			        arm_config_data_write(STOP_TIME_0, value);					
					//tty_printf("STOP_TIME_0 is %d\n", value);
			        break;
					
				case 1202:
			        pcs_manage.period_info[0].set_power = value;
			        arm_config_data_write(SET_POWER_0, pcs_manage.period_info[0].set_power);					
					//tty_printf("SET_POWER_0 is %d\n", value);
			        break;
					
				case 1203:
			        pcs_manage.capacity = value;
			        arm_config_data_write(CAPACITY, pcs_manage.capacity);				
					//tty_printf("CAPACITY is %d\n", value);
			        break;
					
				case 1205:
			        pcs_manage.period_info[1].start_time_h = value / 100;
			        pcs_manage.period_info[1].start_time_m = value % 100;
			        arm_config_data_write(START_TIME_1, value);
			        break;
					
			    case 1206:
			        pcs_manage.period_info[1].stop_time_h = value / 100;
			        pcs_manage.period_info[1].stop_time_m = value % 100;
			        arm_config_data_write(STOP_TIME_1, value);
			        break;	
					
				case 1207:
			        pcs_manage.period_info[1].set_power = value;
			        arm_config_data_write(SET_POWER_1, pcs_manage.period_info[1].set_power);
			        break;
				//case 1208:
			    //    pcs_manage.capacity = value;
			    //    arm_config_data_write(CAPACITY, pcs_manage.capacity);
			    //    break;
					
				case 1210:
			        pcs_manage.period_info[2].start_time_h = value / 100;
			        pcs_manage.period_info[2].start_time_m = value % 100;
			        arm_config_data_write(START_TIME_2, value);
			        break;
					
			    case 1211:
			        pcs_manage.period_info[2].stop_time_h = value / 100;
			        pcs_manage.period_info[2].stop_time_m = value % 100;
			        arm_config_data_write(STOP_TIME_2, value);
			        break;
					
				case 1212:
			        pcs_manage.period_info[2].set_power = value;
			        arm_config_data_write(SET_POWER_2, pcs_manage.period_info[2].set_power);
			        break;
				//case 1213:
			  //    pcs_manage.capacity = value;
			  //    arm_config_data_write(CAPACITY, pcs_manage.capacity);
			  //   break;
					
				case 1215:
			        pcs_manage.period_info[3].start_time_h = value / 100;
			        pcs_manage.period_info[3].start_time_m = value % 100;
			        arm_config_data_write(START_TIME_3, value);
			        break;
					
			    case 1216:
			        pcs_manage.period_info[3].stop_time_h = value / 100;
			        pcs_manage.period_info[3].stop_time_m = value % 100;
			        arm_config_data_write(STOP_TIME_3, value);
			        break;
					
				case 1217:
			        pcs_manage.period_info[3].set_power = value;
			        arm_config_data_write(SET_POWER_3, pcs_manage.period_info[3].set_power);
			        break;
				//case 1218:
			   	//    pcs_manage.capacity = value;
			    //    arm_config_data_write(CAPACITY, pcs_manage.capacity);
			    //    break;
					
				case 1220:
			        pcs_manage.period_info[4].start_time_h = value / 100;
			        pcs_manage.period_info[4].start_time_m = value % 100;
			        arm_config_data_write(START_TIME_4, value);
			        break;
					
			    case 1221:
			        pcs_manage.period_info[4].stop_time_h = value / 100;
			        pcs_manage.period_info[4].stop_time_m = value % 100;
			        arm_config_data_write(STOP_TIME_4, value);
			        break;
					
				case 1222:
			        pcs_manage.period_info[4].set_power = value;
			        arm_config_data_write(SET_POWER_4, pcs_manage.period_info[4].set_power);
			        break;
					
				//case 1223:
			    //    pcs_manage.capacity = value;
			    //    arm_config_data_write(CAPACITY, pcs_manage.capacity);
			    //    break;
				
				case 1225:
					pcs_manage.period_info[5].start_time_h = value / 100;
					pcs_manage.period_info[5].start_time_m = value % 100;
					arm_config_data_write(START_TIME_5, value);
					break;
					
				case 1226:
					pcs_manage.period_info[5].stop_time_h = value / 100;
					pcs_manage.period_info[5].stop_time_m = value % 100;
					arm_config_data_write(STOP_TIME_5, value);
					break;
					
				case 1227:
					pcs_manage.period_info[5].set_power = value;
					arm_config_data_write(SET_POWER_5, pcs_manage.period_info[5].set_power);
					break;
				//case 1228:
				//	pcs_manage.capacity = value;
				//	arm_config_data_write(CAPACITY, pcs_manage.capacity);
				//	break;
				
				case 1230:
					pcs_manage.period_info[6].start_time_h = value / 100;
					pcs_manage.period_info[6].start_time_m = value % 100;
					arm_config_data_write(START_TIME_6, value);
					break;
				case 1231:
					pcs_manage.period_info[6].stop_time_h = value / 100;
					pcs_manage.period_info[6].stop_time_m = value % 100;
					arm_config_data_write(STOP_TIME_6, value);
					break;
				case 1232:
					pcs_manage.period_info[6].set_power = value;
					arm_config_data_write(SET_POWER_6, pcs_manage.period_info[6].set_power);
					break;
					
				case 1235:
					pcs_manage.period_info[7].start_time_h = value / 100;
					pcs_manage.period_info[7].start_time_m = value % 100;
					arm_config_data_write(START_TIME_7, value);
					break;
				case 1236:
					pcs_manage.period_info[7].stop_time_h = value / 100;
					pcs_manage.period_info[7].stop_time_m = value % 100;
					arm_config_data_write(STOP_TIME_7, value);
					break;
				case 1237:
					pcs_manage.period_info[7].set_power = value;
					arm_config_data_write(SET_POWER_7, pcs_manage.period_info[7].set_power);
					break;	

				#if 0	//暂时使用时区1前8个设置数据	
				case 1240:
					pcs_manage.period_info[8].start_time_h = value / 100;
					pcs_manage.period_info[8].start_time_m = value % 100;
					arm_config_data_write(START_TIME_8, value);
					break;
				case 1241:
					pcs_manage.period_info[8].stop_time_h = value / 100;
					pcs_manage.period_info[8].stop_time_m = value % 100;
					arm_config_data_write(STOP_TIME_8, value);
					break;
				case 1242:
					pcs_manage.period_info[8].set_power = value;
					arm_config_data_write(SET_POWER_8, pcs_manage.period_info[8].set_power);
					break;	
					
				case 1245:
					pcs_manage.period_info[9].start_time_h = value / 100;
					pcs_manage.period_info[9].start_time_m = value % 100;
					arm_config_data_write(START_TIME_9, value);
					break;
				case 1246:
					pcs_manage.period_info[9].stop_time_h = value / 100;
					pcs_manage.period_info[9].stop_time_m = value % 100;
					arm_config_data_write(STOP_TIME_9, value);
					break;
				case 1247:
					pcs_manage.period_info[9].set_power = value;
					arm_config_data_write(SET_POWER_9, pcs_manage.period_info[9].set_power);
					break;				
				#endif
						
				case 1299:
					if(GETBIT(value,0) == 1)
					{
						pcs_manage.xftg_en =1;
					}
					else if(GETBIT(value,0) == 0)
					{
						pcs_manage.xftg_en = 0;
					}	
					arm_config_data_write(XFTG_EN, pcs_manage.xftg_en);
					//tty_printf("XFTG_EN is %d\n", pcs_manage.xftg_en);
					break;
					
				case 1300:
					pcs_manage.cos_seta_value = value;
					arm_config_data_write(cos_seta, pcs_manage.cos_seta_value);						
					//tty_printf("cos_seta_value is %d\n", value);
					break;	
				
				default:
					return err;
			}
		}
			
		#if 1
		/* cmd直接操作20171216 */
		 else if(addr >= 1301 && addr <= 1310) 
		 {
            switch(addr)
            {
	            case 1301:					
					//setpara(Addr_Param110,value);
	                addr_tmp = Addr_Param110;
					
	                break;
					
	            case 1302:
	                addr_tmp = Addr_Param111;
	                break;
					
	            case 1303:
	                addr_tmp = Addr_Param112;
	                break;
					
				case 1304:
	                addr_tmp = Addr_Param113;
	                break;
					
	            case 1305:
	                addr_tmp = Addr_Param114;
	                break;
					
	            case 1306:
	                addr_tmp = Addr_Param115;
	                break;
					
				case 1307:
	                addr_tmp = Addr_Param116;
	                break;
					
	            case 1308:
	                addr_tmp = Addr_Param117;
	                break;
					
	            case 1309:
	                addr_tmp = Addr_Param118;
	                break;
					
				case 1310:
	                addr_tmp = Addr_Param119;
	                break;
					
	            default:
	                return err;
            }
			
			CPU_CRITICAL_ENTER();
            fpga_pdatabuf_get[addr_tmp]=setting_data_handle(addr_tmp, value);
            setpara(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
            fpga_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
            fpga_pdatabuf_write(addr_tmp,fpga_pdatabuf_get[addr_tmp]);
            CPU_CRITICAL_EXIT();
		}
	#endif
        /* 发电量 */
        else if(addr >= 3400 && addr <= 3499) {
            switch(addr)
            {
            case 3400:
                CountData.power_ac_day_h=value;
                break;
            case 3401:
                CountData.power_ac_day_l=value;
                CountData.power_ac_day=(float)((CTOS_16(CountData.power_ac_day_h,CountData.power_ac_day_l)/10));
                CountData.Power_Con=(float)(CountData.power_ac_day*3600);
                count_cal_write(POWER_CON_ADD,CountData.Power_Con);
                count_cal_write(POWER_AC_DAY_ADD,CountData.power_ac_day);
                break;
            case 3440:
                CountData.power_ac_con_h=value;
                break;
            case 3441:
                CountData.power_ac_con_l=value;
                CountData.power_ac_con=(float)((CTOS_16(CountData.power_ac_con_h,CountData.power_ac_con_l)/10));
                count_cal_write(POWER_AC_CON_ADD,CountData.power_ac_con);
                break;
            case 3490:
                CountData.power_ac_yesterday_h=value;
                break;
            case 3491:
                CountData.power_ac_yesterday_l=value;
                CountData.power_ac_yesterday=(float)((CTOS_16(CountData.power_ac_yesterday_h,CountData.power_ac_yesterday_l)/10));
                count_cal_write(POWER_AC_Y_ADD,CountData.power_ac_yesterday);
                break;
            default:
                return err;
            }
        }

    } 
	
	else if(ctrl_flag == CTRL_UPPER) 
	{

    }

    //if(addr == 310) 
	//{
    //    Pcs_Ctrl_Mode_Set(value);
	//	tty_printf("[HMI SET]: ctr_mode[%d]\n", value);
    //}

    return err;
}


void usart3_callback(uint32_t event)
{
    switch(event)
    {
    case ARM_USART_EVENT_RX_TIMEOUT:
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET);
        osSignalSet(tid_modbus_screen, 0x01);
        break;
    default:
        break;
    }
}


int modbus_send_packet_u0(uint8_t *buf, uint16_t cnt)
{
    ARM_DRIVER_USART *drv = &Driver_USART3;

    while(1)
    {
        if(drv->GetStatus().tx_busy)
        {
            osDelay(1);
            continue;
        }

        drv->Send(buf, cnt);
        break;
    }

    return 0;
}



void modbus_read_hodingregs_u0(void)
{
    unsigned char* rbuf;
    unsigned char* sbuf;
    unsigned short value;
    unsigned short addr;
    unsigned short num;
    unsigned short i;
    unsigned char err=0;

    rbuf = mb_recv_buf_u0;
    sbuf = mb_send_buf_u0;

    //包大小应为8个字节
    if(mb_inf_u0.plen < 8)
    {
        mb_inf_u0.plen = 0;
        return;
    }

    //计算CRC校验值是否正确
    if(modbus_crc_check_u0(rbuf, 6) != 0)
    {
        mb_inf_u0.crc_err++;
        //	printf("err");
        return;
    }

    //起始地址
    addr = (rbuf[2] << 8) | rbuf[3];
    //读取数量
    num = (rbuf[4] << 8) | rbuf[5];

    //读寄存器值，填充到发送缓冲区
    for(i = 0; i < num; i++)
    {   // printf("读寄存器\n");

        value = modbus_read_reg_u0(addr + i, &err);
        //printf("%d",i+addr);
        //printf("%d",value);
        //printf("err is%d\n",err);
        if(err != 0)
        {
            modbus_resp_err_u0(rbuf[0], rbuf[1], err);
            return;
        }

        sbuf[3 + i * 2] = value >> 8;
        sbuf[4 + i * 2] = value & 0xff;
    }

    //填充发送缓冲区除CRC段的内容
    sbuf[0] = rbuf[0];
    sbuf[1] = rbuf[1];
    sbuf[2] = num * 2;

    //填充CRC校验值
    modbus_crc_set_u0(sbuf, num * 2 + 3);

    //发送回应
    modbus_send_packet_u0(sbuf, num * 2 + 5);
}


void modbus_write_singlereg_u0(void)
{
    unsigned char* rbuf;
    unsigned char* sbuf;
//   unsigned short value;
    short	value;
    unsigned short addr;
    unsigned char err;

    rbuf = mb_recv_buf_u0;
    sbuf = mb_send_buf_u0;

    //包大小应为8个字节
    if(mb_inf_u0.plen < 8)
    {
        mb_inf_u0.plen = 0;
        return;
    }

    //计算CRC校验值是否正确
    if(modbus_crc_check_u0(rbuf, 6) != 0)
    {
        mb_inf_u0.crc_err++;
        return;
    }

    addr = (rbuf[2] << 8) | rbuf[3];
    value = (short)((rbuf[4] << 8) | rbuf[5]);
    //	printf("写寄存器\n");
    err = modbus_write_reg_u0(addr, value);
    //	modbus_handle_delayed();

    if(err != 0)
    {
        modbus_resp_err_u0(rbuf[0], rbuf[1], err);
        return;
    }

    //组包
    memcpy(sbuf, rbuf, 6);
    modbus_crc_set_u0(sbuf, 6);

    //发送回应
    modbus_send_packet_u0(sbuf, 8);
}

void modbus_write_multipleregs_u0(void)
{
    unsigned char* rbuf;
    unsigned char* sbuf;
    unsigned short value;
    unsigned short addr;
    unsigned char err;
    unsigned short num;
    unsigned char bytes;
    int i;

    rbuf = mb_recv_buf_u0;
    sbuf = mb_send_buf_u0;

    addr = (rbuf[2] << 8) | rbuf[3];
    num = (rbuf[4] << 8) | rbuf[5];
    bytes = rbuf[6];

    if(bytes != (2*num))
    {
        return;
    }

    //包大小应为8个字节
    if(mb_inf_u0.plen < bytes + 9)
    {
        mb_inf_u0.plen = 0;
        return;
    }

    //计算CRC校验值是否正确
    if(modbus_crc_check_u0(rbuf, bytes + 7) != 0)
    {
        mb_inf_u0.crc_err++;
        return;
    }

    for(i = 0; i < num; i++)
    {
        value = (rbuf[7 + i * 2] << 8) | rbuf[8 + i * 2];
        err = modbus_write_reg_u0(addr + i, value);
        //	modbus_handle_delayed();

        if(err != 0)
        {
            modbus_resp_err_u0(rbuf[0], rbuf[1], err);
            return;
        }
    }

    //组包
    memcpy(sbuf, rbuf, 6);
    modbus_crc_set_u0(sbuf, 6);

    //发送回应
    modbus_send_packet_u0(sbuf, 8);
}

void modbus_illegal_func_u0(void)
{
    unsigned char addr;
    unsigned char func;

    addr = mb_recv_buf_u0[0];
    func = mb_recv_buf_u0[1];

    mb_dbg("未实现的功能码:0x%x\n", func);

    modbus_resp_err_u0(addr, func, 1);
}

void modbus_resp_err_u0(unsigned char addr, unsigned char func, unsigned char err)
{
    unsigned char* sbuf;

    sbuf = mb_send_buf_u0;

    //功能码不能超出范围
    if(func >= 0x80)
    {
        return;
    }

    //组包
    sbuf[0] = addr;
    sbuf[1] = 0x80 + func;
    sbuf[2] = err;

    modbus_crc_set_u0(sbuf, 3);

    //发送回应
    modbus_send_packet_u0(sbuf, 5);
}

int modbus_crc_set_u0(unsigned char* pdata, unsigned short cntlen)
{
    unsigned short crc;

    crc = crc16(pdata, cntlen);
    pdata[cntlen] = crc >> 8;
    pdata[cntlen+1] = crc & 0xff;

    return 0;
}

int modbus_crc_check_u0(unsigned char* pdata, unsigned short cntlen)
{
    unsigned short crc;
    unsigned short crcget;

    crc = crc16(pdata, cntlen);
    crcget = pdata[cntlen+1] | (pdata[cntlen]<< 8);

    if(crc == crcget)
    {
        return 0;
    }
    else
    {
        printf("CRC校验错误\n");
        return 1;
    }
}

int modbus_recv_packet_u0(void)
{
    unsigned int plen;
    int num;

    ARM_DRIVER_USART *drv = &Driver_USART3;

    while(drv->GetStatus().tx_busy)
    {
        osDelay(1);
    }

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_SET);
    drv->Control(ARM_USART_ABORT_RECEIVE, 0);
    drv->Receive(mb_recv_buf_u0, sizeof(mb_recv_buf_u0));

    /* 等待数据包接收完成 */
    osSignalWait(1, osWaitForever);

    plen = drv->GetRxCount();

    if(plen < 8)
    {
        return 1;
    }

    //if(mb_recv_buf_u0[0] != arm_config_data_read(INI_RESIS_FD_ADD))
	if(mb_recv_buf_u0[0] != 2)
    {
        //tty_printf("add is err %d\n", mb_recv_buf_u0[0]);
        return 1;
    }

    if(mb_recv_buf_u0[1] == 0x10)
    {
        num = CTOS_16(mb_recv_buf_u0[4], mb_recv_buf_u0[5]);

        if(num * 2 != mb_recv_buf_u0[6])
        {
            return 1;
        }

        if(plen < num * 2 + 9)
        {
            return 1;
        }
    }

    mb_inf_u0.plen = plen;

    return 0;
}

void modbus_resp_u0(void)
{
    unsigned char func;
    func = mb_recv_buf_u0[1];
    //tty_printf("func=%d\n", func);
    switch(func)
    {
    case 0x03:
        //printf("功能码为03");
        modbus_read_hodingregs_u0();
        break;
    case 0x04:
        //printf("功能码为04");
        modbus_read_hodingregs_u0();
        break;
    case 0x06:
        //printf("功能码为06");
        modbus_write_singlereg_u0();
        break;
    case 0x10:
        modbus_write_multipleregs_u0();
        break;
    default:
        modbus_illegal_func_u0();
        break;
    }
}



void modbus_screen_task(const void* pdata)
{
    int err;
    mb_dbg("start modbus service\n");

    /* 初始化变量 */
    memset(&mb_inf_u0, 0, sizeof(mb_inf_u0));

    while(1)
    {

        //if(GETBIT(fpga_read(SETTINGSWITCHCMD1_ADD),1)==1)
        {
            //printf("enter\n");
            err = modbus_recv_packet_u0();
            //printf("err is %d\n",err);
            if(err == 0)
            {
                modbus_resp_u0();
            }
        }

        osDelay(5);
    }
}

