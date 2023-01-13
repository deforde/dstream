#ifndef MAP_H
#define MAP_H

#ifdef __cplusplus
extern "C" {
#endif

typedef char *map_key_t;
typedef void *map_value_t;

typedef struct map_kv_t map_kv_t;
struct map_kv_t {
    map_kv_t *n;
    map_key_t k;
    map_value_t v;
};

typedef struct {
    map_kv_t *kvs;
} map_t;

void mapPut(map_t *m, map_key_t k, map_value_t v);

map_value_t mapGet(map_t *m, map_key_t k);

void mapRemove(map_t *m, map_key_t k);

void mapDestroy(map_t *m);

#ifdef __cplusplus
}
#endif

#endif // MAP_H
