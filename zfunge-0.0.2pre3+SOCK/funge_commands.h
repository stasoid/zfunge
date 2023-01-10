#ifndef __FUNGE_COMMANDS_H__
#define __FUNGE_COMMANDS_H__

#include "funge_ip.h"

typedef void (cmd_t)(struct ipoint *ip, long *ipnum, long ins);
#define fcmd_t(f) void f(struct ipoint *ip, long *ipnum, long ins)
#define cmd_t_args ip, ipnum, ins

struct print {
	long id;
	cmd_t *loadfunc, *unloadfunc;
};

cmd_t *default_commands[256];
cmd_t loadcommand_standard;

#define cmd_func(ip, ins) ((ins<0||ins>255)?std_unimp:ip->commands[ins][*(long*)ip->commands[ins]])
#define do_cmd(ip, ipnum, ins) ((ins<0||ins>255)?std_unimp:(*(long*)ip->commands[ins])?cmd_func(ip, ins)(ip, ipnum, ins):default_commands[ins](ip, ipnum, ins))

void override_command(struct ipoint *ip, long ins, cmd_t *func);
void delete_command(struct ipoint *ip, long ins, cmd_t *func);

extern cmd_t std_reflect, std_unimp, std_nop, std_die,
	     std_quit, std_digit, std_hexdig, std_west,
	     std_east, std_north, std_south, std_high,
	     std_low, std_hif, std_vif, std_mif,
	     std_add, std_sub, std_mul, std_div,
	     std_mod, std_swap, std_string, std_odeci,
	     std_ochar, std_dup, std_not, std_greater,
	     std_ramp, std_comment, std_char, std_store,
	     std_pop, std_random, std_ideci, std_ichar,
	     std_getcell, std_putcell, std_left, std_right,
	     std_jump, std_clear, std_compare, std_delta,
	     std_under, std_begin, std_end, std_in, std_out,
	     std_system, std_iterate, std_fload, std_funload,
	     std_sysinfo, std_fork;

extern char *sysinfo_args;
extern int sysinfo_args_len;

extern char *sysinfo_enviro;
extern int sysinfo_enviro_len;

#endif
