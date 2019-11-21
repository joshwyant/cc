#include "protected/map.h"

#include <stdlib.h>
#include <string.h>

#include "../test/stubs.h"
#include "public/assert.h"
#include "public/iterator.h"
#include "public/list.h"

// TODO:
// - Version

bool Map_init(Map *map, KeyInfo *key_info, size_t elem_size);

void Map_cleanup(Map *map);

void _Map_swap(Map *a, Map *b) {
  Map tmp;
  tmp = *a;
  *a = *b;
  *b = tmp;
}

const KeyValuePair *_Map_find_ext(const Map *map, const void *key, int hash);

int _Map_hash(const Map *map, const void *key) {
  ASSERT(map != NULL);
  ASSERT(key != NULL);

  int hash = map->key_info.hash_fn(key);
  return hash > 0 ? hash : -hash;
}

// Initializes a pre-allocated Map object with a custom capacity
bool Map_init_ext(Map *map, KeyInfo *key_info, size_t elem_size,
                  size_t capacity) {
  struct MapBucket bucket;
  if (!(map->buckets = List_alloc(sizeof(struct MapBucket)))) {
    goto error;
  }
  map->capacity = capacity;
  map->count = 0;
  map->key_info = *key_info;
  map->elem_size = elem_size;
  map->version = 0;
  if (!List_reserve(map->buckets, capacity)) {
    goto error_buckets;
  }
  for (size_t i = 0; i < capacity; i++) {
    if (NULL == (bucket.key_value_pairs = List_alloc(sizeof(KeyValuePair)))) {
      goto error_buckets;
    }
    if (!List_add(map->buckets, &bucket)) {
      goto error_buckets;
    }
  }
  return true;
error_buckets:
  free(map->buckets);
  map->buckets = NULL;
error:
  return false;
}

// Creates a new Map object.
Map *Map_alloc(KeyInfo *key_info, size_t elem_size) {
  ASSERT(key_info != NULL);
  ASSERT(elem_size > 0);

  Map *map;
  if ((map = malloc(sizeof(Map))) == NULL) {
    goto error;
  }
  map->buckets = NULL;
  if (!Map_init(map, key_info, elem_size)) {
    goto error_map;
  }
  return map;
error_map:
  free(map);
error:
  return NULL;
}

// Initializes a pre-allocated Map object
bool Map_init(Map *map, KeyInfo *key_info, size_t elem_size) {
  ASSERT(map != NULL);
  ASSERT(key_info != NULL);
  ASSERT(elem_size > 0);

  return Map_init_ext(map, key_info, elem_size, 4);
}

// Gets the number of elements in the Map
size_t Map_count(const Map *map) {
  ASSERT(map != NULL);

  return map->count;
}

// Returns whether the map is empty
bool Map_empty(const Map *map) {
  ASSERT(map != NULL);

  return map->count == 0;
}

// Frees up the Map object.
void Map_free(Map *map) {
  ASSERT(map != NULL);

  Map_cleanup(map);
  free(map);
}

// Cleans up the Map object, but does not free it.
void Map_cleanup(Map *map) {
  ASSERT(map != NULL);

  Map_clear(map);
  if (map->buckets != NULL) {
    List_free(map->buckets);
    map->buckets = NULL;
  }
  map->capacity = 0;
  map->count = 0;
}

// Gets the size of a value element in the Map
size_t Map_element_size(const Map *map) {
  ASSERT(map != NULL);

  return map->elem_size;
}

// Gets the key info for the Map
const KeyInfo *Map_key_info(const Map *map) {
  ASSERT(map != NULL);

  return &map->key_info;
}

bool Map_resize(Map *map, size_t new_capacity) {
  bool status = false;
  if (map->capacity >= new_capacity) {
    status = true;
    goto out; // Nothing to do.
  }
  Map *new_map = NULL;
  if ((new_map = malloc(sizeof(Map))) == NULL) {
    goto out;
  }
  if (!Map_init_ext(new_map, &map->key_info, map->elem_size, new_capacity)) {
    goto out_new_map;
  }
  struct MapBucket *bucket;
  KeyValuePair ikvp;
  size_t nbuckets = List_count(map->buckets);
  for (size_t i = 0; i < nbuckets; i++) {
    bucket = List_get(map->buckets, i);
    size_t nitems = List_count(bucket->key_value_pairs);
    for (size_t j = 0; j < nitems; j++) {
      ikvp = *(KeyValuePair *)List_get(bucket->key_value_pairs, j);
      if (Map_add(new_map, ikvp.key, ikvp.value).key == NULL) {
        goto out_new_map;
      }
    }
  }
  // Put the new map in place of the old map.
  _Map_swap(map, new_map);
  status = true;
out_new_map:
  Map_free(new_map);
out:
  return status;
}

// Copies the key and value to the map and returns pointer to new key/value
// pair. Returns NULL in the key if unsuccessful.
const KeyValuePair Map_add(Map *map, const void *key, const void *data) {
  bool status = false;
  KeyValuePair null_kvp = {NULL, NULL};
  KeyValuePair kvp;
  ASSERT(map != NULL);
  ASSERT(key != NULL);
  ASSERT(data != NULL);

  if (!map->buckets) {
    // Map needs to be re-initialized.
    if (!Map_init(map, &map->key_info, map->elem_size)) {
      goto out;
    }
  }
  const KeyValuePair *pkvp;
  int hash = _Map_hash(map, key);
  if ((pkvp = _Map_find_ext(map, key, hash)) == NULL) {
    size_t ibucket = hash % List_count(map->buckets);
    struct MapBucket *bucket = List_get(map->buckets, ibucket);
    if (!(kvp.key = malloc(map->key_info.key_size))) {
      goto out;
    }
    if (!(kvp.value = malloc(map->elem_size))) {
      goto out_key;
    }
    if (!List_add(bucket->key_value_pairs, &kvp)) {
      goto out_value;
    }
    map->count++;
  }
  // Overwrite any key and value for good measure.
  memcpy(kvp.key, key, map->key_info.key_size);
  memcpy(kvp.value, data, map->elem_size);

  // Have we grown too big?
  if (map->count > map->capacity) {
    if (!Map_resize(map, map->capacity << 1)) {
      // Print an error and ignore
      fprintf(stderr, "Unable to expand capacity of map to %016llx",
              (unsigned long long)(map->capacity << 1));
    }
  }
  status = true;
out_value:
  free(kvp.value);
out_key:
  free(kvp.key);
out:
  return status ? kvp : null_kvp;
}

const KeyValuePair *_Map_find_ext(const Map *map, const void *key, int hash) {
  ASSERT(map != NULL);
  ASSERT(key != NULL);

  size_t ibucket = hash % map->capacity;
  struct MapBucket *bucket = List_get(map->buckets, ibucket);
  size_t nitems = List_count(bucket->key_value_pairs);
  for (size_t i = 0; i < nitems; i++) {
    KeyValuePair *kvp = List_get(bucket->key_value_pairs, i);
    int ihash = _Map_hash(map, kvp->key);
    if (ihash == hash && map->key_info.eq_fn(kvp->key, key)) {
      return kvp;
    }
  }
  return NULL;
}

// Looks up the key in map and returns a key/value pair.
const KeyValuePair Map_find(const Map *map, const void *key) {
  ASSERT(map != NULL);
  ASSERT(key != NULL);

  KeyValuePair null_kvp = {NULL, NULL};
  const KeyValuePair *pkvp;
  int hash = _Map_hash(map, key);
  if (!(pkvp = _Map_find_ext(map, key, hash))) {
    return null_kvp;
  }
  return *pkvp;
}

// Looks up the key in map and stores the data in the given location.
// Returns whether successful.
bool Map_get(const Map *map, const void *key, void *data_out) {
  ASSERT(map != NULL);
  ASSERT(key != NULL);
  ASSERT(data_out != NULL);

  KeyValuePair kvp;
  if (NULL == (kvp = Map_find(map, key)).key) {
    return false;
  }
  memcpy(data_out, kvp.value, map->elem_size);
  return true;
}

// Checks whether the given key exists in the map.
bool Map_exists(const Map *map, const void *key) {
  ASSERT(map != NULL);
  ASSERT(key != NULL);

  return Map_find(map, key).key != NULL;
}

// Removes an item from the map.
void Map_delete(Map *map, const void *key) {
  ASSERT(map != NULL);
  ASSERT(key != NULL);

  int hash = _Map_hash(map, key);
  size_t ibucket = hash % map->capacity;
  struct MapBucket *bucket = List_get(map->buckets, ibucket);
  size_t nitems = List_count(bucket->key_value_pairs);
  for (size_t i = 0; i < nitems; i++) {
    KeyValuePair *kvp = List_get(bucket->key_value_pairs, i);
    int ihash = _Map_hash(map, kvp->key);
    if (ihash == hash && map->key_info.eq_fn(kvp->key, key)) {
      free(kvp->key);
      free(kvp->value);
      List_remove(bucket->key_value_pairs, i);
      map->count--;
      return;
    }
  }
}

// Removes all the items from the map.
// Remember to clean up memory first.
void Map_clear(Map *map) {
  ASSERT(map != NULL);

  struct MapBucket *bucket;
  List *kvps;
  KeyValuePair *kvp;
  for (size_t i = 0; i < map->capacity; i++) {
    bucket = List_get(map->buckets, i);
    kvps = bucket->key_value_pairs;
    size_t nkvps = List_count(kvps);
    for (size_t j = 0; j < nkvps; j++) {
      kvp = List_get(kvps, j);
      free(kvp->key);
      free(kvp->value);
    }
    List_clear(kvps);
  }
}

// Copies a map. Returns whether successful.
bool Map_copy(Map *dest_map, const Map *map) {
  ASSERT(dest_map != NULL);
  ASSERT(map != NULL);

  Map_clear(dest_map);
  if (!Map_resize(dest_map, map->capacity)) {
    return false;
  }
  KeyValuePair *kvp;
  for (size_t i = 0; i < map->capacity; i++) {
    struct MapBucket *bucket = List_get(map->buckets, i);
    size_t nitems = List_count(bucket->key_value_pairs);
    for (size_t j = 0; j < nitems; j++) {
      kvp = List_get(bucket->key_value_pairs, j);
      if (!Map_add(dest_map, kvp->key, kvp->value).key) {
        return false;
      }
    }
  }
  return true;
}

// Creates the union of two maps.
bool Map_union(Map *dest_map, const Map *a, const Map *b) {
  ASSERT(dest_map != NULL);
  ASSERT(a != NULL);
  ASSERT(b != NULL);

  KeyValuePair *kvp;
  for (size_t i = 0; i < a->capacity; i++) {
    struct MapBucket *bucket = List_get(a->buckets, i);
    size_t nitems = List_count(bucket->key_value_pairs);
    for (size_t j = 0; j < nitems; j++) {
      kvp = List_get(bucket->key_value_pairs, j);
      if (!Map_add(dest_map, kvp->key, kvp->value).key) {
        return false;
      }
    }
  }
  for (size_t i = 0; i < b->capacity; i++) {
    struct MapBucket *bucket = List_get(b->buckets, i);
    size_t nitems = List_count(bucket->key_value_pairs);
    for (size_t j = 0; j < nitems; j++) {
      kvp = List_get(bucket->key_value_pairs, j);
      if (!Map_add(dest_map, kvp->key, kvp->value).key) {
        return false;
      }
    }
  }
  return true;
}

// Creates the intersection of two maps.
bool Map_intersection(Map *dest_map, const Map *a, const Map *b) {
  ASSERT(dest_map != NULL);
  ASSERT(a != NULL);
  ASSERT(b != NULL);

  KeyValuePair *kvp;
  for (size_t i = 0; i < a->capacity; i++) {
    struct MapBucket *bucket = List_get(a->buckets, i);
    size_t nitems = List_count(bucket->key_value_pairs);
    for (size_t j = 0; j < nitems; j++) {
      kvp = List_get(bucket->key_value_pairs, j);
      if (!Map_exists(b, kvp->key))
        continue;
      if (!Map_add(dest_map, kvp->key, kvp->value).key) {
        return false;
      }
    }
  }
  return true;
}

// Creates the difference of two maps.
bool Map_difference(Map *dest_map, const Map *a, const Map *b) {
  ASSERT(dest_map != NULL);
  ASSERT(a != NULL);
  ASSERT(b != NULL);

  KeyValuePair *kvp;
  for (size_t i = 0; i < a->capacity; i++) {
    struct MapBucket *bucket = List_get(a->buckets, i);
    size_t nitems = List_count(bucket->key_value_pairs);
    for (size_t j = 0; j < nitems; j++) {
      kvp = List_get(bucket->key_value_pairs, j);
      if (Map_exists(b, kvp->key))
        continue;
      if (!Map_add(dest_map, kvp->key, kvp->value).key) {
        return false;
      }
    }
  }
  return true;
}

// Creates the symmetric difference of two maps.
bool Map_symmetric_difference(Map *dest_map, const Map *a, const Map *b) {
  ASSERT(dest_map != NULL);
  ASSERT(a != NULL);
  ASSERT(b != NULL);

  KeyValuePair *kvp;
  for (size_t i = 0; i < a->capacity; i++) {
    struct MapBucket *bucket = List_get(a->buckets, i);
    size_t nitems = List_count(bucket->key_value_pairs);
    for (size_t j = 0; j < nitems; j++) {
      kvp = List_get(bucket->key_value_pairs, j);
      if (Map_exists(b, kvp->key))
        continue;
      if (!Map_add(dest_map, kvp->key, kvp->value).key) {
        return false;
      }
    }
  }
  for (size_t i = 0; i < b->capacity; i++) {
    struct MapBucket *bucket = List_get(b->buckets, i);
    size_t nitems = List_count(bucket->key_value_pairs);
    for (size_t j = 0; j < nitems; j++) {
      kvp = List_get(bucket->key_value_pairs, j);
      if (Map_exists(a, kvp->key))
        continue;
      if (!Map_add(dest_map, kvp->key, kvp->value).key) {
        return false;
      }
    }
  }
  return true;
}

// Stores the union of two maps into the first map.
bool Map_union_with(Map *dest_map, const Map *map) {
  ASSERT(dest_map != NULL);
  ASSERT(map != NULL);

  KeyValuePair *kvp;
  for (size_t i = 0; i < map->capacity; i++) {
    struct MapBucket *bucket = List_get(map->buckets, i);
    size_t nitems = List_count(bucket->key_value_pairs);
    for (size_t j = 0; j < nitems; j++) {
      kvp = List_get(bucket->key_value_pairs, j);
      if (!Map_add(dest_map, kvp->key, kvp->value).key) {
        return false;
      }
    }
  }
  return true;
}

// Stores the intersection of two maps into the first map.
bool Map_intersect_with(Map *dest_map, const Map *map) {
  ASSERT(dest_map != NULL);
  ASSERT(map != NULL);

  KeyValuePair *kvp;
  for (size_t i = 0; i < dest_map->capacity; i++) {
    struct MapBucket *bucket = List_get(dest_map->buckets, i);
    for (size_t j = 0;
         j <
         List_count(bucket->key_value_pairs) /* inline due to modification */;
         j++) {
      kvp = List_get(bucket->key_value_pairs, j);
      if (!Map_exists(map, kvp->key)) {
        free(kvp->key);
        free(kvp->value);
        List_remove(bucket->key_value_pairs,
                    j-- /* j doesn't change after removal */);
      }
    }
  }
  return true;
}

// Stores the difference of two maps into the first map.
bool Map_difference_with(Map *dest_map, const Map *map) {
  ASSERT(dest_map != NULL);
  ASSERT(map != NULL);

  KeyValuePair *kvp;
  for (size_t i = 0; i < dest_map->capacity; i++) {
    struct MapBucket *bucket = List_get(dest_map->buckets, i);
    for (size_t j = 0;
         j <
         List_count(bucket->key_value_pairs) /* inline due to modification */;
         j++) {
      kvp = List_get(bucket->key_value_pairs, j);
      if (Map_exists(map, kvp->key)) {
        free(kvp->key);
        free(kvp->value);
        List_remove(bucket->key_value_pairs,
                    j-- /* j doesn't change after removal */);
      }
    }
  }
  return true;
}

// Stores the symmetric difference of two maps into the first map.
bool Map_symmetric_difference_with(Map *dest_map, const Map *map) {
  ASSERT(dest_map != NULL);
  ASSERT(map != NULL);

  KeyValuePair *kvp;
  for (size_t i = 0; i < map->capacity; i++) {
    struct MapBucket *bucket = List_get(map->buckets, i);
    size_t nitems = List_count(bucket->key_value_pairs);
    for (size_t j = 0; j < nitems; j++) {
      kvp = List_get(bucket->key_value_pairs, j);
      if (Map_exists(dest_map, kvp->key)) {
        Map_delete(dest_map, kvp->key);
      } else {
        if (!Map_add(dest_map, kvp->key, kvp->value).key) {
          return false;
        }
      }
    }
  }
  return true;
}

void *Map_iter_current_(const Iterator *iter);
bool Map_iter_eof_(const Iterator *iter);
bool Map_iter_move_next_(Iterator *iter);

void *Map_iter_current_(const Iterator *iter) {
  ASSERT(iter != NULL);
  ASSERT(iter->collection_type == COLLECTION_MAP);
  Map *map = iter->collection;
  ASSERT(map != NULL);
  ASSERT(iter->version == map->version &&
         "Collection changed while iterating.");
  if (Map_iter_eof_(iter)) {
    return NULL;
  }
  return ((KeyValuePair *)List_get(
      ((struct MapBucket *)List_get(map->buckets, iter->impl_data1))
          ->key_value_pairs,
      iter->impl_data2));
}

void *Map_key_iter_current_(const Iterator *iter) {
  ASSERT(iter != NULL);
  ASSERT(iter->collection_type == COLLECTION_MAP);
  Map *map = iter->collection;
  ASSERT(map != NULL);
  ASSERT(iter->version == map->version &&
         "Collection changed while iterating.");
  if (Map_iter_eof_(iter)) {
    return NULL;
  }
  return ((KeyValuePair *)List_get(
              ((struct MapBucket *)List_get(map->buckets, iter->impl_data1))
                  ->key_value_pairs,
              iter->impl_data2))
      ->key;
}

void *Map_value_iter_current_(const Iterator *iter) {
  ASSERT(iter != NULL);
  ASSERT(iter->collection_type == COLLECTION_MAP);
  Map *map = iter->collection;
  ASSERT(map != NULL);
  ASSERT(iter->version == map->version &&
         "Collection changed while iterating.");
  if (Map_iter_eof_(iter)) {
    return NULL;
  }
  return ((KeyValuePair *)List_get(
              ((struct MapBucket *)List_get(map->buckets, iter->impl_data1))
                  ->key_value_pairs,
              iter->impl_data2))
      ->value;
}

bool Map_iter_eof_(const Iterator *iter) {
  ASSERT(iter != NULL);
  ASSERT(iter->collection_type == COLLECTION_MAP);
  Map *map = iter->collection;
  ASSERT(map != NULL);
  ASSERT(iter->version == map->version &&
         "Collection changed while iterating.");
  return Map_empty(map) || iter->impl_data1 >= (int)map->capacity;
}

bool Map_iter_move_next_(Iterator *iter) {
  ASSERT(iter != NULL);
  ASSERT(iter->collection_type == COLLECTION_MAP);
  Map *map = iter->collection;
  ASSERT(map != NULL);
  ASSERT(iter->version == map->version &&
         "Collection changed while iterating.");
  if (Map_iter_eof_(iter)) {
    return false;
  }
  iter->impl_data2++;
  while (iter->impl_data1 == -1 ||
         iter->impl_data2 >= List_count(((struct MapBucket *)List_get(
                                            map->buckets, iter->impl_data1))
                                           ->key_value_pairs)) {
    iter->impl_data1++;
    iter->impl_data2 = 0;
    if (Map_iter_eof_(iter))
      break;
  }
  return !Map_iter_eof_(iter);
}

// Gets a key/value Iterator for this Map in an undefined order.
void Map_get_iterator(const Map *map, Iterator *iter) {
  ASSERT(map != NULL);
  ASSERT(iter != NULL);

  iter->collection_type = COLLECTION_MAP;
  iter->collection = (void *)map;
  iter->elem_size = map->elem_size;
  iter->current = Map_iter_current_;
  iter->move_next = Map_iter_move_next_;
  iter->eof = Map_iter_eof_;
  iter->impl_data1 = -1;
  iter->impl_data2 = 0;
  iter->version = 0;
}

// Gets a key Iterator for this Map in an undefined order.
void Map_get_key_iterator(const Map *map, Iterator *iter) {
  ASSERT(map != NULL);
  ASSERT(iter != NULL);

  iter->collection_type = COLLECTION_MAP;
  iter->collection = (void *)map;
  iter->elem_size = map->elem_size;
  iter->current = Map_key_iter_current_;
  iter->move_next = Map_iter_move_next_;
  iter->eof = Map_iter_eof_;
  iter->impl_data1 = -1;
  iter->impl_data2 = 0;
  iter->version = 0;
}

// Gets a value Iterator for this Map in an undefined order.
void Map_get_value_iterator(const Map *map, Iterator *iter) {
  ASSERT(map != NULL);
  ASSERT(iter != NULL);

  iter->collection_type = COLLECTION_MAP;
  iter->collection = (void *)map;
  iter->elem_size = map->elem_size;
  iter->current = Map_value_iter_current_;
  iter->move_next = Map_iter_move_next_;
  iter->eof = Map_iter_eof_;
  iter->impl_data1 = -1;
  iter->impl_data2 = 0;
  iter->version = 0;
}
