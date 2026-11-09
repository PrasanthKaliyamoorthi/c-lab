#include <stdio.h>
#include "../include/debug.h"

int d_print(char *str, debug_colors color) {
    fprintf(stderr, "%s%s%s", colors[color], str, ANSI_COLOR_RESET);
    return 0;
}

int print_green(char * str) {
    return d_print(str, GREEN);
}

int print_red(char * str) {
    return d_print(str, RED);
}
