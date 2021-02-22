#include <stdio.h>
#include <string.h>

#include "cmsis_os.h"
//#include "rl_fs.h"
#include "c_md5.h"
#include "tty.h"
#include "nand_file.h"

#define MD5_BUF_SIZE        2048
int fs_get_md5(const char *filename, uint8_t md5[16])
{
    FILE_STRUCT *f;
    size_t size;
    static uint8_t buf[MD5_BUF_SIZE];

    MD5_CTX context;

    f = getfileaddr((char *)filename);

    MD5Init(&context);

    size = sizeof(buf);

		if(set_readnum(f,0) == false)
		{
				return 1;
		}
	
		osSemaphoreWait(nand_sem,osWaitForever);

    while(1)
    {
        if(f->freadnum + size >= f->fsize)
        {
            size = f->fsize - f->freadnum;
        }
        fread_w(f, buf, size);
				
				if(size == MD5_BUF_SIZE)
				{
						for(uint8_t i = 0;i < 4; i++)
						{
								MD5Update(&context, buf + i*size/4, size/4);
								osDelay(1);
						}
				}
				else
				{
						MD5Update(&context, buf, size);
				}
        if(f->freadnum == 0)
        {
            break;
        }
				osDelay(1);
    }
		osSemaphoreRelease(nand_sem);

		if(set_readnum(f,0) == false)
		{
				return 1;
		}

    MD5Final(md5, &context);

    return 0;
}

int fs_get_md5_string(const char *filename, char md5_string[33])
{
    int i = 0;
    uint8_t md5[16];

    if(fs_get_md5(filename, md5))
    {
        return 1;
    }

    for(i = 0; i < 16; i++)
    {
        snprintf(&md5_string[i * 2], 3, "%02x", md5[i]);
    }

    /* just make sure str[32] == 0 */
    md5_string[32] = '\0';

    return 0;
}

//fsStatus fs_get_time  ( fsTime *  time )
//{
//    time->hr   = 12;    // Hours:   0 - 23
//    time->min  = 0;     // Minutes: 0 - 59
//    time->sec  = 0;     // Seconds: 0 - 59
//    time->day  = 1;     // Day:     1 - 31
//    time->mon  = 1;     // Month:   1 - 12
//    time->year = 2013;  // Year:    1980 - 2107
//    return (fsOK);
//}
