#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>


void init() {
  initscr();
  cbreak();
  noecho();
  curs_set(0);
  timeout(0); 

  border(0, 0, 0, 0, 0, 0, 0, 0);
}

void end(int code, char* location) {
  endwin();
  if (code != 0) {
    fprintf(stderr, "something happened...at : %s", location);
    exit(code);
  }
  exit(code);
}

void bg_stars(WINDOW* win, int key) {
  int x, y;

  getmaxyx(win, y, x);
  x--;
  y = y-2;

  wclear(win);
  box(win, 0, 0);

  for (int i = 1; i < x; i++) {
        int star_y = rand() % (y - key) + 1;
        mvwaddch(win, star_y, i, '.');
        star_y = rand() % (y - (key+1)) + 1;
        mvwaddch(win, star_y, i, '.');
  }
  wrefresh(win);
}

int main(int argc, char* argv[]) {

  int x, y, key=1;

  init();

  getmaxyx(stdscr, y, x);

  WINDOW* main_win = subwin(stdscr, y-2, x-2, 1, 1);
  nodelay(main_win, TRUE);

  const int fps = 30;           // target FPS
  const int frame_delay = 10000 / fps; // milliseconds per frame
  
  do {
    key = rand() % 5 + 1;
    bg_stars(main_win, key);
    napms(frame_delay);       // maintain ~30 FPS
  } while(getch() != 113);
  end(1, "main");

  return EXIT_SUCCESS;
}
