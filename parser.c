#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector.h"

int **getprocess(char *arq_name)
{
	FILE * fp;
    char * line = NULL;
    char *token;
    const char s[1] = " ";
    size_t len = 0;
    ssize_t read;
    int **process = NULL;


    fp = fopen(arq_name, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
        // printf("Line size %zu :\n", read);
    	int *vec = NULL;
    	int count = 1;

        /* get first token */
    	token = strtok(line, s);

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

    	vec_append(&process, &vec);
    }

    fclose(fp);

    return process;
}

// Unit test

#if 0
int main(void)
{
	int **process;

	process = getprocess("example.in");

	for(int i=0; i<vec_length(process); i++){
		for(int j=0; j<vec_length(process[i]); j++){
			printf("%d ", process[i][j]);
		}
		puts("");
	}

	return 0;
}
#endif
