#include "config.h"

/* zfunge mini-funge implementation */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funge_ip.h"
#include "funge_stack.h"
#include "funge_space.h"
#include "funge_commands.h"

#define MINI_MAX_X 40
#define MINI_MAX_Y 10

#define mfsp_inc_ip(ip) { \
	ip->x+=ip->dx; \
	ip->y+=ip->dy; \
\
	if (ip->x < 0) ip->x = MINI_MAX_X-1; \
	if (ip->x >= MINI_MAX_X) ip->x = 0; \
\
	if (ip->y < 0) ip->y = MINI_MAX_Y-1; \
	if (ip->y >= MINI_MAX_Y) ip->y = 0; \
}

struct minispace {
	long space[10][40];
} *cs;

struct ipoint *cip;

struct minispace ***minifinger;

void mfsp_space_init() {
	long x, y;

	for (y = 0; y < MINI_MAX_Y; y++) {
		for (x = 0; x < MINI_MAX_X; x++) {
			cs->space[y][x] = ' ';
		}
	}
}

long mfsp_get_cell(long x, long y) {
	if (x<0 || x>=MINI_MAX_X || y<0 || y>=MINI_MAX_X) return ' ';
	return cs->space[y][x];
}

void mfsp_put_cell(long x, long y, long value) {
	if (x<0 || x>=MINI_MAX_X || y<0 || y>=MINI_MAX_X) return;
	cs->space[y][x] = value;
}

fcmd_t(mf_unimp) {
	fprintf(stderr, "Command not minifunge: '%c'\n", ins);
}

fcmd_t(mf_ramp) {
	mfsp_inc_ip(ip);
}

fcmd_t(mf_char) {
	fst_push(&ip->stack, 0, mfsp_get_cell(ip->x+=ip->dx, ip->y+=ip->dy));
}

fcmd_t(mf_store) {
	mfsp_put_cell(ip->x+=ip->dx, ip->y+=ip->dy, fst_pop(&ip->stack, 0));
}

fcmd_t(mf_iterate) {
	long i = fst_pop(&ip->stack, 0), ournum = *ipnum;

	ins = mfsp_get_cell(ip->x+=ip->dx, ip->y+=ip->dy);
	if (ins == ' ' || ins == ';' || cmd_func(ip, ins) == mf_unimp) return;

	for (; i > 0 && *ipnum == ournum; i--)
		do_cmd(ip, ipnum, ins);
}

fcmd_t(mf_comment) {
        for (;;) {
                mfsp_inc_ip(ip);
                ins = mfsp_get_cell(ip->x, ip->y);
                if (ins == ';') break;
        }
}

fcmd_t(mf_string) {
        long lins;

        for (;;) {
                lins = ins;
		mfsp_inc_ip(ip);
                ins = mfsp_get_cell(ip->x, ip->y);
                if (ins == '"') break;
                if (ins != ' ' || lins != ' ') fst_push(&ip->stack, 0, ins);
        }
}

fcmd_t(mf_random) {
        switch (rand()%4) {
                case 0: ip->dx = -1; ip->dy = 0; ip->dz = 0; break;
                case 1: ip->dx = 1; ip->dy = 0; ip->dz = 0; break;
                case 2: ip->dx = 0; ip->dy = -1; ip->dz = 0; break;
                case 3: ip->dx = 0; ip->dy = 1; ip->dz = 0; break;
        }
}

fcmd_t(mf_die) { (*ipnum)--; }

fcmd_t(mf_real_back) {
	cip->x -= cip->dx;
	cip->y -= cip->dy;
	cip->z -= cip->dz;
}

fcmd_t(mf_real_delta) { fst_popvec(&ip->stack, 0, cip->dx, cip->dy, cip->dz); }

fcmd_t(mf_stackdepth) { fst_push(&ip->stack, 0, *(long*)ip->stack); }

fcmd_t(mf_real_forward) {
	cip->x += cip->dx;
	cip->y += cip->dy;
	cip->z += cip->dz;
}

fcmd_t(mf_getcell) {
	long x, y, z;
	fst_popvec(&ip->stack, 0, x, y, z);
	fst_push(&ip->stack, 0, mfsp_get_cell(x, y));
}

fcmd_t(mf_putcell) {
	long x, y, z, value;
	fst_popvec(&ip->stack, 0, x, y, z);
	value = fst_pop(&ip->stack, 0);
	mfsp_put_cell(x, y, value);
}

fcmd_t(mf_real_move) { fst_popvec(&ip->stack, 0, cip->x, cip->y, cip->z); }

fcmd_t(mf_real_reflect) {
	cip->dx = -cip->dx;
	cip->dy = -cip->dy;
	cip->dz = -cip->dz;
}

cmd_t *minicommands[] = {
/* 0-31 */
/*  0*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,
/*  4*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,
/*  8*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,
/* 12*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,
/* 16*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,
/* 20*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,
/* 24*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,
/* 28*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,

/* 32-63 */
/* 32*/	std_nop   , std_not    , mf_string  , mf_ramp    ,
/* 36*/	std_pop   , std_mod    , std_ideci  , mf_char    ,
/* 40*/	mf_unimp  , mf_unimp   , std_mul    , std_add    ,
/* 44*/	std_ochar , std_sub    , std_odeci  , std_div    ,
/* 48*/	std_digit , std_digit  , std_digit  , std_digit  ,
/* 52*/	std_digit , std_digit  , std_digit  , std_digit  ,
/* 56*/	std_digit , std_digit  , std_dup    , mf_comment ,
/* 60*/	std_west  , mf_unimp   , std_east   , mf_random  ,

/* 64-95 */
/* 64*/	mf_die     , mf_unimp  , mf_real_back, mf_unimp  ,
/* 68*/	mf_real_delta,mf_stackdepth,mf_real_forward,mf_getcell,
/* 72*/	mf_unimp   , mf_unimp  , mf_unimp  , mf_unimp  ,
/* 76*/	mf_real_move, mf_unimp  , mf_unimp  , mf_unimp  ,
/* 80*/	mf_putcell , mf_unimp  , mf_real_reflect, mf_unimp  ,
/* 84*/	mf_unimp   , mf_unimp  , mf_unimp  , mf_unimp  ,
/* 88*/	mf_unimp   , mf_unimp  , mf_unimp  , std_left  ,
/* 92*/	std_swap   , std_right , std_north , std_hif   ,

/* 96-127 */
/* 96*/	std_greater, std_hexdig , std_hexdig , std_hexdig ,
/*100*/	std_hexdig , std_hexdig , std_hexdig , std_getcell,
/*104*/	mf_unimp   , mf_unimp   , std_jump   , mf_iterate ,
/*108*/	mf_unimp   , mf_unimp   , std_clear  , mf_unimp   ,
/*112*/ std_putcell, mf_unimp   , std_reflect, mf_store   ,
/*116*/ mf_unimp   , mf_unimp   , std_south  , std_compare,
/*120*/	mf_unimp   , std_sysinfo, std_nop    , mf_unimp   ,
/*124*/	std_vif    , mf_unimp   , std_ichar  , mf_unimp   ,

/* 128-255 */
/*128*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,
/*132*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,
/*136*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,
/*140*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,
/*144*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,
/*148*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,
/*152*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,
/*156*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,

/*160*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,
/*164*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,
/*168*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,
/*172*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,
/*176*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,
/*180*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,
/*184*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,
/*188*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,

/*192*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,
/*196*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,
/*200*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,
/*204*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,
/*208*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,
/*212*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,
/*216*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,
/*220*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,

/*224*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,
/*228*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,
/*232*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,
/*236*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,
/*240*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,
/*244*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,
/*248*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp  ,
/*252*/	mf_unimp  , mf_unimp  , mf_unimp  , mf_unimp
};

fcmd_t(loadcommand_minifunge) {
        long i, *one;

        for (i = 0; i < 256; i++) {
                one = malloc(sizeof(*one)*2);
                *one = 1;
                ip->commands[i] = one;
		ip->commands[i][1] = minicommands[i];
        }
}

fcmd_t(unloadcommand_minifunge) {
        int i;

        for (i = 0; i < 256; i++) {
                free(ip->commands[i]);
        }
}

fcmd_t(mf_execute) {
	long i, x, y;
	struct ipoint *nip;

	if (ins < 'A' || ins > 'Z') {
		fprintf(stderr, "** Shit. Encountered instruction %d in mf_execute. **\n", ins);
		return;
	}

	for (i = *(long*)minifinger; i > 0; i--) {
		if (*(long*)minifinger[i] == (long)ip && minifinger[i][ins-'A'+2]) {
			nip = malloc(sizeof(*nip));
			nip->stack = ip->stack;
			nip->ox = nip->oy = nip->oz = 0;
			nip->x = -1; nip->y = 0; nip->z = 0;
			nip->dx = 1; nip->dy = 0; nip->dz = 0;
			cip = ip;
			cs = minifinger[i][ins-'A'+2];
			i = 0;
			loadcommand_minifunge(nip, &i, 0);
			while (i == 0) {
				mfsp_inc_ip(nip);
				ins = mfsp_get_cell(nip->x, nip->y);
/*
				printf("\E[2J");
				for (y = 0; y < MINI_MAX_Y; y++) {
					printf("\E[%ld;1H", y+1);
					for (x = 0; x < MINI_MAX_X; x++) {
						if (x == nip->x && y == nip->y) printf("\E[1m");
						else printf("\E[0m");
						printf("%c", mfsp_get_cell(x, y));
					}
					printf("\n");
				}
				for (y = 1; y <= *(long*)ip->stack[*(long*)ip->stack]; y++)
					printf("%d ", ip->stack[*(long*)ip->stack][y]);
				printf("\n%d %d moving %d %d '%c' (%d)\n", nip->x, nip->y, nip->dx, nip->dy, ins, ins);
*/
				do_cmd(nip, &i, ins);

/*				fflush(stdout);
				getchar();*/
			}
			unloadcommand_minifunge(nip, &i, 0);
			free(nip);
			return;
		}
	}

	for (i = *(long*)ip->commands[ins]; i > 0; i--) {
		if (ip->commands[ins][i] != mf_execute) {
			ip->commands[ins][i](cmd_t_args);
			return;
		}
	}

	default_commands[ins](cmd_t_args);
}

fcmd_t(loadcommand_fungelib) {
	unsigned char buffer[1024];
	long x = 0, y = 0, i, l;
	FILE *f;

	strncpy(buffer, FUNGELIBDIR"/XXXX.fl", 1024);
	buffer[l = strlen(FUNGELIBDIR)+1] = ins>>24;
	buffer[++l] = (ins>>16) & 0xff;
	buffer[++l] = (ins>>8) & 0xff;
	buffer[++l] = ins & 0xff;

	if (!(f = fopen(buffer, "r"))) {
		memmove(&buffer[0], &buffer[l-3], 9);
		if (!(f = fopen(buffer, "r"))) {
			fprintf(stderr, "No builtin fingerprint, and can't find fungelib dropin (no lib)\n");
			std_reflect(cmd_t_args);
			return;
		}
	}

	fst_push(&ip->stack, 0, ins);
	fst_push(&ip->stack, 0, 1);

	minifinger = realloc(minifinger, sizeof(*minifinger)*(++(*(long*)minifinger) + 1));
	minifinger[*(long*)minifinger] = malloc(sizeof(*minifinger[*(long*)minifinger])*28);
	*(long*)minifinger[*(long*)minifinger] = (long)ip;
	*(long*)&minifinger[*(long*)minifinger][1] = ins;
	for (ins = 2; ins <= 27; ins++) {
		minifinger[*(long*)minifinger][ins] = NULL;
		override_command(ip, ins+'A'-2, mf_execute);
	}

	ins = 0;
	for (;;) {
		if (fgets(buffer, 1024, f)) {
			l = strlen(buffer);
			for (i = 0; i < l; i++) {
				switch(buffer[i]) {
					case '=':
						ins = buffer[i+1];
						x = y = 0;
						if (ins < 'A' || ins > 'Z') ins = 0;
						else {
							ins = ins-'A'+2;
							if (minifinger[*(long*)minifinger][ins])
								free(minifinger[*(long*)minifinger][ins]);
							minifinger[*(long*)minifinger][ins] = malloc(sizeof(*minifinger[*(long*)minifinger][ins]));
							cs = minifinger[*(long*)minifinger][ins];
							mfsp_space_init();
						}
						i = l;
						break;
					case '\n':
						x = 0; y++;
						break;
					default:
						if (!ins) break;
						mfsp_put_cell(x++, y, buffer[i]);
				}
			}
		} else break;
	}

	fclose(f);
}

fcmd_t(unloadcommand_fungelib) {
	long i, cmd, n, z;

	for (i = *(long*)minifinger; i > 0; i--) {
		if (*(long*)minifinger[i] == (long)ip && *(long*)&minifinger[i][1] == ins) {
			for (cmd = 0; cmd < 26; cmd++) {
				for (z = -1, n = *(long*)ip->commands[cmd+'A']; n > 0; n--) {
					if (ip->commands[cmd+'A'][n] == mf_execute) z++;
					if (z == *(long*)minifinger-i) {
						if (*(long*)ip->commands[cmd+'A'] - n) memcpy(&ip->commands[cmd+'A'][n], &ip->commands[cmd+'A'][n+1], sizeof(ip->commands[cmd+'A'][n])*(*(long*)ip->commands[cmd+'A'] - n));
						ip->commands[cmd+'A'] = realloc(ip->commands[cmd+'A'], (--(*(long*)ip->commands[cmd+'A']) + 1) * sizeof(*ip->commands[cmd+'A']));
						break;
					}
				}
				if (minifinger[i][cmd+2]) free(minifinger[i][cmd+2]);
			}

			if (i < *(long*)minifinger) memcpy(&minifinger[i], &minifinger[i+1], (*(long*)minifinger - i)*sizeof(*minifinger[i]));
			minifinger = realloc(minifinger, sizeof(*minifinger)*(--(*(long*)minifinger) + 1));
			return;
		}
	}
}

void mf_init() {
	minifinger = malloc(sizeof(*minifinger));
	*(long*)minifinger = 0;
}
