#include "config.h"

#include <math.h>
#include "funge_ip.h"
#include "funge_stack.h"
#include "funge_space.h"
#include "funge_commands.h"

/* Hopefully this will be enough precision....... */
#define PI 3.141592653589793238462643383279502884197169399375105820

fcmd_t(fxp_and) {
	fst_push(&ip->stack, 0, fst_pop(&ip->stack, 0) & fst_pop(&ip->stack, 0));
}

fcmd_t(fxp_arccos) {
	double f = fst_pop(&ip->stack, 0);
	f = (f*PI)/1800000;
	f = acos(f) * 10000;
	fst_push(&ip->stack, 0, f);
}

fcmd_t(fxp_cos) {
	double f = fst_pop(&ip->stack, 0);
	f = (f*PI)/1800000;
	f = cos(f) * 10000;
	fst_push(&ip->stack, 0, f);
}

fcmd_t(fxp_random) {
	long a = fst_pop(&ip->stack, 0);
	if (a) fst_push(&ip->stack, 0, rand()%a);
	else fst_push(&ip->stack, 0, 0);
}

fcmd_t(fxp_sin) {
	double f = fst_pop(&ip->stack, 0);
	f = (f*PI)/1800000;
	f = sin(f) * 10000;
	fst_push(&ip->stack, 0, f);
}

fcmd_t(fxp_arcsin) {
	double f = fst_pop(&ip->stack, 0);
	f = (f*PI)/1800000;
	f = asin(f) * 10000;
	fst_push(&ip->stack, 0, f);
}

fcmd_t(fxp_neg) {
	fst_push(&ip->stack, 0, -fst_pop(&ip->stack, 0));
}

fcmd_t(fxp_or) {
	fst_push(&ip->stack, 0, fst_pop(&ip->stack, 0) | fst_pop(&ip->stack, 0));
}

fcmd_t(fxp_bypi) {
	double f = fst_pop(&ip->stack, 0);
	f *= PI;
	fst_push(&ip->stack, 0, f);
}

fcmd_t(fxp_sqrt) {
	fst_push(&ip->stack, 0, sqrt(fst_pop(&ip->stack, 0)));
}

fcmd_t(fxp_pow) {
	long b = fst_pop(&ip->stack, 0);
	fst_push(&ip->stack, 0, pow(fst_pop(&ip->stack, 0), b));
}

fcmd_t(fxp_sign) {
	long a = fst_pop(&ip->stack, 0);
	fst_push(&ip->stack, 0, a<0?-1:a>0?1:0);
}

fcmd_t(fxp_tan) {
	double f = fst_pop(&ip->stack, 0);
	f *= PI/1800000;
	f = tan(f) * 10000;
	fst_push(&ip->stack, 0, f);
}

fcmd_t(fxp_arctan) {
	double f = fst_pop(&ip->stack, 0);
	f *= PI/1800000;
	f = atan(f) * 10000;
	fst_push(&ip->stack, 0, f);
}

fcmd_t(fxp_abs) {
	long a = fst_pop(&ip->stack, 0);
	fst_push(&ip->stack, 0, a<0?-a:a);
}

fcmd_t(fxp_xor) {
	fst_push(&ip->stack, 0, fst_pop(&ip->stack, 0) ^ fst_pop(&ip->stack, 0));
}

fcmd_t(loadcommand_FIXP) {
	override_command(ip, 'A', fxp_and);
	override_command(ip, 'B', fxp_arccos);
	override_command(ip, 'C', fxp_cos);
	override_command(ip, 'D', fxp_random);
	override_command(ip, 'I', fxp_sin);
	override_command(ip, 'J', fxp_arcsin);
	override_command(ip, 'N', fxp_neg);
	override_command(ip, 'O', fxp_or);
	override_command(ip, 'P', fxp_bypi);
	override_command(ip, 'Q', fxp_sqrt);
	override_command(ip, 'R', fxp_pow);
	override_command(ip, 'S', fxp_sign);
	override_command(ip, 'T', fxp_tan);
	override_command(ip, 'U', fxp_arctan);
	override_command(ip, 'V', fxp_abs);
	override_command(ip, 'X', fxp_xor);
}

fcmd_t(unloadcommand_FIXP) {
	delete_command(ip, 'A', fxp_and);
	delete_command(ip, 'B', fxp_arccos);
	delete_command(ip, 'C', fxp_cos);
	delete_command(ip, 'D', fxp_random);
	delete_command(ip, 'I', fxp_sin);
	delete_command(ip, 'J', fxp_arcsin);
	delete_command(ip, 'N', fxp_neg);
	delete_command(ip, 'O', fxp_or);
	delete_command(ip, 'P', fxp_bypi);
	delete_command(ip, 'Q', fxp_sqrt);
	delete_command(ip, 'R', fxp_pow);
	delete_command(ip, 'S', fxp_sign);
	delete_command(ip, 'T', fxp_tan);
	delete_command(ip, 'U', fxp_arctan);
	delete_command(ip, 'V', fxp_abs);
	delete_command(ip, 'X', fxp_xor);
}
