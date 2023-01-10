#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funge_ip.h"
#include "funge_stack.h"
#include "funge_space.h"
#include "funge_commands.h"

fcmd_t(bas_binary) {
	long i = fst_pop(&ip->stack, 0), j = 0;

	for (j = 0; i >> j; j++);
	for (j = j?j-1:j; j >= 0; j--) printf("%d", (i>>j)&1);

	printf(" ");
}

fcmd_t(bas_hex) {
	long i = fst_pop(&ip->stack, 0);
	printf("%lx ", i);
}

fcmd_t(bas_input) {
	long i = fst_pop(&ip->stack, 0);
	char buffer[1024];

	fgets(buffer, 1024, stdin);

	fst_push(&ip->stack, 0, strtol(buffer, NULL, i));
}

fcmd_t(bas_output) {
	char basechars[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	long base = fst_pop(&ip->stack, 0), i, j = 0, n = 0;

	i = fst_pop(&ip->stack, 0);

	if (base <= 0 || base > 36) base = 10;

	do { j *= base; j += i%base; i /= base; n++; } while (i);
	for (n = n?n-1:n; n >= 0; n--, j /= base)
		printf("%c", basechars[j%base]);

	printf(" ");
}

fcmd_t(bas_octal) {
	long i = fst_pop(&ip->stack, 0);
	printf("%lo ", i);
}

fcmd_t(loadcommand_BASE) {
	override_command(ip, 'B', bas_binary);
	override_command(ip, 'H', bas_hex);
	override_command(ip, 'I', bas_input);
	override_command(ip, 'N', bas_output);
	override_command(ip, 'O', bas_octal);
}

fcmd_t(unloadcommand_BASE) {
	delete_command(ip, 'B', bas_binary);
	delete_command(ip, 'H', bas_hex);
	delete_command(ip, 'I', bas_input);
	delete_command(ip, 'N', bas_output);
	delete_command(ip, 'O', bas_octal);
}
