#ifndef SYS_H
#define SYS_H

typedef struct
{
	unsigned char dev_opt_mode; //设备操作模式: 0-上位机模式 1-HMI模式
	unsigned char dbg;			//调试信息显示: 0-关闭 1-显示
	unsigned char onoff;
	unsigned short prtLevel; //打印级别
}system_control;

extern system_control sys_ctl;

#endif

