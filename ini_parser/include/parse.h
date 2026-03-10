#ifndef PARSE_H
#define PARSE_H

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

extern bool is_test_run;
#define M_EXIT(s) if (is_test_run){}else exit(s);

void rtrim(char *s);
void ltrim(char *s);
bool validate_conf(FILE* conf);
void generate_conf(char* filename);
#endif
