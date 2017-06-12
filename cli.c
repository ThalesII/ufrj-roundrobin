#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cli.h"
#include "vector.h"
#include "events.h"

int run_command(FILE *fp)
{
	char line[256];
	char token[64];
	char rest[256];
	int num;

	if (fgets(line, 256, fp) == NULL) {
		return EOF;
	}

	rest[0] = '\0';
	num = sscanf(line, "%s %[^\n]", token, rest);
	if (num < 1) {
		fprintf(stderr, "Error: parse error\n");
		return 1;
	}
	strcpy(line, rest);

	if (strcmp(token, "quit") == 0) {
		if (strlen(line) > 0) {
			fprintf(stderr, "Error: unexpected \"%s\"\n", line);
			return 1;
		}

		return EOF;
	}

	if (strcmp(token, "load") == 0) {
		if (strlen(line) == 0) {
			fprintf(stderr, "Error: missing filename\n");
			return 1;
		}

		FILE *new_fp = fopen(line, "r");
		if (new_fp == NULL) {
			fprintf(stderr, "Error: failed to open \"%s\"\n", line);
			return 1;
		}

		while (run_command(new_fp) != EOF)
			;

		fclose(new_fp);
		return 0;
	}

	char *name;
	int priority;
	int begin;
	int duration;
	io_t *io = NULL;

	name = malloc(strlen(token) + 1);
	strcpy(name, token);

	rest[0] = '\0';
	num = sscanf(line, "%d %d %d %[^\n]", &priority, &begin, &duration, rest);
	if (num < 3) {
		free(name);
		fprintf(stderr, "Error: missing arguments\n");
		return 1;
	}
	strcpy(line, rest);

	for (;;) {
		char type;
		io_t new_io;

		rest[0] = '\0';
		num = sscanf(line, "%s %[^\n]", token, rest);
		if (num < 1) {
			break;
		}
		strcpy(line, rest);

		rest[0] = '\0';
		num = sscanf(token, "%c %[^\n]", &type, rest);
		if (num < 1) {
			break;
		}
		strcpy(token, rest);

		switch (type) {
		case 'A':
			new_io.type = IO_A;
			break;
		case 'B':
			new_io.type = IO_B;
			break;
		default:
			free(name);
			vec_free(io);
			fprintf(stderr, "Error: unknown IO type '%c'\n", type);
			return 1;
		}

		rest[0] = '\0';
		num = sscanf(token, "%d %[^\n]", &new_io.begin, rest);
		if (num < 1) {
			free(name);
			vec_free(io);
			fprintf(stderr, "Error: expected number\n");
			return 1;
		}

		if (strlen(rest) > 0) {
			free(name);
			vec_free(io);
			fprintf(stderr, "Error: unexpected \"%s\"\n", rest);
			return 1;
		}

		vec_append(&io, &new_io);
	}

	if (strlen(line) > 0) {
		free(name);
		vec_free(io);
		fprintf(stderr, "Error: unexpected \"%s\"\n", line);
		return 1;
	}

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
