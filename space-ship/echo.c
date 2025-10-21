
#include <ncurses.h>

int main() {
    initscr();              // Initialize ncurses
    cbreak();               // Disable line buffering
    noecho();               // Don't echo typed characters
    keypad(stdscr, TRUE);   // Enable function keys

    printw("Press the space bar to exit.\n");
    int ch;
    while ((ch = getch()) != ' ') {
        printw("You pressed: %d\n", ch);
    }

    endwin();               // End ncurses mode
    return 0;
}
