#ifndef DEQUE_H
#define DEQUE_H

#include <stddef.h>

// Only for use with homogenous data (i.e. all of the same type)
typedef struct {
	char *begin;
	char *end;
	char *front;
	char *back;
} deque_t;

void deq_push(deque_t *deque, void *src, size_t size);

void deq_pushleft(deque_t *deque, void *src, size_t size);

int deq_pop(deque_t *deque, void *dest, size_t size);

int deq_popleft(deque_t *deque, void *dest, size_t size);

#endif
