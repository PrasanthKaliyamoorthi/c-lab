#ifndef PYDICT_H
#define PYDICT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define DICT_IX_EMPTY -1
#define DICT_IX_DUMMY -2

typedef struct {
    uint64_t hash;
    void *key;
    void *value;
} DictEntry;

typedef uint64_t (*DictHashFunc)(const void *key);
typedef bool (*DictEqFunc)(const void *key1, const void *key2);

typedef struct {
    size_t fill;       /* Number of active and dummy entries */
    size_t used;       /* Number of active entries */
    size_t indices_capacity; /* Size of indices array (must be a power of 2) */
    int32_t *indices;  /* Sparse array storing indexes into entries */
    DictEntry *entries; /* Dense array of entries */
    
    DictHashFunc hash_func;
    DictEqFunc eq_func;
} Dict;

Dict* dict_new(DictHashFunc hash_func, DictEqFunc eq_func);
void dict_free(Dict *d, void (*free_key)(void*), void (*free_value)(void*));

int dict_set(Dict *d, void *key, void *value);
void* dict_get(const Dict *d, const void *key);
int dict_del(Dict *d, const void *key);
size_t dict_len(const Dict *d);

/* Utils for string keys */
uint64_t dict_hash_string(const void *key);
bool dict_eq_string(const void *k1, const void *k2);

#endif /* PYDICT_H */
