#include <stdlib.h>
#include <string.h>
#include "vector.h"

typedef struct {
	size_t used;
	size_t capacity;
} vector_t;

#define HEADER_SIZE ((sizeof (vector_t) + 15) & -16)
#define HEADER(vec) ((vector_t *)((char *)vec - HEADER_SIZE))

static void *resize(void *vec_, size_t capacity)
{
	char *vec = vec_;
	char *new_vec = malloc(HEADER_SIZE + capacity);
	vector_t *new_header = HEADER(new_vec);

	new_header->used = 0;
	new_header->capacity = capacity;

	if (vec != NULL) {
		vector_t *header = HEADER(vec);
		memcpy(new_vec, vec, header->used);
		new_header->used = header->used;
	}

	return new_vec;
}

size_t length_(void *vec, size_t size)
{
	if (vec == NULL) {
		return 0;
	}

	vector_t *header = HEADER(vec);
	return header->used / size;
}

void append_(void *vecp_, void *src, size_t size)
{
	char **vecp = (char **)vecp_;

	if (*vecp == NULL) {
		*vecp = resize(*vecp, 16 * size);
	}

	char *vec = *vecp;
	vector_t *header = HEADER(vec);
	memcpy(vec + header->used, src, size);
	header->used += size;
	
	if (header->used == header->capacity) {
		*vecp = resize(*vecp, 2 * header->capacity);
	}
}

void remove_at_(void *vec_, size_t idx, size_t size)
{
	char *vec = vec_;
	vector_t *header = HEADER(vec);

	header->used -= size;
	memcpy(vec + idx * size, vec + header->used, size);
}

// Unit test

#if 0
#include <stdio.h>

int main(void)
{
	int *vec = NULL;

	for (int i=0; i < 999; ++i) {
		append(&vec, &i);
	}
	//printf("length(vec) = %zu\n", length_(vec, sizeof (int)));
	
	for (int i=0; i < length(vec); i += 2) {
		remove_at(vec, i);
	}
	printf("length(vec) = %zu\n", length_(vec, sizeof (int)));

	for (int i=0; i < length(vec); ++i) {
		printf("vec[%d] = %d\n", i, vec[i]);
	}

	return 0;
}
#endif
