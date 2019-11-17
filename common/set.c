#include "set.h"

#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "iterator.h"
#include "list.h"
#include "../test/stubs.h"

// TODO: 
// - Version

struct Set {
    List *buckets;
    KeyInfo key_info;
    size_t capacity;
    size_t count;
    int version;
};

struct SetBucket {
    List *items; // Holds void* items to allocated copies
};

void _set_swap(Set *a, Set *b)
{
    Set tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}

const void *_set_find_ext(const Set *set, const void *key, int hash);

int _set_hash(const Set *set, const void *key)
{
    ASSERT(set != NULL);
    ASSERT(key != NULL);

    int hash = (*set->key_info.hash_fn)(key);
    return hash > 0 ? hash : -hash;
}

// Initializes a pre-allocated Set object with a custom capacity
bool set_init_ext(Set *set, KeyInfo *key_info, size_t capacity)
{
    struct SetBucket bucket;
    if (set->buckets == NULL || !(set->buckets = list_alloc(sizeof(struct SetBucket)))) {
        return false;
    }
    set->capacity = capacity;
    set->count = 0;
    set->key_info = *key_info;
    if (!list_reserve(set->buckets, capacity)) {
        free(set->buckets);
        set->buckets = NULL;
        return false;
    }
    for (size_t i = 0; i < capacity; i++) {
        if (NULL == (bucket.items = list_alloc(sizeof(void*)))) {
            free(set->buckets);
            set->buckets = NULL;
            return false;
        }
        if (!list_append(set->buckets, &bucket)) {
            free(set->buckets);
            set->buckets = NULL;
            return false;
        }
    }
    return true;
}

// Creates a new Set object.
Set *set_alloc(KeyInfo *key_info)
{
    ASSERT(key_info != NULL);

    Set *set = malloc(sizeof(Set));
    if (set == NULL) {
        return NULL;
    }
    set->buckets = NULL;
    if (!set_init(set, key_info)) {
        free(set);
        return NULL;
    }
    return set;
}

// Initializes a pre-allocated Set object
bool set_init(Set *set, KeyInfo *key_info)
{
    ASSERT(set != NULL);
    ASSERT(key_info != NULL);
    
    return set_init_ext(set, key_info, 4);
}

// Gets the number of elements in the Set
size_t set_size(const Set *set)
{
    ASSERT(set != NULL);

    return set->count;
}

// Returns whether the set is empty
bool set_empty(const Set *set)
{
    ASSERT(set != NULL);

    return set->count == 0;
}

// Frees up the Set object.
void set_free(Set *set)
{
    ASSERT(set != NULL);

    set_cleanup(set);
    free(set);
}

// Cleans up the Set object, but does not free it.
void set_cleanup(Set *set)
{
    ASSERT(set != NULL);

    set_clear(set);
    if (set->buckets != NULL) {
        list_free(set->buckets);
        set->buckets = NULL;
    }
    set->capacity = 0;
    set->count = 0;
}

// Gets the size of an element in the Set
size_t set_element_size(const Set *set)
{
    ASSERT(set != NULL);

    return set->key_info.key_size;
}

// Gets the key info for the Set
const KeyInfo *set_key_info(const Set *set)
{
    ASSERT(set != NULL);

    return &set->key_info;
}

bool set_resize(Set *set, size_t new_capacity) {
    if (set->capacity >= new_capacity) {
        return true; // Nothing to do.
    }
    Set *new_set;
    if ((new_set = malloc(sizeof(Set))) == NULL) {
        return false;
    }
    if (!set_init_ext(new_set, &set->key_info, new_capacity)) {
        free(new_set);
        return false;
    }
    struct SetBucket *bucket;
    void **ptrval;
    size_t nbuckets = list_size(set->buckets);
    for (size_t i = 0; i < nbuckets; i++) {
        bucket = list_get(set->buckets, i);
        size_t nitems = list_size(bucket->items);
        for (size_t j = 0; j < nitems; j++) {
            ptrval = list_get(bucket->items, j);
            if (set_add(new_set, ptrval) == NULL) {
                set_free(new_set);
                return false;
            }
        }
    }
    // Put the new set in place of the old set.
    _set_swap(set, new_set);
    set_free(new_set);
    return true;
}

// Copies the value to the set and returns pointer to the new value.
// Returns NULL if unsuccessful.
void *set_add(Set *set, const void *key)
{
    ASSERT(set != NULL);
    ASSERT(key != NULL);

    if (!set->buckets) {
        // Map needs to be re-initialized.
        set_init(set, &set->key_info);
    }

    void *val;
    int hash = _set_hash(set, key);
    if ((val = (void*)_set_find_ext(set, key, hash)) == NULL) {
        size_t ibucket = hash % list_size(set->buckets);
        struct SetBucket *bucket = list_get(set->buckets, ibucket);
        if (!(val = malloc(set->key_info.key_size))) {
            return NULL;
        }
        if (!list_append(bucket->items, &val)) {
            free(val);
            return NULL;
        }
        set->count++;
    }
    // Overwrite any key for good measure.
    memcpy(val, key, set->key_info.key_size);

    // Have we grown too big?
    if (set->count > set->capacity) {
        if (!set_resize(set, set->capacity << 1)) {
            // Print an error and ignore
            fprintf(stderr, "Unable to expand capacity of set to %016llx", (unsigned long long)(set->capacity << 1));
        }
    }
    return val;
}

const void *_set_find_ext(const Set *set, const void *key, int hash)
{
    ASSERT(set != NULL);
    ASSERT(key != NULL);

    size_t ibucket = hash % set->capacity;
    struct SetBucket *bucket = list_get(set->buckets, ibucket);
    size_t nitems = list_size(bucket->items);
    for (size_t i = 0; i < nitems; i++) {
        void **ptrval = list_get(bucket->items, i);
        int ihash = _set_hash(set, *ptrval);
        if (ihash == hash && (*set->key_info.eq_fn)(*ptrval, key)) {
            return *ptrval;
        }
    }
    return NULL;
}

// Looks up the value in set and stores the data in the given location.
// Returns whether successful.
bool set_get(Set *set, const void *key, void *data_out)
{
    ASSERT(set != NULL);
    ASSERT(key != NULL);

    KeyValuePair null_kvp = { NULL, NULL };
    const void *val;
    int hash = _set_hash(set, key);
    if (!(val = _set_find_ext(set, key, hash))) {
        return false;
    }
    memcpy(data_out, val, set->key_info.key_size);
    return true;
}

// Checks whether the given key exists in the set.
bool set_exists(const Set *set, const void *key)
{

    KeyValuePair null_kvp = { NULL, NULL };
    const void *val;
    int hash = _set_hash(set, key);
    if (!(val = _set_find_ext(set, key, hash))) {
        return false;
    }
    return true;
}

// Removes an item from the set.
void set_delete(Set *set, const void *key)
{
    ASSERT(set != NULL);
    ASSERT(key != NULL);

    int hash = _set_hash(set, key);
    size_t ibucket = hash % set->capacity;
    struct SetBucket *bucket = list_get(set->buckets, ibucket);
    size_t nitems = list_size(bucket->items);
    for (size_t i = 0; i < nitems; i++) {
        void **ptrval = list_get(bucket->items, i);
        int ihash = _set_hash(set, *ptrval);
        if (ihash == hash && (*set->key_info.eq_fn)(*ptrval, key)) {
            free(*ptrval);
            list_remove(bucket->items, i);
            set->count--;
            return;
        }
    }
}

// Removes all the items from the set.
// Remember to clean up memory first.
void set_clear(Set *set)
{
    ASSERT(set != NULL);

    List *items;
    void **ptrval;
    for (size_t i = 0; i < set->capacity; i++) {
        items = list_get(set->buckets, i);
        size_t nitems = list_size(items);
        for (size_t j = 0; j < nitems; j++) {
            ptrval = list_get(items, j);
            free(*ptrval);
        }
        list_clear(items);
    }
}

// Copies a set. Returns whether successful.
bool set_copy(Set *dest_set, const Set *set)
{
    ASSERT(dest_set != NULL);
    ASSERT(set != NULL);

    set_clear(dest_set);
    if (!set_resize(dest_set, set->capacity)) {
        return false;
    }
    void **ptrval;
    for (size_t i = 0; i < set->capacity; i++) {
        struct SetBucket *bucket = list_get(set->buckets, i);
        size_t nitems = list_size(bucket->items);
        for (size_t j = 0; j < nitems; j++) {
            ptrval = list_get(bucket->items, j);
            if (!set_add(dest_set, *ptrval)) {
                return false;
            }
        }
    }
    return true;
}

// Creates the union of two sets.
bool set_union(Set *dest_set, const Set *a, const Set *b)
{
    ASSERT(dest_set != NULL);
    ASSERT(a != NULL);
    ASSERT(b != NULL);

    void **ptrval;
    for (size_t i = 0; i < a->capacity; i++) {
        struct SetBucket *bucket = list_get(a->buckets, i);
        size_t nitems = list_size(bucket->items);
        for (size_t j = 0; j < nitems; j++) {
            ptrval = list_get(bucket->items, j);
            if (!set_add(dest_set, *ptrval)) {
                return false;
            }
        }
    }
    for (size_t i = 0; i < b->capacity; i++) {
        struct SetBucket *bucket = list_get(b->buckets, i);
        size_t nitems = list_size(bucket->items);
        for (size_t j = 0; j < nitems; j++) {
            ptrval = list_get(bucket->items, j);
            if (!set_add(dest_set, *ptrval)) {
                return false;
            }
        }
    }
    return true;
}

// Creates the intersection of two sets.
bool set_intersection(Set *dest_set, const Set *a, const Set *b)
{
    ASSERT(dest_set != NULL);
    ASSERT(a != NULL);
    ASSERT(b != NULL);

    void **ptrval;
    for (size_t i = 0; i < a->capacity; i++) {
        struct SetBucket *bucket = list_get(a->buckets, i);
        size_t nitems = list_size(bucket->items);
        for (size_t j = 0; j < nitems; j++) {
            ptrval = list_get(bucket->items, j);
            if (!set_exists(b, *ptrval)) continue;
            if (!set_add(dest_set, *ptrval)) {
                return false;
            }
        }
    }
    return true;
}

// Creates the difference of two sets.
bool set_difference(Set *dest_set, const Set *a, const Set *b)
{
    ASSERT(dest_set != NULL);
    ASSERT(a != NULL);
    ASSERT(b != NULL);

    void **ptrval;
    for (size_t i = 0; i < a->capacity; i++) {
        struct SetBucket *bucket = list_get(a->buckets, i);
        size_t nitems = list_size(bucket->items);
        for (size_t j = 0; j < nitems; j++) {
            ptrval = list_get(bucket->items, j);
            if (set_exists(b, *ptrval)) continue;
            if (!set_add(dest_set, *ptrval)) {
                return false;
            }
        }
    }
    return true;
}

// Creates the symmetric difference of two sets.
bool set_symmetric_difference(Set *dest_set, const Set *a, const Set *b)
{
    ASSERT(dest_set != NULL);
    ASSERT(a != NULL);
    ASSERT(b != NULL);

    void **ptrval;
    for (size_t i = 0; i < a->capacity; i++) {
        struct SetBucket *bucket = list_get(a->buckets, i);
        size_t nitems = list_size(bucket->items);
        for (size_t j = 0; j < nitems; j++) {
            ptrval = list_get(bucket->items, j);
            if (set_exists(b, *ptrval)) continue;
            if (!set_add(dest_set, *ptrval)) {
                return false;
            }
        }
    }
    for (size_t i = 0; i < b->capacity; i++) {
        struct SetBucket *bucket = list_get(b->buckets, i);
        size_t nitems = list_size(bucket->items);
        for (size_t j = 0; j < nitems; j++) {
            ptrval = list_get(bucket->items, j);
            if (set_exists(a, *ptrval)) continue;
            if (!set_add(dest_set, *ptrval)) {
                return false;
            }
        }
    }
    return true;
}

// Stores the union of two sets into the first set.
bool set_union_with(Set *dest_set, const Set *set)
{
    ASSERT(dest_set != NULL);
    ASSERT(set != NULL);

    void **ptrval;
    for (size_t i = 0; i < set->capacity; i++) {
        struct SetBucket *bucket = list_get(set->buckets, i);
        size_t nitems = list_size(bucket->items);
        for (size_t j = 0; j < nitems; j++) {
            ptrval = list_get(bucket->items, j);
            if (!set_add(dest_set, *ptrval)) {
                return false;
            }
        }
    }
    return true;
}

// Stores the intersection of two sets into the first set.
bool set_intersect_with(Set *dest_set, const Set *set)
{
    ASSERT(dest_set != NULL);
    ASSERT(set != NULL);

    void **ptrval;
    for (size_t i = 0; i < dest_set->capacity; i++) {
        struct SetBucket *bucket = list_get(dest_set->buckets, i);
        for (size_t j = 0; j < list_size(bucket->items) /* inline due to modification */; j++) {
            ptrval = list_get(bucket->items, j);
            if (!set_exists(set, *ptrval)) {
                free(*ptrval);
                list_remove(bucket->items, j-- /* j doesn't change after removal */);
            }
        }
    }
    return true;
}

// Stores the difference of two sets into the first set.
bool set_difference_with(Set *dest_set, const Set *set)
{
    ASSERT(dest_set != NULL);
    ASSERT(set != NULL);

    void **ptrval;
    for (size_t i = 0; i < dest_set->capacity; i++) {
        struct SetBucket *bucket = list_get(dest_set->buckets, i);
        for (size_t j = 0; j < list_size(bucket->items) /* inline due to modification */; j++) {
            ptrval = list_get(bucket->items, j);
            if (set_exists(set, *ptrval)) {
                free(*ptrval);
                list_remove(bucket->items, j-- /* j doesn't change after removal */);
            }
        }
    }
    return true;
}

// Stores the symmetric difference of two sets into the first set.
bool set_symmetric_difference_with(Set *dest_set, const Set *set)
{
    ASSERT(dest_set != NULL);
    ASSERT(set != NULL);

    void **ptrval;
    for (size_t i = 0; i < set->capacity; i++) {
        struct SetBucket *bucket = list_get(set->buckets, i);
        size_t nitems = list_size(bucket->items);
        for (size_t j = 0; j < nitems; j++) {
            ptrval = list_get(bucket->items, j);
            if (set_exists(dest_set, *ptrval)) {
                set_delete(dest_set, *ptrval);
            } else {
                if (!set_add(dest_set, *ptrval)) {
                    return false;
                }
            }
        }
    }
    return true;
}

void *set_iter_current_(const Iterator *iter);
bool set_iter_eof_(const Iterator *iter);
bool set_iter_move_next_(Iterator *iter);

void *set_iter_current_(const Iterator *iter) {
    ASSERT(iter != NULL);
    ASSERT(iter->collection_type == COLLECTION_SET);
    Set *set = iter->collection;
    ASSERT(set != NULL);
    ASSERT(iter->version == set->version && "Collection changed while iterating.");
    if (set_iter_eof_(iter)) {
        return NULL;
    }
    return list_get(((struct SetBucket*)list_get(set->buckets, iter->impl_data1))->items, iter->impl_data2);
}

bool set_iter_eof_(const Iterator *iter) {
    ASSERT(iter != NULL);
    ASSERT(iter->collection_type == COLLECTION_SET);
    Set *set = iter->collection;
    ASSERT(set != NULL);
    ASSERT(iter->version == set->version && "Collection changed while iterating.");
    return set_empty(set) || iter->impl_data1 >= set->capacity;
}

bool set_iter_move_next_(Iterator *iter) {
    ASSERT(iter != NULL);
    ASSERT(iter->collection_type == COLLECTION_SET);
    Set *set = iter->collection;
    ASSERT(set != NULL);
    ASSERT(iter->version == set->version && "Collection changed while iterating.");
    if (set_iter_eof_(iter)) {
        return false;
    }
    iter->impl_data2++;
    if (iter->impl_data2 > list_size(list_get(set->buckets, iter->impl_data1))) {
        iter->impl_data1++;
        iter->impl_data2 = 0;
    }
    return !set_iter_eof_(iter);
}

// Gets an Iterator for this Set in an undefined order.
void set_get_iterator(const Set *set, Iterator *iter)
{
    ASSERT(set != NULL);
    ASSERT(iter != NULL);

    iter->collection_type = COLLECTION_MAP;
    iter->collection = (void*)set;
    iter->elem_size = set->key_info.key_size;
    iter->current = set_iter_current_;
    iter->move_next = set_iter_move_next_;
    iter->eof = set_iter_eof_;
    iter->impl_data1 = -1;
    iter->impl_data2 = 0;
    iter->version = 0;
}
