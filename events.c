#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include "events.h"
#include "vector.h"

typedef struct {
	int time;
	event_t *events;
	proc_info_t info;
} proc_t;

int g_io_duration[] = {
	[IO_A] = 3,
	[IO_B] = 5,
};

int      g_time      = 0;
int      g_running   = -1; // Running process pid
int      g_interrupt = -1; // Interrupt time
proc_t  *g_procs     = NULL;
event_t *g_events    = NULL;

int get_duration(io_e type)
{
	return g_io_duration[type];
}

int create_proc(char *name, int priority, int begin, int duration, io_t *io)
{
	assert(begin >= g_time);
	event_t *events = NULL;
	int pid = vec_length(g_procs);

	event_t begin_event = { EV_PROCESS_BEGIN, pid, begin };
	vec_append(&g_events, &begin_event);

	for (int i = 0; i < vec_length(io); ++i) {
		int io_duration = get_duration(io[i].type);
		event_t io_event = { EV_IO_BEGIN, pid, io[i].begin, io_duration };
		vec_append(&events, &io_event);
	}

	event_t end_event = { EV_PROCESS_END, pid, duration };
	vec_append(&events, &end_event);
	proc_t proc = { 0, events, { name, priority } };
	vec_append(&g_procs, &proc);

	return pid;
}

int get_running(void)
{
	return g_running;
}

void set_running(int pid)
{
	g_running = pid;
}

void set_interrupt(int time)
{
	g_interrupt = time;
}

event_t *next_events(void)
{
	event_t *events = NULL;
	int next_time = INT_MAX;
	proc_t *proc = NULL;

	if (g_running != -1) {
		proc = &g_procs[g_running];
	}

	// Find `next_time'

	if (g_interrupt != -1) {
		next_time = g_interrupt;
	}

	for (int i=0; i < vec_length(g_events); ++i) {
		if (g_events[i].time < next_time) {
			next_time = g_events[i].time;
		}
	}

	for (int i=0; proc && i < vec_length(proc->events); ++i) {
		int event_time = g_time + proc->events[i].time - proc->time;
		if (event_time < next_time) {
			next_time = event_time;
		}
	}

	assert(next_time >= g_time);

	// Copy events

	for (int i=0; proc && i < vec_length(proc->events);) {
		int event_time = g_time + proc->events[i].time - proc->time;

		if (event_time != next_time) {
			i++;
			continue;
		}

		if (proc->events[i].type == EV_IO_BEGIN) {
			event_t io_event = { EV_IO_END, g_running, next_time + proc->events[i].duration };
			vec_append(&g_events, &io_event);
		}

		event_t proc_event = { proc->events[i].type, proc->events[i].pid, event_time };
		vec_append(&events, &proc_event);
		vec_remove(proc->events, i);
	}

	if (g_interrupt == next_time && vec_length(events) == 0) {
		event_t int_event = { EV_INTERRUPT, g_running, next_time };
		vec_append(&events, &int_event);
		g_interrupt = -1;
	}

	for (int i=0; i < vec_length(g_events);) {
		if (g_events[i].time != next_time) {
			i++;
			continue;
		}

		vec_append(&events, &g_events[i]);
		vec_remove(g_events, i);
	}

	// If there are no events, return `EV_NONE'
	if (vec_length(events) == 0) {
		event_t none_event = { EV_NONE, -1, g_time };
		vec_append(&events, &none_event);
		next_time = g_time;
	}

	int delta_time = next_time - g_time;
	g_time += delta_time;
	if (proc) proc->time += delta_time;
	return events;
}

proc_info_t get_info(int pid)
{
	return g_procs[pid].info;
}

void init_test(void){
	io_t *io = NULL;
	create_proc("P1", 0, 0, 5, io);
	create_proc("P2", 0, 0, 4, io);
	io_t new_io = { IO_A, 2 };
	vec_append(&io, &new_io);
	create_proc("P3", 0, 1, 4, io);
}

// Unit test

#if 0
#include <stdio.h>
#include <time.h>

int main(void)
{
	int *pids = NULL;
	int run = -1;
	int **process;

	// srand(time(NULL));
	printf("%% PROC.\tBEGIN\tDUR.\tIO\n");
	process = getprocess("test.in");
	for (int i=0; i<vec_length(process); i++){
		int begin = process[i][1];
		int duration = process[i][2];
		io_t *io = NULL;

		for (int j=3; j < vec_length(process[i]); j+=2) {
			io_t new_io = { process[i][j], process[i][j+1] };
			vec_append(&io, &new_io);
		}

		int pid = create_proc(begin, duration, io, vec_length(io));
		printf("P%d\t%d\t%d\t", pid, begin, duration);
		int tmp = -1;
		vec_append(&pids, &tmp);

		char *prefix = "";
		for (int i=0; i < vec_length(io); ++i) {
			printf("%sA%d", prefix, io[i].begin);
			prefix = ", ";
		}
		printf("\n");

		vec_free(io);
	}
	printf("\n");
	vec_free(process);

	int quit = 0;
	while (!quit) {
		event_t *events = next_events();

		for (int i=0; i < vec_length(events); ++i) {
			event_t *event = &events[i];
			switch (event->type) {
			case EV_NONE:
				printf("> DONE\n");
				quit = 1;
				break;
			case EV_PROCESS_BEGIN:
				printf(": P%d BEGUN (T=%d)\n", event->pid, event->time);
				pids[event->pid] = event->pid;
				if (run == -1) {
					run = event->pid;
					set_running(event->pid);
					printf("> RUN P%d\n", event->pid);
				}
				break;
			case EV_PROCESS_END:
				printf(": P%d ENDED (T=%d)\n", event->pid, event->time);
				pids[event->pid] = -1;
				run = -1;
				for (int i=0; i < 10; ++i)
					if (pids[i] >= 0) run = pids[i];
				set_running(run);
				printf("> RUN P%d\n", run);
				break;
			case EV_IO_BEGIN:
				printf(": P%d BEGUN IO (T=%d)\n", event->pid, event->time);
				pids[event->pid] = -event->pid;
				run = -1;
				for (int i=0; i < 10; ++i)
					if (pids[i] >= 0) run = pids[i];
				set_running(run);
				printf("> RUN P%d\n", run);
				break;
			case EV_IO_END:
				printf(": P%d ENDED IO (T=%d)\n", event->pid, event->time);
				pids[event->pid] = event->pid;
				break;
			case EV_INTERRUPT:
				printf(": P%d INTERRUPTED (T=%d)\n", event->pid, event->time);
				break;
			default:
				printf("???\n");
			}
		}

		vec_free(events);
	}

	return 0;
}
#endif
