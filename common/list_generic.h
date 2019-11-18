#ifndef COMMON_LIST_GENERIC_H__
#define COMMON_LIST_GENERIC_H__

#include <stdbool.h>
#include <stddef.h>

#include "iterator.h"
#include "list.h"

#define DECLARE_LIST(name, type)                                               \
  /* A dynamically growing list. */                                            \
  typedef struct List name##List;                                              \
  /* Creates a new name##List object. */                                       \
  name##List *name##ListAlloc();                                               \
  /* Initializes a pre-allocated name##List object */                          \
  bool name##ListInit(name##List *list);                                       \
  /* Gets the number of elements in the name##List */                          \
  size_t name##ListSize(const name##List *list);                               \
  /* Returns whether the list is empty. */                                     \
  bool name##ListEmpty(const name##List *list);                                \
  /* Gets the capacity of the name##List */                                    \
  size_t name##ListCapacity(const name##List *list);                           \
  /* Reserves memory for a total of `num_elems' objects. Capacity always       \
   * increases by a factor of 2. Returns whether the allocation was            \
   * successful. */                                                            \
  bool name##ListReserve(name##List *list, size_t num_elems);                  \
  /* Removes excess memory for the name##List object. Returns whether the      \
   * allocation was successful. */                                             \
  bool name##ListTrim(name##List *list);                                       \
  /* Frees up the name##List object. */                                        \
  void name##ListFree(name##List *list);                                       \
  /* Cleans up the name##List object, but does not free it. */                 \
  void name##ListCleanup(name##List *list);                                    \
  /* Gets the size of an element in the name##List */                          \
  size_t name##ListElementSize(const name##List *list);                        \
  /* Gets the data for the list. */                                            \
  type *name##ListGetData(const name##List *list);                             \
  /* Gets the item at `index' */                                               \
  type name##ListGet(const name##List *list, size_t index);                    \
  /* Gets the item at `index' */                                               \
  type *name##ListGetRef(const name##List *list, size_t index);                \
  /* Sets the item at `index' to the given data. `index' must be within the    \
   * bounds of the list. */                                                    \
  void name##ListSet(name##List *list, size_t index, const type elem);         \
  /* Sets the item at `index' to the given data. `index' must be within the    \
   * bounds of the list. */                                                    \
  void name##ListSetRef(name##List *list, size_t index, const type *elem);     \
  /* Sets the data at `index' to the given data. `index' and data must be      \
   * within the bounds of the list. */                                         \
  void name##ListSetRange(name##List *list, size_t index, const type *elem,    \
                          int count);                                          \
  /* Inserts the given element somewhere in the middle of the list. Returns a  \
   * pointer to the inserted data. Returns NULL if unsuccessful. */            \
  type *name##ListInsert(name##List *list, size_t index, const type elem);     \
  /* Inserts the given element somewhere in the middle of the list. Returns a  \
   * pointer to the inserted data. Returns NULL if unsuccessful. */            \
  type *name##ListInsertRef(name##List *list, size_t index, const type *elem); \
  /* Inserts the given elements somewhere in the middle of the list. Returns a \
   * pointer to the inserted data. Returns NULL if unsuccessful. */            \
  type *name##ListInsertRange(name##List *list, size_t index,                  \
                              const type *elems, size_t count);                \
  /* Appends the given element to the list. Returns a pointer to the appended  \
   * data. Returns NULL if unsuccessful. */                                    \
  type *name##ListAppend(name##List *list, const type elem);                   \
  /* Appends the given element to the list. Returns a pointer to the appended  \
   * data. Returns NULL if unsuccessful. */                                    \
  type *name##ListAppendRef(name##List *list, const type *elem);               \
  /* Appends the given elements to the list. Returns a pointer to the appended \
   * data. Returns NULL if unsuccessful. */                                    \
  type *name##ListAppendRange(name##List *list, const type *elems,             \
                              size_t count);                                   \
  /* Removes an element from the list. */                                      \
  void name##ListRemove(name##List *list, size_t start_index);                 \
  /* Removes all the items from the list. Remember to clean up memory first.   \
   */                                                                          \
  void name##ListClear(name##List *list);                                      \
  /* Removes a range of elements from the list. */                             \
  void name##ListRemoveRange(name##List *list, size_t start_index,             \
                             size_t count);                                    \
  /* Copies a list. Returns whether successful. */                             \
  bool name##ListCopy(name##List *dest_list, const name##List *list);          \
  /* Gets an Iterator for this name##List */                                   \
  void name##ListGetIterator(const name##List *list, name##Iterator *iter);    \
  /* Gets a reverse Iterator for this name##List */                            \
  void name##ListGetReverseIterator(const name##List *list,                    \
                                    name##Iterator *iter);                     \
  /* Gets a Sink for this list */                                              \
  void name##ListGetSink(const name##List *list, name##Sink *sink);            \
  /* Gets an Indexer for this list */                                          \
  void name##ListGetIndexer(const name##List *list, name##Indexer *indexer);

#define DEFINE_LIST(name, type)                                                \
  name##List *name##ListAlloc() {                                              \
    return (name##List *)list_alloc(sizeof(type));                             \
  }                                                                            \
  bool name##ListInit(name##List *list) {                                      \
    return list_init((List *)list, sizeof(type));                              \
  }                                                                            \
  size_t name##ListSize(const name##List *list) {                              \
    return list_size((const List *)list);                                      \
  }                                                                            \
  bool name##ListEmpty(const name##List *list) {                               \
    return list_empty((const List *)list);                                     \
  }                                                                            \
  size_t name##ListCapacity(const name##List *list) {                          \
    return list_capacity((const List *)list);                                  \
  }                                                                            \
  bool name##ListReserve(name##List *list, size_t num_elems) {                 \
    return list_reserve((List *)list, num_elems);                              \
  }                                                                            \
  bool name##ListTrim(name##List *list) { return list_trim((List *)list); }    \
  void name##ListFree(name##List *list) { list_free((List *)list); }           \
  void name##ListCleanup(name##List *list) { list_cleanup((List *)list); }     \
  size_t name##ListElementSize(const name##List *list) {                       \
    return list_element_size((const List *)list);                              \
  }                                                                            \
  type *name##ListGetData(const name##List *list) {                            \
    return (type *)list_get_data((const List *)list);                          \
  }                                                                            \
  type name##ListGet(const name##List *list, size_t index) {                   \
    return *(type *)list_get((const List *)list, index);                       \
  }                                                                            \
  type *name##ListGetRef(const name##List *list, size_t index) {               \
    return (type *)list_get((const List *)list, index);                        \
  }                                                                            \
  void name##ListSet(name##List *list, size_t index, const type elem) {        \
    list_set((List *)list, index, &elem);                                      \
  }                                                                            \
  void name##ListSetRef(name##List *list, size_t index, const type *elem) {    \
    list_set((List *)list, index, elem);                                       \
  }                                                                            \
  void name##ListSetRange(name##List *list, size_t index, const type *elem,    \
                          int count) {                                         \
    list_set_range((List *)list, index, elem, count);                          \
  }                                                                            \
  type *name##ListInsert(name##List *list, size_t index, const type elem) {    \
    return (type *)list_insert((List *)list, index, &elem);                    \
  }                                                                            \
  type *name##ListInsertRef(name##List *list, size_t index,                    \
                            const type *elem) {                                \
    return (type *)list_insert((List *)list, index, elem);                     \
  }                                                                            \
  type *name##ListInsertRange(name##List *list, size_t index,                  \
                              const type *elems, size_t count) {               \
    return (type *)list_insert_range((List *)list, index, elems, count);       \
  }                                                                            \
  type *name##ListAppend(name##List *list, const type elem) {                  \
    return (type *)list_append((List *)list, &elem);                           \
  }                                                                            \
  type *name##ListAppendRef(name##List *list, const type *elem) {              \
    return (type *)list_append((List *)list, elem);                            \
  }                                                                            \
  type *name##ListAppendRange(name##List *list, const type *elems,             \
                              size_t count) {                                  \
    return (type *)list_append_range((List *)list, elems, count);              \
  }                                                                            \
  void name##ListRemove(name##List *list, size_t start_index) {                \
    list_remove((List *)list, start_index);                                    \
  }                                                                            \
  void name##ListClear(name##List *list) { list_clear((List *)list); }         \
  void name##ListRemoveRange(name##List *list, size_t start_index,             \
                             size_t count) {                                   \
    list_remove_range((List *)list, start_index, count);                       \
  }                                                                            \
  bool name##ListCopy(name##List *dest_list, const name##List *list) {         \
    return list_copy((List *)dest_list, (List *)list);                         \
  }                                                                            \
  void name##ListGetIterator(const name##List *list, name##Iterator *iter) {   \
    list_get_iterator((const List *)list, (Iterator *)iter);                   \
  }                                                                            \
  void name##ListGetReverseIterator(const name##List *list,                    \
                                    name##Iterator *iter) {                    \
    list_get_reverse_iterator((const List *)list, (Iterator *)iter);           \
  }                                                                            \
  void name##ListGetSink(const name##List *list, name##Sink *sink) {           \
    list_get_sink((const List *)list, (Sink *)sink);                           \
  }                                                                            \
  void name##ListGetIndexer(const name##List *list, name##Indexer *indexer) {  \
    list_get_indexer((const List *)list, (Indexer *)indexer);                  \
  }

DECLARE_LIST(Int, int);

DECLARE_LIST(Long, long);

DECLARE_LIST(Char, char);

DECLARE_LIST(Float, float);

DECLARE_LIST(Double, double);

DECLARE_LIST(UnsignedInt, unsigned int);

DECLARE_LIST(UnsignedLong, unsigned long);

DECLARE_LIST(UnsignedChar, unsigned char);

DECLARE_LIST(String, char *);

#endif // COMMON_LIST_GENERIC_H__
