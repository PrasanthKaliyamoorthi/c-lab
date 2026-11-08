#include <stdio.h>
#include "../include/debug.h"

int d_print(char *str, debug_colors color) {
    printf("%s%s%s", colors[color], str, ANSI_COLOR_RESET);
    return 0;
}
