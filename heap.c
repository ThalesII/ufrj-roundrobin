#include <stdlib.h>
#include <stdio.h>
#include "vector.h"
#include "heap.h"


void insert(proc_info** vec, proc_info* val){
	vec_append(vec, val);

	//Para facilitar o acesso
	proc_info *v = *vec;

	size_t len = vec_length(v);
	int done_fixing = 0;
	int current_pos = len - 1;
	int parent_pos = (current_pos - 1)/2;
	while(!done_fixing && current_pos < len){
		if(  v[current_pos].priority < v[parent_pos].priority ||
			(   v[current_pos].priority == v[parent_pos].priority &&
				v[current_pos].time < v[parent_pos].time
			)
		  )
		{
			//Swap values
			proc_info temp = v[parent_pos];
			v[parent_pos] = v[current_pos];
			v[current_pos] = temp;
			current_pos = parent_pos;
			parent_pos = (current_pos - 1)/2;
		}
		else{
			done_fixing = 1;
		}
	}
}

proc_info pop(proc_info* vec){
	proc_info ret_val = vec[0];

	int done_fixing = 0;
	size_t len = vec_length(vec);
	vec[0] = vec[len - 1];
	vec_remove(vec, len - 1);
	int current_pos = 0;
	int child_pos_1 = current_pos * 2 + 1;
	int child_pos_2 = current_pos * 2 + 2;
	
	while(!done_fixing && current_pos < len){
		int smaller_child = (vec[child_pos_1].priority < vec[child_pos_1].priority ||
							 (
							 	vec[child_pos_1].priority == vec[child_pos_1].priority &&
							 	vec[child_pos_1].time <= vec[child_pos_1].time
							 )
							)? child_pos_1: child_pos_2;
		
		if(smaller_child >= len) break;
		if(
			vec[smaller_child].priority < vec[current_pos].priority ||
			(
				vec[smaller_child].priority == vec[current_pos].priority &&
				vec[smaller_child].time < vec[current_pos].time
			)
		  )
		{
			proc_info temp = vec[smaller_child];
			vec[smaller_child] = vec[current_pos];
			vec[current_pos] = temp;
			current_pos = smaller_child;
			child_pos_1 = current_pos * 2 + 1;
			child_pos_2 = current_pos * 2 + 2;
		}
		else{
			done_fixing = 1;
		}
	}
	return ret_val;
}

void print_heap(proc_info *vec){
	for(int i = 0; i < vec_length(vec); i++){
    	printf("[%d] PID = %d, Priority = %d, Time = %d\n", i, vec[i].pid, vec[i].priority, vec[i].time);
    }
}

//Unit test
#if 0
int main(){
    proc_info *vec = NULL;
    proc_info val0 = {0, 2, 1};
    proc_info val1 = {1, 1, 4};
    proc_info val2 = {2, 3, 3};
    proc_info val3 = {3, 1, 2};
    insert(&vec, &val0);
    insert(&vec, &val1);
    insert(&vec, &val2);
    insert(&vec, &val3);
    // for(int i = 0; i < vec_length(vec); i++){
    // 	printf("PID = %d, Priority = %d, Time = %d\n", vec[i].pid, vec[i].priority, vec[i].time);
    // }
    print_heap(vec);
    
    proc_info min = pop(vec);
   	printf("Pop min:\n\tPID = %d, Priority = %d, Time = %d\n", min.pid, min.priority, min.time);
   	// for(int i = 0; i < vec_length(vec); i++){
    // 	printf("PID = %d, Priority = %d, Time = %d\n", vec[i].pid, vec[i].priority, vec[i].time);
    // }
    print_heap(vec);
    vec_free(vec);
	return 0;
}
#endif