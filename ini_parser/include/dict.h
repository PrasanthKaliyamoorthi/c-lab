#ifndef DICT_H
#define DICT_H

typedef struct Item {
  void* key;
  void* value;
} Item;

typedef struct Dict {
  Item bucket[1000];
  int filled;
  int key_max;
  struct Dict* next;
} Dict;

Dict dict(int max);
void put_dict(Dict* dict, void* key, void* value);
Item get_dict(Dict* dict, void* key);
void* get_value(Dict* dict, void* key);
void del_dict(Dict* dict, void* key);
int len_dict(Dict* dict);
void free_dict(Dict* dict);

int key_hash(void* key);
bool key_probe(Dict* dict, int index, char* key);

#endif
