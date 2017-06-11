#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "deque.h"

// TBD: `shrink'

#include <stdio.h>

static void deq_validate(deque_t *deque, size_t size)
{
	if (deque->begin == NULL) {
		return;
	}

	assert(deque->begin < deque->end);
	assert(deque->begin <= deque->front && deque->front <= deque->end);
	assert(deque->begin <= deque->back && deque->back <= deque->end);
	
	if (size != 0) {
		assert((deque->end - deque->begin) % size == 0);
		assert((deque->front - deque->begin) % size == 0);
		assert((deque->end - deque->begin) % size == 0);
	}
}

// `size' must be adequately large to fit deque
// when `front' == `back' deque is assumed full
static void deq_resize(deque_t *deque, size_t capacity)
{
	deq_validate(deque, 0);

	char *begin = malloc(capacity);
	char *back = begin;
	size_t count;

	if (deque->begin == NULL) {
		;
	} else if (deque->front < deque->back) {
		count = deque->back - deque->front;
		assert(count <= capacity);
		memcpy(back, deque->front, count);
		back += count;
	} else {
		count = deque->end - deque->front;
		assert(count <= capacity);
		memcpy(back, deque->front, count);
		back += count;

		count = deque->back - deque->begin;
		assert(back - begin + count <= capacity);
		memcpy(back, deque->begin, count);
		back += count;
	}

	free(deque->begin);
	deque->begin = begin;
	deque->end = begin + capacity;
	deque->front = begin;
	deque->back = back;
}

// deque must either have available space or be default
// when `front' == `back' deque is assumed empty
void deq_push_(deque_t *deque, void *src, size_t size)
{
	deq_validate(deque, size);

	if (deque->begin == NULL) {
		deq_resize(deque, 16 * size);
	}
	
	memcpy(deque->back, src, size);
	deque->back += size;

	if (deque->back == deque->end) {
		deque->back = deque->begin;
	}

	if (deque->front == deque->back) {
		deq_resize(deque, 2 * (deque->end - deque->begin));
	}
}

// deque must either have available space or be default
// when `front' == `back' deque is assumed empty
void deq_pushleft_(deque_t *deque, void *src, size_t size)
{
	deq_validate(deque, size);

	if (deque->begin == NULL) {
		deq_resize(deque, 16 * size);
	}

	if (deque->front == deque->begin) {
		deque->front = deque->end;
	}

	deque->front -= size;
	memcpy(deque->front, src, size);

	if (deque->front == deque->back) {
		deq_resize(deque, 2 * (deque->end - deque->begin));
	}
}

// returns zero on success, fails when deque is empty
// when `front' == `back' deque is assumed empty
int deq_pop_(deque_t *deque, void *dest, size_t size)
{
	deq_validate(deque, size);

	if (deque->front == deque->back) {
		return 1;
	}

	if (deque->back == deque->begin) {
		deque->back = deque->end;
	}

	deque->back -= size;
	memcpy(dest, deque->back, size);

	return 0;
}

// returns zero on success, fails when deque is empty
// when `front' == `back' deque is assumed empty
int deq_popleft_(deque_t *deque, void *dest, size_t size)
{
	deq_validate(deque, size);

	if (deque->front == deque->back) {
		return 1;
	}

	memcpy(dest, deque->front, size);
	deque->front += size;

	if (deque->front == deque->end) {
		deque->front = deque->begin;
	}

	return 0;
}

/* Unit test */

#if 0
#include <stdio.h>
#include <time.h>

int main(void)
{
	deque_t deque = {0};
	int value, count = 0;

	srand(time(NULL));
	for (int i = 0; i < 9999; ++i) {
		if (count == 0 || rand() & 1) {
			deq_pushleft(&deque, &count);
			count++;
		} else {
			// deq_popleft(&deque, &value);
			deq_pop(&deque, &value);
			count--;
			printf("%i\n", value);
		}
	}

	return 0;
}
#endif
