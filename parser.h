#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>

void load_file(char *arq_name);
void close_files();
int parse_line(int id);

#endif