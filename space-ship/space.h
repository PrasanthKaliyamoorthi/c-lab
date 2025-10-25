#ifndef HEADER_H
#define HEADER_H

void init();
void opening();
void* background_animation(void*);
void end(int, char*, char*);

typedef struct {
  int x;
  int y;
} _ship;

extern _ship ship;
extern int fps;

#endif
