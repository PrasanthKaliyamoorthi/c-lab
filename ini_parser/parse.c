#include "./include/parse.h"


typedef struct {
  int num;
  char* alp;
  bool istrue;
} config;

void rtrim(char *s) {
    int i = strlen(s) - 1;
    while (i >= 0 && isspace((unsigned char)s[i]))
        s[i--] = '\0';
}

void generate_conf(char* filename) {
  puts("Generating example config file. `config.ini.sample`.");

  FILE* fr = fopen("./config.ini.sample", "r");
  if (fr != NULL) {
    puts("Old `config.ini.sample` file will be overwritten. Y/n");
    char choice = getchar();
    if ((char) tolower(choice) == 'n') {
      M_EXIT(1);
    };
  };


  FILE* fd = fopen("./config.ini.sample", "w");
  puts("After configuring the sample file rename it to `config.ini`.");

  fprintf(fd, "#--- this is a sample config file after done editing remove this comment ---\n");
  fprintf(fd, "name = \"user\"\n");
  fprintf(fd, "age = 77\n");
  fprintf(fd, "alive = true\n");
  M_EXIT(0);
}

bool validate_conf(FILE* conf) {
  char* line = NULL;
  size_t len = 0;
  ssize_t read_len;

  while ((read_len = getline(&line, &len, conf)) != -1) {
    rtrim(line);
    printf("%s\n", line);
    printf("read length : %ld\n", read_len);
    puts("");
  }
  
  return true;
}

