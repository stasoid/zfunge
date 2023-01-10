#include "config.h"


#include <stdio.h>
#include "funge_ip.h"
#include "funge_stack.h"
#include "funge_space.h"
#include "funge_commands.h"

fcmd_t(b93_unimp) {
	/*
	fprintf(stderr, "Not a befunge93 command: '%c' (%08lx)\n", (char)ins, ins);
	*/
	return;
}

fcmd_t(b93_ideci) {
        long i;
        scanf("%ld", &i);
        fst_push(&ip->stack, 0, i);
}

fcmd_t(b93_ichar) {
	long i;
	fst_push(&ip->stack, 0, fgetc(stdin));
}

fcmd_t(loadcommand_befunged93) {
	int i;
	override_command(ip, '\'', b93_unimp);
	override_command(ip, '(', b93_unimp);
	override_command(ip, ')', b93_unimp);
	override_command(ip, ';', b93_unimp);
	override_command(ip, '=', b93_unimp);
	override_command(ip, '[', b93_unimp);
	override_command(ip, ']', b93_unimp);
	override_command(ip, 'a', b93_unimp);
	override_command(ip, 'b', b93_unimp);
	override_command(ip, 'c', b93_unimp);
	override_command(ip, 'd', b93_unimp);
	override_command(ip, 'e', b93_unimp);
	override_command(ip, 'f', b93_unimp);
	override_command(ip, 'h', b93_unimp);
	override_command(ip, 'i', b93_unimp);
	override_command(ip, 'j', b93_unimp);
	override_command(ip, 'k', b93_unimp);
	override_command(ip, 'l', b93_unimp);
	override_command(ip, 'm', b93_unimp);
	override_command(ip, 'n', b93_unimp);
	override_command(ip, 'o', b93_unimp);
	override_command(ip, 'q', b93_unimp);
	override_command(ip, 'r', b93_unimp);
	override_command(ip, 's', b93_unimp);
	override_command(ip, 't', b93_unimp);
	override_command(ip, 'u', b93_unimp);
	override_command(ip, 'w', b93_unimp);
	override_command(ip, 'x', b93_unimp);
	override_command(ip, 'y', b93_unimp);
	override_command(ip, 'z', b93_unimp);
	override_command(ip, '{', b93_unimp);
	override_command(ip, '}', b93_unimp);

	/* These do NOT reflect in befunged 93 */
	override_command(ip, '&', b93_ideci);
	override_command(ip, '~', b93_ichar);

	for (i = 0; i < 256; i++) {
		if (cmd_func(ip, i) == std_unimp)
			override_command(ip, i, b93_unimp);
	}
}

fcmd_t(unloadcommand_befunged93) {
	/* Once it's done, it's done. */
	return;
}
