#include "../include/dict.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

struct Item {
  void* key;
  void* value;
};

struct Dict {
  Item bucket[1000];
  int used;
  struct Dict* next;
  int key_max;
};

int hash(void* key) {
  char *str = (char*)key;
  int index = 0;

  for (int i = 0; str[i] != '\0'; ++i) {
    index += (int)str[i];
  }

  return index % 1000;
}

bool probe(Dict* dict, int index, char* key) {
  if (dict->bucket[index].key == NULL) return false;

  char* got = (char*)dict->bucket[index].key;
  return strcmp(key, got) == 0;
}


Dict dict(int max) {
  Dict map = { .used = 0, .next = NULL, .key_max = max };
  return map;
};

void put_dict(Dict* dict, void *key, void *value) {
  Item item = { .key = key, .value = value };
  int index = hash(key);
  bool res;

  while (dict->bucket[index].key != NULL &&
         strcmp((char*)dict->bucket[index].key, (char*)key) != 0) {
    index = (index + 1) % 1000;
  }
  (*dict).bucket[index] = item;
}

Item get_dict(Dict* dict, void *key) {
  int index = hash(key);
  bool res;

  while (dict->bucket[index].key != NULL) {
    if (strcmp((char*)dict->bucket[index].key, (char*)key) == 0) {
      return dict->bucket[index];
    }
    index = (index + 1) % 1000;
  }
  index--;
  return (*dict).bucket[index];
}

void* get_value(Dict* dict, void* key) {
  return get_dict(dict, key).value;
}


int main(int argc, char* argv[]) {
  Dict mydict = dict(30);
  put_dict(&mydict, "color", "blue");
  printf("key: %s\nvalue: %s\n", (char*)get_dict(&mydict, "color").key, (char*)get_value(&mydict, "color"));

  return 0;
}
