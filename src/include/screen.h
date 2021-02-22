#ifndef SCREEN_H
#define SCREEN_H

typedef struct
{
	 short method;
	 short coef_id;
} handle_data_info_t;


typedef struct
{
	unsigned char time_set_en;
	unsigned char onoff_set_en;
	unsigned char eth_set_en;
	unsigned char rs485_set_en;
	unsigned char can_set_en;
} screen_protect_t;


extern handle_data_info_t data_handle[];
extern short getting_data_handle(short addr, short value);
extern short setting_data_handle(short addr, short value);
void modbus_screen_task(const void* pdata);

#endif

