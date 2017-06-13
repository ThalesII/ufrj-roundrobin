#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "cli.h"
#include "vector.h"
#include "events.h"
#include "round_robin.h"

#define scan(str, expected, format, ...) scan_(str, expected, format " %[^\n]", __VA_ARGS__, str)

static int scan_(char *str, int expected, const char *format, ...)
{
	va_list args;
	int num;

	va_start(args, format);
	num = vsscanf(str, format, args);

	if (num != expected + 1) {
		str[0] = '\0';
	} else {
		num--;
	}

	va_end(args);
	return num;
}

int run_command(FILE *fp)
{
	char line[256];
	char token[64];
	int num;

	if (fgets(line, 256, fp) == NULL) {
		return EOF;
	}

	num = scan(line, 1, "%s", token);
	if (num != 1) {
		fprintf(stderr, "Error: parse error\n");
		return 1;
	}

	if (strcmp(token, "quit") == 0) {
		if (strlen(line) > 0) {
			fprintf(stderr, "Error: unexpected \"%s\"\n", line);
			return 1;
		}

		return EOF;
	}

	if (strcmp(token, "step") == 0) {
		if (strlen(line) > 0) {
			fprintf(stderr, "Error: unexpected \"%s\"\n", line);
			return 1;
		}
		step();
		return 0;
	}

	if (strcmp(token, "continue") == 0 || strcmp(token, "run") == 0) {
		if (strlen(line) > 0) {
			fprintf(stderr, "Error: unexpected \"%s\"\n", line);
			return 1;
		}
		run();
		return 0;
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

	num = scan(line, 3, "%d %d %d", &priority, &begin, &duration);
	if (num != 3) {
		free(name);
		fprintf(stderr, "Error: missing arguments\n");
		return 1;
	}

	for (;;) {
		char type;
		io_t new_io;

		num = scan(line, 1, "%s", token);
		if (num != 1) {
			break;
		}

		num = scan(token, 1, "%c", &type);
		if (num != 1) {
			break;
		}

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

		num = scan(token, 1, "%d", &new_io.begin);
		if (num != 1) {
			free(name);
			vec_free(io);
			fprintf(stderr, "Error: expected number\n");
			return 1;
		}

		if (strlen(token) > 0) {
			free(name);
			vec_free(io);
			fprintf(stderr, "Error: unexpected \"%s\"\n", token);
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
