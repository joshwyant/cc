#include "protected/array.h"

#include <string.h>

#include "../test/stubs.h"
#include "public/collections.h"
#include "public/assert.h"

DEFINE_ARRAY(Int, int)
DEFINE_ARRAY(Long, long)
DEFINE_ARRAY(Char, char)
DEFINE_ARRAY(Float, float)
DEFINE_ARRAY(Double, double)
DEFINE_ARRAY(UnsignedInt, unsigned int)
DEFINE_ARRAY(UnsignedLong, unsigned long)
DEFINE_ARRAY(UnsignedChar, unsigned char)
DEFINE_ARRAY(String, char *)

bool Array_init(Array *array, size_t count, size_t elem_size)
{
    ASSERT(array != NULL);
    ASSERT(count > 0);
    ASSERT(elem_size > 0);
    array->count = count;
    array->elem_size = elem_size;
    Array_clear(array);
    return true;
}

Array *Array_alloc(size_t count, size_t elem_size)
{
    ASSERT(count > 0);
    ASSERT(elem_size > 0);
    Array *array;
    if (NULL == (array = malloc(sizeof(Array) + count * elem_size))) {
        return NULL;
    }
    Array_init(array, count, elem_size);
    return array;
}

Array *Array_alloca(size_t count, size_t elem_size)
{
    ASSERT(count > 0);
    ASSERT(elem_size > 0);
    Array *array;
    if (NULL == (array = alloca(sizeof(Array) + count * elem_size))) {
        return NULL;
    }
    Array_init(array, count, elem_size);
    return array;
}

void Array_clear(Array *array)
{
    ASSERT(array != NULL);
    memset(array->data, 0, array->count * array->elem_size);
}

Array *Array_realloc(Array *array, size_t count)
{
    ASSERT(array != NULL);
    Array *new_array = NULL;
    if (NULL ==
        (new_array = realloc(array, sizeof(Array) + count * array->elem_size))) {
        return NULL;
    }
    if (count > array->count) {
        memset(new_array->data + array->count * array->elem_size, 0,
            (count - array->count) * array->elem_size);
    }
    array->count = count;
    return new_array;
}

size_t Array_count(const Array *array)
{
    ASSERT(array != NULL);
    return array->count;
}

void Array_free(Array *array)
{
    ASSERT(array != NULL);
    free(array);
}

size_t Array_element_size(const Array *array)
{
    ASSERT(array != NULL);
    return array->elem_size;
}

void *Array_get_data(Array *array)
{
    ASSERT(array != NULL);
    ASSERT(array->data != NULL);
    return array->data;
}

void *Array_get(Array *array, size_t index)
{
    ASSERT(array != NULL);
    ASSERT(index >= 0);
    ASSERT(index < array->count);
    ASSERT(array->data != NULL);
    return array->data + index * array->elem_size;
}

void Array_set(Array *array, size_t index, const void *elem)
{
    ASSERT(array != NULL);
    ASSERT(index >= 0);
    ASSERT(index < array->count);
    ASSERT(elem != NULL);
    ASSERT(array->data != NULL);
    memcpy(array->data + index * array->elem_size, elem, array->elem_size);
}

void Array_set_range(Array *array, size_t index, const void *buffer,
                     int count)
{
    ASSERT(array != NULL);
    ASSERT(index >= 0);
    ASSERT(buffer != NULL);
    ASSERT(count >= 0);
    ASSERT(index + count < array->count);
    ASSERT(array->data != NULL);
    memcpy(array->data + index * array->elem_size, buffer, array->elem_size * count);
}

void Array_copy(Array *dest_array, const Array *array)
{
    ASSERT(dest_array != NULL);
    ASSERT(array != NULL);
    ASSERT(dest_array->count == array->count);
    ASSERT(dest_array->elem_size == array->elem_size);
    memcpy(dest_array, array, sizeof(Array) + array->count * array->elem_size);
}

void *Array_iter_current_(const Iterator *iter);
void *Array_iter_current_reverse_(const Iterator *iter);
bool Array_iter_eof_(const Iterator *iter);
bool Array_iter_eof_reverse_(const Iterator *iter);
bool Array_iter_move_next_(Iterator *iter);
bool Array_iter_move_next_reverse_(Iterator *iter);

void *Array_iter_current_(const Iterator *iter) 
{
  ASSERT(iter != NULL);
  ASSERT(iter->collection_type == COLLECTION_ARRAY);
  Array *array = iter->collection;
  ASSERT(array != NULL);
  if (Array_iter_eof_(iter)) {
    return NULL;
  }
  return Array_get(array, iter->impl_data1);
}

void *Array_iter_current_reverse_(const Iterator *iter) 
{
  ASSERT(iter != NULL);
  ASSERT(iter->collection_type == COLLECTION_ARRAY);
  Array *array = iter->collection;
  ASSERT(array != NULL);
  if (Array_iter_eof_reverse_(iter)) {
    return NULL;
  }
  return Array_get(array, iter->impl_data1);
}

bool Array_iter_eof_(const Iterator *iter) 
{
  ASSERT(iter != NULL);
  ASSERT(iter->collection_type == COLLECTION_ARRAY);
  Array *array = iter->collection;
  ASSERT(array != NULL);
  return iter->impl_data1 >= (int)array->count;
}

bool Array_iter_eof_reverse_(const Iterator *iter) 
{
  ASSERT(iter != NULL);
  ASSERT(iter->collection_type == COLLECTION_ARRAY);
  Array *array = iter->collection;
  ASSERT(array != NULL);
  return iter->impl_data1 < 0;
}

bool Array_iter_move_next_(Iterator *iter) 
{
  ASSERT(iter != NULL);
  ASSERT(iter->collection_type == COLLECTION_ARRAY);
  Array *array = iter->collection;
  ASSERT(array != NULL);
  if (Array_iter_eof_(iter)) {
    return false;
  }
  iter->impl_data1++;
  return !Array_iter_eof_(iter);
}

bool Array_iter_move_next_reverse_(Iterator *iter) 
{
  ASSERT(iter != NULL);
  ASSERT(iter->collection_type == COLLECTION_ARRAY);
  Array *array = iter->collection;
  ASSERT(array != NULL);
  if (Array_iter_eof_reverse_(iter)) {
    return false;
  }
  iter->impl_data1--;
  return !Array_iter_eof_reverse_(iter);
}

// Gets an Iterator for this Array
void Array_get_iterator(const Array *array, Iterator *iter) 
{
  ASSERT(array != NULL);
  ASSERT(iter != NULL);
  iter->collection_type = COLLECTION_ARRAY;
  iter->collection = (void *)array;
  iter->elem_size = array->elem_size;
  iter->current = Array_iter_current_;
  iter->eof = Array_iter_eof_;
  iter->move_next = Array_iter_move_next_;
  iter->impl_data1 = -1; // Current index
  iter->impl_data2 = 0;
  iter->version = 1;
}

void Array_get_reverse_iterator(const Array *array, Iterator *iter) 
{
  ASSERT(array != NULL);
  ASSERT(iter != NULL);
  iter->collection_type = COLLECTION_ARRAY;
  iter->collection = (void *)array;
  iter->elem_size = array->elem_size;
  iter->current = Array_iter_current_reverse_;
  iter->eof = Array_iter_eof_reverse_;
  iter->move_next = Array_iter_move_next_reverse_;
  iter->impl_data1 = array->count; // Current index
  iter->impl_data2 = 0;
  iter->version = 1;
}

void *Array_sink_add_(Sink *sink, const void *elem);

void Array_get_sink(const Array *array, Sink *sink) 
{
  ASSERT(array != NULL);
  ASSERT(sink != NULL);
  sink->collection_type = COLLECTION_ARRAY;
  sink->collection = (void *)array;
  sink->elem_size = array->elem_size;
  sink->state = (void*)array->data;
  sink->add = Array_sink_add_;
}

void *Array_sink_add_(Sink *sink, const void *elem) 
{
  ASSERT(sink != NULL);
  Array *array = sink->collection;
  ASSERT(array != NULL);
  ASSERT(sink->state < (void*)array->data + array->count * array->elem_size);
  void *data = sink->state;
  memcpy(data, elem, array->elem_size);
  sink->state += array->elem_size;
  return data;
}

void *Array_reverse_sink_add_(Sink *sink, const void *elem);

void Array_get_reverse_sink(const Array *array, Sink *sink) 
{
  ASSERT(array != NULL);
  ASSERT(sink != NULL);
  sink->collection_type = COLLECTION_ARRAY;
  sink->collection = (void *)array;
  sink->elem_size = array->elem_size;
  sink->state = (void*)array->data + (array->count - 1) * array->elem_size;
  sink->add = Array_sink_add_;
}

void *Array_reverse_sink_add_(Sink *sink, const void *elem) 
{
  ASSERT(sink != NULL);
  Array *array = sink->collection;
  ASSERT(array != NULL);
  ASSERT(sink->state > (void*)array->data);
  void *data = sink->state;
  memcpy(data, elem, array->elem_size);
  sink->state -= array->elem_size;
  return data;
}

size_t Array_indexer_size_(const Indexer *indexer);
void *Array_indexer_get_(const Indexer *indexer, size_t index);
void Array_indexer_set_(Indexer *indexer, size_t index, const void *data);

// Gets an Indexer for this array
void Array_get_indexer(const Array *array, Indexer *indexer) 
{
  ASSERT(array != NULL);
  ASSERT(indexer != NULL);
  indexer->collection_type = COLLECTION_ARRAY;
  indexer->collection = (void *)array;
  indexer->elem_size = array->elem_size;
  indexer->size = Array_indexer_size_;
  indexer->get = Array_indexer_get_;
  indexer->set = Array_indexer_set_;
}

size_t Array_indexer_size_(const Indexer *indexer) 
{
  ASSERT(indexer != NULL);
  Array *array = indexer->collection;
  ASSERT(array != NULL);
  return array->count;
}

void *Array_indexer_get_(const Indexer *indexer, size_t index) 
{
  ASSERT(indexer != NULL);
  Array *array = indexer->collection;
  ASSERT(array != NULL);
  ASSERT(index >= 0 && index < Array_count(array));
  return array->data + array->elem_size * index;
}

void Array_indexer_set_(Indexer *indexer, size_t index, const void *data) 
{
  ASSERT(indexer != NULL);
  Array *array = indexer->collection;
  ASSERT(array != NULL);
  ASSERT(index >= 0 && index < Array_count(array));
  ASSERT(data != NULL);
  memcpy(array->data + array->elem_size * index, data, array->elem_size);
}
