#include <time.h>
#include <ncurses.h>

#include "../include/calendar.h"


int main(void) {
  time_t now = time(NULL);

  struct tm* tm = localtime(&now);

  int month = tm->tm_mon + 1;
  int year = tm->tm_year + 1900;
  int today = tm->tm_mday;

  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  draw_calendar(month, year, today);

  while (getch() != 'q') {
  }

  endwin();

  return 0;
}
