#include "config.h"

#include <stdio.h>
#include "funge_ip.h"
#include "funge_stack.h"
#include "funge_space.h"
#include "funge_commands.h"

fcmd_t(di1_unimp) {
	fprintf(stderr, "*** Command not in unefunge98: '%c' (0x%08lx)\n", ins, ins);
	std_reflect(cmd_t_args);
}

fcmd_t(di2_unimp) {
	printf("%d\n", ip->stringmode);
	fprintf(stderr, "*** Command not in befunge98: '%c' (0x%08lx)\n", ins, ins);
	std_reflect(cmd_t_args);
}

fcmd_t(loadcommand_unefunge98) {
	override_command(ip, '^', di1_unimp);
	override_command(ip, 'v', di1_unimp);
	override_command(ip, '|', di1_unimp);

	override_command(ip, 'h', di1_unimp);
	override_command(ip, 'l', di1_unimp);
	override_command(ip, 'm', di1_unimp);
}

fcmd_t(loadcommand_befunge98) {
	override_command(ip, 'h', di2_unimp);
	override_command(ip, 'l', di2_unimp);
	override_command(ip, 'm', di2_unimp);
}
