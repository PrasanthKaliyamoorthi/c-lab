#include "../include/parse.h"

int test_pass_count = 0;
int test_fail_count = 0;

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

void print_test_count() {
  printf("%s[#] [pass]%s No tests passed: %s%d%s\n", ansi_pass, ansi_reset, ansi_pass, test_pass_count, ansi_reset);
  printf("%s[#] [fail]%s No tests failed: %s%d%s\n", ansi_fail, ansi_reset, ansi_fail, test_fail_count, ansi_reset);
}


void test_rtrim() {
  char str1[] = "hi\n";
  rtrim(str1);
  if (strcmp(str1, "hi") == 0) {
    test_pass_count++;
    print_pass("rtrim", "`hi\\n`; hi = hi");
  } else {
    test_fail_count++;
    print_fail("rtrim", "`hi\\n`; hi = hi");
  };

  char str2[] ="space ";
  rtrim(str2);
  if (strcmp(str2, "space") == 0) {
    test_pass_count++;
    print_pass("rtrim", "`space `; space = space");
  } else {
    test_fail_count++;
    print_fail("rtrim", "`space `; space = space");
  };
};
void test_ltrim() {
  char str1[] = "\nhi";
  ltrim(str1);
  if (strcmp(str1, "hi") == 0) {
    test_pass_count++;
    print_pass("ltrim", "`\\nhi`; hi = hi");
  } else {
    test_fail_count++;
    print_fail("ltrim", "`\\nhi`; hi = hi");
  };

  char str2[] =" space";
  ltrim(str2);
  if (strcmp(str2, "space") == 0) {
    test_pass_count++;
    print_pass("ltrim", "` space`; space = space");
  } else {
    test_fail_count++;
    print_fail("ltrim", "` space`; space = space");
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
    test_pass_count++;
    print_pass("generate_conf", "FILE* != NULL");
  } else {
    test_fail_count++;
    print_fail("generate_conf", "FILE* != NULL");
  }
  fclose(fd);
}

void test_validate_conf() {
}

void run_tests() {
  test_rtrim();
  test_ltrim();
  test_generate_conf();
}

int main(int argc, char* argv[]) {
  run_tests();
  print_test_count();
  return EXIT_SUCCESS;
}
