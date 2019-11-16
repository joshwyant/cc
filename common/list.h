#ifndef COMMON_LIST_H__
#define COMMON_LIST_H__

#include <stdbool.h>
#include <stddef.h>

#include "iterator.h"

// A dynamically growing list.
typedef struct List List;

// Creates a new List object.
List *list_alloc(size_t elem_size);

// Initializes a pre-allocated List object
void list_init(List *list, size_t elem_size);

// Gets the number of elements in the List
size_t list_size(const List *list);

// Returns whether the list is empty.
bool list_empty(const List *list);

// Gets the capacity of the List
size_t list_capacity(const List *list);

// Reserves memory for a total of `num_elems' objects.
// Capacity always increases by a factor of 2.
// Returns whether the allocation was successful.
bool list_reserve(List *list, size_t num_elems);

// Removes excess memory for the List object.
// Returns whether the allocation was successful.
bool list_trim(List *list);

// Frees up the List object.
void list_free(List *list);

// Cleans up the List object, but does not free it.
void list_cleanup(List *list);

// Gets the size of an element in the List
size_t list_element_size(const List *list);

// Gets the data for the list.
void *list_get_data(const List *list);

// Gets the item at `index'
void *list_get(const List *list, size_t index);

// Sets the item at `index' to the given data.
// `index' must be within the bounds of the list.
void list_set(List *list, size_t index, const void *elem);

// Sets the data at `index' to the given data.
// `index' and data must be within the bounds of the list.
void list_set_range(List *list, size_t index, const void *elem, int count);

// Inserts the given element somewhere in the middle of the list.
// Returns a pointer to the inserted data. Returns NULL if unsuccessful.
void *list_insert(List *list, size_t index, const void *elem);

// Inserts the given elements somewhere in the middle of the list.
// Returns a pointer to the inserted data. Returns NULL if unsuccessful.
void *list_insert_range(List *list, size_t index, const void *elems, size_t count);

// Appends the given element to the list.
// Returns a pointer to the appended data. Returns NULL if unsuccessful.
void *list_append(List *list, const void *elem);

// Appends the given elements to the list.
// Returns a pointer to the appended data. Returns NULL if unsuccessful.
void *list_append_range(List *list, const void *elems, size_t count);

// Removes an element from the list.
void list_remove(List *list, size_t start_index);

// Removes all the items from the list.
// Remember to clean up memory first.
void list_clear(List *list);

// Removes a range of elements from the list.
void list_remove_range(List *list, size_t start_index, size_t count);

// Copies a list. Returns whether successful.
bool list_copy(List *dest_list, const List *list);

// Gets an Iterator for this List
void list_get_iterator(const List *list, Iterator *iter);

// Gets a reverse Iterator for this List
void list_get_reverse_iterator(const List *list, Iterator *iter);

// Gets a Sink for this list
void list_get_sink(const List *list, Sink *sink);

// Gets an Indexer for this list
void list_get_indexer(const List *list, Indexer *indexer);

#endif // COMMON_LIST_H__
