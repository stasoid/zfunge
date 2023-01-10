#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "funge_ip.h"
#include "funge_stack.h"
#include "funge_space.h"
#include "funge_commands.h"

extern char **environ;

void do_help(char *prgname) {
fprintf(stderr,
"\
zfunge "VERSION" by Zinx Verituse\n\
%s [-(1|2|3)[ht][d delay]] [--befunged93] [--] file [args]\n\
", prgname);
}

int main(int argc, char *argv[]) {
	char numbuff[20];
	long i = 0, ins = 0;
	int longop, ch, befunged93 = -1;
	char *fname = NULL;
	long cx, cy, x, y, z, lx, ly, lz;
	long lastip, traceon = 0, col, tracedelay = 100000;

	struct option longopts[] = {
		{ "help", 0, NULL, 'h' },
		{ "trace", 0, NULL, 't' },
		{ "delay", 1, NULL, 'd' },
		{ "befunged93", 0, &longop, 1 },
		{ "dimensions", 1, &longop, 2 },
		{ "funge98", 0, &longop, 3 },
		/**/ { 0, 0, 0, 0 } /**/
	};

	srand(time(NULL)+getpid());

	dimensions = 2;

	while ((ch = getopt_long(argc, argv, "123htd:", longopts, NULL)) != EOF) {
		switch (ch) {
			case 0:
				if (longop == 1) {
					if (befunged93 < 0) befunged93 = 1;
					else { do_help(argv[0]); exit(-1); }
				} else if (longop == 2) {
					ins = atol(optarg);
					if (ins >=1 && ins <= 3)
						dimensions = ins;
					else { do_help(argv[0]); exit(-1); }
				} else if (longop == 3) {
					if (befunged93 < 0) befunged93 = 0;
					else { do_help(argv[0]); exit(-1); }
				}
				break;
			case '1': dimensions = 1; break;
			case '2': dimensions = 2; break;
			case '3': dimensions = 3; break;

			case ':':
			case '?':
			case 'h': do_help(argv[0]); exit(0);

			case 't': traceon = 1; break;

			case 'd': tracedelay = atol(optarg); break;
		}
	}

	if (optind >= argc) { do_help(argv[0]); exit(-1); }

	fname = argv[optind++];

	if (befunged93 < 0) {
		if (!strcmp(&fname[strlen(fname)-3], ".bf")) {
			befunged93 = 1;
		} else {
			befunged93 = 0;
		}
	}

	fsp_init();
	fip_init();
	mf_init();

	if (*environ) {
		sysinfo_enviro = strdup(*environ);
		sysinfo_enviro_len = strlen(sysinfo_enviro);
		for (i = 1; environ[i]; i++) {
			sysinfo_enviro = realloc(sysinfo_enviro, sysinfo_enviro_len+strlen(environ[i])+3);
			strcpy(sysinfo_enviro+sysinfo_enviro_len, environ[i]);
			sysinfo_enviro_len += strlen(environ[i])+1;
		}
		sysinfo_enviro[sysinfo_enviro_len++] = 0;
	}

	if (optind < argc) {
		sysinfo_args = malloc(strlen(argv[optind])+3);
		strcpy(sysinfo_args, argv[optind]);
		sysinfo_args_len = strlen(sysinfo_args)+1;
		for (i = optind+1; i < argc; i++) {
			if (strlen(argv[optind])) {
				sysinfo_args = realloc(sysinfo_args, sysinfo_args_len+strlen(argv[i])+3);
				strcpy(sysinfo_args+sysinfo_args_len, argv[i]);
				sysinfo_args_len += strlen(argv[i])+1;
			} else {
				sysinfo_args = realloc(sysinfo_args, sysinfo_args_len+4);
				strcpy(sysinfo_args+sysinfo_args_len, "-");
				sysinfo_args_len += 2;
			}
		}
		sysinfo_args[sysinfo_args_len++] = 0;
		sysinfo_args[sysinfo_args_len++] = 0;
	}

	loadcommand_standard(ips[1], &i, ins);

	if (befunged93) {
		if (dimensions != 2) {
			fprintf(stderr, "Setting to two dimensions for befunged93\n");
			dimensions = 2;
		}

		loadcommand_befunged93(ips[1], &i, ins);
	} else {
		if (dimensions <= 1) loadcommand_unefunge98(ips[1], &i, ins);
		else if (dimensions <= 2) loadcommand_befunge98(ips[1], &i, ins);
	}

	if (traceon && dimensions > 2) {
		fprintf(stderr, "Trace only in one or two dimensions.\n");
		traceon = 0;
	}

	if (fsp_load(fname, ips[1]->ox, ips[1]->oy, ips[1]->oz)) {
		fprintf(stderr, "Error loading %s (%s)\n", fname, strerror(errno));
		exit(-1);
	}

	if (traceon) printf("\E[2J\E[20;1H\E[s");

	for (; *(long*)ips;) {
		if (traceon) {
			fsp_cell_rect(&x, &y, &z, &lx, &ly, &lz);

			if (ly >= 22) ly = 18;
			if (lx >= 80) lx = 79;

			printf("\E[0m\E[1;1H");

			for (cy = 0; cy <= ly; cy++) {
				for (cx = 0; cx <= 79; cx++) {
					printf("\E[%ld;%ldH", cy+1, cx+1);
					ins = fsp_get_cell(cx, cy, 0);
					if (ins < 32) printf("\E[32m%c", (char)ins+'A'-1);
					else printf("\E[0m%c", (char)ins);
				}
				printf("\n");
			}
		}

		if (traceon) lastip = *(long*)ips;
		for (i = 1; i <= *(long*)ips; i++) {
			fsp_inc_ip(ips[i]);
			ins = fsp_get_cell(ips[i]->x, ips[i]->y, ips[i]->z);

			if (traceon) {
				printf("\E[%ld;%ldH\E[3%ld;1m%c\E[%ld;1H\E[0m",
					ips[i]->y+1, ips[i]->x+1, (i-1)%7+1, (char)ins,
					ly + 2 + *(long*)ips);

				printf("\E[u");
				fflush(stdout);
			}

			if (!ips[i]->stringmode) {
				do_cmd(ips[i], &i, ins);
			} else {
				if (ins == '"') {
					ips[i]->stringmode = 0;
				} else {
					fst_push(&ips[i]->stack, 0, ins);
				}
			}

			if (traceon) {
				printf("\E[s\E[r\E[24;1H");

				if (i > 0 && i <= *(long*)ips) {
					ins = fsp_get_cell(ips[i]->x, ips[i]->y, ips[i]->z);
					printf("\E[%ld;%ldH\E[3%ld;1m%c\E[%ld;1H\E[0m",
						ips[i]->y+1, ips[i]->x+1, (i-1)%7+1, (char)ins,
						ly + 2 + *(long*)ips);

					printf("\E[%ld;1H", ly + i + 1);

					col = 0;
					for (cx = 0; cx < *ips[i]->stack[*(long*)ips[i]->stack]; cx++) {
						snprintf(numbuff, 20, "%ld %n", fst_peek(&ips[i]->stack, 0, cx), &lz);
						if ((col+=lz) <= 79) printf("%s", numbuff);
						else { printf("\E[%ld;79H\E[1m>\E[0m", ly + i + 1); break; }
					}
					printf("\E[K\E[u");
				}
			}

		}
		if (traceon) {
			for (i = 0; i < (lastip-*(long*)ips); i++)
				printf("\E[%ld;1H\E[K\E[u", ly + (lastip-i) + 1);
		}
		fflush(stdout);
		if (traceon) usleep(tracedelay);
	}

	exit(0);
}
