#ifndef ROUND_ROBIN_H
#define ROUND_ROBIN_H

void set_interrupt_interval(int interval);

void set_remaining_procs(int num_processes);

void run(void);

int step(void);

#endif