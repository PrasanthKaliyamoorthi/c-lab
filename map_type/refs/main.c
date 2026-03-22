#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "pydict.h"

int main() {
    Dict *d = dict_new(dict_hash_string, dict_eq_string);
    assert(d != NULL);

    // Test setting and getting
    dict_set(d, "key1", "value1");
    dict_set(d, "key2", "value2");
    dict_set(d, "key3", "value3");

    assert(dict_len(d) == 3);

    const char *v1 = dict_get(d, "key1");
    assert(v1 != NULL && dict_eq_string(v1, "value1"));

    const char *v2 = dict_get(d, "key2");
    assert(v2 != NULL && dict_eq_string(v2, "value2"));

    const char *v_missing = dict_get(d, "key4");
    assert(v_missing == NULL);

    // Test updating
    dict_set(d, "key2", "new_value2");
    v2 = dict_get(d, "key2");
    assert(v2 != NULL && dict_eq_string(v2, "new_value2"));
    assert(dict_len(d) == 3);

    // Test deleting
    int deleted = dict_del(d, "key1");
    assert(deleted == 1);
    assert(dict_len(d) == 2);
    assert(dict_get(d, "key1") == NULL);

    deleted = dict_del(d, "key_missing");
    assert(deleted == 0);

    // Test resizing and collision (implicitly tested by adding many)
    for (int i = 0; i < 100; i++) {
        char key_buf[32];
        char val_buf[32];
        snprintf(key_buf, sizeof(key_buf), "k-%d", i);
        snprintf(val_buf, sizeof(val_buf), "v-%d", i);
        dict_set(d, strdup(key_buf), strdup(val_buf));
    }

    assert(dict_len(d) == 102);

    for (int i = 0; i < 100; i++) {
        char key_buf[32];
        snprintf(key_buf, sizeof(key_buf), "k-%d", i);
        char *val = dict_get(d, key_buf);
        assert(val != NULL);
    }

    // Clean up
    for (int i = 0; i < d->fill; i++) {
        if (d->entries[i].key != NULL) {
            // Only free the dynamically allocated strings, not the static ones
            if (((char*)d->entries[i].key)[0] == 'k' && ((char*)d->entries[i].key)[1] == '-') {
                free(d->entries[i].key);
                free(d->entries[i].value);
            }
        }
    }
    
    // Set entries to NULL so dict_free doesn't free them again
    for (int i = 0; i < d->fill; i++) {
        d->entries[i].key = NULL;
        d->entries[i].value = NULL;
    }

    dict_free(d, NULL, NULL);

    printf("All dictionary tests passed successfully!\n");
    return 0;
}
