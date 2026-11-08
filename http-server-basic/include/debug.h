#include <stdio.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

typedef enum {
  RED,
  GREEN,
  YELLOW,
  BLUE,
  MAGENTA,
  CYAN,
  RESET,
  COLOR_COUNT
} debug_colors;

static const char *colors[COLOR_COUNT] = {
    [RED]     = ANSI_COLOR_RED,
    [GREEN]   = ANSI_COLOR_GREEN,
    [YELLOW]  = ANSI_COLOR_YELLOW,
    [BLUE]    = ANSI_COLOR_BLUE,
    [MAGENTA] = ANSI_COLOR_MAGENTA,
    [CYAN]    = ANSI_COLOR_CYAN,
    [RESET]   = ANSI_COLOR_RESET
};

int d_print(char * str, debug_colors color);
