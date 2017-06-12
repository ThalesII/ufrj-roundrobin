#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector.h"
#include "events.h"

static int command_error(FILE *fp, int num, char *msg)
{
	fscanf(fp, "%*[^;]");
	fscanf(fp, ";");
	fprintf(stderr, "Error: %s\n", msg);

	if (num == EOF) {
		return EOF;
	} else {
		return 1;
	}
}

int run_command(FILE *fp)
{
	static char buffer[1024];
	int num;

	num = fscanf(fp, "%s", buffer); // May read semicolon
	if (num != 1) {
		return command_error(fp, num, "unknown command");
	}

	if (strncmp(buffer, "quit", strlen("quit")) == 0) {
		return EOF;
	}

	if (strncmp(buffer, "load", strlen("load")) == 0) {
		num = fscanf(fp, "%s", buffer); // May read semicolon
		if (num != 1) {
			return command_error(fp, num, "missing filename");
		}

		FILE *new_fp = fopen(buffer, "r");
		if (new_fp == NULL) {
			return command_error(fp, num, "failed to open file");
		}

		while (run_command(new_fp) != EOF)
			;

		fclose(new_fp);
		fscanf(fp, "%*[^;]");
		fscanf(fp, ";");
		return 0;
	}

	char *name;
	int priority;
	int begin;
	int duration;
	io_t *io = NULL;

	name = malloc(strlen(buffer) + 1);
	strcpy(name, buffer);

	num = fscanf(fp, "%d %d %d", &priority, &begin, &duration);
	if (num != 3) {
		free(name);
		return command_error(fp, num, "incorrect arguments");
	}

	while (fscanf(fp, " %[^,;],", buffer) == 1) {
		io_t new_io;

		switch (buffer[0]) {
		case 'A':
			new_io.type = IO_A;
			break;
		case 'B':
			new_io.type = IO_B;
			break;
		default:
			free(name);
			vec_free(io);
			return command_error(fp, 0, "unknown io type");
		}

		int num = sscanf(&buffer[1], "%d", &new_io.begin);
		if (num != 1) {
			free(name);
			vec_free(io);
			return command_error(fp, num, "expected number");
		}

		vec_append(&io, &new_io);
	}

	fscanf(fp, "%*[^;]"); // Should probably check rest of line
	fscanf(fp, ";");
	create_proc(name, priority, begin, duration, io);
	vec_free(io);
	return 0;
}

int main(void)
{
	do {
		printf("> ");
	} while (run_command(stdin) != -1);
		;
	return 0;
}
