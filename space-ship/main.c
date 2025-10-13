#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>


void init();
void opening();
void bg_stars(WINDOW* win, int key);
void end(int code, char* location, char* msg);


void init() {
  initscr();
  cbreak();
  noecho();
  curs_set(0);
  timeout(0); 
  nodelay(stdscr, TRUE);

  border(0, 0, 0, 0, 0, 0, 0, 0);
}

void opening() {
  int x,y;
  getmaxyx(stdscr, y, x);
  if (x < 64 || y < 25) {
    end(2, "opening", "terminal size should be bigger than height 25, width 64");
  }
  mvaddstr(y/2, (x - 40) / 2, "welcome to the star-ship!");
  mvaddstr(y/2 + 1, (x - 40) / 2, "Press any key to start...");
  mvaddstr(y/2 + 2, (x - 40) / 2, "Press `q` too exit the game.");
  refresh();

  timeout(-1);
  getch();
  timeout(0);
}

void end(int code, char* location, char* msg) {
  endwin();
  if (code != 0) {
    fprintf(stderr, "something happened...at : %s\n", location);
  }
  if (msg != NULL) {
    fprintf(stdout, "%s\n", msg);
  }
  exit(code);
}

void bg_stars(WINDOW* win, int key) {
  int x, y;

  getmaxyx(win, y, x);
  x--;
  y = y-2;

  werase(win);
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
  opening();

  getmaxyx(stdscr, y, x);

  WINDOW* main_win = subwin(stdscr, y-2, x-2, 1, 1);
  WINDOW *bg = newwin(y - 2, x - 2, 1, 1);
  WINDOW *fg = newwin(y - 2, x -2, 1, 1);

  const int fps = 30;           // target FPS
  const int frame_delay = 10000 / fps; // milliseconds per frame
  
  do {
    key = rand() % 5 + 1;
    bg_stars(bg, key);
    napms(frame_delay);       // maintain ~30 FPS
  } while(getch() != 113);
  end(0, "main", "closing the game..");

  return EXIT_SUCCESS;
}
