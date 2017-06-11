#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector.h"
#include "events.h"

//Vector of file states
FILE **files = NULL;
char **lines = NULL;
size_t *lens = NULL;
int *ids = NULL;

//Open a file and append it on vector files
void load_file(char *arq_name){
    FILE *fp = fopen(arq_name, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);
    vec_append(&files, &fp);

    char * line = NULL;
    size_t len = 0;
    int id = vec_length(files) - 1;

    vec_append(&lines, &line);
    vec_append(&lens, &len);
    vec_append(&ids, &id);
}

//Close file and free all vectors
void close_files(){
    for(int i=0; i<vec_length(files); i++){
        fclose(files[i]);
    }
    vec_free(files);
    vec_free(lines);
    vec_free(lens);
    vec_free(ids);
    files = NULL;
    lines = NULL;
    lens = NULL;
    ids = NULL;
}

int parse_line(int id)
{
    char *token;
    char io_letter;
    char *name;
    const char s[1] = " ";

    FILE  **fp = &files[id];
    char  **line = &lines[id];
    size_t *len = &lens[id];

   	int *vec = NULL;
    int io_number;

    if (getline(&(*line), &(*len), *fp) != -1) {
    	int count = 1;

        /* get first token */
    	token = strtok(*line, s);

    	/* get other tokens */
    	while (token != NULL){
    		int c;
    		if (count == 1){
    			name = token;
    		}
    		else if(count > 4){
                sscanf(token, "%c%d", &io_letter, &c);
    			io_number = io_letter-'A';
    			vec_append(&vec, &io_number);
    			vec_append(&vec, &c);
    		}
    		else{
    			c = atoi(token);
    			vec_append(&vec, &c);
    		}
    		count++;

    		token = strtok(NULL, s);
    	}
    }else{
        return -1;
    }

    int priority = vec[1];
    int begin = vec[2];
    int duration = vec[3];
    io_t *io = NULL;

    for (int i=3; i < vec_length(vec); i+=2) {
        io_t new_io = { vec[i], vec[i+1] };
        vec_append(&io, &new_io);
    }

    for (int i=0; i < vec_length(vec); ++i) {
        printf("%d ", vec[i]);
    }
    puts("");

    // return create_proc(name, priority, begin, duration, io);
    return create_proc(begin, duration, io, vec_length(io));
}

// Unit test

#if 1
int main(void)
{
    int pid;
    load_file("test.in");

    //Parser on first file
    puts("Pid of processes on first file:");
    do{
        pid = parse_line(0);
    }while(pid != -1);
    puts("");
    
    close_files();
    load_file("test2.in");
    //Parser on second file
    puts("Pid of processes on second file:");
    do{
        pid = parse_line(0);
    }while(pid != -1);
    puts("");

    close_files();


	return 0;
}
#endif
