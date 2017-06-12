#ifndef HEAP_H
#define HEAP_H

typedef struct {
	int pid;
	int priority;
	int time;
} proc_info;

void insert(proc_info** vec, proc_info* val);

proc_info pop(proc_info* vec);

void print_heap(proc_info *vec);

#endif