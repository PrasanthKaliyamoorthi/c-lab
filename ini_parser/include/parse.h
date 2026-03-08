#ifndef PARSE_H
#define PARSE_H

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>



void rtrim(char *s);
bool validate_conf(FILE* conf);
void generate_conf(char* filename);
#endif
