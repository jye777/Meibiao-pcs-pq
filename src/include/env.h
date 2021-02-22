/**
 * @file			env.h
 * @brief			
 * @author			wangpeng
 * @date			2016/5/6
 * @version			Initial Draft
 * @par				Copyright (C),  2013-2023, SCNEE
 * @par History:
 * 1.Date: 			2016/5/6
 *   Author: 			wangpeng
 *   Modification: 		Created file
*/

#ifndef ENV_H
#define ENV_H

#include <stdint.h>

#define ENV_MAXSIZE         1024
#define ENV_DATA_MAXSIZE    (ENV_MAXSIZE - sizeof(uint32_t))

typedef struct
{
    uint32_t crc;
    uint8_t data[ENV_DATA_MAXSIZE];
}ub_env;

extern ub_env *env_cache;
extern void env_crc_update(void);

int env_init(void);
char *getenv(const char *name);
int saveenv(void);
int setenv(char *envname, char *value);

/* ���������ﱣ�����������MAC��ַ */
#define MAC              ("mac")
/* ���������ﱣ�����������IP��ַ */
#define IP               ("ip")
/* ���������ﱣ�����������NETMASK��ַ */
#define NETMASK          ("netmask")
/* ���������ﱣ�����������GATEWAY��ַ */
#define GATEWAY          ("gateway")
#define DNS              ("dns")


/* ���������ﱣ�������0��MAC��ַ */
#define MAC0              ("mac0")
/* ���������ﱣ�������0��IP��ַ */
#define IP0               ("ip0")
/* ���������ﱣ�������0��NETMASK��ַ */
#define NETMASK0          ("netmask0")
/* ���������ﱣ�������0��GATEWAY��ַ */
#define GATEWAY0          ("gateway0")
#define DNS0              ("dns0")

/* ���������ﱣ�������1��MAC��ַ */
#define MAC1              ("mac1")
/* ���������ﱣ�������1��IP��ַ */
#define IP1               ("ip1")
/* ���������ﱣ�������1��NETMASK��ַ */
#define NETMASK1          ("netmask1")
/* ���������ﱣ�������1��GATEWAY��ַ */
#define GATEWAY1          ("gateway1")
#define DNS1              ("dns1")



#endif /* ENV_H */

