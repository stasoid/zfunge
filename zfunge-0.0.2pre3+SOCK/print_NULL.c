#include <config.h>

#include "funge_ip.h"
#include "funge_stack.h"
#include "funge_space.h"
#include "funge_commands.h"

fcmd_t(nul_null) { std_reflect(cmd_t_args); }

fcmd_t(loadcommand_NULL) {
	long i;

	for (i = 0; i < 26; i++)
		override_command(ip, i+'A', nul_null);
}

fcmd_t(unloadcommand_NULL) {
	long i;

	for (i = 0; i < 26; i++)
		delete_command(ip, i+'A', nul_null);
}
