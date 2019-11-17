#include "map.h"

#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "iterator.h"
#include "list.h"
#include "../test/stubs.h"

// TODO: 
// - Version

struct Map {
    List *buckets;
    KeyInfo key_info;
    size_t elem_size;
    size_t capacity;
    size_t count;
    int version;
};

struct MapBucket {
    List *key_value_pairs;
};

void _map_swap(Map *a, Map *b)
{
    Map tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}

const KeyValuePair *_map_find_ext(const Map *map, const void *key, int hash);

int _map_hash(const Map *map, const void *key)
{
    ASSERT(map != NULL);
    ASSERT(key != NULL);

    int hash = (*map->key_info.hash_fn)(key);
    return hash > 0 ? hash : -hash;
}

// Initializes a pre-allocated Map object with a custom capacity
bool map_init_ext(Map *map, KeyInfo *key_info, size_t elem_size, size_t capacity)
{
    struct MapBucket bucket;
    if (map->buckets == NULL && !(map->buckets = list_alloc(sizeof(struct MapBucket)))) {
        return false;
    }
    map->capacity = capacity;
    map->count = 0;
    map->key_info = *key_info;
    map->elem_size = elem_size;
    if (!list_reserve(map->buckets, capacity)) {
        free(map->buckets);
        map->buckets = NULL;
        return false;
    }
    for (size_t i = 0; i < capacity; i++) {
        if (NULL == (bucket.key_value_pairs = list_alloc(sizeof(KeyValuePair)))) {
            free(map->buckets);
            map->buckets = NULL;
            return false;
        }
        if (!list_append(map->buckets, &bucket)) {
            free(map->buckets);
            map->buckets = NULL;
            return false;
        }
    }
    return true;
}

// Creates a new Map object.
Map *map_alloc(KeyInfo *key_info, size_t elem_size)
{
    ASSERT(key_info != NULL);
    ASSERT(elem_size > 0);

    Map *map = malloc(sizeof(Map));
    if (map == NULL) {
        return NULL;
    }
    map->buckets = NULL;
    if (!map_init(map, key_info, elem_size)) {
        free(map);
        return NULL;
    }
    return map;
}

// Initializes a pre-allocated Map object
bool map_init(Map *map, KeyInfo *key_info, size_t elem_size)
{
    ASSERT(map != NULL);
    ASSERT(key_info != NULL);
    ASSERT(elem_size > 0);
    
    return map_init_ext(map, key_info, elem_size, 4);
}

// Gets the number of elements in the Map
size_t map_size(const Map *map)
{
    ASSERT(map != NULL);
    
    return map->count;
}

// Returns whether the map is empty
bool map_empty(const Map *map)
{
    ASSERT(map != NULL);
    
    return map->count == 0;
}

// Frees up the Map object.
void map_free(Map *map)
{
    ASSERT(map != NULL);

    map_cleanup(map);
    free(map);
}

// Cleans up the Map object, but does not free it.
void map_cleanup(Map *map)
{
    ASSERT(map != NULL);

    map_clear(map);
    if (map->buckets != NULL) {
        list_free(map->buckets);
        map->buckets = NULL;
    }
    map->capacity = 0;
    map->count = 0;
}

// Gets the size of a value element in the Map
size_t map_element_size(const Map *map)
{
    ASSERT(map != NULL);

    return map->elem_size;
}

// Gets the key info for the Map
const KeyInfo *map_key_info(const Map *map)
{
    ASSERT(map != NULL);

    return &map->key_info;
}

bool map_resize(Map *map, size_t new_capacity) {
    if (map->capacity >= new_capacity) {
        return true; // Nothing to do.
    }
    Map *new_map;
    if ((new_map = malloc(sizeof(Map))) == NULL) {
        return false;
    }
    if (!map_init_ext(new_map, &map->key_info, map->elem_size, new_capacity)) {
        free(new_map);
        return false;
    }
    struct MapBucket *bucket;
    KeyValuePair ikvp;
    size_t nbuckets = list_size(map->buckets);
    for (size_t i = 0; i < nbuckets; i++) {
        bucket = list_get(map->buckets, i);
        size_t nitems = list_size(bucket->key_value_pairs);
        for (size_t j = 0; j < nitems; j++) {
            ikvp = *(KeyValuePair*)list_get(bucket->key_value_pairs, j);
            if (map_add(new_map, ikvp.key, ikvp.value).key == NULL) {
                map_free(new_map);
                return false;
            }
        }
    }
    // Put the new map in place of the old map.
    _map_swap(map, new_map);
    map_free(new_map);
    return true;
}

// Copies the key and value to the map and returns pointer to new key/value pair.
// Returns NULL in the key if unsuccessful.
const KeyValuePair map_add(Map *map, const void *key, const void *data)
{
    ASSERT(map != NULL);
    ASSERT(key != NULL);
    ASSERT(data != NULL);

    if (!map->buckets) {
        // Map needs to be re-initialized.
        map_init(map, &map->key_info, map->elem_size);
    }

    KeyValuePair null_kvp = { NULL, NULL };
    KeyValuePair kvp;
    const KeyValuePair *pkvp;
    int hash = _map_hash(map, key);
    if ((pkvp = _map_find_ext(map, key, hash)) == NULL) {
        kvp = *pkvp;
        size_t ibucket = hash % list_size(map->buckets);
        struct MapBucket *bucket = list_get(map->buckets, ibucket);
        if (!(kvp.key = malloc(map->key_info.key_size))) {
            return null_kvp;
        }
        if (!(kvp.value = malloc(map->elem_size))) {
            free(kvp.key);
            return null_kvp;
        }
        if (!list_append(bucket->key_value_pairs, &kvp)) {
            free(kvp.key);
            free(kvp.value);
            return null_kvp;
        }
        map->count++;
    }
    // Overwrite any key and value for good measure.
    memcpy(kvp.key, key, map->key_info.key_size);
    memcpy(kvp.value, data, map->elem_size);

    // Have we grown too big?
    if (map->count > map->capacity) {
        if (!map_resize(map, map->capacity << 1)) {
            // Print an error and ignore
            fprintf(stderr, "Unable to expand capacity of map to %016llx", (unsigned long long)(map->capacity << 1));
        }
    }
    return kvp;
}

const KeyValuePair *_map_find_ext(const Map *map, const void *key, int hash)
{
    ASSERT(map != NULL);
    ASSERT(key != NULL);

    size_t ibucket = hash % map->capacity;
    struct MapBucket *bucket = list_get(map->buckets, ibucket);
    size_t nitems = list_size(bucket->key_value_pairs);
    for (size_t i = 0; i < nitems; i++) {
        KeyValuePair *kvp = list_get(bucket->key_value_pairs, i);
        int ihash = _map_hash(map, kvp->key);
        if (ihash == hash && (*map->key_info.eq_fn)(kvp->key, key)) {
            return kvp;
        }
    }
    return NULL;
}

// Looks up the key in map and returns a key/value pair.
const KeyValuePair map_find(const Map *map, const void *key)
{
    ASSERT(map != NULL);
    ASSERT(key != NULL);

    KeyValuePair null_kvp = { NULL, NULL };
    const KeyValuePair *pkvp;
    int hash = _map_hash(map, key);
    if (!(pkvp = _map_find_ext(map, key, hash))) {
        return null_kvp;
    }
    return *pkvp;
}

// Looks up the key in map and stores the data in the given location.
// Returns whether successful.
bool map_get(const Map *map, const void *key, void *data_out)
{
    ASSERT(map != NULL);
    ASSERT(key != NULL);
    ASSERT(data_out != NULL);

    KeyValuePair kvp;
    if (NULL == (kvp = map_find(map, key)).key) {
        return false;
    }
    memcpy(data_out, kvp.value, map->elem_size);
    return true;
}

// Checks whether the given key exists in the map.
bool map_exists(const Map *map, const void *key)
{
    ASSERT(map != NULL);
    ASSERT(key != NULL);

    return map_find(map, key).key != NULL;
}

// Removes an item from the map.
void map_delete(Map *map, const void *key)
{
    ASSERT(map != NULL);
    ASSERT(key != NULL);

    int hash = _map_hash(map, key);
    size_t ibucket = hash % map->capacity;
    struct MapBucket *bucket = list_get(map->buckets, ibucket);
    size_t nitems = list_size(bucket->key_value_pairs);
    for (size_t i = 0; i < nitems; i++) {
        KeyValuePair *kvp = list_get(bucket->key_value_pairs, i);
        int ihash = _map_hash(map, kvp->key);
        if (ihash == hash && (*map->key_info.eq_fn)(kvp->key, key)) {
            free(kvp->key);
            free(kvp->value);
            list_remove(bucket->key_value_pairs, i);
            map->count--;
            return;
        }
    }
}

// Removes all the items from the map.
// Remember to clean up memory first.
void map_clear(Map *map)
{
    ASSERT(map != NULL);

    List *kvps;
    KeyValuePair *kvp;
    for (size_t i = 0; i < map->capacity; i++) {
        kvps = list_get(map->buckets, i);
        size_t nkvps = list_size(kvps);
        for (size_t j = 0; j < nkvps; j++) {
            kvp = list_get(kvps, j);
            free(kvp->key);
            free(kvp->value);
        }
        list_clear(kvps);
    }
}

// Copies a map. Returns whether successful.
bool map_copy(Map *dest_map, const Map *map)
{
    ASSERT(dest_map != NULL);
    ASSERT(map != NULL);

    map_clear(dest_map);
    if (!map_resize(dest_map, map->capacity)) {
        return false;
    }
    KeyValuePair *kvp;
    for (size_t i = 0; i < map->capacity; i++) {
        struct MapBucket *bucket = list_get(map->buckets, i);
        size_t nitems = list_size(bucket->key_value_pairs);
        for (size_t j = 0; j < nitems; j++) {
            kvp = list_get(bucket->key_value_pairs, j);
            if (!map_add(dest_map, kvp->key, kvp->value).key) {
                return false;
            }
        }
    }
    return true;
}

// Creates the union of two maps.
bool map_union(Map *dest_map, const Map *a, const Map *b)
{
    ASSERT(dest_map != NULL);
    ASSERT(a != NULL);
    ASSERT(b != NULL);

    KeyValuePair *kvp;
    for (size_t i = 0; i < a->capacity; i++) {
        struct MapBucket *bucket = list_get(a->buckets, i);
        size_t nitems = list_size(bucket->key_value_pairs);
        for (size_t j = 0; j < nitems; j++) {
            kvp = list_get(bucket->key_value_pairs, j);
            if (!map_add(dest_map, kvp->key, kvp->value).key) {
                return false;
            }
        }
    }
    for (size_t i = 0; i < b->capacity; i++) {
        struct MapBucket *bucket = list_get(b->buckets, i);
        size_t nitems = list_size(bucket->key_value_pairs);
        for (size_t j = 0; j < nitems; j++) {
            kvp = list_get(bucket->key_value_pairs, j);
            if (!map_add(dest_map, kvp->key, kvp->value).key) {
                return false;
            }
        }
    }
    return true;
}

// Creates the intersection of two maps.
bool map_intersection(Map *dest_map, const Map *a, const Map *b)
{
    ASSERT(dest_map != NULL);
    ASSERT(a != NULL);
    ASSERT(b != NULL);

    KeyValuePair *kvp;
    for (size_t i = 0; i < a->capacity; i++) {
        struct MapBucket *bucket = list_get(a->buckets, i);
        size_t nitems = list_size(bucket->key_value_pairs);
        for (size_t j = 0; j < nitems; j++) {
            kvp = list_get(bucket->key_value_pairs, j);
            if (!map_exists(b, kvp->key)) continue;
            if (!map_add(dest_map, kvp->key, kvp->value).key) {
                return false;
            }
        }
    }
    return true;
}

// Creates the difference of two maps.
bool map_difference(Map *dest_map, const Map *a, const Map *b)
{
    ASSERT(dest_map != NULL);
    ASSERT(a != NULL);
    ASSERT(b != NULL);

    KeyValuePair *kvp;
    for (size_t i = 0; i < a->capacity; i++) {
        struct MapBucket *bucket = list_get(a->buckets, i);
        size_t nitems = list_size(bucket->key_value_pairs);
        for (size_t j = 0; j < nitems; j++) {
            kvp = list_get(bucket->key_value_pairs, j);
            if (map_exists(b, kvp->key)) continue;
            if (!map_add(dest_map, kvp->key, kvp->value).key) {
                return false;
            }
        }
    }
    return true;
}

// Creates the symmetric difference of two maps.
bool map_symmetric_difference(Map *dest_map, const Map *a, const Map *b)
{
    ASSERT(dest_map != NULL);
    ASSERT(a != NULL);
    ASSERT(b != NULL);

    KeyValuePair *kvp;
    for (size_t i = 0; i < a->capacity; i++) {
        struct MapBucket *bucket = list_get(a->buckets, i);
        size_t nitems = list_size(bucket->key_value_pairs);
        for (size_t j = 0; j < nitems; j++) {
            kvp = list_get(bucket->key_value_pairs, j);
            if (map_exists(b, kvp->key)) continue;
            if (!map_add(dest_map, kvp->key, kvp->value).key) {
                return false;
            }
        }
    }
    for (size_t i = 0; i < b->capacity; i++) {
        struct MapBucket *bucket = list_get(b->buckets, i);
        size_t nitems = list_size(bucket->key_value_pairs);
        for (size_t j = 0; j < nitems; j++) {
            kvp = list_get(bucket->key_value_pairs, j);
            if (map_exists(a, kvp->key)) continue;
            if (!map_add(dest_map, kvp->key, kvp->value).key) {
                return false;
            }
        }
    }
    return true;
}

// Stores the union of two maps into the first map.
bool map_union_with(Map *dest_map, const Map *map)
{
    ASSERT(dest_map != NULL);
    ASSERT(map != NULL);

    KeyValuePair *kvp;
    for (size_t i = 0; i < map->capacity; i++) {
        struct MapBucket *bucket = list_get(map->buckets, i);
        size_t nitems = list_size(bucket->key_value_pairs);
        for (size_t j = 0; j < nitems; j++) {
            kvp = list_get(bucket->key_value_pairs, j);
            if (!map_add(dest_map, kvp->key, kvp->value).key) {
                return false;
            }
        }
    }
    return true;
}

// Stores the intersection of two maps into the first map.
bool map_intersect_with(Map *dest_map, const Map *map)
{
    ASSERT(dest_map != NULL);
    ASSERT(map != NULL);

    KeyValuePair *kvp;
    for (size_t i = 0; i < dest_map->capacity; i++) {
        struct MapBucket *bucket = list_get(dest_map->buckets, i);
        for (size_t j = 0; j < list_size(bucket->key_value_pairs) /* inline due to modification */; j++) {
            kvp = list_get(bucket->key_value_pairs, j);
            if (!map_exists(map, kvp->key)) {
                free(kvp->key);
                free(kvp->value);
                list_remove(bucket->key_value_pairs, j-- /* j doesn't change after removal */);
            }
        }
    }
    return true;
}

// Stores the difference of two maps into the first map.
bool map_difference_with(Map *dest_map, const Map *map)
{
    ASSERT(dest_map != NULL);
    ASSERT(map != NULL);

    KeyValuePair *kvp;
    for (size_t i = 0; i < dest_map->capacity; i++) {
        struct MapBucket *bucket = list_get(dest_map->buckets, i);
        for (size_t j = 0; j < list_size(bucket->key_value_pairs) /* inline due to modification */; j++) {
            kvp = list_get(bucket->key_value_pairs, j);
            if (map_exists(map, kvp->key)) {
                free(kvp->key);
                free(kvp->value);
                list_remove(bucket->key_value_pairs, j-- /* j doesn't change after removal */);
            }
        }
    }
    return true;
}

// Stores the symmetric difference of two maps into the first map.
bool map_symmetric_difference_with(Map *dest_map, const Map *map)
{
    ASSERT(dest_map != NULL);
    ASSERT(map != NULL);

    KeyValuePair *kvp;
    for (size_t i = 0; i < map->capacity; i++) {
        struct MapBucket *bucket = list_get(map->buckets, i);
        size_t nitems = list_size(bucket->key_value_pairs);
        for (size_t j = 0; j < nitems; j++) {
            kvp = list_get(bucket->key_value_pairs, j);
            if (map_exists(dest_map, kvp->key)) {
                map_delete(dest_map, kvp->key);
            } else {
                if (!map_add(dest_map, kvp->key, kvp->value).key) {
                    return false;
                }
            }
        }
    }
    return true;
}

void *map_iter_current_(const Iterator *iter);
bool map_iter_eof_(const Iterator *iter);
bool map_iter_move_next_(Iterator *iter);

void *map_iter_current_(const Iterator *iter) {
    ASSERT(iter != NULL);
    ASSERT(iter->collection_type == COLLECTION_MAP);
    Map *map = iter->collection;
    ASSERT(map != NULL);
    ASSERT(iter->version == map->version && "Collection changed while iterating.");
    if (map_iter_eof_(iter)) {
        return NULL;
    }
    return ((KeyValuePair*)list_get(((struct MapBucket*)list_get(map->buckets, iter->impl_data1))->key_value_pairs, iter->impl_data2));
}

void *map_key_iter_current_(const Iterator *iter) {
    ASSERT(iter != NULL);
    ASSERT(iter->collection_type == COLLECTION_MAP);
    Map *map = iter->collection;
    ASSERT(map != NULL);
    ASSERT(iter->version == map->version && "Collection changed while iterating.");
    if (map_iter_eof_(iter)) {
        return NULL;
    }
    return ((KeyValuePair*)list_get(((struct MapBucket*)list_get(map->buckets, iter->impl_data1))->key_value_pairs, iter->impl_data2))->key;
}

void *map_value_iter_current_(const Iterator *iter) {
    ASSERT(iter != NULL);
    ASSERT(iter->collection_type == COLLECTION_MAP);
    Map *map = iter->collection;
    ASSERT(map != NULL);
    ASSERT(iter->version == map->version && "Collection changed while iterating.");
    if (map_iter_eof_(iter)) {
        return NULL;
    }
    return ((KeyValuePair*)list_get(((struct MapBucket*)list_get(map->buckets, iter->impl_data1))->key_value_pairs, iter->impl_data2))->value;
}

bool map_iter_eof_(const Iterator *iter) {
    ASSERT(iter != NULL);
    ASSERT(iter->collection_type == COLLECTION_MAP);
    Map *map = iter->collection;
    ASSERT(map != NULL);
    ASSERT(iter->version == map->version && "Collection changed while iterating.");
    return map_empty(map) || iter->impl_data1 >= map->capacity;
}

bool map_iter_move_next_(Iterator *iter) {
    ASSERT(iter != NULL);
    ASSERT(iter->collection_type == COLLECTION_MAP);
    Map *map = iter->collection;
    ASSERT(map != NULL);
    ASSERT(iter->version == map->version && "Collection changed while iterating.");
    if (map_iter_eof_(iter)) {
        return false;
    }
    iter->impl_data2++;
    if (iter->impl_data2 > list_size(list_get(map->buckets, iter->impl_data1))) {
        iter->impl_data1++;
        iter->impl_data2 = 0;
    }
    return !map_iter_eof_(iter);
}

// Gets a key/value Iterator for this Map in an undefined order.
void map_get_iterator(const Map *map, Iterator *iter)
{
    ASSERT(map != NULL);
    ASSERT(iter != NULL);

    iter->collection_type = COLLECTION_MAP;
    iter->collection = (void*)map;
    iter->elem_size = map->elem_size;
    iter->current = map_iter_current_;
    iter->move_next = map_iter_move_next_;
    iter->eof = map_iter_eof_;
    iter->impl_data1 = -1;
    iter->impl_data2 = 0;
    iter->version = 0;
}

// Gets a key Iterator for this Map in an undefined order.
void map_get_key_iterator(const Map *map, Iterator *iter)
{
    ASSERT(map != NULL);
    ASSERT(iter != NULL);

    iter->collection_type = COLLECTION_MAP;
    iter->collection = (void*)map;
    iter->elem_size = map->elem_size;
    iter->current = map_key_iter_current_;
    iter->move_next = map_iter_move_next_;
    iter->eof = map_iter_eof_;
    iter->impl_data1 = -1;
    iter->impl_data2 = 0;
    iter->version = 0;
}

// Gets a value Iterator for this Map in an undefined order.
void map_get_value_iterator(const Map *map, Iterator *iter)
{
    ASSERT(map != NULL);
    ASSERT(iter != NULL);

    iter->collection_type = COLLECTION_MAP;
    iter->collection = (void*)map;
    iter->elem_size = map->elem_size;
    iter->current = map_value_iter_current_;
    iter->move_next = map_iter_move_next_;
    iter->eof = map_iter_eof_;
    iter->impl_data1 = -1;
    iter->impl_data2 = 0;
    iter->version = 0;
}
