#ifndef __FUNGE_SPACE_H__
#define __FUNGE_SPACE_H__

#include "funge_ip.h"

/* performance will be better with powers of two */
#define HASHLEN 2048

struct hashent {
        long x, y, z, value;
};

extern struct hashent **spacehash[HASHLEN];
extern struct hashent **totalspace;

long hash(long x, long y, long z);

/********************************************************************/

void fsp_init();

void fsp_add_cell(long x, long y, long z, long value);
long fsp_get_cell(long x, long y, long z);
inline void fsp_next_cell(struct ipoint *ip);

int fsp_load_binary(char *filename, long ox, long oy, long oz, long *mx, long *my, long *mz);
int fsp_load_with_len(char *filename, long ox, long oy, long oz, long *mx, long *my, long *mz);
int fsp_load(char *filename, long ox, long oy, long oz);
int fsp_save(char *filename, long x, long y, long z, long lx, long ly, long lz);


extern long dimensions;

#define fsp_inc_ip(ip) fsp_next_cell(ip)

#endif
