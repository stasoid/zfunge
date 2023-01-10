#include "config.h"

#include "funge_ip.h"
#include "funge_stack.h"
#include "funge_space.h"
#include "funge_commands.h"

fcmd_t(sub_call) {
	long i, x, y, z;
	long **tmpstack;

	i = fst_pop(&ip->stack, 0);
	fst_popvec(&ip->stack, 0, x, y, z);

	fst_init(&tmpstack);
	for (; i > 0; i--) fst_push(&tmpstack, 0, fst_pop(&ip->stack, 0));
	fst_pushvec(&ip->stack, 0, ip->x, ip->y, ip->z);
	fst_pushvec(&ip->stack, 0, ip->dx, ip->dy, ip->dz);
	while (*tmpstack[*(long*)tmpstack])
		fst_push(&ip->stack, 0, fst_pop(&tmpstack, 0));
	fst_delete_stackstack(&tmpstack);

	ip->x = x-1; ip->y = y; ip->z = z;
	ip->dx = 1; ip->dy = 0; ip->dz = 0;
}

fcmd_t(sub_ret) {
	long i, **tmpstack;

	i = fst_pop(&ip->stack, 0);
	fst_init(&tmpstack);
	for (; i > 0; i--) fst_push(&tmpstack, 0, fst_pop(&ip->stack, 0));
	fst_popvec(&ip->stack, 0, ip->dx, ip->dy, ip->dz);
	fst_popvec(&ip->stack, 0, ip->x, ip->y, ip->z);
	while (*tmpstack[*(long*)tmpstack])
		fst_push(&ip->stack, 0, fst_pop(&tmpstack, 0));
	fst_delete_stackstack(&tmpstack);
}

fcmd_t(sub_jump) {
	long x, y, z;

	fst_popvec(&ip->stack, 0, x, y, z);

	ip->x = x-1; ip->y = y; ip->z = z;
	ip->dx = 1; ip->dy = 0; ip->dz = 0;
}

fcmd_t(loadcommand_SUBR) {
	override_command(ip, 'C', sub_call);
	override_command(ip, 'J', sub_jump);
	override_command(ip, 'R', sub_ret);
}

fcmd_t(unloadcommand_SUBR) {
	delete_command(ip, 'C', sub_call);
	delete_command(ip, 'J', sub_jump);
	delete_command(ip, 'R', sub_ret);
}
