#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

typedef struct {
  int num;
  char* alp;
  bool istrue;
} config;


void generate_conf(char* filename) {
  puts("Generating example config file. `config.ini.sample`.");
  puts("Old `config.ini.sample` file will be overwritten. Y/n");

  char choice = getchar();
  if ((char) tolower(choice) == 'n') {
    exit(0);
  }

  puts("After configuring the sample file rename it to `config.ini`.");
  FILE* fd = fopen("./config.ini.sample", "a");

  fprintf(fd, "#--- this is a sample config file after done editing remove this comment ---\n");
  fprintf(fd, "name = \"user\"\n");
  fprintf(fd, "age = 77\n");
  fprintf(fd, "alive = true\n");
  exit(0);
}



int main(int argc, char* argv[]) {
  FILE* fd = fopen("./config.ini", "r");

  if (fd == NULL) {
    puts("Config file is missing from current directory.");
    generate_conf("config.ini.sample");
  };
  return EXIT_SUCCESS;
}
