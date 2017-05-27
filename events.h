#ifndef EVENTS_H
#define EVENTS_H

#include <stddef.h>

typedef enum {
	EV_PROCESS_BEGIN,
	EV_PROCESS_END,
	EV_IO_BEGIN,
	EV_IO_END,
} event_e;

typedef struct {
	event_e type;
	int time;
	int pid;
} event_t;

int create_event(event_t event);

size_t next_events(event_t *dest, size_t count);

#endif
