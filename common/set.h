#ifndef COMMON_SET_H__
#define COMMON_SET_H__

#include <stdbool.h>
#include <stddef.h>

#include "iterator.h"

// A set data structure.
typedef struct Set Set;

// Creates a new Set object.
Set *set_alloc(KeyInfo *key_info);

// Initializes a pre-allocated Set object
void set_init(Set *set, KeyInfo *key_info);

// Gets the number of elements in the Set
size_t set_size(const Set *set);

// Returns whether the set is empty
bool set_empty(const Set *set);

// Frees up the Set object.
void set_free(Set *set);

// Cleans up the Set object, but does not free it.
void set_cleanup(Set *set);

// Gets the size of an element in the Set
size_t set_element_size(const Set *set);

// Gets the key info for the Set
KeyInfo *set_key_info(const Set *set);

// Copies the value to the set and returns pointer to the new value.
// Returns NULL if unsuccessful.
void *set_add(Set *set, const void *data);

// Looks up the value in set and stores the data in the given location.
// Returns whether successful.
bool set_get(Set *set, const void *key, const void *data_out);

// Checks whether the given key exists in the set.
bool set_exists(Set *set, const void *key);

// Removes an item from the set.
void set_delete(Set *set, const void *key);

// Removes all the items from the set.
// Remember to clean up memory first.
void set_clear(Set *set);

// Copies a set. Returns whether successful.
bool set_copy(Set *dest_set, const Set *set);

// Creates the union of two sets.
void set_union(Set *dest_set, const Set *a, const Set *b);

// Creates the intersection of two sets.
void set_intersection(Set *dest_set, const Set *a, const Set *b);

// Creates the difference of two sets.
void set_difference(Set *dest_set, const Set *a, const Set *b);

// Creates the symmetric difference of two sets.
void set_symmetric_difference(Set *dest_set, const Set *a, const Set *b);

// Stores the union of two sets into the first set.
void set_union_with(Set *dest_set, const Set *set);

// Stores the intersection of two sets into the first set.
void set_intersect_with(Set *dest_set, const Set *set);

// Stores the difference of two sets into the first set.
void set_difference_with(Set *dest_set, const Set *set);

// Stores the symmetric difference of two sets into the first set.
void set_symmetric_difference_with(Set *dest_set, const Set *set);

// Gets an Iterator for this Set in an undefined order.
void set_get_iterator(const Set *set, Iterator *iter);

#endif // COMMON_SET_H__
