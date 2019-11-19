#ifndef COMMON_SET_H__
#define COMMON_SET_H__

#include <stdbool.h>
#include <stddef.h>

#include "iterator.h"

// A set data structure.
typedef struct Set Set;

// Creates a new Set object.
Set *Set_alloc(KeyInfo *key_info);

// Gets the number of elements in the Set
size_t Set_count(const Set *set);

// Returns whether the set is empty
bool Set_empty(const Set *set);

// Frees up the Set object.
void Set_free(Set *set);

// Gets the size of an element in the Set
size_t Set_element_size(const Set *set);

// Gets the key info for the Set
const KeyInfo *Set_key_info(const Set *set);

// Copies the value to the set and returns pointer to the new value.
// Returns NULL if unsuccessful.
void *Set_add(Set *set, const void *data);

// Looks up the value in set and stores the data in the given location.
// Returns whether successful.
bool Set_get(Set *set, const void *key, void *data_out);

// Checks whether the given key exists in the set.
bool Set_exists(const Set *set, const void *key);

// Removes an item from the set.
void Set_delete(Set *set, const void *key);

// Removes all the items from the set.
// Remember to clean up memory first.
void Set_clear(Set *set);

// Copies a set. Returns whether successful.
bool Set_copy(Set *dest_set, const Set *set);

// Creates the union of two sets.
bool Set_union(Set *dest_set, const Set *a, const Set *b);

// Creates the intersection of two sets.
bool Set_intersection(Set *dest_set, const Set *a, const Set *b);

// Creates the difference of two sets.
bool Set_difference(Set *dest_set, const Set *a, const Set *b);

// Creates the symmetric difference of two sets.
bool Set_symmetric_difference(Set *dest_set, const Set *a, const Set *b);

// Stores the union of two sets into the first set.
bool Set_union_with(Set *dest_set, const Set *set);

// Stores the intersection of two sets into the first set.
bool Set_intersect_with(Set *dest_set, const Set *set);

// Stores the difference of two sets into the first set.
bool Set_difference_with(Set *dest_set, const Set *set);

// Stores the symmetric difference of two sets into the first set.
bool Set_symmetric_difference_with(Set *dest_set, const Set *set);

// Gets an Iterator for this Set in an undefined order.
void Set_get_iterator(const Set *set, Iterator *iter);

#endif // COMMON_SET_H__
