#include "../include/io.h"
#include <unistd.h>

int test_putc() {
  int fds[2];
  pipe(fds);

  putc('a', fds[1]);
  close(fds[1]);

  char c;
  read(fds[0], &c, 1);
  close(fds[0]);

  return c == 'a' ? 0 : 1;
}

int io_test() {
  if (test_putc() == 0) {
    puts("PASS\tputc()");
  } else {
    puts("FAIL\tputc()");
  }
}
