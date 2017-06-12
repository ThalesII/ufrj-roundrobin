#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector.h"
#include "events.h"

int run_command(FILE *fp)
{
	static char buffer[1024];

	char *name;
	int priority;
	int begin;
	int duration;
	io_t *io = NULL;

	int num = fscanf(fp, "%s %d %d %d ", buffer, &priority, &begin, &duration);
	if (num != 4) {
		return num;
	}

	name = malloc(strlen(buffer) + 1);
	strcpy(name, buffer);

	while (fscanf(fp, "%[^,;], ", buffer) == 1) {
		io_t new_io;

		switch (buffer[0]) {
		case 'A':
			new_io.type = IO_A;
			break;
		case 'B':
			new_io.type = IO_B;
		}

		int num = sscanf(&buffer[1], "%d", &new_io.begin);
		if (num != 1) {
			free(name);
			vec_free(io);
			return num;
		}

		vec_append(&io, &new_io);
	}

	fscanf(fp, "; ");
	create_proc(name, priority, begin, duration, io);
	vec_free(io);
	return 0;
}

// Unit test

#if 0
int main(void)
{
	FILE *fp = fopen("test.in", "r");
	int num = 0;

	while (run_command(fp) != -1) {
		num++;
	}

	printf("%d\n", num);
	fclose(fp);
	return 0;
}
#endif
