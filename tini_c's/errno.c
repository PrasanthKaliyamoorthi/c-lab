#include <asm-generic/errno-base.h>
#include <errno.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
  errno = 1;
  printf("errno = 1 : %d\n", errno);
  errno = ERANGE;
  printf("errno[ERANGE] : %d\n", errno);
  errno = EINVAL;
  printf("errno[EINVAL] : %d\n", errno);
  FILE *f = fopen("./non.non", "r");
  printf("errno[file] : %d\n", errno);
  return 0;
}
