#ifndef COMMON_PUBLIC_LIST_H__
#define COMMON_PUBLIC_LIST_H__

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

// Reserves memory for a total of `num_elems' objects.
// Increases the count of elements in the list,
// but does not initialize the new objects.
// Capacity always increases by a factor of 2.
// Returns whether the allocation was successful.
bool List_expand(List *list, size_t num_elems);

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

// Truncates a list by removing elements after `start_index'.
void List_truncate(List *list, size_t start_index);

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

#define DECLARE_LIST(name, T)                                                  \
  /* A dynamically growing list. */                                            \
  typedef struct name##List name##List;                                        \
  /* Creates a new name##List object. */                                       \
  name##List *name##List_alloc();                                              \
  /* Gets the data for the list. */                                            \
  T *name##List_get_data(const name##List *list);                              \
  /* Gets the item at `index' */                                               \
  T name##List_get(const name##List *self, size_t index);                      \
  /* Gets the item at `index' */                                               \
  T *name##List_get_ref(const name##List *self, size_t index);                 \
  /* Sets the item at `index' to the given data. `index' must be within the    \
   * bounds of the list. */                                                    \
  void name##List_set(name##List *self, size_t index, const T elem);           \
  /* Sets the item at `index' to the given data. `index' must be within the    \
   * bounds of the list. */                                                    \
  void name##List_set_ref(name##List *self, size_t index, const T *elem);      \
  /* Sets the data at `index' to the given data. `index' and data must be      \
   * within the bounds of the list. */                                         \
  void name##List_set_range(name##List *self, size_t index, const T *elem,     \
                            int count);                                        \
  /* Inserts the given element somewhere in the middle of the list. Returns a  \
   * pointer to the inserted data. Returns NULL if unsuccessful. */            \
  T *name##List_insert(name##List *self, size_t index, const T elem);          \
  /* Inserts the given element somewhere in the middle of the list. Returns a  \
   * pointer to the inserted data. Returns NULL if unsuccessful. */            \
  T *name##List_insert_ref(name##List *self, size_t index, const T *elem);     \
  /* Inserts the given elements somewhere in the middle of the list. Returns a \
   * pointer to the inserted data. Returns NULL if unsuccessful. */            \
  T *name##List_insert_range(name##List *self, size_t index, const T *elems,   \
                             size_t count);                                    \
  /* Appends the given element to the list. Returns a pointer to the appended  \
   * data. Returns NULL if unsuccessful. */                                    \
  T *name##List_add(name##List *self, const T elem);                           \
  /* Appends the given element to the list. Returns a pointer to the appended  \
   * data. Returns NULL if unsuccessful. */                                    \
  T *name##List_add_ref(name##List *self, const T *elem);                      \
  /* Appends the given elements to the list. Returns a pointer to the appended \
   * data. Returns NULL if unsuccessful. */                                    \
  T *name##List_add_range(name##List *self, const T *elems, size_t count);     \
  /* Gets an Iterator for this name##List */                                   \
  void name##List_get_iterator(const name##List *self, name##Iterator *iter);  \
  /* Gets a reverse Iterator for this name##List */                            \
  void name##List_get_reverse_iterator(const name##List *self,                 \
                                       name##Iterator *iter);                  \
  /* Gets a Sink for this list */                                              \
  void name##List_get_sink(const name##List *self, name##Sink *sink);          \
  /* Gets an Indexer for this list */                                          \
  void name##List_get_indexer(const name##List *self, name##Indexer *indexer);

#define DEFINE_LIST(name, T)                                                   \
  name##List *name##List_alloc() {                                             \
    return (name##List *)List_alloc(sizeof(T));                                \
  }                                                                            \
  T *name##List_get_data(const name##List *self) {                             \
    return (T *)List_get_data((const List *)self);                             \
  }                                                                            \
  T name##List_get(const name##List *self, size_t index) {                     \
    return *(T *)List_get((const List *)self, index);                          \
  }                                                                            \
  T *name##List_get_ref(const name##List *self, size_t index) {                \
    return (T *)List_get((const List *)self, index);                           \
  }                                                                            \
  void name##List_set(name##List *self, size_t index, const T elem) {          \
    List_set((List *)self, index, &elem);                                      \
  }                                                                            \
  void name##List_set_ref(name##List *self, size_t index, const T *elem) {     \
    List_set((List *)self, index, elem);                                       \
  }                                                                            \
  void name##List_set_range(name##List *self, size_t index, const T *elem,     \
                            int count) {                                       \
    List_set_range((List *)self, index, elem, count);                          \
  }                                                                            \
  T *name##List_insert(name##List *self, size_t index, const T elem) {         \
    return (T *)List_insert((List *)self, index, &elem);                       \
  }                                                                            \
  T *name##List_insert_ref(name##List *self, size_t index, const T *elem) {    \
    return (T *)List_insert((List *)self, index, elem);                        \
  }                                                                            \
  T *name##List_insert_range(name##List *self, size_t index, const T *elems,   \
                             size_t count) {                                   \
    return (T *)List_insert_range((List *)self, index, elems, count);          \
  }                                                                            \
  T *name##List_add(name##List *self, const T elem) {                          \
    return (T *)List_add((List *)self, &elem);                                 \
  }                                                                            \
  T *name##List_add_ref(name##List *self, const T *elem) {                     \
    return (T *)List_add((List *)self, elem);                                  \
  }                                                                            \
  T *name##List_add_range(name##List *self, const T *elems, size_t count) {    \
    return (T *)List_add_range((List *)self, elems, count);                    \
  }                                                                            \
  void name##List_get_iterator(const name##List *self, name##Iterator *iter) { \
    List_get_iterator((const List *)self, (Iterator *)iter);                   \
  }                                                                            \
  void name##List_get_reverse_iterator(const name##List *self,                 \
                                       name##Iterator *iter) {                 \
    List_get_reverse_iterator((const List *)self, (Iterator *)iter);           \
  }                                                                            \
  void name##List_get_sink(const name##List *self, name##Sink *sink) {         \
    List_get_sink((const List *)self, (Sink *)sink);                           \
  }                                                                            \
  void name##List_get_indexer(const name##List *self,                          \
                              name##Indexer *indexer) {                        \
    List_get_indexer((const List *)self, (Indexer *)indexer);                  \
  }

DECLARE_LIST(Int, int)
DECLARE_LIST(Long, long)
DECLARE_LIST(Char, char)
DECLARE_LIST(Float, float)
DECLARE_LIST(Double, double)
DECLARE_LIST(UnsignedInt, unsigned int)
DECLARE_LIST(UnsignedLong, unsigned long)
DECLARE_LIST(UnsignedChar, unsigned char)
DECLARE_LIST(String, char *)

#endif // COMMON_PUBLIC_LIST_H__
