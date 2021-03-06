#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "vector.h"

typedef struct {
	size_t used;
	size_t capacity;
	long dummy;
} vector_t;

#define HEADER_SIZE ((sizeof (vector_t) + 15) & -16)
#define HEADER(vec) ((vector_t *)((char *)(vec) - HEADER_SIZE))
#define DUMMY 0x0c07fefe

static void vec_validate(vector_t *header, size_t size)
{
	assert(header->used <= header->capacity);
	assert(header->dummy == DUMMY);

	if (size != 0) {
		assert(header->used % size == 0);
		assert(header->capacity % size == 0);
	}
}

// `capacity' must be adequately large to fit vector
static void *vec_resize(void *vec_, size_t capacity)
{
	char *vec = vec_;
	vector_t *new_header = malloc(HEADER_SIZE + capacity);
	char *new_vec = (char *)new_header + HEADER_SIZE; 

	new_header->used = 0;
	new_header->capacity = capacity;
	new_header->dummy = DUMMY;

	if (vec != NULL) {
		vector_t *header = HEADER(vec);

		vec_validate(header, 0);
		assert(new_header->capacity >= header->used);

		memcpy(new_vec, vec, header->used);
		new_header->used = header->used;
	}

	vec_free(vec);
	vec_validate(new_header, 0);
	return new_vec;
}

size_t vec_length_(void *vec, size_t size)
{
	if (vec == NULL) {
		return 0;
	}

	vector_t *header = HEADER(vec);

	vec_validate(header, size);
	return header->used / size;
}

// vector must either have available space or be NULL
void vec_append_(void *vecp_, void *src, size_t size)
{
	char **vecp = (char **)vecp_;

	if (*vecp == NULL) {
		*vecp = vec_resize(*vecp, 16 * size);
	}

	char *vec = *vecp;
	vector_t *header = HEADER(vec);

	vec_validate(header, size);
	assert(header->used + size <= header->capacity);

	memcpy(vec + header->used, src, size);
	header->used += size;
	
	if (header->used == header->capacity) {
		*vecp = vec_resize(*vecp, 2 * header->capacity);
	}
}

// `idx' must be a valid index
void vec_remove_(void *vec_, size_t idx, size_t size)
{
	char *vec = vec_;
	vector_t *header = HEADER(vec);

	vec_validate(header, size);
	assert(header->used > 0);

	header->used -= size;
	memcpy(vec + idx * size, vec + header->used, size);
}

void vec_free(void *vec)
{
	if (vec == NULL) {
		return;
	}

	vector_t *header = HEADER(vec);

	vec_validate(header, 0);
	free(header);
}

// Unit test

#if 0
#include <stdio.h>

int main(void)
{
	int *vec = NULL;

	for (int i=0; i < 999; ++i) {
		vec_append(&vec, &i);
	}
	//printf("vec_length(vec) = %zu\n", vec_length_(vec, sizeof (int)));
	
	for (int i=0; i < vec_length(vec); i += 2) {
		vec_remove(vec, i);
	}
	printf("vec_length(vec) = %zu\n", vec_length_(vec, sizeof (int)));

	for (int i=0; i < vec_length(vec); ++i) {
		printf("vec[%d] = %d\n", i, vec[i]);
	}

	vec_free(vec);
	return 0;
}
#endif
