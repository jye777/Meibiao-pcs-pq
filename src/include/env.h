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

/* 环境变量里保存的主网卡的MAC地址 */
#define MAC              ("mac")
/* 环境变量里保存的主网卡的IP地址 */
#define IP               ("ip")
/* 环境变量里保存的主网卡的NETMASK地址 */
#define NETMASK          ("netmask")
/* 环境变量里保存的主网卡的GATEWAY地址 */
#define GATEWAY          ("gateway")
#define DNS              ("dns")


/* 环境变量里保存的网卡0的MAC地址 */
#define MAC0              ("mac0")
/* 环境变量里保存的网卡0的IP地址 */
#define IP0               ("ip0")
/* 环境变量里保存的网卡0的NETMASK地址 */
#define NETMASK0          ("netmask0")
/* 环境变量里保存的网卡0的GATEWAY地址 */
#define GATEWAY0          ("gateway0")
#define DNS0              ("dns0")

/* 环境变量里保存的网卡1的MAC地址 */
#define MAC1              ("mac1")
/* 环境变量里保存的网卡1的IP地址 */
#define IP1               ("ip1")
/* 环境变量里保存的网卡1的NETMASK地址 */
#define NETMASK1          ("netmask1")
/* 环境变量里保存的网卡1的GATEWAY地址 */
#define GATEWAY1          ("gateway1")
#define DNS1              ("dns1")



#endif /* ENV_H */

