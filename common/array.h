#ifndef COMMON_ARRAY_H__
#define COMMON_ARRAY_H__

#include <stdbool.h>
#include <stddef.h>

#include "iterator.h"

typedef struct Array Array;

bool array_init(Array *array, size_t count, void *data, size_t elem_size);

Array *array_alloc(size_t count, size_t elem_size);

void array_clear(Array *array);

// Gets the number of elements in the Array
size_t array_size(const Array *array);

// Frees up the Array object.
void array_free(Array *array);

// Cleans up the Array object, but does not free it.
void array_cleanup(Array *array);

// Gets the size of an element in the Array
size_t array_element_size(const Array *array);

// Gets the data for the array.
void *array_get_data(const Array *array);

// Gets the item at `index'
void *array_get(const Array *array, size_t index);

// Sets the item at `index' to the given data.
// `index' must be within the bounds of the array.
void array_set(const Array *array, size_t index, const void *elem);

// Sets the data at `index' to the given data.
// `index' and data must be within the bounds of the array.
void array_set_range(const Array *array, size_t index, const void *elem,
                     int count);

// Copies a array. Returns whether successful.
bool array_copy(Array *dest_array, const Array *array);

// Gets an Iterator for this Array
void array_get_iterator(const Array *array, Iterator *iter);

// Gets a reverse Iterator for this Array
void array_get_reverse_iterator(const Array *array, Iterator *iter);

// Gets a Sink for this array
void array_get_sink(const Array *array, Sink *sink);

// Gets a reverse Sink for this array
void array_get_reverse_sink(const Array *array, Sink *sink);

// Gets an Indexer for this array
void array_get_indexer(const Array *array, Indexer *indexer);

#endif // COMMON_ARRAY_H__
