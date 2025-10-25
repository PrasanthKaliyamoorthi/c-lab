#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include "space.h"


pthread_t pt_bg;
pthread_t pt_movement;

void init() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    timeout(0);
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    border(0, 0, 0, 0, 0, 0, 0, 0);
}

void opening() {
    int x, y;
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

void end(int exit_code, char* exit_caller, char* exit_msg) {
    endwin();
    pthread_cancel(pt_movement);
    pthread_cancel(pt_bg);

    if (exit_code != 0) {
        fprintf(stderr, "Something happened at %s!\n", exit_caller);
    }
    if (exit_msg != NULL) {
        fprintf(stdout, "%s\n", exit_msg);
    }

    exit(exit_code);
}

void* background_animation(void* arg) {
    int seed = 1, y, x;

    WINDOW* bg = (WINDOW*) arg;
    getmaxyx(bg, y, x);
    srand(time(NULL));

    x--; y -= 2;
    int star_y;

    while (TRUE) {
        werase(bg);
        box(bg, 0,0);

        for (int i = 1; i < x; i++) {
            star_y = rand() % (y - seed) + 1;
            mvwaddch(bg, star_y, i, '.');
            star_y = rand() % (y - (seed + 1)) + 1;
        }

        wrefresh(bg);
        usleep(fps); //30 fps or 300MS
    }
}

void* pilot_ship(void* arg) {
    WINDOW* movement = (WINDOW*) arg;
    srand(time(NULL));

    while (TRUE) {
        werase(movement);
        mvwaddch(movement, ship.y, ship.x, '@');
        wrefresh(movement);
        usleep(fps);
    }
}


/////////////////////main///////////////////////

int main(int argc, char* argv[]) {
    int y, x, ch;

    init();
    opening();

    getmaxyx(stdscr, y, x);

    WINDOW *bg = newwin(y - 2, x - 2, 1, 1);
    pthread_create(&pt_bg, NULL, background_animation, bg);

    ship.y = y / 2;
    ship.x = 2;

    WINDOW *movement = newwin(y - 2, x - 2, 1, 1);
    pthread_create(&pt_movement, NULL, pilot_ship, movement);

    do {
        ch = getch();
        switch (ch) {
            case 120:
                break;
        }
    }while (ch != 'q');

    end(0, "main", "closing the game..");

    return EXIT_SUCCESS;
}
