#include <stdlib.h>
#include <string.h>
#include "deque.h"

// TBD: `shrink', macros

// `size' must be adequately large to fit deque
// when `front' == `back' deque is assumed full
static void resize(deque_t *deque, size_t size)
{
	char *begin = malloc(size);
	char *back = begin;
	size_t count;

	if (deque->begin == NULL) {
		;
	} else if (deque->front < deque->back) {
		count = deque->back - deque->front;
		memcpy(back, deque->front, count);
		back += count;
	} else {
		count = deque->end - deque->front;
		memcpy(back, deque->front, count);
		back += count;
		count = deque->back - deque->begin;
		memcpy(back, deque->begin, count);
		back += count;
	}

	free(deque->begin);
	deque->begin = begin;
	deque->end = begin + size;
	deque->front = begin;
	deque->back = back;
}

// deque must either have available space or be default
// when `front' == `back' deque is assumed empty
void push(deque_t *deque, void *src, size_t size)
{
	if (deque->begin == NULL)
		resize(deque, size); // k * size

	memcpy(deque->back, src, size);
	deque->back += size;

	if (deque->back == deque->end)
		deque->back = deque->begin;

	if (deque->front == deque->back)
		resize(deque, 2 * (deque->end - deque->begin));
}

// deque must either have available space or be default
// when `front' == `back' deque is assumed empty
void pushleft(deque_t *deque, void *src, size_t size)
{
	if (deque->begin == NULL)
		resize(deque, size);

	if (deque->front == deque->begin)
		deque->front = deque->end;

	deque->front -= size;
	memcpy(deque->front, src, size);

	if (deque->front == deque->back)
		resize(deque, 2 * (deque->end - deque->begin));
}

// returns zero on success, fails when deque is empty
// when `front' == `back' deque is assumed empty
int pop(deque_t *deque, void *dest, size_t size)
{
	if (deque->front == deque->back)
		return 1;

	if (deque->back == deque->begin)
		deque->back = deque->end;

	deque->back -= size;
	memcpy(dest, deque->back, size);

	return 0;
}

// returns zero on success, fails when deque is empty
// when `front' == `back' deque is assumed empty
int popleft(deque_t *deque, void *dest, size_t size)
{
	if (deque->front == deque->back)
		return 1;

	memcpy(dest, deque->front, size);
	deque->front += size;

	if (deque->front == deque->end)
		deque->front = deque->begin;

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
			pushleft(&deque, &count, sizeof(count));
			count++;
		} else {
			// popleft(&deque, &value, sizeof(value));
			pop(&deque, &value, sizeof(value));
			count--;
			printf("%i\n", value);
		}
	}

	return 0;
}
#endif
