#include "pydict.h"
#include <stdlib.h>
#include <string.h>

#define PERTURB_SHIFT 5
#define DICT_MINSIZE 8
#define USABLE_FRACTION(n) (((n) << 1) / 3)

static void dict_resize(Dict *d, size_t minused);

Dict* dict_new(DictHashFunc hash_func, DictEqFunc eq_func) {
    Dict *d = malloc(sizeof(Dict));
    if (!d) return NULL;
    
    d->fill = 0;
    d->used = 0;
    d->indices_capacity = DICT_MINSIZE;
    
    d->indices = malloc(d->indices_capacity * sizeof(int32_t));
    for (size_t i = 0; i < d->indices_capacity; i++) {
        d->indices[i] = DICT_IX_EMPTY;
    }
    
    size_t usable = USABLE_FRACTION(d->indices_capacity);
    d->entries = malloc(usable * sizeof(DictEntry));
    
    d->hash_func = hash_func;
    d->eq_func = eq_func;
    
    return d;
}

void dict_free(Dict *d, void (*free_key)(void*), void (*free_value)(void*)) {
    if (!d) return;
    if (free_key || free_value) {
        for (size_t i = 0; i < d->fill; i++) {
            if (d->entries[i].key != NULL) {
                if (free_key) free_key(d->entries[i].key);
                if (free_value) free_value(d->entries[i].value);
            }
        }
    }
    free(d->indices);
    free(d->entries);
    free(d);
}

static int32_t lookdict(const Dict *d, void *key, uint64_t hash, int32_t *freeslot) {
    size_t mask = d->indices_capacity - 1;
    size_t i = hash & mask;
    size_t perturb = hash;
    int32_t current_freeslot = DICT_IX_EMPTY;
    
    while (1) {
        int32_t ix = d->indices[i];
        if (ix == DICT_IX_EMPTY) {
            if (freeslot) *freeslot = current_freeslot == DICT_IX_EMPTY ? (int32_t)i : current_freeslot;
            return DICT_IX_EMPTY;
        }
        if (ix == DICT_IX_DUMMY) {
            if (current_freeslot == DICT_IX_EMPTY) {
                current_freeslot = (int32_t)i;
            }
        } else {
            DictEntry *ep = &d->entries[ix];
            if (ep->key == key || (ep->hash == hash && d->eq_func(ep->key, key))) {
                return ix;
            }
        }
        perturb >>= PERTURB_SHIFT;
        i = (i * 5 + 1 + perturb) & mask;
    }
}

static int32_t find_empty_slot(const Dict *d, uint64_t hash) {
    size_t mask = d->indices_capacity - 1;
    size_t i = hash & mask;
    size_t perturb = hash;
    
    while (1) {
        int32_t ix = d->indices[i];
        if (ix == DICT_IX_EMPTY) {
            return (int32_t)i;
        }
        perturb >>= PERTURB_SHIFT;
        i = (i * 5 + 1 + perturb) & mask;
    }
}

int dict_set(Dict *d, void *key, void *value) {
    uint64_t hash = d->hash_func(key);
    int32_t freeslot = DICT_IX_EMPTY;
    int32_t ix = lookdict(d, key, hash, &freeslot);
    
    if (ix != DICT_IX_EMPTY) {
        d->entries[ix].value = value;
        return 0; // updated
    }
    
    size_t usable = USABLE_FRACTION(d->indices_capacity);
    if (d->fill >= usable) {
        dict_resize(d, d->used > 50000 ? d->used * 2 : d->used * 4);
        freeslot = find_empty_slot(d, hash);
    }
    
    int32_t entry_ix = (int32_t)d->fill;
    d->fill++;
    d->used++;
    
    DictEntry *ep = &d->entries[entry_ix];
    ep->hash = hash;
    ep->key = key;
    ep->value = value;
    
    d->indices[freeslot] = entry_ix;
    return 1; // added
}

void* dict_get(const Dict *d, const void *key) {
    uint64_t hash = d->hash_func(key);
    int32_t ix = lookdict(d, (void*)key, hash, NULL);
    if (ix >= 0) {
        return d->entries[ix].value;
    }
    return NULL;
}

int dict_del(Dict *d, const void *key) {
    uint64_t hash = d->hash_func(key);
    int32_t ix = lookdict(d, (void*)key, hash, NULL);
    if (ix >= 0) {
        size_t mask = d->indices_capacity - 1;
        size_t i = hash & mask;
        size_t perturb = hash;
        
        while (1) {
            if (d->indices[i] == ix) {
                d->indices[i] = DICT_IX_DUMMY;
                break;
            }
            perturb >>= PERTURB_SHIFT;
            i = (i * 5 + 1 + perturb) & mask;
        }
        
        d->entries[ix].key = NULL;
        d->entries[ix].value = NULL;
        d->used--;
        return 1;
    }
    return 0;
}

static void dict_resize(Dict *d, size_t minused) {
    size_t newsize = DICT_MINSIZE;
    while (newsize <= minused && newsize > 0) {
        newsize <<= 1;
    }
    
    int32_t *old_indices = d->indices;
    DictEntry *old_entries = d->entries;
    size_t old_fill = d->fill;
    
    d->indices_capacity = newsize;
    d->indices = malloc(d->indices_capacity * sizeof(int32_t));
    for (size_t i = 0; i < d->indices_capacity; i++) {
        d->indices[i] = DICT_IX_EMPTY;
    }
    
    size_t usable = USABLE_FRACTION(d->indices_capacity);
    d->entries = malloc(usable * sizeof(DictEntry));
    
    d->fill = 0;
    d->used = 0;
    
    for (size_t i = 0; i < old_fill; i++) {
        if (old_entries[i].key != NULL) {
            DictEntry *ep = &old_entries[i];
            
            size_t mask = d->indices_capacity - 1;
            size_t idx = ep->hash & mask;
            size_t perturb = ep->hash;
            
            while (d->indices[idx] != DICT_IX_EMPTY) {
                perturb >>= PERTURB_SHIFT;
                idx = (idx * 5 + 1 + perturb) & mask;
            }
            
            int32_t entry_ix = (int32_t)d->fill;
            d->indices[idx] = entry_ix;
            
            d->entries[entry_ix].hash = ep->hash;
            d->entries[entry_ix].key = ep->key;
            d->entries[entry_ix].value = ep->value;
            
            d->fill++;
            d->used++;
        }
    }
    
    free(old_indices);
    free(old_entries);
}

size_t dict_len(const Dict *d) {
    return d->used;
}

uint64_t dict_hash_string(const void *key) {
    const char *str = (const char*)key;
    uint64_t hash = 14695981039346656037ULL;
    while (*str) {
        hash ^= (unsigned char)*str++;
        hash *= 1099511628211ULL;
    }
    return hash;
}

bool dict_eq_string(const void *k1, const void *k2) {
    return strcmp((const char*)k1, (const char*)k2) == 0;
}
