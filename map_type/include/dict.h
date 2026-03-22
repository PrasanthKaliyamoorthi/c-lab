#ifndef DICT_H
#define DICT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct Dict Dict;
typedef struct Item Item;

Dict dict(int max);
void put_dict(Dict* dict, void* key, void* value);
Item get_dict(Dict* dict, void* key);
void* get_value(Dict* dict, void* key);
void del_dict(Dict* dict, void* key);
void len_dict(Dict* dict);
void free_dict(Dict* dict);

int hash(void* key);
bool probe(Dict* dict, int index, char* key);

#endif
