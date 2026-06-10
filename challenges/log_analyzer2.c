#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


typedef enum {
  DEFAULT,
  TIMESTAMP,
  INVALID
} LogFormat;

typedef struct {
    char level[10];
    char event_name[256];
    int count;
} Event;

typedef struct {
  int cap;
  int len;
  int info;
  int warning;
  int error;
  Event* event_list;
} EventList;

EventList events;

void p_exit(char* msg, int status) {
    printf("%s\n", msg);

    __asm__ volatile (
        "movq %0, %%rdi\n\t"
        "movq $60, %%rax\n\t"
        "syscall"
        :
        : "r"((long)status)
        : "%rax", "%rdi"
    );
}

void swap(Event* a, Event* b) {
  Event tmp = *a;
  *a = *b;
  *b = tmp;
}

void quick_sort(int low, int high) {
  if (low < high) {
    int pivot = events.event_list[high].count;
    int i = (low -1);

    for (int j = low; j < high; j++) {
      if (events.event_list[j].count > pivot) {
        i++;
        swap(&events.event_list[i], &events.event_list[j]);
      }
    }

    swap(&events.event_list[i+1], &events.event_list[high]);

    int pi = i + 1;

    quick_sort(low, pi - 1);
    quick_sort(pi + 1, high);
  }
}

int strlength(const char* s) {
  int i;
  for (i = 0; s[i] != '\0'; ++i) {
  }
  return i;
}

int stringcmp(const char* s1, const char* s2) {
  int l1 = strlength(s1);
  int l2 = strlength(s2);

  if (l1 != l2)
    return -1;

  for (int i = 0; s1[i] != '\0'; ++i) {
    if (s1[i] != s2[i])
      return -1;
  }

  return 0;
}

int getidx(char* event_name) {
  int i;

  for (i = 0; i < events.len; ++i) {
    if (events.event_list[i].count == 0)
      return i;
    if (stringcmp(events.event_list[i].event_name, event_name) == 0)
      return i;
  }

  return i + 1;
}

Event getEvent(char* line, LogFormat format) {
  Event event = { .level="", .event_name="", .count=1 };
  int i = 0;
  int wi = 0;
  char word[255];

  if (format == TIMESTAMP)
    i = 20;

  for (; line[i] != '\0'; i++) {
    if (line[i] == ' ' && event.level[0] == '\0') {
      word[wi] = '\0';
      for (int j = 0; word[j] != '\0'; j++) {
        event.level[j] = word[j];
      }
      event.level[i+1] = '\0';
      break;
    }

    word[wi] = line[i];
    wi++;
  }

  for (int k = 0; line[i] != '\0' && line[i] != '\n'; ++k) {
    ++i;
    if (line[i] != '\n')
      event.event_name[k] = line[i];
  }

  return event;
}

LogFormat validate(char* line) {
  LogFormat result = DEFAULT;
  int i;
  char level[10];

  if (line[0] > 64 && line[0] < 91) {
    for (i = 0; line[i] != ' '; ++i) {
      level[i] = line[i];
    }
    level[i] = '\0';

    if (stringcmp(level, "INFO") == 0)
      result = DEFAULT;
    else if (stringcmp(level, "WARNING") == 0)
      result = DEFAULT;
    else if (stringcmp(level, "ERROR") == 0)
      result = DEFAULT;
    else
     result = INVALID;
  } else if (line[0] > 47 && line[0] < 58) {

    for (i = 0; i < 4; ++i) {
      if (line[i] <= 47 && line[0] >= 58)
        return INVALID;
    }

    if (line[4] != '-')
      return INVALID;
    else if (line[5] <= 47 && line[0] >= 58)
      return INVALID;
    else if (line[6] <= 47 && line[0] >= 58)
      return INVALID;
    else if (line[7] != '-')
      return INVALID;
    else if (line[8] <= 47 && line[0] >= 58)
      return INVALID;
    else if (line[9] <= 47 && line[0] >= 58)
      return INVALID;

    else if (line[10] != ' ')
      return INVALID;


    else if (line[11] <= 47 && line[0] >= 58)
      return INVALID;
    else if (line[12] <= 47 && line[0] >= 58)
      return INVALID;
    else if (line[13] != ':')
      return INVALID;
    else if (line[14] <= 47 && line[0] >= 58)
      return INVALID;
    else if (line[15] <= 47 && line[0] >= 58)
      return INVALID;
    else if (line[16] != ':')
      return INVALID;
    else if (line[17] <= 47 && line[0] >= 58)
      return INVALID;
    else if (line[18] <= 47 && line[0] >= 58)
      return INVALID;
    else if (line[19] != ' ')
      return INVALID;
    else
     return TIMESTAMP;

  } else {
    result = INVALID;
  }

  return result;
}

void analyze(char* line) {
  Event tmp;
  int idx = 0;

  switch (validate(line)) {
    case DEFAULT:
      tmp = getEvent(line, DEFAULT);
      break;
    case TIMESTAMP:
      tmp = getEvent(line, TIMESTAMP);
      break;
    case INVALID:
      printf("Invalid log format: %s\n", line);
      return;
  }

  if (events.cap == events.len) {
    events.cap *= 2;
    events.event_list = realloc(events.event_list, sizeof(Event) * events.cap);
  }

  idx = getidx(tmp.event_name);


  if (events.event_list[idx].count == 0) {
    events.event_list[idx] = tmp;
    events.len += 1;
  } else {
    events.event_list[idx].count += 1;
  }

  if (stringcmp(tmp.level, "INFO") == 0)
    events.info += 1;
  else if (stringcmp(tmp.level, "WARNING") == 0)
    events.warning += 1;
  else if (stringcmp(tmp.level, "ERROR") == 0)
    events.error += 1;

}

void analyze_log(FILE* fd) {
  char* line = NULL;
  size_t size = 0;
  ssize_t read;

  while ((read = getline(&line, &size, fd)) != -1) {
    analyze(line);
  }

  free(line);
}

void summery() {
  int frq = 0;
  puts("===== Log Summary =====");
  puts("");
  printf("INFO    : %i\n", events.info);
  printf("WARNING : %i\n", events.warning);
  printf("ERROR   : %i\n", events.error);
  puts("");
  puts("===== Event Statistics =====");
  puts("");
  for (int i = 0; events.event_list[i].count != 0; ++i) {
    printf("%-26s : %d\n", events.event_list[i].event_name, events.event_list[i].count);
    if (events.event_list[i].count > events.event_list[frq].count)
      frq = i;
  }
  puts("");
  puts("Most frequent event:");
  printf("%s (%d)\n", events.event_list[frq].event_name, events.event_list[frq].count);
}

void log_filter(char* filter) {
  for (int i = 0; events.event_list[i].count != 0; ++i) {
    if (stringcmp(events.event_list[i].level, filter) == 0)
      printf("%-26s : %d\n", events.event_list[i].event_name, events.event_list[i].count);
  }
}

void top_n_events(int n) {
  int j = (n > events.len) ? events.len : n;

  for (int i = 0; i < j; i++) {
    printf("%s (%d)\n", events.event_list[i].event_name, events.event_list[i].count);
  }
}

int main(int argc, char* argv[]) {
  char* filename;
  char* filter = "\0";
  int limit = 0;
  FILE* fd = NULL;

  if (argc > 1) {
    for (int i = 1; i < argc; ++i) {
      if (stringcmp(argv[i], "-f") == 0) {
        if ((i+1) >= argc)
          p_exit("invalid args", 1);
        filename = argv[i+1];
        ++i;
        fd = fopen(filename, "r");
      } else if (stringcmp(argv[i], "--top") == 0) {
        if ((i+1) >= argc)
          p_exit("invalid args", 1);
        limit = atoi(argv[i+1]);
        ++i;
      } else if (stringcmp(argv[i], "--filter") == 0) {
        if ((i+1) >= argc)
          p_exit("invalid args", 1);
        filter = argv[i+1];
        ++i;
      }
    }
  }

  events.cap = 100;
  events.len = 0;
  events.event_list = calloc(100, sizeof(Event));

  if (!isatty(STDIN_FILENO)) {
    analyze_log(stdin);
    quick_sort(0, events.len - 1);
    if (filter[0] != '\0') {
      log_filter(filter);
      return 0;
    }else if (limit != 0) {
      top_n_events(limit);
      return 0;
    }
    summery();
  }else if (fd != NULL) {
    analyze_log(fd);
    quick_sort(0, events.len - 1);
    if (filter[0] != '\0') {
      log_filter(filter);
      return 0;
    }else if (limit != 0) {
      top_n_events(limit);
      return 0;
    }
    summery();
  }

  free(events.event_list);
  return 0;
}
