#ifndef SYS_H
#define SYS_H

typedef struct
{
	unsigned char dev_opt_mode; //�豸����ģʽ: 0-��λ��ģʽ 1-HMIģʽ
	unsigned char dbg;			//������Ϣ��ʾ: 0-�ر� 1-��ʾ
	unsigned char onoff;
	unsigned short prtLevel; //��ӡ����
}system_control;

extern system_control sys_ctl;

#endif

