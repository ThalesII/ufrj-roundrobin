#include <stddef.h>

// Only for use with homogenous data (i.e. all of the same type)
typedef struct deque {
	void *begin;
	void *end;
	void *front;
	void *back;
} deque_t;

void enqueue(deque_t *deque, void *src, size_t size);

int dequeue(deque_t *deque, void *dest, size_t size);
