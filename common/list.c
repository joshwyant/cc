#include "protected/list.h"

#include <math.h>
#include <string.h>

#include "../test/stubs.h"
#include "public/assert.h"

bool List_init(List *list, size_t elem_size);

void List_cleanup(List *list);

DEFINE_LIST(Int, int);
DEFINE_LIST(Long, long);
DEFINE_LIST(Char, char);
DEFINE_LIST(Float, float);
DEFINE_LIST(Double, double);
DEFINE_LIST(UnsignedInt, unsigned int);
DEFINE_LIST(UnsignedLong, unsigned long);
DEFINE_LIST(UnsignedChar, unsigned char);
DEFINE_LIST(String, char *);

List *List_alloc(size_t elem_size) 
{
  ASSERT(elem_size > 0);

  List *list = malloc(sizeof(List));
  if (list && !List_init(list, elem_size)) {
    free(list);
    return NULL;
  }
  return list;
}

bool List_init(List *list, size_t elem_size) 
{
  ASSERT(list != NULL);
  ASSERT(elem_size > 0);

  list->data = NULL;
  list->data_size = 0;
  list->elem_size = elem_size;
  list->elem_count = 0;
  list->reserve_count = 0;
  list->version = 0;
  return true;
}

size_t List_count(const List *list) 
{
  ASSERT(list != NULL);
  return list->elem_count;
}

bool List_empty(const List *list) 
{
  ASSERT(list != NULL);
  return list->elem_count == 0;
}

size_t List_capacity(const List *list) 
{
  ASSERT(list != NULL);
  return list->data_size / list->elem_size;
}

bool _List_reserve_ext(List *list, size_t num_elems, bool slim) 
{
  ASSERT(list != NULL);
  ASSERT(num_elems >= 0);

  if (list->reserve_count >= num_elems) {
    return true; // No-op case
  }
  // Increase capacity by factor of 2
  size_t reserve_count = list->reserve_count == 0 ? 1 : list->reserve_count;
  while (reserve_count < num_elems) {
    reserve_count <<= 1;
  }
  size_t new_data_size = list->elem_size * (slim ? num_elems : reserve_count);
  void *data = NULL;
  if ((data = realloc(list->data, new_data_size)) == NULL) {
    return false;
  }
  list->data = data;
  list->reserve_count = reserve_count;
  list->data_size = (slim ? num_elems : reserve_count) * list->elem_size;
  return true;
} // _List_reserve_ext

bool List_reserve(List *list, size_t num_elems) 
{
  return _List_reserve_ext(list, num_elems, true);
}

bool List_expand(List *list, size_t num_elems)
{
  if (!List_reserve(list, num_elems)) return false;
  list->elem_count = num_elems;
  return true;
}

bool List_trim(List *list) 
{
  ASSERT(list != NULL);

  if (list->elem_count == 0) {
    if (list->data) {
      free(list->data);
      list->data = NULL;
    }
    list->data_size = 0;
    list->reserve_count = 0;
    return true;
  }
  if (list->elem_count < list->reserve_count) {
    // Allocate and copy the data
    void *data = realloc(list->data, list->elem_size * list->elem_count);
    if (!data) {
      return false;
    }
    list->data_size = list->elem_size * list->elem_count;
    list->data = data;
    // Lower the potential capacity.
    while ((list->reserve_count >> 1) >= list->elem_count) {
      list->reserve_count >>= 1;
    }
  }
  return true;
} // List_trim

void List_free(List *list) 
{
  ASSERT(list != NULL);
  List_cleanup(list);
  free(list);
}

void List_cleanup(List *list) 
{
  ASSERT(list != NULL);

  if (list->data) {
    free(list->data);
  }
  list->data = NULL;
  list->data_size = 0;
  list->elem_count = 0;
  list->reserve_count = 0;
  list->version++;
} // List_cleanup

size_t List_element_size(const List *list) 
{
  ASSERT(list != NULL);
  return list->elem_size;
}

// Gets the data for the list.
void *List_get_data(const List *list) {
  ASSERT(list != NULL);
  return list->data;
}

// Gets the item at `index'
void *List_get(const List *list, size_t index) 
{
  ASSERT(list != NULL);
  ASSERT(index >= 0);
  ASSERT(index < list->elem_count && "index is greater than list size.");

  return list->data + list->elem_size * index;
}

// Sets the item at `index' to the given data.
// `index' must be within the bounds of the list.
void List_set(List *list, size_t index, const void *elem) 
{
  List_set_range(list, index, elem, 1);
}

// Sets the data at `index' to the given data.
// `index' and data must be within the bounds of the list.
void List_set_range(List *list, size_t index, const void *elems, int count) 
{
  ASSERT(list != NULL);
  ASSERT(index >= 0);
  ASSERT(index < list->elem_count && "index is >= list size.");
  ASSERT(index + count <= list->elem_count &&
         "index + count is greater than list size.");
  ASSERT(elems != NULL);
  ASSERT(count >= 0);

  memcpy(list->data + list->elem_size * index, elems, list->elem_size * count);

  list->version++;
}

void *List_insert(List *list, size_t index, const void *elem) 
{
  return List_insert_range(list, index, elem, 1);
}

void *List_insert_range(List *list, size_t index, const void *elems,
                        size_t count) 
{
  ASSERT(list != NULL);
  ASSERT(index >= 0);
  ASSERT(elems != NULL);
  ASSERT(count >= 0);
  ASSERT(index <= list->elem_count);

  list->version++;
  if (count == 0) {
    return NULL; // No-op case
  }
  if (!_List_reserve_ext(list, list->elem_count + count, false)) {
    return NULL;
  }
  size_t shifted_count = list->elem_count - index;
  size_t shifted_size = list->elem_size * shifted_count;
  size_t data_size = list->elem_size * count;
  void *insertion_dest = list->data + list->elem_size * index;
  // Shift the data to make room if not appending
  if (index != list->elem_count) {
    memcpy(insertion_dest + data_size, insertion_dest, shifted_size);
  }
  if (elems !=
      insertion_dest) { // a trick to shift the data without initializing
    // Copy the elements to the list
    memcpy(insertion_dest, elems, data_size);
  }
  list->elem_count += count;
  return insertion_dest;
} // List_insert_range

// Appends the given element to the end of the list.
// Returns a pointer to the appended element. Returns NULL if unsuccessful.
void *List_add(List *list, const void *elem) 
{
  return List_insert_range(list, list->elem_count, elem, 1);
}

// Appends the given element to the end of the list.
// Returns a pointer to the appended element. Returns NULL if unsuccessful.
void *List_add_range(List *list, const void *elems, size_t count) 
{
  return List_insert_range(list, list->elem_count, elems, count);
}

// Removes an element from the list.
void List_remove(List *list, size_t start_index)
{
  List_remove_range(list, start_index, 1); // Additional assertions done here
}

void List_clear(List *list) 
{
  List_truncate(list, 0);
}

void List_truncate(List *list, size_t start_index) 
{
  List_remove_range(list, start_index, list->elem_count - start_index);
}

// Removes a range of elements from the list.
void List_remove_range(List *list, size_t start_index, size_t count) 
{
  ASSERT(list != NULL);
  ASSERT(start_index >= 0);
  ASSERT(start_index <= list->elem_count);
  ASSERT(count >= 0);
  ASSERT(start_index + count <= list->elem_count);

  if (count == list->elem_count) {
    List_cleanup(list); // clean slate; everything short of freeing the list.
    return;
  }
  list->version++;
  void *new_data = list->data;
  // Can we reduce the size of the list by at least half?
  if (list->reserve_count >> 1 >= list->elem_count - count) {
    do {
      list->reserve_count >>= 1;
    } while (list->reserve_count >> 1 > list->elem_count - count);
    new_data = malloc(list->reserve_count * list->elem_size);
    if (new_data == NULL) {
      new_data = list->data;
    } else {
      if (list->data && start_index > 0) {
        // Copy the first part of the old data.
        memcpy(new_data, list->data, start_index * list->elem_size);
      }
    }
  }
  if (count > 0 && start_index + count < list->elem_count) {
    // Shift the end of the list down
    memcpy(list->elem_size * start_index + new_data,
           list->elem_size * (start_index + count) + list->data,
           list->elem_size * count);
  }
  if (new_data && new_data != list->data) {
    free(list->data);
  }
  list->elem_count -= count;
  list->data = new_data;
} // List_remove_range

// Copies a list. Returns whether successful.
bool List_copy(List *dest_list, const List *list) 
{
  ASSERT(dest_list != NULL);
  ASSERT(list != NULL);
  ASSERT(list->elem_size == dest_list->elem_size);
  // Is there enough space?
  if (dest_list->reserve_count != list->reserve_count ||
      dest_list->data_size / dest_list->elem_size < list->elem_count) {
    List_cleanup(dest_list);
    if (!List_reserve(dest_list, list->elem_count)) {
      return false;
    }
  }
  memcpy(dest_list->data, list->data, list->elem_size * list->elem_count);
  dest_list->elem_count = list->elem_count;
  return true;
} // List_copy

void *List_iter_current_(const Iterator *iter);
void *List_iter_current_reverse_(const Iterator *iter);
bool List_iter_eof_(const Iterator *iter);
bool List_iter_eof_reverse_(const Iterator *iter);
bool List_iter_move_next_(Iterator *iter);
bool List_iter_move_next_reverse_(Iterator *iter);

void *List_iter_current_(const Iterator *iter) 
{
  ASSERT(iter != NULL);
  ASSERT(iter->collection_type == COLLECTION_LIST);
  List *list = iter->collection;
  ASSERT(list != NULL);
  ASSERT(iter->version == list->version &&
         "Collection changed while iterating.");
  if (List_iter_eof_(iter)) {
    return NULL;
  }
  return List_get(list, iter->impl_data1);
}

void *List_iter_current_reverse_(const Iterator *iter) 
{
  ASSERT(iter != NULL);
  ASSERT(iter->collection_type == COLLECTION_LIST);
  List *list = iter->collection;
  ASSERT(list != NULL);
  ASSERT(iter->version == list->version &&
         "Collection changed while iterating.");
  if (List_iter_eof_reverse_(iter)) {
    return NULL;
  }
  return List_get(list, iter->impl_data1);
}

bool List_iter_eof_(const Iterator *iter) 
{
  ASSERT(iter != NULL);
  ASSERT(iter->collection_type == COLLECTION_LIST);
  List *list = iter->collection;
  ASSERT(list != NULL);
  ASSERT(iter->version == list->version &&
         "Collection changed while iterating.");
  return List_empty(list) || iter->impl_data1 >= (int)list->elem_count;
}

bool List_iter_eof_reverse_(const Iterator *iter) 
{
  ASSERT(iter != NULL);
  ASSERT(iter->collection_type == COLLECTION_LIST);
  List *list = iter->collection;
  ASSERT(list != NULL);
  ASSERT(iter->version == list->version &&
         "Collection changed while iterating.");
  return List_empty(list) || iter->impl_data1 < 0;
}

bool List_iter_move_next_(Iterator *iter) 
{
  ASSERT(iter != NULL);
  ASSERT(iter->collection_type == COLLECTION_LIST);
  List *list = iter->collection;
  ASSERT(list != NULL);
  ASSERT(iter->version == list->version &&
         "Collection changed while iterating.");
  if (List_iter_eof_(iter)) {
    return false;
  }
  iter->impl_data1++;
  return !List_iter_eof_(iter);
}

bool List_iter_move_next_reverse_(Iterator *iter) 
{
  ASSERT(iter != NULL);
  ASSERT(iter->collection_type == COLLECTION_LIST);
  List *list = iter->collection;
  ASSERT(list != NULL);
  ASSERT(iter->version == list->version &&
         "Collection changed while iterating.");
  if (List_iter_eof_reverse_(iter)) {
    return false;
  }
  iter->impl_data1--;
  return !List_iter_eof_reverse_(iter);
}

// Gets an Iterator for this List
void List_get_iterator(const List *list, Iterator *iter) 
{
  ASSERT(list != NULL);
  ASSERT(iter != NULL);
  iter->collection_type = COLLECTION_LIST;
  iter->collection = (void *)list;
  iter->elem_size = list->elem_size;
  iter->current = List_iter_current_;
  iter->eof = List_iter_eof_;
  iter->move_next = List_iter_move_next_;
  iter->impl_data1 = -1; // Current index
  iter->impl_data2 = 0;
  iter->version = list->version;
}

void List_get_reverse_iterator(const List *list, Iterator *iter) 
{
  ASSERT(list != NULL);
  ASSERT(iter != NULL);
  iter->collection_type = COLLECTION_LIST;
  iter->collection = (void *)list;
  iter->elem_size = list->elem_size;
  iter->current = List_iter_current_reverse_;
  iter->eof = List_iter_eof_reverse_;
  iter->move_next = List_iter_move_next_reverse_;
  iter->impl_data1 = list->elem_count; // Current index
  iter->impl_data2 = 0;
  iter->version = list->version;
}

void *List_sink_add_(Sink *sink, const void *elem);

void List_get_sink(const List *list, Sink *sink) 
{
  ASSERT(list != NULL);
  ASSERT(sink != NULL);
  sink->collection_type = COLLECTION_LIST;
  sink->collection = (void *)list;
  sink->elem_size = list->elem_size;
  sink->add = List_sink_add_;
}

void *List_sink_add_(Sink *sink, const void *elem) 
{
  ASSERT(sink != NULL);
  List *list = sink->collection;
  ASSERT(list != NULL);
  return List_add(list, elem);
}

size_t List_indexer_size_(const Indexer *indexer);
void *List_indexer_get_(const Indexer *indexer, size_t index);
void List_indexer_set_(Indexer *indexer, size_t index, const void *data);

// Gets an Indexer for this list
void List_get_indexer(const List *list, Indexer *indexer) 
{
  ASSERT(list != NULL);
  ASSERT(indexer != NULL);
  indexer->collection_type = COLLECTION_LIST;
  indexer->collection = (void *)list;
  indexer->elem_size = list->elem_size;
  indexer->size = List_indexer_size_;
  indexer->get = List_indexer_get_;
  indexer->set = List_indexer_set_;
}

size_t List_indexer_size_(const Indexer *indexer) 
{
  ASSERT(indexer != NULL);
  List *list = indexer->collection;
  ASSERT(list != NULL);
  return list->elem_count;
}

void *List_indexer_get_(const Indexer *indexer, size_t index) 
{
  ASSERT(indexer != NULL);
  List *list = indexer->collection;
  ASSERT(list != NULL);
  ASSERT(index >= 0 && index < List_count(list));
  return list->data + list->elem_size * index;
}

void List_indexer_set_(Indexer *indexer, size_t index, const void *data) 
{
  ASSERT(indexer != NULL);
  List *list = indexer->collection;
  ASSERT(list != NULL);
  ASSERT(index >= 0 && index < List_count(list));
  ASSERT(data != NULL);
  memcpy(list->data + list->elem_size * index, data, list->elem_size);
}
