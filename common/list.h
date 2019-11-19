#ifndef COMMON_LIST_H__
#define COMMON_LIST_H__

#include <stdbool.h>
#include <stddef.h>

#include "iterator.h"

// A dynamically growing list.
typedef struct List List;

// Creates a new List object.
List *List_alloc(size_t elem_size);

// Gets the number of elements in the List
size_t List_count(const List *list);

// Returns whether the list is empty.
bool List_empty(const List *list);

// Gets the capacity of the List
size_t List_capacity(const List *list);

// Reserves memory for a total of `num_elems' objects.
// Capacity always increases by a factor of 2.
// Returns whether the allocation was successful.
bool List_reserve(List *list, size_t num_elems);

// Removes excess memory for the List object.
// Returns whether the allocation was successful.
bool List_trim(List *list);

// Frees up the List object.
void List_free(List *list);

// Gets the size of an element in the List
size_t List_element_size(const List *list);

// Gets the data for the list.
void *List_get_data(const List *list);

// Gets the item at `index'
void *List_get(const List *list, size_t index);

// Sets the item at `index' to the given data.
// `index' must be within the bounds of the list.
void List_set(List *list, size_t index, const void *elem);

// Sets the data at `index' to the given data.
// `index' and data must be within the bounds of the list.
void List_set_range(List *list, size_t index, const void *elem, int count);

// Inserts the given element somewhere in the middle of the list.
// Returns a pointer to the inserted data. Returns NULL if unsuccessful.
void *List_insert(List *list, size_t index, const void *elem);

// Inserts the given elements somewhere in the middle of the list.
// Returns a pointer to the inserted data. Returns NULL if unsuccessful.
void *List_insert_range(List *list, size_t index, const void *elems,
                        size_t count);

// Appends the given element to the list.
// Returns a pointer to the appended data. Returns NULL if unsuccessful.
void *List_add(List *list, const void *elem);

// Appends the given elements to the list.
// Returns a pointer to the appended data. Returns NULL if unsuccessful.
void *List_add_range(List *list, const void *elems, size_t count);

// Removes an element from the list.
void List_remove(List *list, size_t start_index);

// Removes all the items from the list.
// Remember to clean up memory first.
void List_clear(List *list);

// Removes a range of elements from the list.
void List_remove_range(List *list, size_t start_index, size_t count);

// Copies a list. Returns whether successful.
bool List_copy(List *dest_list, const List *list);

// Gets an Iterator for this List
void List_get_iterator(const List *list, Iterator *iter);

// Gets a reverse Iterator for this List
void List_get_reverse_iterator(const List *list, Iterator *iter);

// Gets a Sink for this list
void List_get_sink(const List *list, Sink *sink);

// Gets an Indexer for this list
void List_get_indexer(const List *list, Indexer *indexer);

#endif // COMMON_LIST_H__
