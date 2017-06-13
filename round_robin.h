#ifndef ROUND_ROBIN_H
#define ROUND_ROBIN_H

#define MAX_ITERATIONS_LIMIT 1000

void set_interrupt_interval(int interval);

void run(void);

int step(void);

#endif