#ifndef COMMON_PUBLIC_ARRAY_H__
#define COMMON_PUBLIC_ARRAY_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "iterator.h"

typedef struct Array Array;

Array *Array_alloc(size_t count, size_t elem_size);

// Allocates the array on the stack
Array *Array_alloca(size_t count, size_t elem_size);

// Re-zeroes out the memory of the used array.
void Array_clear(Array *array);

// Reallocates the array with a new size. Returns new array!
Array *Array_realloc(Array *array, size_t count);

// Gets the number of elements in the Array
size_t Array_count(const Array *array);

// Frees up the Array object.
void Array_free(Array *array);

// Gets the size of an element in the Array
size_t Array_element_size(const Array *array);

// Gets the data for the array.
void *Array_get_data(Array *array);

// Gets the item at `index'
void *Array_get(Array *array, size_t index);

// Sets the item at `index' to the given data.
// `index' must be within the bounds of the array.
void Array_set(Array *array, size_t index, const void *elem);

// Sets the data at `index' to the given data.
// `index' and data must be within the bounds of the array.
void Array_set_range(Array *array, size_t index, const void *buffer,
                     int count);

// Copies a array. Returns whether successful.
bool Array_copy(Array *dest_array, const Array *array);

// Gets an Iterator for this Array
void Array_get_iterator(const Array *array, Iterator *iter);

// Gets a reverse Iterator for this Array
void Array_get_reverse_iterator(const Array *array, Iterator *iter);

// Gets a Sink for this array
void Array_get_sink(const Array *array, Sink *sink);

// Gets a reverse Sink for this array
void Array_get_reverse_sink(const Array *array, Sink *sink);

// Gets an Indexer for this array
void Array_get_indexer(const Array *array, Indexer *indexer);

#endif // COMMON_PUBLIC_ARRAY_H__
