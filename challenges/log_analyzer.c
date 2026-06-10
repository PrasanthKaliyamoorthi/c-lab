#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int info = 0;
int warning = 0;
int error = 0;

typedef struct {
  char* name;
  int count;
} events;

events event_counts[100] = {0};

char** split(char* line) {
  char c;
  char* tmp = malloc(sizeof(char) * 100);
  char** tokens = malloc(sizeof(char*) * 100);
  int tok_count = 0;
  int tmp_count = 0;

  for (int i = 0; line[i] != '\0'; ++i) {
    c = line[i];

    if (c == ' ' ) {
      tmp[tmp_count] = '\0';
      tokens[tok_count] = strdup(tmp);
      tok_count++;
      tmp_count = 0;
      memset(tmp, 0, sizeof(char)*100);
      continue;
    }
    if (c == '\n') {
      tmp[tmp_count] = '\0';
      tokens[tok_count] = strdup(tmp);
      tokens[tok_count+1] = NULL;
      free(tmp);
      return tokens;
    }

    tmp[tmp_count] = c;
    tmp_count++;
  }

  free(tmp);
  return tokens;
}

void analyze(char* line) {
  char** tokens;
  tokens = split(line);
  if (strcmp(tokens[0], "INFO") == 0)
    info++;
  else if (strcmp(tokens[0], "WARNING") == 0)
    warning++;
  else if (strcmp(tokens[0], "ERROR") == 0)
    error++;
  else
    printf("Unkown event: %s\n", tokens[0]);

  for (int i = 0; i < 100; ++i) {
    if (event_counts[i].name == NULL) {
      event_counts[i].name = strdup(tokens[1]);
      event_counts[i].count++;
      break;
    }
    if (event_counts[i].name != NULL && (strcmp(event_counts[i].name, tokens[1]) == 0)) {
      event_counts[i].count++;
      break;
    }
  }

  for (int i = 0; tokens[i] != NULL; ++i) {
    free(tokens[i]);
  }
  free(tokens);
}


int main(int argc, char* argv[]) {
  char* line = NULL;
  size_t len;
  ssize_t read;
  int highest = 0;
  int json = 0;

  if (isatty(STDIN_FILENO)) {
    printf("No piped input\n");
    return 0;
  }

  while ((read = getline(&line, &len, stdin)) != -1) {
    analyze(line);
  }
  for (int i = 0; i < 100; ++i) {
    if (event_counts[i].name == NULL)
      break;
    if (event_counts[i].count > event_counts[highest].count)
      highest = i;
  }

  if(argc > 1) {
    for (int i = 1; i < argc; ++i) {
      if ((strcmp(argv[i], "-j") == 0) || (strcmp(argv[i], "--json")) == 0)
          json = 1;
    }
  }
  if (json == 1) {
    printf( "{\n"
            "  \"info\": %i,\n"
            "  \"warning\": %i,\n"
            "  \"error\": %i,\n"
            "  \"most_frequent_event\": \"%s\"\n"
            "}\n", info, warning, error, event_counts[highest].name);
  } else {
      printf("INFO: %i\n", info);
      printf("WARNING: %i\n", warning);
      printf("ERROR: %i\n", error);
      printf("Most Frequent Event: %s (%d)\n", event_counts[highest].name, event_counts[highest].count);
  }

  for (int i = 0; i < 100; ++i) {
    if (event_counts[i].name)
        free(event_counts[i].name);
  }
  free(line);

  return 0;
}
