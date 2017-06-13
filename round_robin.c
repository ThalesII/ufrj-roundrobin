#include <stdlib.h>
#include <stdio.h>
#include "events.h"
#include "vector.h"
#include "round_robin.h"
#include "heap.h"

int interrupt_interval = 3;
int remaining_procs = 3;
proc_info *proc_heap = NULL;
int proc_heap_count = 0;

void set_interrupt_interval(int interval){
	interrupt_interval = interval;
}

void set_remaining_procs(int num_processes){
	remaining_procs = num_processes;
}

int get_remaining_procs(){
	return remaining_procs;
}

int step(void){
	int final_step = 0;
	int time = 0;
	event_t *events = NULL;
	int events_count = 0;


	events = next_events();
	events_count = vec_length(events);
	time = events[0].time;
	proc_info p;
	printf("Received %d events at T = %d\n", events_count, time);
	for(int i = 0; i < events_count; ++i){
		switch(events[i].type){
			case EV_NONE:
				printf("\tType: EV_NONE\n");
				final_step = 1;
				break;
			case EV_PROCESS_BEGIN:
				printf("  Type: EV_PROCESS_BEGIN\n");
				printf("  - PID: %d\n", events[i].pid);
				p = (proc_info){events[i].pid, get_info(events[i].pid).priority, time};
				insert(&proc_heap, &p);
				proc_heap_count++;
				break;
			case EV_PROCESS_END:
				printf("  Type: EV_PROCESS_END\n");
				printf("  - PID: %d\n", events[i].pid);
				set_running(-1);
				remaining_procs--;
				break;
			case EV_IO_BEGIN:
				printf("  Type: EV_IO_BEGIN\n");
				printf("  - PID: %d\n", events[i].pid);
				set_running(-1);
				break;
			case EV_IO_END:
				printf("  Type: EV_IO_END\n");
				printf("  - PID: %d\n", events[i].pid);
				p = (proc_info){events[i].pid, get_info(events[i].pid).priority, time};
				insert(&proc_heap, &p);
				proc_heap_count++;
				break;
			case EV_INTERRUPT:
				printf("  Type: EV_INTERRUPT\n");
				printf("  - PID: %d\n", events[i].pid);
				set_running(-1);
				p = (proc_info){events[i].pid, get_info(events[i].pid).priority, time};
				insert(&proc_heap, &p);
				proc_heap_count++;
				break;
			default:
				printf("  Something's wrong...");
		}
	}
	print_heap(proc_heap);
	if(vec_length(proc_heap) > 0 && get_running() == -1){
		proc_info next_proc = pop(proc_heap);
		proc_heap_count--;
		printf("Next Interrupt: P%d @ T = %d\n", next_proc.pid, time + interrupt_interval);
		set_running(next_proc.pid);
		set_interrupt(time + interrupt_interval);
	}
	printf("QUEUE SIZE: %d\n", proc_heap_count);
	if(remaining_procs == 0){
		set_interrupt(-1);
	}

	return final_step;
}

void run(void){
	int inf_loop_avoider = 0;
	int quit = 0;

	while(!quit){
		inf_loop_avoider++;
		quit = step();
		if(inf_loop_avoider > MAX_ITERATIONS_LIMIT){
			printf("ABORT\n");
			exit(-1);
		}
	}
}

//Unit test
#if 1
int main(){
	init_test();
	printf("Created 3 processes\n");
	run();
	return 0;
}
#endif