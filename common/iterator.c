#include "public/iterator.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "public/array.h"
#include "public/list.h"
#include "public/forward_list.h"
#include "public/map.h"
#include "public/priority_queue.h"
#include "public/queue.h"
#include "public/set.h"
#include "public/stack.h"
#include "public/vector.h"

void for_each(Iterator *iter, void (*action)(void *elem)) {
  ASSERT(iter != NULL);
  if (action) {
    while (iter->move_next(iter)) {
      action(iter->current(iter));
    }
  }
}

void for_each_ext(Iterator *iter,
                  void (*action)(void *elem, const Iterator *iter)) {
  ASSERT(iter != NULL);

  if (action) {
    while (iter->move_next(iter)) {
      action(iter->current(iter), iter);
    }
  }
}

void sink_ensure_size_(const Sink *sink, size_t num_elems) {
  ASSERT(sink != NULL);
  ASSERT(num_elems >= 0);

  if (sink->collection_type == COLLECTION_VECTOR) {
    Vector_reserve((Vector *)sink->collection, num_elems);
  }
}

void sink_ensure_iterator_size_(const Sink *dest, Iterator *source) {
  ASSERT(dest != NULL);
  ASSERT(source != NULL);

  size_t size = 0;
  switch (source->collection_type) {
  case COLLECTION_ARRAY:
    size = Array_count((Array *)source->collection);
    break;
  case COLLECTION_VECTOR:
    size = Vector_count((Vector *)source->collection);
    break;
  case COLLECTION_LIST:
    size = List_count((List *)source->collection);
    break;
  case COLLECTION_FORWARD_LIST:
    size = ForwardList_count((ForwardList *)source->collection);
    break;
  case COLLECTION_STACK:
    size = Stack_count((Stack *)source->collection);
    break;
  case COLLECTION_QUEUE:
    size = Queue_count((Queue *)source->collection);
    break;
  case COLLECTION_PRIORITY_QUEUE:
    size = PriorityQueue_count((PriorityQueue *)source->collection);
    break;
  case COLLECTION_SET:
    size = Set_count((Set *)source->collection);
    break;
  case COLLECTION_MAP:
    size = Map_count((Map *)source->collection);
    break;
  default:
    return;
  }
  if (size) {
    sink_ensure_size_(dest, size);
  }
}

bool Iterator_copy(Sink *dest, Iterator *iter) {
  ASSERT(dest != NULL);
  ASSERT(iter != NULL);

  sink_ensure_iterator_size_(dest, iter);
  while (iter->move_next(iter)) {
    void *added = dest->add(dest, iter->current(iter));
    if (!added) {
      return false;
    }
  }
  return true;
}

void indexer_sort(const Indexer *indexer,
                  int (*compare_fn)(const void *a, const void *b)) {
  ASSERT(indexer->collection_type & (COLLECTION_ARRAY | COLLECTION_VECTOR));
  void *data = NULL;
  switch (indexer->collection_type) {
  case COLLECTION_VECTOR:
    data = Vector_get_data((Vector *)indexer->collection);
    break;
  case COLLECTION_ARRAY:
    data = Array_get_data((Array *)indexer->collection);
    break;
  default:
    return; // not supported
  }
  if (data) {
    qsort(data, indexer->size(indexer), indexer->elem_size, compare_fn);
  }
}

bool Iterator_sort(Sink *dest, Iterator *iter,
               int (*compare_fn)(const void *a, const void *b)) {
  bool status = false;
  Indexer indexer;
  Sink interm_sink;
  Vector *temp_list;
  Iterator list_iter;
  Sink *interm_dest = dest;
  if (dest->collection_type & (COLLECTION_ARRAY | COLLECTION_VECTOR)) {
    // Write directly to the collection instead of using a temporary list
    if (dest->collection_type == COLLECTION_VECTOR) {
      Vector *dest_list = dest->collection;
      Vector_clear(dest_list);
      Vector_get_indexer(dest_list, &indexer);
    } else {
      Array *dest_array = dest->collection;
      Array_get_indexer(dest_array, &indexer);
    }
    interm_dest = dest;
  } else {
    // Use a temporary list
    temp_list = Vector_alloc(iter->elem_size);
    if (!temp_list) {
      goto out;
    }
    Vector_get_indexer(temp_list, &indexer);
    Vector_get_iterator(temp_list, &list_iter);
    Vector_get_sink(temp_list, &interm_sink);
    interm_dest = &interm_sink;
  }
  // Copy the source and sort
  Iterator_copy(interm_dest, iter);
  indexer_sort(&indexer, compare_fn);
  // Copy and free any temporary list.
  if (temp_list) {
    Iterator_copy(dest, &list_iter);
  }
out_temp_list:
  Vector_free(temp_list);
out:
  return status;
}

void Iterator_map(const Sink *dest, Iterator *iter,
              void (*map_fn)(void *dest, const void *elem)) {}

void Iterator_flat_map(const Sink *dest, Iterator *iter,
                   void (*map_fn)(Sink *dest, const void *elem)) {}

void Iterator_filter(const Sink *dest, Iterator *iter,
                 bool (*filter_fn)(const void *elem)) {}

void Iterator_reduce(const void *dest, Iterator *iter,
                     void (*reduce_fn)(void *dest, const void *elem)) {}

int CString_compare(const void *a, const void *b) {
  return strcmp((char *)a, (char *)b);
}

int CStringCase_compare(const void *a, const void *b) {
  return strcasecmp((char *)a, (char *)b);
}

int CString_hash(const void *key) {
  int hash = 13;
  for (char *c = (char *)key; *c; c++) {
    hash = hash * 7 + 17 * *c;
  }
  return hash;
}

int CStringCase_hash(const void *key) {
  int hash = 13;
  for (char *c = (char *)key; *c; c++) {
    hash = hash * 7 + 17 * toupper(*c);
  }
  return hash;
}

DEFINE_RELATIONAL_CONTAINER_BASIC(Int, int)

DEFINE_RELATIONAL_CONTAINER_BASIC(Long, long)

DEFINE_RELATIONAL_CONTAINER_BASIC(Short, short)

DEFINE_RELATIONAL_CONTAINER_BASIC(Char, char)

DEFINE_RELATIONAL_CONTAINER_BASIC(Float, float)

DEFINE_RELATIONAL_CONTAINER_BASIC(Double, double)

DEFINE_RELATIONAL_CONTAINER_BASIC(LongDouble, long double)

DEFINE_RELATIONAL_CONTAINER_BASIC(UnsignedShort, unsigned short)

DEFINE_RELATIONAL_CONTAINER_BASIC(UnsignedInt, unsigned int)

DEFINE_RELATIONAL_CONTAINER_BASIC(UnsignedLong, unsigned long)

DEFINE_RELATIONAL_CONTAINER_BASIC(UnsignedChar, unsigned char)

DEFINE_RELATIONAL_CONTAINER_FN(CString, char *, CString_hash, CString_compare)

DEFINE_RELATIONAL_CONTAINER_FN(CStringCase, char *, CStringCase_hash,
                               CString_compare)
