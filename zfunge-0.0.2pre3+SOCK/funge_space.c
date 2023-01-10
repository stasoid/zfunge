#include "config.h"
#include <stdio.h>
#include <stdlib.h>

#include "funge_ip.h"
#include "funge_space.h"

long dimensions = -1;

struct hashent **spacehash[HASHLEN];
struct hashent **totalspace;

long hash(long x, long y, long z) {
	unsigned long hashed = 0xc0d1f1ed; /* y0 it's codified */

	hashed *= x|1;
	hashed ^= x;
	hashed = (hashed<<3)|(hashed>>29);

	hashed *= y|1;
	hashed ^= ~y;
	hashed = (hashed<<5)|(hashed>>27);

	hashed *= z|3;
	hashed ^= z;

	return hashed%HASHLEN;
}

void fsp_init() {
	long i, *s;

	for (i = 0; i < HASHLEN; i++) {
		s = malloc(sizeof(long));
		*s = 0;
		(long*)spacehash[i] = s;
	}

	totalspace = malloc(sizeof(*totalspace));
	*(long*)totalspace = 0;
}

long *fsp_get_cell_internal(int hashed, long x, long y, long z) {
	long cell;

	for (cell = 1; cell <= *(long*)spacehash[hashed]; cell++) {
		if (x == spacehash[hashed][cell]->x &&
		    y == spacehash[hashed][cell]->y &&
		    z == spacehash[hashed][cell]->z) {
			return &spacehash[hashed][cell]->value;
		}
	}

	return NULL;
}

void fsp_delete_cell(long hashed, long x, long y, long z) {
	long cell;

	for (cell = 1; cell <= *(long*)spacehash[hashed]; cell++) {
		if (x == spacehash[hashed][cell]->x &&
		    y == spacehash[hashed][cell]->y &&
		    z == spacehash[hashed][cell]->z) {
			if (cell != *(long*)spacehash[hashed]) {
				spacehash[hashed][cell] = spacehash[hashed][*(long*)spacehash[hashed]];
			}

			spacehash[hashed] = realloc(spacehash[hashed], sizeof(long)+sizeof(*spacehash[0])*(--(*(long*)spacehash[hashed])));
			return;
		}
	}
}

void fsp_add_cell(long x, long y, long z, long value) {
	long hashed = hash(x,y,z), *lv;
	struct hashent *newent;

	if (value == ' ') {
		fsp_delete_cell(hashed, x, y, z);
	}

	if ((lv = fsp_get_cell_internal(hashed, x, y, z))) {
		*lv = value;
		return;
	}

	newent = malloc(sizeof(struct hashent));

	newent->x = x; newent->y = y; newent->z = z; newent->value = value;

	spacehash[hashed] = realloc(spacehash[hashed], sizeof(long)+sizeof(*spacehash[0])*(++(*(long*)spacehash[hashed])));
	spacehash[hashed][*(long*)spacehash[hashed]] = newent;

	totalspace = realloc(totalspace, sizeof(*totalspace)*(++(*(long*)totalspace) + 1));
	totalspace[*(long*)totalspace] = newent;
}

long fsp_get_cell(long x, long y, long z) {
	long hashed = hash(x,y,z), cell;

	for (cell = 1; cell <= *(long*)spacehash[hashed]; cell++) {
		if (x == spacehash[hashed][cell]->x &&
		    y == spacehash[hashed][cell]->y &&
		    z == spacehash[hashed][cell]->z) {
			return spacehash[hashed][cell]->value;
		}
	}

	return ' ';
}

void fsp_cell_rect(long *sx, long *sy, long *sz,
		   long *bx, long *by, long *bz) {
	long i;


	if (*(long*)totalspace < 1) {
		*sx = *sy = *sz = 0;
		*bx = *by = *bz = 0;
		return;
	}

	*bx = *sx = totalspace[1]->x;
	*by = *sy = totalspace[1]->y;
	*bz = *sz = totalspace[1]->z;

	for (i = 2; i <= *(long*)totalspace; i++) {
		if (*bx < totalspace[i]->x) *bx = totalspace[i]->x;
		if (*sx > totalspace[i]->x) *sx = totalspace[i]->x;

		if (*by < totalspace[i]->y) *by = totalspace[i]->y;
		if (*sy > totalspace[i]->y) *sy = totalspace[i]->y;

		if (*bz < totalspace[i]->z) *bz = totalspace[i]->z;
		if (*sz > totalspace[i]->z) *sz = totalspace[i]->z;
	}
}

/* this function has WAY too many vars... */
int fsp_real_next_cell(struct ipoint *ip, long comflag) {
	long bx = ip->x, by = ip->y, bz = ip->z,
	     sx = ip->x, sy = ip->y, sz = ip->z,
	     lbx = 0, lby = 0, lbz = 0,
	     lsx = 0, lsy = 0, lsz = 0,
	     ax = ip->dx<0?-1:ip->dx>0?1:0,
	     ay = ip->dy<0?-1:ip->dy>0?1:0,
	     az = ip->dz<0?-1:ip->dz>0?1:0,
	     tx, ty, tz, i;

	for (i = 1; i <= *(long*)totalspace; i++) {
		if (!ip->dx && (totalspace[i]->x != ip->x)) continue;
		if (!ip->dy && (totalspace[i]->y != ip->y)) continue;
		if (!ip->dz && (totalspace[i]->z != ip->z)) continue;

		tx = ax*(totalspace[i]->x - ip->x);
		ty = ay*(totalspace[i]->y - ip->y);
		tz = az*(totalspace[i]->z - ip->z);

		if (ip->dx && tx % ip->dx) continue;
		if (ip->dy && tx % ip->dy) continue;
		if (ip->dz && tx % ip->dz) continue;

		if (((!ip->dx || tx < 0) && (!ip->dy || ty < 0) && (!ip->dz || tz < 0)) &&
		    (!(lbx || lby || lbz) ||
		    (tx <= lbx && ty <= lby && tz <= lbz))) {
			lbx = tx; lby = ty; lbz = tz;
			bx = totalspace[i]->x;
			by = totalspace[i]->y;
			bz = totalspace[i]->z;
		}

		if (((!ip->dx || tx > 0) && (!ip->dy || ty > 0) && (!ip->dz || tz > 0)) &&
		    (!(lsx || lsy || lsz) ||
		    (tx <= lsx && ty <= lsy && tz <= lsz))) {
			lsx = tx; lsy = ty; lsz = tz;
			sx = totalspace[i]->x;
			sy = totalspace[i]->y;
			sz = totalspace[i]->z;
		}
	}

	if (lsx || lsy || lsz) {
		if (!comflag && ip->stringmode && (lsx > (ip->dx*ax) || lsy > (ip->dy*ay) || lsz > (ip->dz*az))) {
			ip->x = sx - ip->dx;
			ip->y = sy - ip->dy;
			ip->z = sz - ip->dz;
		} else {
			ip->x = sx; ip->y = sy; ip->z = sz;
		}
	} else {
		if (!comflag && ip->stringmode) {
			ip->x = bx - ip->dx;
			ip->y = by - ip->dy;
			ip->z = bz - ip->dz;
		} else {
			ip->x = bx; ip->y = by; ip->z = bz;
		}
	}

	if (!ip->stringmode && fsp_get_cell(ip->x, ip->y, ip->z) == ';') {
		if (comflag) return 1;
		while (!fsp_real_next_cell(ip, 1));
		fsp_real_next_cell(ip, 1);
	}

	return 0;
}

inline void fsp_next_cell(struct ipoint *ip) {
	fsp_real_next_cell(ip, 0);
}

int fsp_load_binary(char *filename, long ox, long oy, long oz, long *mx, long *my, long *mz) {
	long x = 0, ch;
	FILE *f = fopen(filename, "r");

	if (!f) return 1;

	*mx = *my = *mz = 0;

	while ((ch = fgetc(f)) != EOF)
		fsp_add_cell(x++ + ox, oy, oz, ch);

	*mx = x;

	fclose(f);

	return 0;
}

int fsp_load_with_len(char *filename, long ox, long oy, long oz, long *mx, long *my, long *mz) {
	FILE *f = fopen(filename, "r");
	char buffer[256];
	int i;
	long x = 0, y = 0, z = 0;

	*mx = *my = *mz = 0;

	if (!f) return 1;

	for (;;) {
		if (fgets(buffer, 256, f)) {
			if (x > *mx) *mx = x;
			if (y > *my) *my = y;
			if (z > *mz) *mz = z;
			for (i = 0; buffer[i]; i++) {
				if (buffer[i] == '\n' && dimensions > 1) {
					y++; x = 0;
				} else if (buffer[i] == '\x0c' && dimensions > 2) {
					y = 0; x = 0; z++;
				} else {
					if (buffer[i] != ' ')
						fsp_add_cell(x + ox, y + oy, z + oz, buffer[i]);
					x++;
				}
			}
		} else break;
	}

	fclose(f);

	return 0;
}

int fsp_load(char *filename, long ox, long oy, long oz) {
	long dummy;
	return fsp_load_with_len(filename, ox, oy, oz, &dummy, &dummy, &dummy);
}

int fsp_save(char *filename, long x, long y, long z, long lx, long ly, long lz) {
	long cx, cy, cz;
	char ch;
	FILE *f = fopen(filename, "w");

	if (!f) return 1;

	for (cz = 0; cz <= lz; cz++) {
		for (cy = 0; cy <= ly; cy++) {
			for (cx = 0; cx <= lx; cx++) {
				ch = fsp_get_cell(x+cx, y+cy, z+cz);
				fprintf(f, "%c", ch);
			}
			if (dimensions > 1) fprintf(f, "\n");
		}
		if (dimensions > 2) fprintf(f, "%c", '\x0c');
	}

	fclose(f);

	return 0;
}

/*
void clearscr() {
	printf("\33[1;1H\33[2J");
}

void gotoxy(x, y) {
	printf("\33[%d;%dH", y, x);
}

int main() {
	long x = 0, y = 0, z = 0, i;

	fsp_init();

	if (fsp_load("sig.f", x, y, z)) {
		printf("doh\n");
		exit(-1);
	}

	clearscr();
	for (i = 0; i < 80; i++) {
		printf("%d", i/10);
	} fflush(stdout);

	for (i = 0; i < 80; i++) {
		printf("%d", i%10);
	} fflush(stdout);

	for (i = 1; i <= *(long*)totalspace; i++) {
		gotoxy(totalspace[i]->x+1, totalspace[i]->y+3);
		printf("%c", totalspace[i]->value); fflush(stdout);
	}

	printf("\n");
}
*/
