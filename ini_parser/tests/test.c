#include "../include/parse.h"


void test_rtrim() {
  char str[] = "hi\n";
  rtrim(str);
  if (strcmp(str, "hi") == 0) {
    printf("rtrim test pass. case: %s = hi\n", str);
  }else {
    printf("rtrim test fail. case: %s = hi\n", str);
  };
}

int main(int argc, char* argv[]) {
  test_rtrim();
  return EXIT_SUCCESS;
}
