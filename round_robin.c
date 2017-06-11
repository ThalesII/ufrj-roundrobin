// #include <stdlib.h>
#include <stdio.h>
#include "events.h"
#include "vector.h"
#include "round_robin.h"
#include "deque.c"

int interrupt_interval = 3;
int remaining_procs = 3;
deque_t proc_queue;

void set_interrupt_interval(int interval){
	interrupt_interval = interval;
}

void set_remaining_procs(int num_processes){
	remaining_procs = num_processes;
}

void start(void){
	int inf_loop_avoider = 0;
	int quit = 0;
	event_t *events = NULL;
	int events_count = 0;

	int proc_queue_count = 0;

	while(!quit){
		inf_loop_avoider++;
		events = next_events();
		events_count = vec_length(events);
		printf("Received %d events at T = %d\n", events_count, g_time);
		for(int i = 0; i < events_count; ++i){
			switch(events[i].type){
				case EV_NONE:
					printf("\tType: EV_NONE\n");
					quit = 1;
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
		if(proc_queue_count > 0 && g_running == -1){
			int next_proc = -1;
			deq_popleft(&proc_queue, &next_proc);
			proc_queue_count--;
			printf("Next Interrupt: P%d @ T = %d\n", next_proc, g_time + interrupt_interval);
			set_running(next_proc);
			set_interrupt(g_time + interrupt_interval);
		}
		printf("QUEUE SIZE: %d\n", proc_queue_count);
		if(remaining_procs == 0){
			set_interrupt(-1);
		}
		if(inf_loop_avoider > 10){
			printf("ABORT\n");
			exit(-1);
		}
	}
}

int main(){
	init_test();
	printf("Created 3 processes\n");
	start();
	return 0;
}