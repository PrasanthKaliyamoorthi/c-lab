#ifndef HEADER_H
#define HEADER_H

#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

void init();
void opening();
void* background_animation(void*);
void end(int, char*, char*);

typedef struct {
  int x;
  int y;
} _ship;

typedef enum {
    EMPTY,
    BULLET,
    ENEMY,
    SHIP
} CellType;

typedef struct {
  CellType object;
  int colour;
} CellObjects;

extern const CellObjects objects[];

extern pthread_t pt_bg;
extern pthread_t pt_movement;
extern pthread_t pt_field_render;


#define MAX_ROWS 100
#define MAX_COLS 200
extern int gridb_x, gridb_y, gride_x, gride_y;

extern CellType field[MAX_ROWS][MAX_COLS];
extern int scr_y, scr_x;
extern _ship ship;
extern int FPS;

extern pthread_mutex_t screen_lock;

extern WINDOW* bg;
extern WINDOW* movement;

#endif
