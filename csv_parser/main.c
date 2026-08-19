#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char * header[124];
typedef struct {
  int len;
  char ** values[32];
} list;


void proccess_csv(char * csv){
  FILE * fd = fopen(csv, "r");
  if (fd == NULL) {
    perror("Can't read the csv file!\n- [Reason]");
    exit(1);
  }

  return;
}

int main(int argc, char * argv[]) {
  if (argc <= 1) {
    printf("USAGE: %s FILE.csv\n", argv[0]);
    return 0;
  }

  char * filename = argv[1];

  proccess_csv(filename);

  return 0;
}
