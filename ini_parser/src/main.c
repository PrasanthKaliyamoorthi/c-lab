#include "../include/parse.h"


bool is_test_run = false;


int main(int argc, char* argv[]) {
  FILE* fd = fopen("./config.ini", "r");

  if (fd == NULL) {
    puts("Config file is missing from current directory.");
    generate_conf("config.ini.sample");
  };
  if (!validate_conf(fd)) {
    puts("Config error. config validation error.!");
    exit(-1);
  };

  return EXIT_SUCCESS;
}
