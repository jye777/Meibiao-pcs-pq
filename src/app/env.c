#include <stdio.h>
#include <string.h>
#include "cmsis_os.h"
#include "stm32f4xx.h"
#include "crc.h"
#include "env.h"
#include "flashdata.h"

#define env_crc         crc32_get
#define ub_env_puts     printf

ub_env *env_cache;
static uint32_t env_size = ENV_MAXSIZE;

#define env_get_char(x)     (env_cache->data[x])
#define env_get_addr(x)     (&env_cache->data[x])

const char env_filename[] = "env.dat";

void env_crc_update(void)
{
    env_cache->crc = env_crc(0, env_get_addr(0), env_size - sizeof(uint32_t));
}

static int envmatch(uint8_t *s1, int i2)
{
    while(*s1 == env_get_char(i2++))
    {
        if(*s1++ == '=')
        {
            return(i2);
        }
    }

    if(*s1 == '\0' && env_get_char(i2 - 1) == '=')
    {
        return(i2);
    }

    return(-1);
}

char *getenv(const char *name)
{
    int i, nxt;

    for(i = 0; env_get_char(i) != '\0'; i = nxt + 1)
    {
        int val;

        for(nxt = i; env_get_char(nxt) != '\0'; ++nxt)
        {
            if(nxt >= env_size)
            {
                return (NULL);
            }
        }

        if((val = envmatch((uint8_t*)name, i)) < 0)
        {
            continue;
        }

        return ((char *)env_get_addr(val));
    }

    return (NULL);
}

int setenv(char *envname, char *value)
{
    int   len, oldval;
    uint8_t *env, *nxt = NULL;
    char *name;
    uint8_t *env_data = env_get_addr(0);

    if(!env_data)   /* need copy in RAM */
    {
        return 1;
    }

    name = envname;
    /*
     * search if variable with this name already exists
     */
    oldval = -1;

    for(env = env_data; *env; env = nxt + 1)
    {
        for(nxt = env; *nxt; ++nxt)
            ;

        if((oldval = envmatch((uint8_t*)name, env - env_data)) >= 0)
        {
            break;
        }
    }

    /*
     * Delete any existing definition
     */
    if(oldval >= 0)
    {
        if(*++nxt == '\0')
        {
            if(env > env_data)
            {
                env--;
            }
            else
            {
                *env = '\0';
            }
        }
        else
        {
            for(;;)
            {
                *env = *nxt++;

                if((*env == '\0') && (*nxt == '\0'))
                {
                    break;
                }

                ++env;
            }
        }

        *++env = '\0';
    }

    /* Delete only ? */
    if(value[0] == NULL)
    {
        env_crc_update();
        return 0;
    }

    /*
     * Append new definition at the end
     */
    for(env = env_data; *env || *(env + 1); ++env)
        ;

    if(env > env_data)
    {
        ++env;
    }

    /*
     * Overflow when:
     * "name" + "=" + "val" +"\0\0"  > env_size - (env-env_data)
     */
    len = strlen(name) + 2;
    /* add '=' for first arg, ' ' for all others */
    len += strlen(value) + 1;

    if(len > (&env_data[env_size] - env))
    {
        return 1;
    }

    while((*env = *name++) != '\0')
    {
        env++;
    }

    *env = '=';

    while((*++env = *value++) != '\0');

    /* end is marked with double '\0' */
    *++env = '\0';

    /* Update CRC */
    env_crc_update();
    return 0;
}

int saveenv(void)
{
    if(env_cache->crc != env_crc(0, env_get_addr(0), env_size - sizeof(uint32_t)))
    {
        return 1;
    }
    return 0;
}

int env_init(void)
{
	setenv("main_update", "0");
	if(env_cache->crc != env_crc(0, &(env_cache->data[0]), env_size - sizeof(uint32_t)))
    {
        return 1;
    }
    return 0;
}
