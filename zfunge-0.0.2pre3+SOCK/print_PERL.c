#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "funge_ip.h"
#include "funge_stack.h"
#include "funge_space.h"
#include "funge_commands.h"

#ifndef PERL_BIN
#define PERL_BIN "/usr/bin/perl"
#endif

char *run_it_and_give_me_the_output(char *bin, char *args[]) {
	char buffer[1024], *ret;
	pid_t pid;
	int fds[2];
	FILE *theprogramsoutput;

	pipe(fds);

	if ((pid = fork()) < 0) {
		fprintf(stderr, "fork says: \"NO DAMNIT!\".\n");
		close(fds[0]);
		close(fds[1]);
		return NULL;
	}

	if (!pid) {
		close(0); /* heh.. no input >:) */
		close(fds[0]);
		dup2(fds[1], 1);
		dup2(fds[1], 2);
		execv(bin, args);
		printf("error.\n");
		exit(127);
	}

	close(fds[1]);

	theprogramsoutput = fdopen(fds[0], "r");
	ret = malloc(1);
	*ret = 0;

	for (;;) {
		if (fgets(buffer, 1024, theprogramsoutput)) {
			ret = realloc(ret, strlen(ret)+strlen(buffer)+1);
			strcat(ret, buffer);
		} else break;

		if (waitpid(pid, NULL, WNOHANG)) break;
	}

	waitpid(pid, NULL, 0);

	return ret;
}

fcmd_t(prl_interptype) {
	fst_push(&ip->stack, 0, 1);
}

fcmd_t(prl_evalstring) {
	char *s = fst_popstring(&ip->stack, 0), *c, *argv[4];

	c = malloc(strlen(s)+15);
	sprintf(c, "print eval(\"%s\")", s);
	free(s);

	argv[0] = "perl";
	argv[1] = "-e";
	argv[2] = c;
	argv[3] = NULL;
	s = run_it_and_give_me_the_output(PERL_BIN, argv);
	if (s) {
		fst_pushstring(&ip->stack, 0, s);
		free(s);
	} else {
		std_reflect(cmd_t_args);
	}
}

fcmd_t(prl_evalinteger) {
	char *s = fst_popstring(&ip->stack, 0), *c, *argv[4];

	c = malloc(strlen(s)+15);
	sprintf(c, "print eval(\"%s\")", s);
	free(s);

	argv[0] = "perl";
	argv[1] = "-e";
	argv[2] = c;
	argv[3] = NULL;
	s = run_it_and_give_me_the_output(PERL_BIN, argv);
	if (s) {
		fst_push(&ip->stack, 0, atol(s));
		free(s);
	} else {
		std_reflect(cmd_t_args);
	}
}

fcmd_t(loadcommand_PERL) {
	override_command(ip, 'E', prl_evalstring);
	override_command(ip, 'I', prl_evalinteger);
	override_command(ip, 'S', prl_interptype);
}

fcmd_t(unloadcommand_PERL) {
	delete_command(ip, 'E', prl_evalstring);
	delete_command(ip, 'I', prl_evalinteger);
	delete_command(ip, 'S', prl_interptype);
}
