#include <ncurses.h>

#include "calendar.h"
#include "date.h"

static const char *months[] =
{
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December"
};

void draw_calendar(int month,
                   int year,
                   int today)
{
    clear();

    mvprintw(1,
             20,
             "%s %d",
             months[month - 1],
             year);

    mvprintw(3, 5,
             "Mo Tu We Th Fr Sa Su");

    int start =
        first_weekday(month,
                      year);

    int days =
        days_in_month(month,
                      year);

    int row = 5;
    int col = 5 + (start * 3);

    for(int day = 1;
        day <= days;
        day++)
    {
        if(day == today)
            attron(A_REVERSE);

        mvprintw(row,
                 col,
                 "%2d",
                 day);

        if(day == today)
            attroff(A_REVERSE);

        col += 3;

        if(col > 23)
        {
            col = 5;
            row++;
        }
    }

    mvprintw(row + 2,
             5,
             "Press q to quit");

    refresh();
}
