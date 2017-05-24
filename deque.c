#include <stdlib.h>
#include <string.h>
#include "deque.h"

// TBD: `shrink', `push', `pop', macros

static void resize(deque_t *deque, size_t size)
{
	void *begin = malloc(size);
	void *back = begin;
	size_t count;

	if (deque->front < deque->back) {
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

	deque->begin = begin;
	deque->end = begin + size;
	deque->front = begin;
	deque->back = back;
}

void enqueue(deque_t *deque, void *src, size_t size)
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

int dequeue(deque_t *deque, void *dest, size_t size)
{
	if (deque->front == deque->back)
		return 1;

	memcpy(dest, deque->front, size);
	deque->front += size;

	if (deque->front == deque->end)
		deque->front = deque->begin;

	return 0;
}


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
			enqueue(&deque, &i, sizeof(i));
			count++;
		} else {
			dequeue(&deque, &value, sizeof(value));
			count--;
			printf("%i\n", value);
		}
	}

	return 0;
}
#endif
