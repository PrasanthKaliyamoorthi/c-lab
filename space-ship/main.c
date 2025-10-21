#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
//#include <pthread.h>


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

typedef struct {
  unsigned int y;
  unsigned int x;
} ship_position;

void ship(WINDOW* win, ship_position* point) {
  werase(win);
//  mvwaddch(win, point->y, point->x, '@');
  mvwaddstr(win, point->y, point->x, "<-@->");
  wrefresh(win);
}

void shoot(WINDOW* win, int y, int x, int end, WINDOW* bg) {
    for (int i = x + 3; i < end - 3; i++) {
        mvwaddch(win, y, i, '-');
        wrefresh(win);   // refresh THIS window, not stdscr
        napms(30);
        mvwaddch(win, y, i, ' ');
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

  //nodelay(stdscr, TRUE);
  keypad(stdscr, TRUE);

  const int fps = 30;           // target FPS
  const int frame_delay = 6000 / fps; // milliseconds per frame

  ship_position point = { .y = y / 2, x = 1 };

  int ch;
  do {
    bg_stars(bg, 1);
    ship(fg, &point);
    overlay(fg, bg);
    wrefresh(bg);

    ch = getch();

    switch (ch) {
      case KEY_UP:    point.y--; break;
      case KEY_DOWN:  point.y++; break;
      case KEY_LEFT:  point.x--; break;
      case KEY_RIGHT: point.x++; break;
      case 120: shoot(fg, point.y, point.x, x, bg); break;
      default: break;
    }

    // boundaries
    if (point.y < 1) point.y = 1;
    if (point.y > y - 4) point.y = y - 4;
    if (point.x < 1) point.x = 1;
    if (point.x > x - 4) point.x = x - 4;

    napms(frame_delay);

  } while(ch != 113);
  end(0, "main", "closing the game..");

  return EXIT_SUCCESS;
}
