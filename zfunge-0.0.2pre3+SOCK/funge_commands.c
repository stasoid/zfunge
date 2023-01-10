#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "funge_ip.h"
#include "funge_space.h"
#include "funge_stack.h"
#include "funge_commands.h"

cmd_t loadcommand_standard, std_unimp;

cmd_t
      loadcommand_BASE, unloadcommand_BASE,
      loadcommand_FILE, unloadcommand_FILE,
      loadcommand_FIXP, unloadcommand_FIXP,
      loadcommand_NULL, unloadcommand_NULL,
      loadcommand_PERL, unloadcommand_PERL,
      loadcommand_SOCK, unloadcommand_SOCK,
      loadcommand_STRN, unloadcommand_STRN,
      loadcommand_SUBR, unloadcommand_SUBR,
      loadcommand_TERM, unloadcommand_TERM,
      loadcommand_TOYS, unloadcommand_TOYS;
/* these *MUST* be sorted */
struct print fingerprints[] = {
/*BASE*/	{ 0x42415345, loadcommand_BASE, unloadcommand_BASE },
/*FILE*/	{ 0x46494C45, loadcommand_FILE, unloadcommand_FILE },
/*FIXP*/	{ 0x46495850, loadcommand_FIXP, unloadcommand_FIXP },
/*NULL*/	{ 0x4e554c4c, loadcommand_NULL, unloadcommand_NULL },
/*PERL*/	{ 0x5045524c, loadcommand_PERL, unloadcommand_PERL },
/*SOCK*/	{ 0x534f434b, loadcommand_SOCK, unloadcommand_SOCK },
/*STRN*/	{ 0x5354524e, loadcommand_STRN, unloadcommand_STRN },
/*SUBR*/	{ 0x53554252, loadcommand_SUBR, unloadcommand_SUBR },
/*TERM*/	{ 0x5445524d, loadcommand_TERM, unloadcommand_TERM },
/*TOYS*/	{ 0x544f5953, loadcommand_TOYS, unloadcommand_TOYS },
		{ 0x0, NULL, NULL }
};

fcmd_t(std_reflect) { ip->dx = -ip->dx; ip->dy = -ip->dy; ip->dz = -ip->dz; }
fcmd_t(std_unimp) { std_reflect(cmd_t_args); }
fcmd_t(std_nop) { return; }
fcmd_t(std_space) { return; }

fcmd_t(std_die) { fip_killip(*ipnum); (*ipnum)--; }
fcmd_t(std_quit) { exit(fst_pop(&ip->stack, 0)); }

fcmd_t(std_digit) { fst_push(&ip->stack, 0, ins - '0'); }
fcmd_t(std_hexdig) { fst_push(&ip->stack, 0, ins - 'a' + 10); }

fcmd_t(std_west) { ip->dx = -1; ip->dy = 0; ip->dz = 0; }
fcmd_t(std_east) { ip->dx = 1; ip->dy = 0; ip->dz = 0; }
fcmd_t(std_north) { ip->dx = 0; ip->dy = -1; ip->dz = 0; }
fcmd_t(std_south) { ip->dx = 0; ip->dy = 1; ip->dz = 0; }
fcmd_t(std_high) { ip->dx = 0; ip->dy = 0; ip->dz = -1; }
fcmd_t(std_low) { ip->dx = 0; ip->dy = 0; ip->dz = 0; }

fcmd_t(std_hif) { ip->dx = fst_pop(&ip->stack, 0)?-1:1; ip->dy = 0; ip->dz = 0; }
fcmd_t(std_vif) { ip->dy = fst_pop(&ip->stack, 0)?-1:1; ip->dx = 0; ip->dz = 0; }
fcmd_t(std_mif) { ip->dz = fst_pop(&ip->stack, 0)?-1:1; ip->dx = 0; ip->dz = 0; }

fcmd_t(std_add) { fst_push(&ip->stack, 0, fst_pop(&ip->stack, 0) + fst_pop(&ip->stack, 0)); }
fcmd_t(std_sub) { int i = fst_pop(&ip->stack, 0); fst_push(&ip->stack, 0, fst_pop(&ip->stack, 0) - i); }
fcmd_t(std_mul) { int i = fst_pop(&ip->stack, 0); fst_push(&ip->stack, 0, fst_pop(&ip->stack, 0) * i); }
fcmd_t(std_div) { int i = fst_pop(&ip->stack, 0); fst_push(&ip->stack, 0, i?(fst_pop(&ip->stack, 0)/i):0); }
fcmd_t(std_mod) { int i = fst_pop(&ip->stack, 0); fst_push(&ip->stack, 0, i?(fst_pop(&ip->stack, 0)%i):0); }

fcmd_t(std_swap) {
	int a = fst_pop(&ip->stack, 0), b;
	b = fst_pop(&ip->stack, 0);
	fst_push(&ip->stack, 0, a);
	fst_push(&ip->stack, 0, b);
}

fcmd_t(std_string) {
	ip->stringmode = 1;
	return;
/*
	long lins;

	for (;;) {
		lins = ins;

		ins = fsp_get_cell(ip->x+ip->dx, ip->y+ip->dy, ip->z+ip->dz);
		if (ins == ' ')
			fst_push(&ip->stack, 0, ins);

		fsp_inc_ip(ip);
		ins = fsp_get_cell(ip->x, ip->y, ip->z);
		if (ins == '"') break;
		fst_push(&ip->stack, 0, ins);
	}
*/
}

fcmd_t(std_odeci) { printf("%ld ", fst_pop(&ip->stack, 0)); }
fcmd_t(std_ochar) { char ch = fst_pop(&ip->stack, 0); printf("%c", ch); }

fcmd_t(std_dup) {
	int i = fst_pop(&ip->stack, 0);
	fst_push(&ip->stack, 0, i);
	fst_push(&ip->stack, 0, i);
}

fcmd_t(std_not) { fst_push(&ip->stack, 0, !fst_pop(&ip->stack, 0)); }
fcmd_t(std_greater) {
	int a = fst_pop(&ip->stack, 0);
	if (a < fst_pop(&ip->stack, 0)) fst_push(&ip->stack, 0, 1);
	else fst_push(&ip->stack, 0, 0);
}

fcmd_t(std_ramp) {
	ip->x += ip->dx;
	ip->y += ip->dy;
	ip->z += ip->dz;
}
fcmd_t(std_comment) {
	fprintf(stderr, "Uh.. you shouldn't be seeing this.. my comment stuff fucked..\n");
/*
	for (;;) {
		fsp_inc_ip(ip);
		ins = fsp_get_cell(ip->x, ip->y, ip->z);
		if (ins == ';') break;
	}
	fsp_inc_ip(ip);
	ins = fsp_get_cell(ip->x, ip->y, ip->z);
	do_cmd(ip, ipnum, ins);
*/
}

fcmd_t(std_char) {
	ip->x += ip->dx; ip->y += ip->dy; ip->z += ip->dz;
	fst_push(&ip->stack, 0, fsp_get_cell(ip->x, ip->y, ip->z));
}

fcmd_t(std_store) {
	ip->x += ip->dx; ip->y += ip->dy; ip->z += ip->dz;
	fsp_add_cell(ip->x, ip->y, ip->z, fst_pop(&ip->stack, 0));
}

fcmd_t(std_pop) { fst_pop(&ip->stack, 0); }
fcmd_t(std_random) {
	switch (rand()%(2*dimensions)) {
		case 0: ip->dx = -1; ip->dy = 0; ip->dz = 0; break;
		case 1: ip->dx = 1; ip->dy = 0; ip->dz = 0; break;
		case 2: ip->dx = 0; ip->dy = -1; ip->dz = 0; break;
		case 3: ip->dx = 0; ip->dy = 1; ip->dz = 0; break;
		case 4: ip->dx = 0; ip->dy = 0; ip->dz = -1; break;
		case 5: ip->dx = 0; ip->dy = 0; ip->dz = 1; break;
	}
}

fcmd_t(std_ideci) {
	long i;
	if (scanf("%ld", &i) < 0) { std_reflect(cmd_t_args); return; }
	fst_push(&ip->stack, 0, i);
}
fcmd_t(std_ichar) {
	long i;
	if ((i = fgetc(stdin)) == EOF) { std_reflect(cmd_t_args); return; }
	fst_push(&ip->stack, 0, i);
}

fcmd_t(std_getcell) {
	long x = 0, y = 0, z = 0;
	if (dimensions > 2) z = fst_pop(&ip->stack, 0) + ip->oz;
	if (dimensions > 1) y = fst_pop(&ip->stack, 0) + ip->oy;
	x = fst_pop(&ip->stack, 0) + ip->ox;

	fst_push(&ip->stack, 0, fsp_get_cell(x, y, z));
}

fcmd_t(std_putcell) {
	long x = 0, y = 0, z = 0, i;
	if (dimensions > 2) z = fst_pop(&ip->stack, 0) + ip->oz;
	if (dimensions > 1) y = fst_pop(&ip->stack, 0) + ip->oy;
	x = fst_pop(&ip->stack, 0) + ip->ox;

	i = fst_pop(&ip->stack, 0);

	fsp_add_cell(x, y, z, i);
}

fcmd_t(std_left) {
	long t;
	t = ip->dx;
	ip->dx = ip->dy;
	ip->dy = -t;
}
fcmd_t(std_right) {
	long t;
	t = ip->dx;
	ip->dx = -ip->dy;
	ip->dy = t;
}

fcmd_t(std_jump) {
	long i = fst_pop(&ip->stack, 0);
	ip->x += ip->dx*i;
	ip->y += ip->dy*i;
	ip->z += ip->dz*i;
}

fcmd_t(std_clear) { fst_clear_stack(&ip->stack, 0); }
fcmd_t(std_compare) {
	long a = fst_pop(&ip->stack, 0);
	if (a < fst_pop(&ip->stack, 0))
		std_left(cmd_t_args);
	else std_right(cmd_t_args);
}

fcmd_t(std_delta) {
	if (dimensions > 2) ip->dz = fst_pop(&ip->stack, 0);
	if (dimensions > 1) ip->dy = fst_pop(&ip->stack, 0);
	ip->dx = fst_pop(&ip->stack, 0);
}

fcmd_t(std_under) {
	long a = fst_pop(&ip->stack, 0), i;

	if (a < 0) {
		for (i = 0; i < a; i++)
			fst_push(&ip->stack, 0, fst_peek(&ip->stack, 0, i<<1));
	} else {
		for (i = 0; i < a; i++)
			fst_push(&ip->stack, 0, 0);
	}
}

fcmd_t(std_begin) {
	fst_copycreatestack(&ip->stack, fst_pop(&ip->stack, 0));
	fst_pushvec(&ip->stack, 1, ip->ox, ip->oy, ip->oz);
	ip->ox = ip->x + ip->dx;
	ip->oy = ip->y + ip->dy;
	ip->oz = ip->z + ip->dz;
}

fcmd_t(std_end) {
	fst_popvec(&ip->stack, 1, ip->ox, ip->oy, ip->oz);
	fst_copydeletestack(&ip->stack, fst_pop(&ip->stack, 0));
}

fcmd_t(std_in) {
	long x = 0, y = 0, z = 0,
	     lx = 0, ly = 0, lz = 0, flags;
	char *file;

	file = fst_popstring(&ip->stack, 0);
	flags = fst_pop(&ip->stack, 0);

	if (dimensions > 2) z = fst_pop(&ip->stack, 0) + ip->oz;
	if (dimensions > 1) y = fst_pop(&ip->stack, 0) + ip->oy;
	x = fst_pop(&ip->stack, 0) + ip->ox;

	if (flags&1) {
		if (fsp_load_binary(file, x, y, z, &lx, &ly, &lz)) {
			free(file); std_reflect(cmd_t_args); return;
		}
	} else {
		if (fsp_load_with_len(file, x, y, z, &lx, &ly, &lz)) {
			free(file); std_reflect(cmd_t_args); return;
		}
	}

	free(file);

	fst_push(&ip->stack, 0, lx);
	if (dimensions > 1) fst_push(&ip->stack, 0, ly);
	if (dimensions > 2) fst_push(&ip->stack, 0, lz);

	fst_push(&ip->stack, 0, x);
	if (dimensions > 1) fst_push(&ip->stack, 0, y);
	if (dimensions > 2) fst_push(&ip->stack, 0, z);
}

fcmd_t(std_out) {
	long x = 0, y = 0, z = 0,
	     lx = 0, ly = 0, lz = 0, flags;
	char *file;

	file = fst_popstring(&ip->stack, 0);
	flags = fst_pop(&ip->stack, 0);

	if (dimensions > 2) z = fst_pop(&ip->stack, 0) + ip->oz;
	if (dimensions > 1) y = fst_pop(&ip->stack, 0) + ip->oy;
	x = fst_pop(&ip->stack, 0) + ip->ox;

	if (dimensions > 2) lz = fst_pop(&ip->stack, 0);
	if (dimensions > 1) ly = fst_pop(&ip->stack, 0);
	lx = fst_pop(&ip->stack, 0);

	if (fsp_save(file, x, y, z, lx, ly, lz)) {
		std_reflect(cmd_t_args);
	}

	free(file);
}

/* I may make an option to disable this command....... */
fcmd_t(std_system) {
	char *cmd = fst_popstring(&ip->stack, 0);
	system(cmd);
	free(cmd);
}

fcmd_t(std_iterate) {
	long i = fst_pop(&ip->stack, 0), ournum = *ipnum,
	     x, y, z;


	fsp_inc_ip(ip);
	ins = fsp_get_cell(x = ip->x, y = ip->y, z = ip->z);

	for (; i > 0 && *ipnum == ournum; i--)
		do_cmd(ip, ipnum, ins);
}

fcmd_t(std_fload) {
	long fprint = 0, i, n = fst_pop(&ip->stack, 0);

	for (i = 0; i < n; i++) {
		fprint <<= 8;
		fprint |= fst_pop(&ip->stack, 0);
	}

	for (i = 0; fingerprints[i].id && fprint >= fingerprints[i].id; i++) {
		if (fprint == fingerprints[i].id) {
			fingerprints[i].loadfunc(cmd_t_args);
			fst_push(&ip->stack, 0, fprint);
			fst_push(&ip->stack, 0, 1);
			return;
		}
	}

	loadcommand_fungelib(ip, ipnum, fprint);
}

fcmd_t(std_funload) {
	long fprint = 0, i, n = fst_pop(&ip->stack, 0);

	for (i = 0; i < n; i++) {
		fprint <<= 8;
		fprint |= fst_pop(&ip->stack, 0);
	}

	for (i = 0; fingerprints[i].id && fprint >= fingerprints[i].id; i++) {
		if (fprint == fingerprints[i].id) {
			fingerprints[i].unloadfunc(cmd_t_args);
			return;
		}
	}

	unloadcommand_fungelib(ip, ipnum, fprint);

	/* std_reflect(cmd_t_args); */
}

fcmd_t(std_fork) {
	fip_fork(*ipnum);
}

long sysinfo[] = {
/* 0*/	0xf,					/* flags */
/* 1*/	sizeof(long),				/* cell size */
/* 2*/	0x7a463938, /* zF98 */			/* hand print */
/* 3*/	0x0,					/* version */
/* 4*/	0x1,					/* '=' type */
/* 5*/	'/',					/* Directory separater */
/* 6*/	-1,					/* Dimensions */
/* 7*/	-1,					/* ip ID */
/* 8*/	0,					/* team ID, not used */

/* 9*/	-1, -1, -1,				/* ip vector */
/*12*/	-1, -1, -1,				/* ip delta */
/*15*/	-1, -1, -1,				/* ip offset */

/*18*/	-1, -1, -1,				/* least point, relative to origin */
/*21*/	-1, -1, -1,				/* greatest point, relative to least point */

/*24*/	-1,					/* ((year-1900) << 16) | (month << 8) | day */
/*25*/	-1,					/* (hour << 16) | (minute << 8) | second; */

/*26*/	-1					/* size of stack stack */

/* 27 ...  all not shown here */
	/* size of stack stack cells not shown here */
	/* command line not shown here */
	/* environment not shown here */
};

char *sysinfo_args;
int sysinfo_args_len = 0;

char *sysinfo_enviro;
int sysinfo_enviro_len = 0;

fcmd_t(std_sysinfo) {
	long i = fst_pop(&ip->stack, 0), oldtosslen, delta;
	time_t t;
	struct tm *tm;

	/* Fill in variable info */
	sysinfo[6] = dimensions;
	sysinfo[7] = (long)ip; /* Pointer to IP, we use it to identify */

	sysinfo[9] = ip->x; sysinfo[10] = ip->y; sysinfo[11] = ip->z;
	sysinfo[12] = ip->dx; sysinfo[13] = ip->dy; sysinfo[14] = ip->dz;
	sysinfo[15] = ip->ox; sysinfo[16] = ip->oy; sysinfo[17] = ip->oz;

	fsp_cell_rect(&sysinfo[18], &sysinfo[19], &sysinfo[20],
		      &sysinfo[21], &sysinfo[22], &sysinfo[23]);
	sysinfo[21] -= sysinfo[18];
	sysinfo[22] -= sysinfo[19];
	sysinfo[23] -= sysinfo[20];

	time(&t);
	tm = localtime(&t);

	sysinfo[24] = ((tm->tm_year-1900)<<16)|(tm->tm_mon<<8)|(tm->tm_mday);
	sysinfo[25] = (tm->tm_hour<<16)|(tm->tm_min<<8)|(tm->tm_sec);

	sysinfo[26] = *(long*)ip->stack;
	oldtosslen = *ip->stack[*(long*)ip->stack];

	if (i <= 0) {
		for (i = sysinfo_enviro_len-1; i >= 0; i--)
			fst_push(&ip->stack, 0, sysinfo_enviro[i]);

		for (i = sysinfo_args_len-1; i >= 0; i--)
			fst_push(&ip->stack, 0, sysinfo_args[i]);

		fst_push(&ip->stack, 0, oldtosslen);
		for (i = sysinfo[26] - 1; i > 0; i--)
			fst_push(&ip->stack, 0, *ip->stack[*(long*)ip->stack]);

		fst_push(&ip->stack, 0, sysinfo[26]);
		fst_push(&ip->stack, 0, sysinfo[25]);
		fst_push(&ip->stack, 0, sysinfo[24]);

		fst_pushvec(&ip->stack, 0, sysinfo[21], sysinfo[22], sysinfo[23]);
		fst_pushvec(&ip->stack, 0, sysinfo[18], sysinfo[19], sysinfo[20]);
		fst_pushvec(&ip->stack, 0, sysinfo[15], sysinfo[16], sysinfo[17]);
		fst_pushvec(&ip->stack, 0, sysinfo[12], sysinfo[13], sysinfo[14]);
		fst_pushvec(&ip->stack, 0, sysinfo[ 9], sysinfo[10], sysinfo[11]);

		for (i = 8; i >= 0; i--)
			fst_push(&ip->stack, 0, sysinfo[i]);
	} else {
		i--;
		if (i < 9) {
			fst_push(&ip->stack, 0, sysinfo[i]);
			return;
		}

		i -= 9;

		if ((i / dimensions) < 5) {
			fst_push(&ip->stack, 0, sysinfo[9+(i/3)+(i%dimensions)]);
			return;
		}
		i -= 5 * dimensions;

		if (!i) { fst_push(&ip->stack, 0, sysinfo[26]); return; }
		if (i==1) { fst_push(&ip->stack, 0, oldtosslen); return; }
		if (i <= sysinfo[26]) { fst_push(&ip->stack, 0, *ip->stack[i]); return; }
		i -= sysinfo[26];

		if (i < sysinfo_args_len) { fst_push(&ip->stack, 0, sysinfo_args[i]); return; }
		i -= sysinfo_args_len;
		if (i < sysinfo_enviro_len) { fst_push(&ip->stack, 0, sysinfo_enviro[i]); return; }
		i -= sysinfo_enviro_len;

		fst_push(&ip->stack, 0, fst_peek(&ip->stack, 0, i));
	}

	return;
}

cmd_t *default_commands[256] = {
/* 0-31 */
/*  0*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/*  4*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/*  8*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/* 12*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/* 16*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/* 20*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/* 24*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/* 28*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,

/* 32-63 */
/* 32*/	std_nop    , std_not    , std_string , std_ramp   ,
/* 36*/	std_pop    , std_mod    , std_ideci  , std_char   ,
/* 40*/	std_fload  , std_funload, std_mul    , std_add    ,
/* 44*/	std_ochar  , std_sub    , std_odeci  , std_div    ,
/* 48*/	std_digit  , std_digit  , std_digit  , std_digit  ,
/* 52*/	std_digit  , std_digit  , std_digit  , std_digit  ,
/* 56*/	std_digit  , std_digit  , std_dup    , std_comment,
/* 60*/	std_west   , std_system , std_east   , std_random ,

/* 64-95 */
/* 64*/	std_die    , std_unimp  , std_unimp  , std_unimp  ,
/* 78*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/* 72*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/* 76*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/* 80*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/* 84*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/* 88*/	std_unimp  , std_unimp  , std_unimp  , std_left   ,
/* 92*/	std_swap   , std_right  , std_north  , std_hif    ,

/* 96-127 */
/* 96*/	std_greater, std_hexdig , std_hexdig , std_hexdig ,
/*100*/	std_hexdig , std_hexdig , std_hexdig , std_getcell,
/*104*/	std_high   , std_in     , std_jump   , std_iterate,
/*108*/	std_low    , std_mif    , std_clear  , std_out    ,
/*112*/ std_putcell, std_quit   , std_reflect, std_store  ,
/*116*/ std_fork   , std_under  , std_south  , std_compare,
/*120*/	std_delta  , std_sysinfo, std_nop    , std_begin  ,
/*124*/	std_vif    , std_end    , std_ichar  , std_unimp  ,

/* 128-255 */
/*128*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/*132*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/*136*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/*140*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/*144*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/*148*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/*152*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/*156*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,

/*160*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/*164*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/*168*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/*172*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/*176*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/*180*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/*184*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/*188*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,

/*192*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/*196*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/*200*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/*204*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/*208*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/*212*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/*216*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/*220*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,

/*224*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/*228*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/*232*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/*236*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/*240*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/*244*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/*248*/	std_unimp  , std_unimp  , std_unimp  , std_unimp  ,
/*252*/	std_unimp  , std_unimp  , std_unimp  , std_unimp
};

void override_command(struct ipoint *ip, long ins, cmd_t *func) {
	ip->commands[ins] = realloc(ip->commands[ins], (++(*(long*)ip->commands[ins]) + 1) * sizeof(*ip->commands[ins]));
	ip->commands[ins][*(long*)ip->commands[ins]] = func;
}

void delete_command(struct ipoint *ip, long ins, cmd_t *func) {
	int i = *(long*)ip->commands[ins];

	for (; i > 0; i--) {
		if (ip->commands[ins][i] == func) {
			if (i != *(long*)ip->commands[ins])
				memmove(&ip->commands[ins][i], &ip->commands[ins][i+1], ((*(long*)ip->commands[ins]) - i) * sizeof(*ip->commands[ins]));
			ip->commands[ins] = realloc(ip->commands[ins], (--(*(long*)ip->commands[ins]) + 1) * sizeof(*ip->commands[ins]));
			return;
		}
	}
}

fcmd_t(loadcommand_standard) {
	long i, *zero;

	for (i = 0; i < 256; i++) {
		zero = malloc(sizeof(*zero));
		*zero = 0;
		(long*)ip->commands[i] = zero;
	}
}
