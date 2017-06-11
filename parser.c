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
}

int parse_line(int id)
{
    char *token;
    const char s[1] = " ";

    FILE  **fp = &files[id];
    char  **line = &lines[id];
    size_t *len = &lens[id];

   	int *vec = NULL;

    if (getline(&(*line), &(*len), *fp) != -1) {
    	int count = 1;

        /* get first token */
    	token = strtok(*line, s);

    	/* get other tokens */
    	while (token != NULL){
    		int c;
    		if (count == 1){
    			c = token[1]-'0';
    			vec_append(&vec, &c);
    		}
    		else if(count > 3){
    			c = token[0]-'A';
    			vec_append(&vec, &c);
    			c = token[1]-'0';
    			vec_append(&vec, &c);
    		}
    		else{
    			c = token[0]-'0';
    			vec_append(&vec, &c);
    		}
    		count++;

    		token = strtok(NULL, s);
    	}
    }else{
        return -1;
    }

    int begin = vec[1];
    int duration = vec[2];
    io_t *io = NULL;
    for (int i=3; i < vec_length(vec); i+=2) {
        io_t new_io = { vec[i], vec[i+1] };
        vec_append(&io, &new_io);
    }

    return create_proc(begin, duration, io, vec_length(io));
}

// Unit test

#if 0
int main(void)
{
    int pid;
    load_file("test.in");
    load_file("test2.in");

    //Parser on first file
    puts("Pid of processes on first file:");
    do{
        pid = parse_line(0);
        printf("%d ", pid);
    }while(pid != -1);
    puts("");
    //Parser on second file
    puts("Pid of processes on second file:");
    do{
        pid = parse_line(1);
        printf("%d ", pid);
    }while(pid != -1);
    puts("");

    close_files();


	return 0;
}
#endif
