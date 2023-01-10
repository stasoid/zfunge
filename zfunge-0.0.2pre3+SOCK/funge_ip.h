#ifndef __FUNGE_IP_H__
#define __FUNGE_IP_H__

struct ipoint;

#include "funge_commands.h"

struct ipoint {
	long x, y, z;
	long dx, dy, dz;
	long oz, ox, oy;
	long stringmode;

	cmd_t **commands[256];
	long **stack;
};

extern struct ipoint **ips;

void fip_init();
void fip_fork(long ipnum);
void fip_killip(long ipnum);

#endif
