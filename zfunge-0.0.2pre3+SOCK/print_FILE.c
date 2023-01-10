#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include "funge_ip.h"
#include "funge_stack.h"
#include "funge_space.h"
#include "funge_commands.h"

struct fileent {
	long id, x, y, z;
	FILE *handle;
} **files = NULL;

long globid = 0;

long file_findid(long id) {
	int i;
	for (i = 1; i <= (*(long*)files); i++)
		if (files[i]->id == id) return i;
	return 0;
}

fcmd_t(fil_close) {
	long i = fst_pop(&ip->stack, 0);

	if ((i = file_findid(i)) < 1) { std_reflect(cmd_t_args); return; }

	fclose(files[i]->handle);
	free(files[i]);

        if (i < (*(long*)files))
                files[i] = files[*(long*)files];
        files = realloc(files, sizeof(*files)*(--(*(long*)files) + 1));
}

fcmd_t(fil_fgets) {
	long i = fst_peek(&ip->stack, 0, 0);
	char buffer[1024];

	if ((i = file_findid(i)) < 1) { std_reflect(cmd_t_args); return; }
	if (!(fgets(buffer, 1024, files[i]->handle))) { std_reflect(cmd_t_args); return; }

	fst_pushstring(&ip->stack, 0, buffer);
}

fcmd_t(fil_ftell) {
	long i = fst_peek(&ip->stack, 0, 0);

	if ((i = file_findid(i)) < 1) { std_reflect(cmd_t_args); return; }
	fst_push(&ip->stack, 0, ftell(files[i]->handle));
}

fcmd_t(fil_fopen) {
	char *s, *smode[] = { "r", "w", "a", "r+", "w+", "a+" };
	long mode, x, y, z, i = 0;
	struct fileent *f = malloc(sizeof(*f));

	s = fst_popstring(&ip->stack, 0);
	mode = fst_pop(&ip->stack, 0);
	fst_popvec(&ip->stack, 0, x, y, z);

	if (mode < 0 || mode > 5) { free(s); std_reflect(cmd_t_args); return; }
	if (!(f->handle = fopen(s, smode[mode]))) { free(s); std_reflect(cmd_t_args); return; }

	free(s);

	f->id = ++globid;
	f->x = x; f->y = y; f->z = z;

        files = realloc(files, sizeof(*files)*(++(*(long*)files) + 1));
        files[*(long*)files] = f;

	fst_push(&ip->stack, 0, f->id);
}

fcmd_t(fil_fputs) {
	char *s;
	long i;

	s = fst_popstring(&ip->stack, 0);
	i = fst_peek(&ip->stack, 0, 0);

	if ((i = file_findid(i)) < 1) { free(s); std_reflect(cmd_t_args); return; }
	if (fputs(s, files[i]->handle) < 0) { free(s); std_reflect(cmd_t_args); return; }
	free(s);
}

fcmd_t(fil_fread) {
	long i, bytes, x;
	char *buffer;

	bytes = fst_pop(&ip->stack, 0);
	i = fst_peek(&ip->stack, 0, 0);

	if ((i = file_findid(i)) < 1) { std_reflect(cmd_t_args); return; }

	if (!(buffer = malloc(bytes))) { std_reflect(cmd_t_args); return; }

	if (fread(buffer, bytes, 1, files[i]->handle) < 0) {
		free(buffer); std_reflect(cmd_t_args); return;
	}

	for (x = 0; x < bytes; x++) {
		fsp_add_cell(files[i]->x+x, files[i]->y, files[i]->z, buffer[x]);
	}

	free(buffer);
}

fcmd_t(fil_fseek) {
	long i, bytes, origin;
	fpos_t poss[] = { SEEK_SET, SEEK_CUR, SEEK_END };

	bytes = fst_pop(&ip->stack, 0);
	origin = fst_pop(&ip->stack, 0);
	i = fst_peek(&ip->stack, 0, 0);

	if (origin < 0 || origin > 2) { std_reflect(cmd_t_args); return; }
	if ((i = file_findid(i)) < 0) { std_reflect(cmd_t_args); return; }

	if (fseek(files[i]->handle, bytes, poss[origin])) { std_reflect(cmd_t_args); return; }
}

fcmd_t(fil_fwrite) {
	long i, bytes, x;
	char *buffer;

	bytes = fst_pop(&ip->stack, 0);
	i = fst_peek(&ip->stack, 0, 0);
	if ((i = file_findid(i)) < 0) { std_reflect(cmd_t_args); return; }

	if (!(buffer = malloc(bytes))) { std_reflect(cmd_t_args); return; }

	for (x = 0; x < bytes; x++)
		buffer[i] = fsp_get_cell(files[i]->x+x, files[i]->y, files[i]->z);

	if (fwrite(buffer, bytes, 1, files[i]->handle) < 1) { free(buffer); std_reflect(cmd_t_args); return; }
	free(buffer);
}

fcmd_t(loadcommand_FILE) {
	if (!files) {
		files = malloc(sizeof(*files));
		*(long*)files = 0;
	}
	override_command(ip, 'C', fil_close);
	override_command(ip, 'G', fil_fgets);
	override_command(ip, 'L', fil_ftell);
	override_command(ip, 'O', fil_fopen);
	override_command(ip, 'P', fil_fputs);
	override_command(ip, 'R', fil_fread);
	override_command(ip, 'S', fil_fseek);
	override_command(ip, 'W', fil_fwrite);
}

fcmd_t(unloadcommand_FILE) {
	delete_command(ip, 'C', fil_close);
	delete_command(ip, 'G', fil_fgets);
	delete_command(ip, 'L', fil_ftell);
	delete_command(ip, 'O', fil_fopen);
	delete_command(ip, 'P', fil_fputs);
	delete_command(ip, 'R', fil_fread);
	delete_command(ip, 'S', fil_fseek);
	delete_command(ip, 'W', fil_fwrite);
}
