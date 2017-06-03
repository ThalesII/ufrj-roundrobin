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

#include <stdio.h>

size_t next_events(event_t *dest, size_t count)
{
	int next_time = INT_MAX;
	size_t num = 0;
	proc_t *p = NULL;

	if (running != -1)
		p = &procs[running];

	// Find `next_time'

	if (interrupt != -1)
		next_time = interrupt;

	for (int i = 0; i < event_count; ++i) {
		if (events[i].time < next_time)
			next_time = events[i].time;
	}

	for (int i = 0; p && i < p->count; ++i) {
		int time = curr_time + p->events[i].time - p->time;
		if (time < next_time)
			next_time = time;
	}

	assert(next_time >= curr_time);

	// Copy events

	if (interrupt == next_time && num < count) {
		dest[num++] = (event_t) { EV_INTERRUPT, running };
	}

	for (int i = 0; i < event_count; ++i) {
		if (events[i].time == next_time && num < count) {
			dest[num++] = events[i];
			event_count--;
			events[i] = events[event_count];
		}
	}

	for (int i = 0; p && i < p->count; ++i) {
		int time = curr_time + p->events[i].time - p->time;
		if (time == next_time && num < count) {
			if (p->events[i].type == EV_IO_BEGIN) {
				assert(event_count < MAX_EVENTS - 1);
				events[event_count++] = (event_t) {
					EV_IO_END,
					running,
					next_time + io_duration[p->events[i].type]
				};
			}
			dest[num++] = (event_t) {
				p->events[i].type,
				p->events[i].pid,
				time
			};
			p->count--;
			p->events[i] = p->events[p->count];
		}
	}

	curr_time = next_time;
	return num;
}

#include <stdio.h>

int main(void)
{
	int pids[10];
	io_t io[10];
	event_t evts[10];
	int run = -1;

	printf("Creating processes...\n");
	for (int i = 0; i < 10; ++i) {
		int begin = rand() % 10;
		int duration = rand() % 10 + 1;
		int count = rand() % 3;

		for (int i=0; i < count; ++i) {
			io[i] = (io_t) { IO_A
			               , rand() % duration
			               };
		}

		pids[i] = create_proc(begin, duration, io, count);
	}

	printf("Running simulation\n");
	for (;;) {
		int count = next_events(evts, 10);

		if (count == 0)
			break;

		for (int i=0; i < count; ++i) {
			event_t *evt = &evts[i];
			switch (evt->type) {
			case EV_PROCESS_BEGIN:
				printf("%d: PROCESS BEGIN (%d)\n", evt->time, evt->pid);
				if (run == -1) {
					run = evt->pid;
					set_running(evt->pid);
				}
				break;
			case EV_PROCESS_END:
				printf("%d: PROCESS END (%d)\n", evt->time, evt->pid);
				pids[evt->pid] = -1;
				run = -1;
				for (int i=0; i < 10; ++i)
					if (pids[i] >= 0)
						run = pids[i];
				set_running(run);
				break;
			case EV_IO_BEGIN:
				printf("%d: IO BEGIN (%d)\n", evt->time, evt->pid);
				pids[evt->pid] = -evts->pid;
				run = -1;
				for (int i=0; i < 10; ++i)
					if (pids[i] >= 0)
						run = pids[i];
				set_running(run);
				break;
			case EV_IO_END:
				printf("%d: IO END (%d)\n", evt->time, evt->pid);
				pids[evt->pid] = evts->pid;
				break;
			case EV_INTERRUPT:
				printf("%d: INTERRUPT (%d)", evt->time, evt->pid);
				break;
			default:
				printf("don't know");
			}
		}
	}

	printf("End\n");
	return 0;
}
