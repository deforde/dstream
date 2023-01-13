#include "map.h"

#include <stdlib.h>
#include <string.h>

static void mapKVDestroy(map_kv_t *mkv) {
    free(mkv->k);
    free(mkv);
}

void mapPut(map_t *m, map_key_t k, map_value_t v) {
    map_kv_t *prev = NULL;
    for (map_kv_t *p = m->kvs; p; p = p->n) {
        if (strcmp(k, p->k) == 0) {
            p->v = v;
            return;
        }
        prev = p;
    }
    map_kv_t *n = calloc(1, sizeof(map_kv_t));
    n->k = strdup(k);
    n->v = v;
    if (prev) {
        prev->n = n;
    } else {
        m->kvs = n;
    }
}

map_value_t mapGet(map_t *m, map_key_t k) {
    for (map_kv_t *p = m->kvs; p; p = p->n) {
        if (strcmp(k, p->k) == 0) {
            return p->v;
        }
    }
    return NULL;
}

void mapRemove(map_t *m, map_key_t k) {
    map_kv_t *prev = NULL;
    for (map_kv_t *p = m->kvs; p; p = p->n) {
        if (strcmp(k, p->k) == 0) {
            if (prev) {
                prev->n = p->n;
            } else {
                m->kvs = p->n;
            }
            mapKVDestroy(p);
            return;
        }
        prev = p;
    }
}

void mapDestroy(map_t *m) {
    for (map_kv_t *p = m->kvs; p;) {
        map_kv_t *n = p->n;
        mapKVDestroy(p);
        p = n;
    }
}
