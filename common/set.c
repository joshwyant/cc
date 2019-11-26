#include "protected/set.h"

#include <stdlib.h>
#include <string.h>

#include "../test/stubs.h"
#include "public/assert.h"
#include "public/iterator.h"
#include "public/vector.h"

// TODO:
// - Version

bool Set_init(Set *set, KeyInfo *key_info);

void Set_cleanup(Set *set);

void _Set_swap(Set *a, Set *b) {
  Set tmp;
  tmp = *a;
  *a = *b;
  *b = tmp;
}

const void *_Set_find_ext(const Set *set, const void *key, int hash);

int _Set_hash(const Set *set, const void *key) {
  ASSERT(set != NULL);
  ASSERT(key != NULL);

  int hash = set->key_info.hash_fn(key);
  return hash > 0 ? hash : -hash;
}

// Initializes a pre-allocated Set object with a custom capacity
bool Set_init_ext(Set *set, KeyInfo *key_info, size_t capacity) {
  struct SetBucket bucket;
  if (!(set->buckets = Vector_alloc(sizeof(struct SetBucket)))) {
    goto error;
  }
  set->capacity = capacity;
  set->count = 0;
  set->key_info = *key_info;
  set->version = 0;
  if (!Vector_reserve(set->buckets, capacity)) {
      goto error_buckets;
  }
  for (size_t i = 0; i < capacity; i++) {
    if (NULL == (bucket.items = Vector_alloc(sizeof(void *)))) {
      goto error_buckets;
    }
    if (!Vector_add(set->buckets, &bucket)) {
      goto error_buckets;
    }
  }
  return true;
error_buckets:
  Vector_free(set->buckets);
  set->buckets = NULL;
error:
  return false;
}

// Creates a new Set object.
Set *Set_alloc(KeyInfo *key_info) {
  ASSERT(key_info != NULL);

  Set *set = malloc(sizeof(Set));
  if (set == NULL) {
    return NULL;
  }
  set->buckets = NULL;
  if (!Set_init(set, key_info)) {
    free(set);
    return NULL;
  }
  return set;
}

// Initializes a pre-allocated Set object
bool Set_init(Set *set, KeyInfo *key_info) {
  ASSERT(set != NULL);
  ASSERT(key_info != NULL);

  return Set_init_ext(set, key_info, 4);
}

// Gets the number of elements in the Set
size_t Set_count(const Set *set) {
  ASSERT(set != NULL);

  return set->count;
}

// Returns whether the set is empty
bool Set_empty(const Set *set) {
  ASSERT(set != NULL);

  return set->count == 0;
}

// Frees up the Set object.
void Set_free(Set *set) {
  ASSERT(set != NULL);

  Set_cleanup(set);
  free(set);
}

// Cleans up the Set object, but does not free it.
void Set_cleanup(Set *set) {
  ASSERT(set != NULL);

  Set_clear(set);
  if (set->buckets != NULL) {
    Vector_free(set->buckets);
    set->buckets = NULL;
  }
  set->capacity = 0;
  set->count = 0;
}

// Gets the size of an element in the Set
size_t Set_element_size(const Set *set) {
  ASSERT(set != NULL);

  return set->key_info.key_size;
}

// Gets the key info for the Set
const KeyInfo *Set_key_info(const Set *set) {
  ASSERT(set != NULL);

  return &set->key_info;
}

bool Set_resize(Set *set, size_t new_capacity) {
  bool status = false;
  if (set->capacity >= new_capacity) {
    status = true;
    goto out; // Nothing to do.
  }
  Set *new_set;
  if ((new_set = malloc(sizeof(Set))) == NULL) {
    goto out;
  }
  if (!Set_init_ext(new_set, &set->key_info, new_capacity)) {
    goto out_new_set;
  }
  struct SetBucket *bucket;
  void **ptrval;
  size_t nbuckets = Vector_count(set->buckets);
  for (size_t i = 0; i < nbuckets; i++) {
    bucket = Vector_get(set->buckets, i);
    size_t nitems = Vector_count(bucket->items);
    for (size_t j = 0; j < nitems; j++) {
      ptrval = Vector_get(bucket->items, j);
      if (Set_add(new_set, ptrval) == NULL) {
        goto out_new_set;
      }
    }
  }
  // Put the new set in place of the old set.
  _Set_swap(set, new_set);
out_new_set:
  Set_free(new_set);
out:
  return status;
}

// Copies the value to the set and returns pointer to the new value.
// Returns NULL if unsuccessful.
void *Set_add(Set *set, const void *key) {
  ASSERT(set != NULL);
  ASSERT(key != NULL);

  if (!set->buckets) {
    // Map needs to be re-initialized.
    Set_init(set, &set->key_info);
  }

  void *val = NULL;
  int hash = _Set_hash(set, key);
  if ((val = (void *)_Set_find_ext(set, key, hash)) == NULL) {
    size_t ibucket = hash % Vector_count(set->buckets);
    struct SetBucket *bucket = Vector_get(set->buckets, ibucket);
    if (!(val = malloc(set->key_info.key_size))) {
      goto error;
    }
    if (!Vector_add(bucket->items, &val)) {
      val = NULL;
      goto error_buckets;
    }
    set->count++;
  }
  // Overwrite any key for good measure.
  memcpy(val, key, set->key_info.key_size);

  // Have we grown too big?
  if (set->count > set->capacity) {
    if (!Set_resize(set, set->capacity << 1)) {
      // Print an error and ignore
      fprintf(stderr, "Unable to expand capacity of set to %016llx",
              (unsigned long long)(set->capacity << 1));
    }
  }
  return val;
error_buckets:
  free(val);
error:
  return NULL;
}

const void *_Set_find_ext(const Set *set, const void *key, int hash) {
  ASSERT(set != NULL);
  ASSERT(key != NULL);

  size_t ibucket = hash % set->capacity;
  struct SetBucket *bucket = Vector_get(set->buckets, ibucket);
  size_t nitems = Vector_count(bucket->items);
  for (size_t i = 0; i < nitems; i++) {
    void **ptrval = Vector_get(bucket->items, i);
    int ihash = _Set_hash(set, *ptrval);
    if (ihash == hash && set->key_info.eq_fn(*ptrval, key)) {
      return *ptrval;
    }
  }
  return NULL;
}

// Looks up the value in set and stores the data in the given location.
// Returns whether successful.
bool Set_get(Set *set, const void *key, void *data_out) {
  ASSERT(set != NULL);
  ASSERT(key != NULL);

  KeyValuePair null_kvp = {NULL, NULL};
  const void *val;
  int hash = _Set_hash(set, key);
  if (!(val = _Set_find_ext(set, key, hash))) {
    return false;
  }
  memcpy(data_out, val, set->key_info.key_size);
  return true;
}

// Checks whether the given key exists in the set.
bool Set_contains(const Set *set, const void *key) {

  KeyValuePair null_kvp = {NULL, NULL};
  const void *val;
  int hash = _Set_hash(set, key);
  if (!(val = _Set_find_ext(set, key, hash))) {
    return false;
  }
  return true;
}

// Removes an item from the set.
void Set_delete(Set *set, const void *key) {
  ASSERT(set != NULL);
  ASSERT(key != NULL);

  int hash = _Set_hash(set, key);
  size_t ibucket = hash % set->capacity;
  struct SetBucket *bucket = Vector_get(set->buckets, ibucket);
  size_t nitems = Vector_count(bucket->items);
  for (size_t i = 0; i < nitems; i++) {
    void **ptrval = Vector_get(bucket->items, i);
    int ihash = _Set_hash(set, *ptrval);
    if (ihash == hash && set->key_info.eq_fn(*ptrval, key)) {
      free(*ptrval);
      Vector_remove(bucket->items, i);
      set->count--;
      return;
    }
  }
}

// Removes all the items from the set.
// Remember to clean up memory first.
void Set_clear(Set *set) {
  ASSERT(set != NULL);

  struct SetBucket *bucket;
  Vector *items;
  void **ptrval;
  for (size_t i = 0; i < set->capacity; i++) {
    bucket = Vector_get(set->buckets, i);
    items = bucket->items;
    size_t nitems = Vector_count(items);
    for (size_t j = 0; j < nitems; j++) {
      ptrval = Vector_get(items, j);
      free(*ptrval);
    }
    Vector_clear(items);
  }
}

// Copies a set. Returns whether successful.
bool Set_copy(Set *dest_set, const Set *set) {
  ASSERT(dest_set != NULL);
  ASSERT(set != NULL);

  Set_clear(dest_set);
  if (!Set_resize(dest_set, set->capacity)) {
    return false;
  }
  void **ptrval;
  for (size_t i = 0; i < set->capacity; i++) {
    struct SetBucket *bucket = Vector_get(set->buckets, i);
    size_t nitems = Vector_count(bucket->items);
    for (size_t j = 0; j < nitems; j++) {
      ptrval = Vector_get(bucket->items, j);
      if (!Set_add(dest_set, *ptrval)) {
        return false;
      }
    }
  }
  return true;
}

// Creates the union of two sets.
bool Set_union(Set *dest_set, const Set *a, const Set *b) {
  ASSERT(dest_set != NULL);
  ASSERT(a != NULL);
  ASSERT(b != NULL);

  void **ptrval;
  for (size_t i = 0; i < a->capacity; i++) {
    struct SetBucket *bucket = Vector_get(a->buckets, i);
    size_t nitems = Vector_count(bucket->items);
    for (size_t j = 0; j < nitems; j++) {
      ptrval = Vector_get(bucket->items, j);
      if (!Set_add(dest_set, *ptrval)) {
        return false;
      }
    }
  }
  for (size_t i = 0; i < b->capacity; i++) {
    struct SetBucket *bucket = Vector_get(b->buckets, i);
    size_t nitems = Vector_count(bucket->items);
    for (size_t j = 0; j < nitems; j++) {
      ptrval = Vector_get(bucket->items, j);
      if (!Set_add(dest_set, *ptrval)) {
        return false;
      }
    }
  }
  return true;
}

// Creates the intersection of two sets.
bool Set_intersection(Set *dest_set, const Set *a, const Set *b) {
  ASSERT(dest_set != NULL);
  ASSERT(a != NULL);
  ASSERT(b != NULL);

  void **ptrval;
  for (size_t i = 0; i < a->capacity; i++) {
    struct SetBucket *bucket = Vector_get(a->buckets, i);
    size_t nitems = Vector_count(bucket->items);
    for (size_t j = 0; j < nitems; j++) {
      ptrval = Vector_get(bucket->items, j);
      if (!Set_contains(b, *ptrval))
        continue;
      if (!Set_add(dest_set, *ptrval)) {
        return false;
      }
    }
  }
  return true;
}

// Creates the difference of two sets.
bool Set_difference(Set *dest_set, const Set *a, const Set *b) {
  ASSERT(dest_set != NULL);
  ASSERT(a != NULL);
  ASSERT(b != NULL);

  void **ptrval;
  for (size_t i = 0; i < a->capacity; i++) {
    struct SetBucket *bucket = Vector_get(a->buckets, i);
    size_t nitems = Vector_count(bucket->items);
    for (size_t j = 0; j < nitems; j++) {
      ptrval = Vector_get(bucket->items, j);
      if (Set_contains(b, *ptrval))
        continue;
      if (!Set_add(dest_set, *ptrval)) {
        return false;
      }
    }
  }
  return true;
}

// Creates the symmetric difference of two sets.
bool Set_symmetric_difference(Set *dest_set, const Set *a, const Set *b) {
  ASSERT(dest_set != NULL);
  ASSERT(a != NULL);
  ASSERT(b != NULL);

  void **ptrval;
  for (size_t i = 0; i < a->capacity; i++) {
    struct SetBucket *bucket = Vector_get(a->buckets, i);
    size_t nitems = Vector_count(bucket->items);
    for (size_t j = 0; j < nitems; j++) {
      ptrval = Vector_get(bucket->items, j);
      if (Set_contains(b, *ptrval))
        continue;
      if (!Set_add(dest_set, *ptrval)) {
        return false;
      }
    }
  }
  for (size_t i = 0; i < b->capacity; i++) {
    struct SetBucket *bucket = Vector_get(b->buckets, i);
    size_t nitems = Vector_count(bucket->items);
    for (size_t j = 0; j < nitems; j++) {
      ptrval = Vector_get(bucket->items, j);
      if (Set_contains(a, *ptrval))
        continue;
      if (!Set_add(dest_set, *ptrval)) {
        return false;
      }
    }
  }
  return true;
}

// Stores the union of two sets into the first set.
bool Set_union_with(Set *dest_set, const Set *set) {
  ASSERT(dest_set != NULL);
  ASSERT(set != NULL);

  void **ptrval;
  for (size_t i = 0; i < set->capacity; i++) {
    struct SetBucket *bucket = Vector_get(set->buckets, i);
    size_t nitems = Vector_count(bucket->items);
    for (size_t j = 0; j < nitems; j++) {
      ptrval = Vector_get(bucket->items, j);
      if (!Set_add(dest_set, *ptrval)) {
        return false;
      }
    }
  }
  return true;
}

// Stores the intersection of two sets into the first set.
bool Set_intersect_with(Set *dest_set, const Set *set) {
  ASSERT(dest_set != NULL);
  ASSERT(set != NULL);

  void **ptrval;
  for (size_t i = 0; i < dest_set->capacity; i++) {
    struct SetBucket *bucket = Vector_get(dest_set->buckets, i);
    for (size_t j = 0;
         j < Vector_count(bucket->items) /* inline due to modification */; j++) {
      ptrval = Vector_get(bucket->items, j);
      if (!Set_contains(set, *ptrval)) {
        free(*ptrval);
        Vector_remove(bucket->items, j-- /* j doesn't change after removal */);
      }
    }
  }
  return true;
}

// Stores the difference of two sets into the first set.
bool Set_difference_with(Set *dest_set, const Set *set) {
  ASSERT(dest_set != NULL);
  ASSERT(set != NULL);

  void **ptrval;
  for (size_t i = 0; i < dest_set->capacity; i++) {
    struct SetBucket *bucket = Vector_get(dest_set->buckets, i);
    for (size_t j = 0;
         j < Vector_count(bucket->items) /* inline due to modification */; j++) {
      ptrval = Vector_get(bucket->items, j);
      if (Set_contains(set, *ptrval)) {
        free(*ptrval);
        Vector_remove(bucket->items, j-- /* j doesn't change after removal */);
      }
    }
  }
  return true;
}

// Stores the symmetric difference of two sets into the first set.
bool Set_symmetric_difference_with(Set *dest_set, const Set *set) {
  ASSERT(dest_set != NULL);
  ASSERT(set != NULL);

  void **ptrval;
  for (size_t i = 0; i < set->capacity; i++) {
    struct SetBucket *bucket = Vector_get(set->buckets, i);
    size_t nitems = Vector_count(bucket->items);
    for (size_t j = 0; j < nitems; j++) {
      ptrval = Vector_get(bucket->items, j);
      if (Set_contains(dest_set, *ptrval)) {
        Set_delete(dest_set, *ptrval);
      } else {
        if (!Set_add(dest_set, *ptrval)) {
          return false;
        }
      }
    }
  }
  return true;
}

void *Set_iter_current_(const Iterator *iter);
bool Set_iter_eof_(const Iterator *iter);
bool Set_iter_move_next_(Iterator *iter);

void *Set_iter_current_(const Iterator *iter) {
  ASSERT(iter != NULL);
  ASSERT(iter->collection_type == COLLECTION_SET);
  Set *set = iter->collection;
  ASSERT(set != NULL);
  ASSERT(iter->version == set->version &&
         "Collection changed while iterating.");
  if (Set_iter_eof_(iter)) {
    return NULL;
  }
  return Vector_get(
      ((struct SetBucket *)Vector_get(set->buckets, iter->impl_data1))->items,
      iter->impl_data2);
}

bool Set_iter_eof_(const Iterator *iter) {
  ASSERT(iter != NULL);
  ASSERT(iter->collection_type == COLLECTION_SET);
  Set *set = iter->collection;
  ASSERT(set != NULL);
  ASSERT(iter->version == set->version &&
         "Collection changed while iterating.");
  return Set_empty(set) || iter->impl_data1 >= (int)set->capacity;
}

bool Set_iter_move_next_(Iterator *iter) {
  ASSERT(iter != NULL);
  ASSERT(iter->collection_type == COLLECTION_SET);
  Set *set = iter->collection;
  ASSERT(set != NULL);
  ASSERT(iter->version == set->version &&
         "Collection changed while iterating.");
  if (Set_iter_eof_(iter)) {
    return false;
  }
  iter->impl_data2++;
  while (iter->impl_data1 == -1 ||
         iter->impl_data2 >= Vector_count(((struct SetBucket *)Vector_get(
                                            set->buckets, iter->impl_data1))
                                           ->items)) {
    iter->impl_data1++;
    iter->impl_data2 = 0;
    if (Set_iter_eof_(iter))
      break;
  }
  return !Set_iter_eof_(iter);
}

// Gets an Iterator for this Set in an undefined order.
void Set_get_iterator(const Set *set, Iterator *iter) {
  ASSERT(set != NULL);
  ASSERT(iter != NULL);

  iter->collection_type = COLLECTION_MAP;
  iter->collection = (void *)set;
  iter->elem_size = set->key_info.key_size;
  iter->current = Set_iter_current_;
  iter->move_next = Set_iter_move_next_;
  iter->eof = Set_iter_eof_;
  iter->impl_data1 = -1;
  iter->impl_data2 = 0;
  iter->version = 0;
}
