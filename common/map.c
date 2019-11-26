#include "protected/map.h"

#include <stdlib.h>
#include <string.h>

#include "../test/stubs.h"
#include "public/assert.h"
#include "public/iterator.h"
#include "public/vector.h"

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

void _Vector_print_MapBucket(const Vector *self, char *str, struct MapBucket *bucket) {
  char buffer[10000];
  sprintf(str, " *(Vector<KeyValuePair>)(%p): {", bucket->key_value_pairs);
  size_t len = Vector_count(bucket->key_value_pairs);
  if (len) {
    for (size_t i = 0; i < len; i++) {
      KeyValuePair kvp = *(KeyValuePair*)Vector_get(bucket->key_value_pairs, i);
      sprintf(str + strlen(str), " %zu { *key (%p): ", i, kvp.key);
      bucket->map->print_key(bucket->map, buffer, kvp.key);
      strcat(str, buffer);
      sprintf(str + strlen(str), " , *value (%p): ", kvp.value);
      bucket->map->print_value(bucket->map, buffer, kvp.value);
      strcat(str, buffer);
      strcat(str, " }");
    }
  }
  strcat(str, " }");
}

void _Map_default_key_print_fn(const Map *self, char *str, const unsigned char *elem) {
  char byte[4];
  str[0] = '\0';
  for (size_t i = 0; i < self->elem_size; i++) {
    sprintf(byte, " %02x", elem[i]);
    strcat(str, byte);
  }
}

void _Map_default_value_print_fn(const Map *self, char *str, const unsigned char *elem) {
  char byte[4];
  str[0] = '\0';
  for (size_t i = 0; i < self->key_info.key_size; i++) {
    sprintf(byte, " %02x", elem[i]);
    strcat(str, byte);
  }
}

void Map_print_key_fn(Map *map, void (*print)(const Map *self, char *target_str, const void *elem))
{
  ASSERT(map);
  ASSERT(print);
  map->print_key = print;
}

void Map_print_value_fn(Map *map, void (*print)(const Map *self, char *target_str, const void *elem))
{
  ASSERT(map);
  ASSERT(print);
  map->print_value = print;
}

void Map_print_element(const Map *map, char *dest_buffer, const char *key)
{
  char print_buffer[10000];
  dest_buffer[0] = '\0';
  KeyValuePair kvp = Map_find(map, key);
  map->print_key(map, print_buffer, kvp.key);
  sprintf(dest_buffer + strlen(dest_buffer), "\n  *key (%p): %s", kvp.key, print_buffer);
  map->print_value(map, print_buffer, kvp.value);
  sprintf(dest_buffer + strlen(dest_buffer), "\n*value (%p): %s", kvp.value, print_buffer);
}

void Map_print(const Map *map, char *buffer)
{
  buffer[0] = '\0';
  Iterator iter;
  Map_get_iterator(map, &iter);
  while (iter.move_next(&iter)) {
    sprintf(buffer + strlen(buffer), " { ");
    KeyValuePair kvp = *(KeyValuePair*)iter.current(&iter);
    Map_print_element(map, buffer + strlen(buffer), kvp.key);
    sprintf(buffer + strlen(buffer), " }");
  }
}

void _Map_log_element(const Map *map, const char *message, const char *key) {
  char buffer[10000];
  Map_print_element(map, buffer, key);
  LOG_FORMAT(TRACE, "%s: %s", message, buffer);
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
  LOG_FORMAT(TRACE,
             "Map_init_ext(*map: %p, *key_info: %p, elem_size: %zu, capacity: "
             "%zu) called...",
             map, key_info, elem_size, capacity);
  LOG_INDENT();
  struct MapBucket bucket;
  LOG(TRACE, "Allocating vector...");
  if (!(map->buckets = Vector_alloc(sizeof(struct MapBucket)))) {
    goto error;
  }
  Vector_print_fn(map->buckets, (void(*)(const Vector*, char*, const void*))_Vector_print_MapBucket);
  map->capacity = capacity;
  map->count = 0;
  map->key_info = *key_info;
  map->elem_size = elem_size;
  map->version = 0;
  map->print_key = (void (*)(const Map *, char *, const void*))_Map_default_key_print_fn;
  map->print_value = (void (*)(const Map *, char *, const void*))_Map_default_value_print_fn;
  LOG(TRACE, "Reserving capacity for vector...");
  if (!Vector_reserve(map->buckets, capacity)) {
    goto error_buckets;
  }
  LOG(TRACE, "Adding bucket key/value pair vectors...");
  LOG_INDENT();
  for (size_t i = 0; i < capacity; i++) {
    LOG(TRACE, "Allocating key/value pair vector...");
    if (NULL == (bucket.key_value_pairs = Vector_alloc(sizeof(KeyValuePair)))) {
      goto error_kvps;
    }
    bucket.map = map;
    LOG(TRACE, "Adding key/value pair vector to bucket vector...");
    if (!Vector_add(map->buckets, &bucket)) {
      goto error_kvps;
    }
  }
  LOG_DEINDENT();
  LOG_DEINDENT();
  return true;
error_kvps:
  LOG(WARN, "Error occurred. Freeing key value pairs...");
  for (size_t i = 0; i < Vector_count(map->buckets); i++) {
    Vector_free(((struct MapBucket *)Vector_get(map->buckets, i))->key_value_pairs);
  }
error_buckets:
  LOG(WARN, "Error occurred. Freeing buckets vector...");
  Vector_free(map->buckets);
  map->buckets = NULL;
  LOG_DEINDENT();
error:
  return false;
}

// Creates a new Map object.
Map *Map_alloc(KeyInfo *key_info, size_t elem_size) {
  LOG_FORMAT(TRACE, "Map_alloc(*key_info: %p, elem_size: %zu) called...",
             key_info, elem_size);
  ASSERT(key_info != NULL);
  ASSERT(elem_size > 0);
  LOG_INDENT();

  Map *map;
  LOG(TRACE, "Allocating map...");
  if ((map = malloc(sizeof(Map))) == NULL) {
    goto error;
  }
  map->buckets = NULL;
  LOG(TRACE, "Initializing map...");
  if (!Map_init(map, key_info, elem_size)) {
    goto error_map;
  }
  LOG_DEINDENT();
  return map;
error_map:
  LOG(WARN, "Error occurred. Freeing map...");
  free(map);
error:
  LOG_DEINDENT();
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
  LOG_FORMAT(TRACE, "Map_free(*map: %p) called...", map);
  ASSERT(map != NULL);
  LOG_INDENT();

  LOG(TRACE, "Cleaning up...");
  Map_cleanup(map);
  LOG(TRACE, "Freeing...");
  free(map);
  LOG_DEINDENT();
}

// Cleans up the Map object, but does not free it.
void Map_cleanup(Map *map) {
  ASSERT(map != NULL);

  LOG(TRACE, "Clearing map...");
  Map_clear(map);
  if (map->buckets != NULL) {
    for (int i = 0; i < Vector_count(map->buckets); i++) {
      Vector *kvps = ((struct MapBucket *)Vector_get(map->buckets, i))->key_value_pairs;
      LOG_FORMAT(TRACE, "Freeing key/value pair vector (%p)...", kvps);
      Vector_free(kvps);
    }
      LOG_FORMAT(TRACE, "Freeing buckets vector (%p)...", map->buckets);
    Vector_free(map->buckets);
    map->buckets = NULL;
  }
  map->capacity = 0;
  map->count = 0;
}

// Removes all the items from the map.
// Remember to clean up memory first.
void Map_clear(Map *map) {
  LOG_FORMAT(TRACE, "Map_clear(*map: %p) called...", map);
  ASSERT(map != NULL);
  LOG_INDENT();

  struct MapBucket *bucket;
  Vector *kvps;
  KeyValuePair *kvp;
  for (size_t i = 0; i < map->capacity; i++) {
    bucket = Vector_get(map->buckets, i);
    kvps = bucket->key_value_pairs;
    size_t nkvps = Vector_count(kvps);
    LOG_FORMAT(TRACE, "Freeing elements of key/value pair vector (%p)...", kvps);
    LOG_INDENT();
    for (size_t j = 0; j < nkvps; j++) {
      kvp = Vector_get(kvps, j);
      LOG_FORMAT(TRACE, "Freeing (*key: %p *value: %p)...", kvp->key, kvp->value);
      free(kvp->key);
      free(kvp->value);
    }
    LOG_DEINDENT();
    LOG_FORMAT(TRACE, "Clearing key/value pair vector (%p)...", kvps);
    Vector_clear(kvps);
  }
  LOG_DEINDENT();
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
  LOG_FORMAT(TRACE, "Map_resize(*map: %p, new_capacity: %zu) called...", map, new_capacity);
  ASSERT(map != NULL);
  LOG_INDENT();
  if (map->capacity >= new_capacity) {
    LOG_FORMAT(TRACE, "Nothing to do (capacity = %zu)", map->capacity);
    goto out; // Nothing to do.
  }
  LOG(TRACE, "Allocating temporary map...");
  Map *temp_map = NULL;
  if ((temp_map = malloc(sizeof(Map))) == NULL) {
    goto error;
  }
  LOG_FORMAT(TRACE, "Temporary map address: %p", temp_map);
  if (!Map_init_ext(temp_map, &map->key_info, map->elem_size, new_capacity)) {
    free(temp_map);
    goto error;
  }
  temp_map->print_key = map->print_key;
  temp_map->print_value = map->print_value;
  temp_map->version = map->version;
  struct MapBucket *bucket;
  KeyValuePair ikvp;
  size_t nbuckets = Vector_count(map->buckets);
  LOG(TRACE, "Adding elements into temporary map...");
  for (size_t i = 0; i < nbuckets; i++) {
    bucket = Vector_get(map->buckets, i);
    size_t nitems = Vector_count(bucket->key_value_pairs);
    for (size_t j = 0; j < nitems; j++) {
      ikvp = *(KeyValuePair *)Vector_get(bucket->key_value_pairs, j);
      if (Map_add(temp_map, ikvp.key, ikvp.value).key == NULL) {
        goto error_temp_map;
      }
    }
    LOG_FORMAT(TRACE,
               "Clearing bucket of temp map so key/value pairs are not deleted "
               "(*bucket->key_value_pairs = %p)",
               bucket->key_value_pairs);
    // Clear the bucket so the values can be reused in the new map,
    // and the old map can be cleaned up.
    Vector_clear(bucket->key_value_pairs);
  }
  // Put the new map in place of the old map, and free the old one now stored in `temp_map'.
  LOG(TRACE, "Swapping temp_map <-> map and freeing temp_map...");
  _Map_swap(map, temp_map);
  Map_free(temp_map);
out:
  LOG_DEINDENT();
  return true;
error_temp_map:
  LOG(WARN, "Error occurred. Freeing temp_map...");
  Map_free(temp_map);
error:
  LOG_DEINDENT();
  return false;
}

// Copies the key and value to the map and returns pointer to new key/value
// pair. Returns NULL in the key if unsuccessful.
const KeyValuePair Map_add(Map *map, const void *key, const void *data) {
  LOG_FORMAT(TRACE, "Map_add(*map: %p, *key: %p, *data: %p) called...", map, key, data);
  KeyValuePair null_kvp = {NULL, NULL};
  KeyValuePair kvp;
  ASSERT(map != NULL);
  ASSERT(key != NULL);
  ASSERT(data != NULL);
  LOG_INDENT();

  if (!map->buckets) {
    LOG(TRACE, "No buckets vector, re-initializing.");
    // Map needs to be re-initialized.
    if (!Map_init(map, &map->key_info, map->elem_size)) {
      goto error;
    }
  }
  const KeyValuePair *pkvp;
  int hash = _Map_hash(map, key);
  LOG_FORMAT(TRACE, "Key hash = %d", hash);
  if ((pkvp = _Map_find_ext(map, key, hash)) == NULL) {
    LOG(TRACE, "Item doesn't already exist in the map. Adding...");
    size_t ibucket = hash % Vector_count(map->buckets);
    LOG_FORMAT(TRACE, "Bucket = %zu", ibucket);
    struct MapBucket *bucket = Vector_get(map->buckets, ibucket);
    LOG(TRACE, "Allocating space for key...");
    if (!(kvp.key = malloc(map->key_info.key_size))) {
      goto error;
    }
    LOG(TRACE, "Allocating space for value...");
    if (!(kvp.value = malloc(map->elem_size))) {
      goto error_key;
    }
    LOG_FORMAT(TRACE, "Adding key/value to the bucket (*key: %p, *value: %p)...",
        kvp.key, kvp.value);
    if (!Vector_add(bucket->key_value_pairs, &kvp)) {
      goto error_value;
    }
    map->count++;
  } else {
    LOG(TRACE, "Item already exists in the map.");
  }
  LOG(TRACE, "Copying key and value...");
  // Overwrite any key and value for good measure.
  memcpy(kvp.key, key, map->key_info.key_size);
  memcpy(kvp.value, data, map->elem_size);

  // Have we grown too big?
  if (map->count > map->capacity) {
    LOG(TRACE, "Map is getting too big. Resizing...");
    if (!Map_resize(map, map->capacity << 1)) {
      // Print an error and ignore
      LOG_FORMAT(WARN, "Unable to expand capacity of map to %016llx",
              (unsigned long long)(map->capacity << 1));
    }
  }
  _Map_log_element(map, "Added item with", kvp.key);
  LOG_DEINDENT();
  return kvp;
error_value:
  LOG_FORMAT(TRACE, "Error occurred. Freeing value pointer (%p)...", kvp.value);
  free(kvp.value);
error_key:
  LOG_FORMAT(TRACE, "freeing key pointer (%p)...", kvp.value);
  free(kvp.key);
error:
  LOG_DEINDENT();
  return null_kvp;
}

const KeyValuePair *_Map_find_ext(const Map *map, const void *key, int hash) {
  ASSERT(map != NULL);
  ASSERT(key != NULL);

  size_t ibucket = hash % map->capacity;
  struct MapBucket *bucket = Vector_get(map->buckets, ibucket);
  size_t nitems = Vector_count(bucket->key_value_pairs);
  for (size_t i = 0; i < nitems; i++) {
    KeyValuePair *kvp = Vector_get(bucket->key_value_pairs, i);
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
bool Map_contains_key(const Map *map, const void *key) {
  ASSERT(map != NULL);
  ASSERT(key != NULL);

  return Map_find(map, key).key != NULL;
}

// Removes an item from the map.
void Map_delete(Map *map, const void *key) {
  LOG_FORMAT(TRACE, "Map_delete(*map: %p, *key: %p) called...", map, key);
  LOG_INDENT();
  ASSERT(map != NULL);
  ASSERT(key != NULL);

  int hash = _Map_hash(map, key);
  size_t ibucket = hash % map->capacity;
  LOG_FORMAT(TRACE, "Key hash = %d", hash);
  LOG_FORMAT(TRACE, "Bucket = %zu", ibucket);
  struct MapBucket *bucket = Vector_get(map->buckets, ibucket);
  size_t nitems = Vector_count(bucket->key_value_pairs);
  LOG_FORMAT(TRACE, "Scanning %zu items in bucket...", nitems);
  for (size_t i = 0; i < nitems; i++) {
    KeyValuePair *kvp = Vector_get(bucket->key_value_pairs, i);
    int ihash = _Map_hash(map, kvp->key);
    if (ihash == hash && map->key_info.eq_fn(kvp->key, key)) {
      LOG_FORMAT(TRACE, "Item found at index %zu in bucket. Freeing key (%p)...", i, kvp->key);
      free(kvp->key);
      LOG_FORMAT(TRACE, "Freeing value (%p)...", kvp->value);
      free(kvp->value);
      LOG_FORMAT(TRACE, "Removing item in bucket vector at index %zu...", i);
      Vector_remove(bucket->key_value_pairs, i);
      map->count--;
      LOG_DEINDENT();
      return;
    }
  }
  LOG_FORMAT(TRACE, "Key not found to delete (%p).", key);
  LOG_DEINDENT();
}

// Copies a map. Returns whether successful.
bool Map_copy(Map *dest_map, const Map *map) {
  LOG_FORMAT(TRACE, "Map_copy(*dest_map: %p, *map: %p) called...", dest_map, map);
  LOG_INDENT();
  ASSERT(dest_map != NULL);
  ASSERT(map != NULL);

  LOG(TRACE, "Clearing destination map...");
  Map_clear(dest_map);
  LOG(TRACE, "Resizing destination map...");
  if (!Map_resize(dest_map, map->capacity)) {
    goto error;
  }
  LOG(TRACE, "Copying items...");
  KeyValuePair *kvp;
  for (size_t i = 0; i < map->capacity; i++) {
    struct MapBucket *bucket = Vector_get(map->buckets, i);
    size_t nitems = Vector_count(bucket->key_value_pairs);
    for (size_t j = 0; j < nitems; j++) {
      kvp = Vector_get(bucket->key_value_pairs, j);
      if (!Map_add(dest_map, kvp->key, kvp->value).key) {
        goto error;
      }
    }
  }
  LOG_DEINDENT();
  return true;
error:
  LOG_DEINDENT();
  return false;
}

// Creates the union of two maps.
bool Map_union(Map *dest_map, const Map *a, const Map *b) {
  ASSERT(dest_map != NULL);
  ASSERT(a != NULL);
  ASSERT(b != NULL);

  KeyValuePair *kvp;
  for (size_t i = 0; i < a->capacity; i++) {
    struct MapBucket *bucket = Vector_get(a->buckets, i);
    size_t nitems = Vector_count(bucket->key_value_pairs);
    for (size_t j = 0; j < nitems; j++) {
      kvp = Vector_get(bucket->key_value_pairs, j);
      if (!Map_add(dest_map, kvp->key, kvp->value).key) {
        return false;
      }
    }
  }
  for (size_t i = 0; i < b->capacity; i++) {
    struct MapBucket *bucket = Vector_get(b->buckets, i);
    size_t nitems = Vector_count(bucket->key_value_pairs);
    for (size_t j = 0; j < nitems; j++) {
      kvp = Vector_get(bucket->key_value_pairs, j);
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
    struct MapBucket *bucket = Vector_get(a->buckets, i);
    size_t nitems = Vector_count(bucket->key_value_pairs);
    for (size_t j = 0; j < nitems; j++) {
      kvp = Vector_get(bucket->key_value_pairs, j);
      if (!Map_contains_key(b, kvp->key))
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
    struct MapBucket *bucket = Vector_get(a->buckets, i);
    size_t nitems = Vector_count(bucket->key_value_pairs);
    for (size_t j = 0; j < nitems; j++) {
      kvp = Vector_get(bucket->key_value_pairs, j);
      if (Map_contains_key(b, kvp->key))
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
    struct MapBucket *bucket = Vector_get(a->buckets, i);
    size_t nitems = Vector_count(bucket->key_value_pairs);
    for (size_t j = 0; j < nitems; j++) {
      kvp = Vector_get(bucket->key_value_pairs, j);
      if (Map_contains_key(b, kvp->key))
        continue;
      if (!Map_add(dest_map, kvp->key, kvp->value).key) {
        return false;
      }
    }
  }
  for (size_t i = 0; i < b->capacity; i++) {
    struct MapBucket *bucket = Vector_get(b->buckets, i);
    size_t nitems = Vector_count(bucket->key_value_pairs);
    for (size_t j = 0; j < nitems; j++) {
      kvp = Vector_get(bucket->key_value_pairs, j);
      if (Map_contains_key(a, kvp->key))
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
    struct MapBucket *bucket = Vector_get(map->buckets, i);
    size_t nitems = Vector_count(bucket->key_value_pairs);
    for (size_t j = 0; j < nitems; j++) {
      kvp = Vector_get(bucket->key_value_pairs, j);
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
    struct MapBucket *bucket = Vector_get(dest_map->buckets, i);
    for (size_t j = 0;
         j <
         Vector_count(bucket->key_value_pairs) /* inline due to modification */;
         j++) {
      kvp = Vector_get(bucket->key_value_pairs, j);
      if (!Map_contains_key(map, kvp->key)) {
        free(kvp->key);
        free(kvp->value);
        Vector_remove(bucket->key_value_pairs,
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
    struct MapBucket *bucket = Vector_get(dest_map->buckets, i);
    for (size_t j = 0;
         j <
         Vector_count(bucket->key_value_pairs) /* inline due to modification */;
         j++) {
      kvp = Vector_get(bucket->key_value_pairs, j);
      if (Map_contains_key(map, kvp->key)) {
        free(kvp->key);
        free(kvp->value);
        Vector_remove(bucket->key_value_pairs,
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
    struct MapBucket *bucket = Vector_get(map->buckets, i);
    size_t nitems = Vector_count(bucket->key_value_pairs);
    for (size_t j = 0; j < nitems; j++) {
      kvp = Vector_get(bucket->key_value_pairs, j);
      if (Map_contains_key(dest_map, kvp->key)) {
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
  return ((KeyValuePair *)Vector_get(
      ((struct MapBucket *)Vector_get(map->buckets, iter->impl_data1))
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
  return ((KeyValuePair *)Vector_get(
              ((struct MapBucket *)Vector_get(map->buckets, iter->impl_data1))
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
  return ((KeyValuePair *)Vector_get(
              ((struct MapBucket *)Vector_get(map->buckets, iter->impl_data1))
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
         iter->impl_data2 >= Vector_count(((struct MapBucket *)Vector_get(
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
