#include "../include/date.h"

int days_in_month(int month, int year) {

  if (month == 2) {
    if ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0))
      return 29;
    return 28;
  }

  if (month == 4 ||
      month == 6 ||
      month == 9 ||
      month == 11)
    return 30;

  return 31;
}

// int first_weekday(int month, int year) {
//
//   static const int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
//
//   if (month < 3)
//     year--;
//
//   return (year + year/4 - year/100 + year/400 + t[month-1] + 1) % 7;
// }


int first_weekday(int month, int year)
{
    int q = 1;

    if(month < 3)
    {
        month += 12;
        year--;
    }

    int k = year % 100;
    int j = year / 100;

    int h =
        (q +
         (13 * (month + 1)) / 5 +
         k +
         k / 4 +
         j / 4 +
         (5 * j)) % 7;

    return (h + 5) % 7;
}
