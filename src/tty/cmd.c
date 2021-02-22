#include <stdio.h>

#include "cmd.h"
#include "cmd_comm.h"
#include "cmd_sys.h"
#include "cmd_mem.h"
#include "cmd_net.h"
#include "cmd_test.h"
#include "cmd_env.h"
#include "cmd_tftp.h"
#include "cmd_file.h"
#include "cmd_time.h"
#include "cmd_log.h"
#include "cmd_fpga.h"
#include "cmd_data.h"
#include "cmd_epcs.h"
#include "cmd_fireware.h"
#include "cmd_power.h"

cmdt *main_help_cmdt[] =
{
    &cmd_main_help,
};

cmdt *normal_cmdt[] =
{
    &cmd_clear,
    &cmd_md,
    &cmd_md_byte,
    &cmd_md_long,
    &cmd_md_word,
    &cmd_reset,
		&cmd_dbg,
    &cmd_showconn,
    &cmd_test,
    &cmd_setenv,
    &cmd_printenv,
    &cmd_clearenv,
//    &cmd_saveenv,
    &cmd_tftp,
    &cmd_fireware,
		&cmd_Specified_def,
//    &cmd_ls,
    &cmd_format,
		&cmd_formatall,
		&cmd_badblockcheck,
		&cmd_printfileinf,
//    &cmd_del,
//    &cmd_fsinfo,
    &cmd_gettime,
    &cmd_settime,
    &cmd_logtest,
    &cmd_logclear,
    &cmd_showlog,
    &cmd_fread,
    &cmd_fwrite,
    &cmd_readparas,
    &cmd_readpara,  
	&cmd_powerclear,
	&cmd_timeclear,
	&cmd_showrunstate,
    //&cmd_meter,
    &cmd_epcs,
};

cmdt_list main_cmdt_list[] =
{
    {main_help_cmdt, sizeof(main_help_cmdt) / sizeof(main_help_cmdt[0])},
    {normal_cmdt, sizeof(normal_cmdt) / sizeof(normal_cmdt[0])},
};

cmdt_list *cmd_main_list(void)
{
    return main_cmdt_list;
}

int cmd_main_listlen(void)
{
    return sizeof(main_cmdt_list) / sizeof(main_cmdt_list[0]);
}

