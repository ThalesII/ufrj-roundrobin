#ifndef ROUND_ROBIN_H
#define ROUND_ROBIN_H

#define MAX_ITERATIONS_LIMIT 1000

void set_interrupt_interval(int interval);

void set_remaining_procs(int num_processes);

int get_remaining_procs();

void run(void);

int step(void);

#endif