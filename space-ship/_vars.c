#include"space.h"
#include <stdlib.h>

_ship ship = { .x = 0, .y =0};
int scr_x, scr_y;
int FPS = 300000;


pthread_t pt_bg;
pthread_t pt_movement;
pthread_t pt_field_render;

pthread_mutex_t screen_lock = PTHREAD_MUTEX_INITIALIZER;

WINDOW *bg = NULL;
WINDOW *movement = NULL;

int gridb_x = 0;
int gridb_y = 0;
int gride_x = 100;
int gride_y = 100;
CellType field[MAX_ROWS][MAX_COLS] = {0}; 

const CellObjects objects[] = {
    [EMPTY]  = {.object = ' ', .colour = 0},
    [BULLET] = {.object = '-', .colour = 1},
    [ENEMY]  = {.object = '#', .colour = 2},
    [SHIP]   = {.object = '@', .colour = 3}
};

