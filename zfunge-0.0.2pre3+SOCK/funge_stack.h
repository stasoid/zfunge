#ifndef __FUNGE_STACK_H__
#define __FUNGE_STACK_H__

void fst_init(long ***stack);
long fst_pop(long ***stack, long stackstack);
int fst_push(long ***stack, long stackstack, long value);
long fst_peek(long ***stack, long stackstack, long num);
int fst_poke(long ***stack, long stackstack, long num, long value);
int fst_copycreatestack(long ***stack, long numvals);
int fst_copydeletestack(long ***stack, long numvals);
char *fst_popstring(long ***stack, long stackstack);
void fst_pushstring(long ***stack, long stackstack, char *string);
void fst_clear_stack(long ***stack, long stackstack);
long **fst_duplicate_stack(long ***stack);

#define fst_popvec(s,ss,x,y,z) \
{\
	x = y = z = 0;\
	if (dimensions > 2) z = fst_pop(s, ss);\
	if (dimensions > 1) y = fst_pop(s, ss);\
	x = fst_pop(s, ss);\
}

#define fst_pushvec(s,ss,x,y,z) \
{\
	fst_push(s, ss, x);\
	if (dimensions > 1) fst_push(s, ss, y);\
	if (dimensions > 2) fst_push(s, ss, z);\
}

#endif
