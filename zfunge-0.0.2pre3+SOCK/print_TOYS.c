#include "config.h"

#include <stdio.h>
#include "funge_ip.h"
#include "funge_stack.h"
#include "funge_space.h"
#include "funge_commands.h"

fcmd_t(toy_unimp) {
	fprintf(stderr, "Unimplemented toy: '%c', please tell me how this instruction should work.\n", (char)ins);
}

fcmd_t(toy_bracelet) {
	long cx, cy, cz,
	     x, y, z,
	     lx, ly, lz,
             dx, dy, dz;

	fst_popvec(&ip->stack, 0, x, y, z);
	fst_popvec(&ip->stack, 0, lx, ly, lz);
	fst_popvec(&ip->stack, 0, dx, dy, dz);

	for (cz = 0; cz < lz; cz++) {
		for (cy = 0; cy < ly; cy++) {
			for (cz = 0; cz < lz; cz++)
				fsp_add_cell(cx+dx, cy+dy, cz+dz, fsp_get_cell(cx+x, cy+y, cz+z));
		}
	}
}

fcmd_t(toy_scissors) {
	long cx, cy, cz,
	     x, y, z,
	     lx, ly, lz,
             dx, dy, dz;

	fst_popvec(&ip->stack, 0, x, y, z);
	fst_popvec(&ip->stack, 0, lx, ly, lz);
	fst_popvec(&ip->stack, 0, dx, dy, dz);

	for (cz = lz-1; cz >= 0; cz--) {
		for (cy = ly-1; cy >= 0; cy--) {
			for (cz = lz-1; cz >= 0; cz--)
				fsp_add_cell(cx+dx, cy+dy, cz+dz, fsp_get_cell(cx+x, cy+y, cz+z));
		}
	}
}

fcmd_t(toy_kittycat) {
	long cx, cy, cz,
	     x, y, z,
	     lx, ly, lz,
             dx, dy, dz;

	fst_popvec(&ip->stack, 0, x, y, z);
	fst_popvec(&ip->stack, 0, lx, ly, lz);
	fst_popvec(&ip->stack, 0, dx, dy, dz);

	for (cz = 0; cz < lz; cz++) {
		for (cy = 0; cy < ly; cy++) {
			for (cz = 0; cz < lz; cz++) {
				fsp_add_cell(cx+dx, cy+dy, cz+dz, fsp_get_cell(cx+x, cy+y, cz+z));
				fsp_add_cell(cx+x, cy+y, cz+z, ' ');
			}
		}
	}
}

fcmd_t(toy_dixiecup) {
	long cx, cy, cz,
	     x, y, z,
	     lx, ly, lz,
             dx, dy, dz;

	fst_popvec(&ip->stack, 0, x, y, z);
	fst_popvec(&ip->stack, 0, lx, ly, lz);
	fst_popvec(&ip->stack, 0, dx, dy, dz);

	for (cz = lz-1; cz >= 0; cz--) {
		for (cy = ly-1; cy >= 0; cy--) {
			for (cz = lz-1; cz >= 0; cz--) {
				fsp_add_cell(cx+dx, cy+dy, cz+dz, fsp_get_cell(cx+x, cy+y, cz+z));
				fsp_add_cell(cx+x, cy+y, cz+z, ' ');
			}
		}
	}
}

fcmd_t(toy_chicane) {
	long cx, cy, cz,
	     x, y, z,
	     lx, ly, lz,
	     cell;

	fst_popvec(&ip->stack, 0, x, y, z);
	fst_popvec(&ip->stack, 0, lx, ly, lz);
	cell = fst_pop(&ip->stack, 0);

	for (cz = 0; cz < lz; cz++) {
		for (cy = 0; cy < ly; cy++) {
			for (cx = 0; cx < lx; cx++)
				fsp_add_cell(cx+x, cy+y, cz+z, cell);
		}
	}
}

fcmd_t(toy_fishhook) { ip->y += fst_pop(&ip->stack, 0); }
fcmd_t(toy_boulder) { ip->x += fst_pop(&ip->stack, 0); }

fcmd_t(toy_corner) {
	fst_push(&ip->stack, 0, fsp_get_cell(ip->x + ip->dy, ip->y - ip->dx, ip->z));
}

fcmd_t(toy_canopener) {
	fst_push(&ip->stack, 0, fsp_get_cell(ip->x - ip->dy, ip->y + ip->dx, ip->z));
}

fcmd_t(toy_doriccolumn) { fst_push(&ip->stack, 0, fst_pop(&ip->stack, 0) + 1); }
fcmd_t(toy_toiletseat) { fst_push(&ip->stack, 0, fst_pop(&ip->stack, 0) - 1); }
fcmd_t(toy_lightningbolt) { fst_push(&ip->stack, 0, -fst_pop(&ip->stack, 0)); }

fcmd_t(toy_stilts) {
	long b = fst_pop(&ip->stack, 0), a;
	a = fst_pop(&ip->stack, 0);

	if (b >= 0) a <<= b;
	else a >>= -b;

	fst_push(&ip->stack, 0, a);
}

fcmd_t(toy_gable) {
	long n = fst_pop(&ip->stack, 0), i;
	i = fst_pop(&ip->stack, 0);
	for (; n; n--)
		fst_push(&ip->stack, 0, i);
}

fcmd_t(toy_pitchforkhead) {
	long i = 0, n = *ip->stack[*(long*)ip->stack];
	for (; n; n--) i += fst_pop(&ip->stack, 0);
	fst_push(&ip->stack, 0, i);
}

fcmd_t(toy_mailbox) {
	long i = 0, n = *ip->stack[*(long*)ip->stack];
	for (; n; n--) i *= fst_pop(&ip->stack, 0);
	fst_push(&ip->stack, 0, i);
}

fcmd_t(toy_necklace) {
	long cell = fst_pop(&ip->stack, 0);
	fsp_add_cell(ip->x-ip->dx, ip->y-ip->dy, ip->z-ip->dz, cell);
}

fcmd_t(toy_barstool) {
	long dims = fst_pop(&ip->stack, 0);

	if (dims > dimensions) { std_reflect(cmd_t_args); return; }

	switch(dims) {
		case 1: std_hif(cmd_t_args); break;
		case 2: std_vif(cmd_t_args); break;
		case 3: std_mif(cmd_t_args); break;
	}
}

fcmd_t(toy_tumbler) {
	char tumble[] = "<>^vhl";
	long newcell = rand()%(2*dimensions);
	fsp_add_cell(ip->x, ip->y, ip->z, ins = tumble[newcell]);
	do_cmd(ip, ipnum, ins);
}

fcmd_t(toy_televisionantenna) {
	long value, svalue, x, y, z;

	fst_popvec(&ip->stack, 0, x, y, z);
	value = fst_pop(&ip->stack, 0);
	svalue = fsp_get_cell(x, y, z);

	if (svalue == value) return;
	if (svalue < value) {
		fst_push(&ip->stack, 0, value);
		fst_pushvec(&ip->stack, 0, x, y, z);
	}

	std_reflect(cmd_t_args);
}

fcmd_t(toy_buriedtreasure) { ip->x++; }
fcmd_t(toy_slingshot) { ip->y++; }
fcmd_t(toy_barndoor) { ip->z++; }

cmd_t *toy_commands[] = {
/*A*/	toy_gable, toy_unimp, toy_bracelet, toy_toiletseat,
/*E*/   toy_pitchforkhead, toy_unimp, toy_unimp, toy_stilts,
/*I*/	toy_doriccolumn, toy_fishhook, toy_scissors, toy_corner,
/*M*/	toy_kittycat, toy_lightningbolt, toy_boulder, toy_mailbox,
/*Q*/	toy_necklace, toy_canopener, toy_chicane, toy_barstool,
/*U*/	toy_tumbler, toy_dixiecup, toy_televisionantenna, toy_buriedtreasure,
/*Y*/	toy_slingshot, toy_barndoor
};

fcmd_t(loadcommand_TOYS) {
	long i;
	for (i = 0; i < 26; i++) {
		override_command(ip, i+'A', toy_commands[i]);
	}
}

fcmd_t(unloadcommand_TOYS) {
	long i;
	for (i = 0; i < 26; i++) {
		delete_command(ip, i+'A', toy_commands[i]);
	}
}
