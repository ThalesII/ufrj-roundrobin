// #include <stdlib.h>
#include <stdio.h>
#include "events.h"
#include "vector.h"
#include "round_robin.h"
#include "deque.c"

int interrupt_interval = 3;
int remaining_procs = 3;
deque_t proc_queue;
int proc_queue_count = 0;

void set_interrupt_interval(int interval){
	interrupt_interval = interval;
}

void set_remaining_procs(int num_processes){
	remaining_procs = num_processes;
}

int step(void){
	int final_step = 0;
	int time = 0;
	event_t *events = NULL;
	int events_count = 0;


	events = next_events();
	events_count = vec_length(events);
	time = events[0].time;
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
				deq_push(&proc_queue, &events[i].pid);
				proc_queue_count++;
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
				deq_push(&proc_queue, &events[i].pid);
				proc_queue_count++;
				break;
			case EV_INTERRUPT:
				printf("  Type: EV_INTERRUPT\n");
				printf("  - PID: %d\n", events[i].pid);
				set_running(-1);
				deq_push(&proc_queue, &events[i].pid);
				proc_queue_count++;
				break;
			default:
				printf("  Something's wrong...");
		}
	}
	if(proc_queue_count > 0 && get_running() == -1){
		int next_proc = -1;
		deq_popleft(&proc_queue, &next_proc);
		proc_queue_count--;
		printf("Next Interrupt: P%d @ T = %d\n", next_proc, time + interrupt_interval);
		set_running(next_proc);
		set_interrupt(time + interrupt_interval);
	}
	printf("QUEUE SIZE: %d\n", proc_queue_count);
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
		if(inf_loop_avoider > 10){
			printf("ABORT\n");
			exit(-1);
		}
	}
}

#if 1
int main(){
	init_test();
	printf("Created 3 processes\n");
	run();
	return 0;
}
#endif