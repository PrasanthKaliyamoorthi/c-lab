#include "space.h"
#include <ncurses.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>


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
    werase(stdscr);
    border(0, 0, 0, 0, 0, 0, 0, 0);
    refresh();
}

void render(WINDOW* fg_w, WINDOW* bg_w) {
    overlay(fg_w, bg_w);  // fg over bg
    wnoutrefresh(bg_w);
    doupdate();
}

void end(int exit_code, char* exit_caller, char* exit_msg) {
    endwin();
    pthread_cancel(pt_movement);
    pthread_cancel(pt_bg);
    pthread_join(pt_movement, NULL);
    pthread_join(pt_bg, NULL);

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
        pthread_mutex_lock(&screen_lock);
        werase(bg);
        box(bg, 0,0);

        for (int i = 1; i < x; i++) {
            star_y = rand() % (y - seed) + 1;
            mvwaddch(bg, star_y, i, '.');
            star_y = rand() % (y - (seed + 1)) + 1;
        }

        render(movement, bg);
        pthread_mutex_unlock(&screen_lock);
        usleep(FPS); //30 FPS or 300MS
    }
}

void* pilot_ship(void* arg) {
    WINDOW* movement = (WINDOW*) arg;
    srand(time(NULL));

    while (TRUE) {
        pthread_mutex_lock(&screen_lock);
        werase(movement);
        mvwaddch(movement, ship.y, ship.x, '@');

        render(movement, bg);
        pthread_mutex_unlock(&screen_lock);
        usleep(FPS / 3);
    }
}

void* field_render(void* arg) {
    while (TRUE) {
        pthread_mutex_lock(&screen_lock);


        for (int steep = gride_y; steep > gridb_y; steep--) {
            for (int wide = gride_x; wide > gridb_x; wide--) {
                if (field[steep][wide] == objects[BULLET].object) {
                    if (field[steep][wide + 1] == objects[ENEMY].object ||  field[steep][wide + 1] == objects[SHIP].object) {
                        field[steep][wide] = objects[EMPTY].object;
                        mvwaddch(movement, steep, wide + 1, objects[EMPTY].object);

                        field[steep][wide + 1] = objects[EMPTY].object;
                        mvwaddch(movement, steep, wide, objects[EMPTY].object);
                    } else {
                        field[steep][wide] = objects[EMPTY].object;
                        mvwaddch(movement, steep, wide, objects[EMPTY].object);

                        field[steep][wide + 1] = objects[BULLET].object;
                        mvwaddch(movement, steep, wide + 1, objects[BULLET].object);
                    }
                }
            }
        }

        render(movement, bg);

        pthread_mutex_unlock(&screen_lock);
        usleep(FPS);
    }
}

void fill_the_field(void) {
    gridb_y = 2;
    gridb_x = 2;
    gride_x = scr_x - 4;
    gride_y = scr_y - 4;
    for (int i = gridb_y; i < gride_y; i++) {
        for (int j = gridb_x; j < gride_x; j++) {
            field[i][j] = EMPTY;
        }
    }
}


/////////////////////main///////////////////////

int main(int argc, char* argv[]) {
    int ch;

    init();
    opening();

    getmaxyx(stdscr, scr_y, scr_x);

    fill_the_field();

    bg = newwin(scr_y - 2, scr_x - 2, 1, 1);
    movement = newwin(scr_y - 4, scr_x - 4, 2, 2);

    pthread_create(&pt_bg, NULL, background_animation, bg);

    ship.y = (scr_y - 3) / 2;
    ship.x = 2;

    pthread_create(&pt_movement, NULL, pilot_ship, movement);
    pthread_create(&pt_field_render, NULL, field_render, NULL);

    do {
        ch = getch();
        switch (ch) {
            case 120:
                field[ship.y][ship.x + 1] = objects[BULLET].object;
                break;
            case KEY_UP:
                if (ship.y > 0)
                    ship.y -= 1;
                break;
            case KEY_DOWN:
                if (ship.y < scr_y - 4)
                    ship.y += 1;
                break;
            case KEY_LEFT:
                if (ship.x > 0)
                    ship.x -= 1;
                break;
            case KEY_RIGHT:
                if (ship.x < scr_x - 4)
                    ship.x += 1;
                break;
        }
    }while (ch != 'q');

    end(0, "main", "closing the game..");

    return EXIT_SUCCESS;
}
