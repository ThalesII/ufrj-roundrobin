#ifndef EVENTS_H
#define EVENTS_H

#include <stddef.h>

typedef enum {
	EV_NONE,
	EV_PROCESS_BEGIN,
	EV_PROCESS_END,
	EV_IO_BEGIN,
	EV_IO_END,
	EV_INTERRUPT,
} event_e;

typedef struct {
	event_e type;
	int pid;
	int time;
	int duration;
} event_t;

typedef enum {
	IO_A,
	IO_B,
} io_e;

typedef struct {
	io_e type;
	int begin;
} io_t;

extern int io_duration[];

extern int g_time;
extern int g_running;

int create_proc(int begin,
                int duration,
                io_t *io,
                size_t count);

void set_running(int pid);

void set_interrupt(int time);

event_t *next_events(void);

void init_test(void);

#endif
