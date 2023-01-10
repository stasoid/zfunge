#include "config.h"

#include <stdio.h>
#include "funge_ip.h"
#include "funge_stack.h"
#include "funge_space.h"
#include "funge_commands.h"

fcmd_t(trm_clear) { printf("\E[2J\33[1;1H"); }
fcmd_t(trm_down) { printf("\E[%ldB", fst_pop(&ip->stack, 0)); }
fcmd_t(trm_goto) {
	long x = fst_pop(&ip->stack, 0);
	printf("\E[%ld;%ldH", x, fst_pop(&ip->stack, 0));
}
fcmd_t(trm_home) { printf("\E[1;1H"); }
fcmd_t(trm_clreol) { printf("\E[K"); }
fcmd_t(trm_clreos) { printf("\E[J"); }
fcmd_t(trm_up) { printf("\E[%ldA", fst_pop(&ip->stack, 0)); }

fcmd_t(loadcommand_TERM) {
	override_command(ip, 'C', trm_clear);
	override_command(ip, 'D', trm_down);
	override_command(ip, 'G', trm_goto);
	override_command(ip, 'H', trm_home);
	override_command(ip, 'L', trm_clreol);
	override_command(ip, 'S', trm_clreos);
	override_command(ip, 'U', trm_up);
}

fcmd_t(unloadcommand_TERM) {
	delete_command(ip, 'C', trm_clear);
	delete_command(ip, 'D', trm_down);
	delete_command(ip, 'G', trm_goto);
	delete_command(ip, 'H', trm_home);
	delete_command(ip, 'L', trm_clreol);
	delete_command(ip, 'S', trm_clreos);
	delete_command(ip, 'U', trm_up);
}
