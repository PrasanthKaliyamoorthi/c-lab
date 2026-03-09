#include "../include/parse.h"

bool is_test_run = true;


char* ansi_reset  = "\033[0m";
char* ansi_func   = "\033[1;38;2;50;50;200m";
char* ansi_pass   = "\033[1;38;2;50;200;50m";
char* ansi_fail   = "\033[1;38;2;200;50;50m";

void print_pass(char* func, char* tcase) {
  printf("%s[#] [pass] %s%s %s test pass. case : %s\n", ansi_pass, ansi_func, func, ansi_reset, tcase);
}

void print_fail(char* func, char* tcase) {
  printf("%s[#] [fail] %s%s %s test pass. case : %s\n", ansi_fail, ansi_func, func, ansi_reset, tcase);
}


void test_rtrim() {
  char str[] = "hi\n";
  rtrim(str);
  if (strcmp(str, "hi") == 0) {
    print_pass("rtrim", "hi = hi");
  }else {
    print_fail("rtrim", "hi = hi");
  };
}
void test_generate_conf() {
  FILE* fd;
  char fn[] = "./config.ini.sample";
  fd = fopen(fn, "r");
  if (fd) {
    fclose(fd);
    if (remove(fn)!=0) {
      printf("file deletion error: %d\n",errno);
    }
  }

  generate_conf(fn);
  fd = fopen(fn, "r");
  if (fd) {
    print_pass("generate_conf", "FILE* != NULL");
  } else {
    print_fail("generate_conf", "FILE* != NULL");
  }
  fclose(fd);
}

void run_tests() {
  test_rtrim();
  test_generate_conf();
}

int main(int argc, char* argv[]) {
  run_tests();
  return EXIT_SUCCESS;
}
