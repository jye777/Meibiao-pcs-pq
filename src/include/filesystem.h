/**
 * @file			filesystem.h
 * @brief			
 * @author			wangpeng
 * @date			2016/4/26
 * @version			Initial Draft
 * @par				Copyright (C),  2013-2023, SCNEE
 * @par History:
 * 1.Date: 			2016/4/26
 *   Author: 			wangpeng
 *   Modification: 		Created file
*/

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "rl_fs.h"                      /* FileSystem definitions             */

//fsStatus filesystem_init (void);
//fsStatus fcopy(const char *src, const char *dest);
//int get_file_info(const char * fn,fsFileInfo * info);
int fs_get_md5(const char *filename, uint8_t md5[16]);
int fs_get_md5_string(const char *filename, char md5_string[33]);

#endif /* FILESYSTEM_H */

