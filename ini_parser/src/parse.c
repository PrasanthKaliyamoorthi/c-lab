#include "../include/parse.h"


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
void ltrim(char *s) {
    int i = 0;
    while (isspace((unsigned char)s[i]))
        i++;
    
    memmove(s, s+i, strlen(s+i)+1);
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
  fprintf(fd, "[basic]\n");
  fprintf(fd, "name = \"user\"\n");
  fprintf(fd, "age = 77\n");
  fprintf(fd, "alive = true\n");
  M_EXIT(0);
}

void parse_section(FILE* conf, char** list) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read_len;
    int i = 0;

    while (1) {
        long pos = ftell(conf);

        read_len = getline(&line, &len, conf);
        if (read_len == -1)
            break;

        rtrim(line);
        ltrim(line);

        if (line[0] == '[') {
            fseek(conf, pos, SEEK_SET);
            break;
        }

        if (line[0] == '#' || line[0] == '\0')
            continue;

        list[i] = strdup(line);
        i++;
    }
    list[i] = NULL;

    free(line);

}

bool validate_basic(FILE* conf) {
    char* section[100];
    parse_section(conf, section);
    return true;
};
bool validate_conf(FILE* conf) {
  char* line = NULL;
  size_t len = 0;
  ssize_t read_len;

  while ((read_len = getline(&line, &len, conf)) != -1) {
    rtrim(line);
    ltrim(line);
    if (line[0] == '#' || line[0] == '\0')
      continue;
    if (line[0] == '[') {
        char option[30];
      if (sscanf(line, "[%29[^]]", option) == 1) {
        
        if (strcmp("basic", option) == 0) {
          if (validate_basic(conf)) { puts("pass"); };
        }
      }
    }
  }
  
  return true;
}

