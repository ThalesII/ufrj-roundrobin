#ifndef DEQUE_H
#define DEQUE_H

#include <stddef.h>

/* Usage example:
 *     deque_t deque = {0};
 *     int val = 10;
 *     int error;
 *     deq_push(&deque, &val);
 *     error = deq_pop(&deque, &val);
 */

#define deq_push(deque, src) deq_push_(deque, src, sizeof *(src))
#define deq_pushleft(deque, src) deq_pushleft_(deque, src, sizeof *(src))
#define deq_pop(deque, dest) deq_pop_(deque, dest, sizeof *(dest))
#define deq_popleft(deque, dest) deq_popleft_(deque, dest, sizeof *(dest))

// Only for use with homogenous data (i.e. all of the same type)
typedef struct {
	char *begin;
	char *end;
	char *front;
	char *back;
} deque_t;

void deq_push_(deque_t *deque, void *src, size_t size);

void deq_pushleft_(deque_t *deque, void *src, size_t size);

int deq_pop_(deque_t *deque, void *dest, size_t size);

int deq_popleft_(deque_t *deque, void *dest, size_t size);

#endif
