#define MAX_EVENTS 1024
#define MAX_PROCS 1024

#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include "events.h"

typedef struct {
	int time;
	event_t *events;
	size_t count;
} proc_t;

int curr_time = 0;
int running = -1;
int interrupt = -1;
proc_t procs[MAX_PROCS];
size_t proc_count = 0;
event_t events[MAX_EVENTS];
size_t event_count = 0;

int create_proc(int begin, int duration, io_t *io, size_t count)
{
	assert(proc_count < MAX_PROCS - 1);
	assert(event_count < MAX_EVENTS - 1);
	assert(begin >= curr_time);
	event_t *proc_events = malloc((count+1) * sizeof(*proc_events));
	int pid = event_count;

	events[event_count++] = (event_t) { EV_PROCESS_BEGIN, pid, begin };

	for (int i = 0; i < count; ++i) {
		proc_events[i] = (event_t) {
			EV_IO_BEGIN,
			pid,
			io[i].begin,
			io_duration[io[i].type]
		};
	}
	proc_events[count] = (event_t) { EV_PROCESS_END, pid, duration };

	procs[proc_count++] = (proc_t) { 0, proc_events, count+1 };

	return pid;
}

void set_running(int pid)
{
	running = pid;
}

void set_interrupt(int time)
{
	interrupt = time;
}

size_t next_events(event_t *dest, size_t count)
{
	int next_time = INT_MAX;
	size_t num = 0;
	proc_t *p = NULL;

	if (running != -1) {
		p = &procs[running];
	}

	// Find `next_time'

	if (interrupt != -1) {
		next_time = interrupt;
	}

	for (int i = 0; i < event_count; ++i) {
		if (events[i].time < next_time) {
			next_time = events[i].time;
		}
	}

	for (int i = 0; p && i < p->count; ++i) {
		int time = curr_time + p->events[i].time - p->time;
		if (time < next_time) {
			next_time = time;
		}
	}

	assert(next_time >= curr_time);

	// Copy events

	if (interrupt == next_time && num < count) {
		dest[num++] = (event_t) { EV_INTERRUPT, running };
	}

	for (int i = 0; i < event_count; ++i) {
		if (events[i].time == next_time) {
			if (num < count) {
				dest[num++] = events[i];
			}
			event_count--;
			events[i] = events[event_count];
			i--; // Tretas
		}
	}

	for (int i = 0; p && i < p->count; ++i) {
		int time = curr_time + p->events[i].time - p->time;
		if (time == next_time) {
			if (p->events[i].type == EV_IO_BEGIN) {
				assert(event_count < MAX_EVENTS - 1);
				events[event_count++] = (event_t) {
					EV_IO_END,
					running,
					next_time + p->events[i].duration
				};
			}
			if (num < count) {
				dest[num++] = (event_t) { p->events[i].type, p->events[i].pid, time };
			}
			p->count--;
			p->events[i] = p->events[p->count];
			i--; // Tretas
		}
	}

	// If there are no events, return `EV_NONE'
	if (next_time == INT_MAX) {
		if (num < count) {
			dest[num++] = (event_t) { EV_NONE, -1, curr_time };
		}
		return num;
	}

	if (p) {
		p->time += next_time - curr_time;
	}

	curr_time = next_time;
	return num;
}

// Unit test

#if 0
#include <stdio.h>

int main(void)
{
	int pids[10];
	io_t io[10];
	event_t evts[10];
	int run = -1;

	printf("PROC.\tBEGIN\tDUR.\tIO\n");
	for (int i = 0; i < 10; ++i) {
		int begin = rand() % 10;
		int duration = rand() % 10 + 1;
		int count = rand() % 3;

		for (int i=0; i < count; ++i)
			io[i] = (io_t) { IO_A, rand() % duration };
		pids[i] = create_proc(begin, duration, io, count);
		printf("P%d\t%d\t%d\t", pids[i], begin, duration);
		char *prefix = "";
		for (int i=0; i < count; ++i) {
			printf("%sA%d", prefix, io[i].begin);
			prefix = ", ";
		}
		printf("\n");
	}
	printf("\n");

	int quit = 0;
	while (!quit) {
		int count = next_events(evts, 10);

		for (int i=0; i < count; ++i) {
			event_t *evt = &evts[i];
			switch (evt->type) {
			case EV_NONE:
				printf("DONE (T=%d)\n", evt->time);
				quit = 1;
				break;
			case EV_PROCESS_BEGIN:
				printf(": P%d BEGUN (T=%d)\n", evt->pid, evt->time);
				if (run == -1) {
					run = evt->pid;
					set_running(evt->pid);
					printf("> RUN P%d\n", evt->pid);
				}
				break;
			case EV_PROCESS_END:
				printf(": P%d ENDED (T=%d)\n", evt->pid, evt->time);
				pids[evt->pid] = -1;
				run = -1;
				for (int i=0; i < 10; ++i)
					if (pids[i] >= 0) run = pids[i];
				set_running(run);
				printf("> RUN P%d\n", run);
				break;
			case EV_IO_BEGIN:
				printf(": P%d BEGUN IO (T=%d)\n", evt->pid, evt->time);
				pids[evt->pid] = -evts->pid;
				run = -1;
				for (int i=0; i < 10; ++i)
					if (pids[i] >= 0) run = pids[i];
				set_running(run);
				printf("> RUN P%d\n", run);
				break;
			case EV_IO_END:
				printf(": P%d ENDED IO (T=%d)\n", evt->pid, evt->time);
				pids[evt->pid] = evts->pid;
				break;
			case EV_INTERRUPT:
				printf(": P%d INTERRUPTED (T=%d)\n", evt->pid, evt->time);
				break;
			default:
				printf("???\n");
			}
		}
	}

	return 0;
}
#endif
