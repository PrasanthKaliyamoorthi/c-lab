#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "../include/dict.h"


int key_hash(void *key) {
  char *str = (char*)key;
  int index = 0;

  for (int i = 0; str[i] != '\0'; ++i) {
    index += (int)str[i] * i;
  }

  return index % 1000;
}

bool key_probe(Dict *dict, int index, char *key) {
  if (dict->bucket[index].key == NULL) return false;

  char* got = (char*)dict->bucket[index].key;
  return strcmp(key, got) == 0;
}

Dict dict(int max) {
  Dict map = { .filled = 0, .next = NULL, .key_max = max};
  return map;
}

void put_dict(Dict *dict, void *key, void *value) {
  Item item = { .key = key, .value = value };
  int index = key_hash(key);

  while (dict->bucket[index].key != NULL &&
         strcmp((char*)dict->bucket[index].key, (char*)key) != 0) {
    index = (index + 1) % 1000;
  }

  (*dict).bucket[index] = item;
}

Item get_dict(Dict *dict, void *key) {
  int index = key_hash(key);

  while (dict->bucket[index].key != 0) {
    if (strcmp((char*)dict->bucket[index].key, (char*)key) == 0) {
      return dict->bucket[index];
    }
    index = (index + 1) % 1000;
  }

  return (*dict).bucket[index];
}

void* get_value(Dict *dict, void *key) {
  return get_dict(dict, key).value;
}

void del_dict(Dict *dict, void *key) {
  int index = key_hash(key);

  free(dict->bucket[index].key);
  free(dict->bucket[index].value);

  dict->bucket[index].key = NULL;
}

int len_dict(Dict *dict) {
  return dict->filled;
}

void free_dict(Dict *dict) {
  for (int i = 0; i < 1000; ++i) {
    free(dict->bucket[i].key);
    free(dict->bucket[i].value);
  }
}
