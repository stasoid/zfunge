#include "config.h"
/* Stuff to handle the IP */

#include <stdlib.h>
#include "funge_ip.h"
#include "funge_stack.h"
#include "funge_commands.h"

struct ipoint **ips;

void fip_init() {
	ips = malloc(sizeof(*ips)*2);
	*(long*)ips = 1;
	ips[1] = malloc(sizeof(*ips[1]));
	fst_init(&ips[1]->stack);
	ips[1]->x = -1;
	ips[1]->y = 0;
	ips[1]->z = 0;
	ips[1]->dx = 1;
	ips[1]->dy = 0;
	ips[1]->dz = 0;
	ips[1]->ox = 0;
	ips[1]->oy = 0;
	ips[1]->oz = 0;
	ips[1]->stringmode = 0;
}

void fip_fork(long ipnum) {
	int i;

	ips = realloc(ips, sizeof(*ips) * (++(*(long*)ips) + 1));
	for (i = *(long*)ips; i > ipnum; i--)
		ips[i] = ips[i-1];

	ips[ipnum+1] = malloc(sizeof(*ips[1]));
	ips[ipnum+1]->stack = fst_duplicate_stack(&ips[ipnum]->stack);

	ips[ipnum+1]->x = ips[ipnum]->x;
	ips[ipnum+1]->y = ips[ipnum]->y;
	ips[ipnum+1]->z = ips[ipnum]->z;

	ips[ipnum+1]->dx = -ips[ipnum]->dx;
	ips[ipnum+1]->dy = -ips[ipnum]->dy;
	ips[ipnum+1]->dz = -ips[ipnum]->dz;

	ips[ipnum+1]->ox = ips[ipnum]->ox;
	ips[ipnum+1]->oy = ips[ipnum]->oy;
	ips[ipnum+1]->oz = ips[ipnum]->oz;

	ips[ipnum+1]->stringmode = ips[ipnum]->stringmode;

	for (i = 0; i < 256; i++) {
		ips[ipnum+1]->commands[i] = malloc(sizeof(*ips[ipnum+1]->commands[i]) * (*(long*)ips[ipnum]->commands[i] + 1));
		memcpy(ips[ipnum+1]->commands[i], ips[ipnum]->commands[i], sizeof(*ips[ipnum+1]->commands[i]) * (*(long*)ips[ipnum]->commands[i] + 1));
	}
}

void fip_killip(long ipnum) {
	int i;

	for (i = 0; i < 256; i++) {
		free(ips[ipnum]->commands[i]);
	}

	fst_delete_stackstack(&ips[ipnum]->stack);

	if (ipnum < *(long*)ips)
		memcpy(&ips[ipnum], &ips[ipnum+1], sizeof(ips[ipnum]) * (*(long*)ips - ipnum));

	ips = realloc(ips, sizeof(*ips) * (--(*(long*)ips) + 1));
}
