#include "config.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funge_ip.h"
#include "funge_stack.h"
#include "funge_space.h"
#include "funge_commands.h"

fcmd_t(str_append) {
	unsigned char *a, *b;
	int i;

	a = fst_popstring(&ip->stack, 0);
	b = fst_popstring(&ip->stack, 0);

	fst_push(&ip->stack, 0, 0);

	for (i = strlen(b)-1; i >= 0; i--) {
		fst_push(&ip->stack, 0, b[i]);
	}

	for (i = strlen(a)-1; i >= 0; i--) {
		fst_push(&ip->stack, 0, a[i]);
	}

	free(a);
	free(b);
}

fcmd_t(str_compare) {
	unsigned char *a, *b;

	a = fst_popstring(&ip->stack, 0);
	b = fst_popstring(&ip->stack, 0);

	fst_push(&ip->stack, 0, strcmp(a, b));

	free(a);
	free(b);
}

fcmd_t(str_display) {
	unsigned char *a;

	a = fst_popstring(&ip->stack, 0);

	printf("%s", a);

	free(a);
}

fcmd_t(str_find) {
	unsigned char *a, *b, *f;
	int i;

	a = fst_popstring(&ip->stack, 0);
	b = fst_popstring(&ip->stack, 0);

	fst_push(&ip->stack, 0, 0);

	if ((f = strstr(a, b))) {
		for (i = strlen(f)-1; i >= 0; i--)
			fst_push(&ip->stack, 0, f[i]);
	}

	free(a);
	free(b);
}

fcmd_t(str_get) {
	long x = 0, y = 0, z = 0, t, i;
	unsigned char *c = malloc(1), ch = -1;
	*c = 0;

	if (dimensions > 2) z = fst_pop(&ip->stack, 0) + ip->oz;
	if (dimensions > 1) y = fst_pop(&ip->stack, 0) + ip->oy;
	x = fst_pop(&ip->stack, 0) + ip->ox;

	fst_push(&ip->stack, 0, 0);

	for (i = 0;; i++) {
		if ((ch = fsp_get_cell(x+i, y, z))) {
			c = realloc(c, t = strlen(c)+2);
			c[t-1] = 0;
			c[t-2] = ch;
		} else break;
	}

	for (i--; i >= 0; i--)
		fst_push(&ip->stack, 0, c[i]);

	free(c);
}

fcmd_t(str_input) {
	unsigned char buffer[1024], *t;
	int i;

	if (!(fgets(buffer, 1024, stdin))) {
		std_reflect(cmd_t_args); return;
	}
	buffer[1023] = 0;
	if ((t = strchr(buffer, '\n'))) *t = 0;

	fst_push(&ip->stack, 0, 0);

	for (i = strlen(buffer)-1; i >= 0; i--)
		fst_push(&ip->stack, 0, buffer[i]);
}

fcmd_t(str_left) {
	unsigned char *c;
	long i;

	i = fst_pop(&ip->stack, 0);
	c = fst_popstring(&ip->stack, 0);

	if (i < strlen(c)) c[i] = 0;

	fst_push(&ip->stack, 0, 0);

	for (i = strlen(c)-1; i >= 0; i--)
		fst_push(&ip->stack, 0, c[i]);

	free(c);
}

fcmd_t(str_right) {
	unsigned char *c, *t;
	long i;

	i = fst_pop(&ip->stack, 0);
	c = fst_popstring(&ip->stack, 0);

	if (i < strlen(c)) t = c + (strlen(c) - i);
	else t = c;

	fst_push(&ip->stack, 0, 0);

	for (i = strlen(t)-1; i >= 0; i--)
		fst_push(&ip->stack, 0, t[i]);

	free(c);
}

fcmd_t(str_middle) {
	unsigned char *c, *t;
	long l, i;

	l = fst_pop(&ip->stack, 0);
	i = fst_pop(&ip->stack, 0);

	c = fst_popstring(&ip->stack, 0);

	fst_push(&ip->stack, 0, 0);

	if (i < strlen(c)) {
		t = c + i;

		if (l < strlen(t)) t[l] = 0;

		for (i = strlen(t)-1; i >= 0; i--)
			fst_push(&ip->stack, 0, t[i]);
	}

	free(c);
}

fcmd_t(str_length) {
	long i;
	for (i = 0; fst_peek(&ip->stack, 0, i); i++);
	fst_push(&ip->stack, 0, i);
}

fcmd_t(str_put) {
	long x = 0, y = 0, z = 0, i;
	unsigned char *s;

	if (dimensions > 2) z = fst_pop(&ip->stack, 0) + ip->oz;
	if (dimensions > 1) y = fst_pop(&ip->stack, 0) + ip->oy;
	x = fst_pop(&ip->stack, 0) + ip->ox;

	s = fst_popstring(&ip->stack, 0);

	for (i = 0; s[i]; i++)
		fsp_add_cell(x + i, y, z, s[i]);
	fsp_add_cell(x + i, y, z, s[i]);

	free(s);
}

fcmd_t(str_numtostr) {
	unsigned char buffer[20];
	long i;

	i = fst_pop(&ip->stack, 0);

	snprintf(buffer, 20, "%ld", i);

	fst_push(&ip->stack, 0, 0);

	for (i = strlen(buffer)-1; i >= 0; i--)
		fst_push(&ip->stack, 0, buffer[i]);
}

fcmd_t(str_strtonum) {
	unsigned char *s;

	s = fst_popstring(&ip->stack, 0);
	fst_push(&ip->stack, 0, atol(s));

	free(s);
}

fcmd_t(loadcommand_STRN) {
	override_command(ip, 'A', str_append);
	override_command(ip, 'C', str_compare);
	override_command(ip, 'D', str_display);
	override_command(ip, 'F', str_find);
	override_command(ip, 'G', str_get);
	override_command(ip, 'I', str_input);
	override_command(ip, 'L', str_left);
	override_command(ip, 'M', str_middle);
	override_command(ip, 'N', str_length);
	override_command(ip, 'P', str_put);
	override_command(ip, 'R', str_right);
	override_command(ip, 'S', str_numtostr);
	override_command(ip, 'V', str_strtonum);
}

fcmd_t(unloadcommand_STRN) {
	delete_command(ip, 'A', str_append);
	delete_command(ip, 'C', str_compare);
	delete_command(ip, 'D', str_display);
	delete_command(ip, 'F', str_find);
	delete_command(ip, 'G', str_get);
	delete_command(ip, 'I', str_input);
	delete_command(ip, 'L', str_left);
	delete_command(ip, 'M', str_middle);
	delete_command(ip, 'N', str_length);
	delete_command(ip, 'P', str_put);
	delete_command(ip, 'R', str_right);
	delete_command(ip, 'S', str_numtostr);
	delete_command(ip, 'V', str_strtonum);
}
