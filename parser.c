#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector.h"
#include "events.h"

int *getprocess(FILE **fp, char **line, size_t *len)
{
    char *token;
    const char s[1] = " ";

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
    }

    if(vec == NULL){
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

#if 1
int main(void)
{
    int pid;
    FILE *fp;
    char * line = NULL;
    size_t len = 0;
    

    fp = fopen("test.in", "r");
    if (fp == NULL)
            exit(EXIT_FAILURE);
    
    do
    {
        pid = getprocess(&fp, &line, &len);
        printf("%d\n", pid);
    }while(pid != -1);

    fclose(fp);


	return 0;
}
#endif
