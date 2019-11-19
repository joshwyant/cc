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
  name##List *name##List_alloc();                                              \
  /* Gets the number of elements in the name##List */                          \
  size_t name##List_count(const name##List *list);                              \
  /* Returns whether the list is empty. */                                     \
  bool name##List_empty(const name##List *list);                               \
  /* Gets the capacity of the name##List */                                    \
  size_t name##List_capacity(const name##List *list);                          \
  /* Reserves memory for a total of `num_elems' objects. Capacity always       \
   * increases by a factor of 2. Returns whether the allocation was            \
   * successful. */                                                            \
  bool name##List_reserve(name##List *self, size_t num_elems);                 \
  /* Removes excess memory for the name##List object. Returns whether the      \
   * allocation was successful. */                                             \
  bool name##List_trim(name##List *list);                                      \
  /* Frees up the name##List object. */                                        \
  void name##List_free(name##List *list);                                      \
  /* Gets the size of an element in the name##List */                          \
  size_t name##List_element_size(const name##List *list);                      \
  /* Gets the data for the list. */                                            \
  type *name##List_get_data(const name##List *list);                           \
  /* Gets the item at `index' */                                               \
  type name##List_get(const name##List *self, size_t index);                   \
  /* Gets the item at `index' */                                               \
  type *name##List_get_ref(const name##List *self, size_t index);              \
  /* Sets the item at `index' to the given data. `index' must be within the    \
   * bounds of the list. */                                                    \
  void name##List_set(name##List *self, size_t index, const type elem);        \
  /* Sets the item at `index' to the given data. `index' must be within the    \
   * bounds of the list. */                                                    \
  void name##List_set_ref(name##List *self, size_t index, const type *elem);   \
  /* Sets the data at `index' to the given data. `index' and data must be      \
   * within the bounds of the list. */                                         \
  void name##List_set_range(name##List *self, size_t index, const type *elem,  \
                            int count);                                        \
  /* Inserts the given element somewhere in the middle of the list. Returns a  \
   * pointer to the inserted data. Returns NULL if unsuccessful. */            \
  type *name##List_insert(name##List *self, size_t index, const type elem);    \
  /* Inserts the given element somewhere in the middle of the list. Returns a  \
   * pointer to the inserted data. Returns NULL if unsuccessful. */            \
  type *name##List_insert_ref(name##List *self, size_t index,                  \
                              const type *elem);                               \
  /* Inserts the given elements somewhere in the middle of the list. Returns a \
   * pointer to the inserted data. Returns NULL if unsuccessful. */            \
  type *name##List_insert_range(name##List *self, size_t index,                \
                                const type *elems, size_t count);              \
  /* Appends the given element to the list. Returns a pointer to the appended  \
   * data. Returns NULL if unsuccessful. */                                    \
  type *name##List_add(name##List *self, const type elem);                     \
  /* Appends the given element to the list. Returns a pointer to the appended  \
   * data. Returns NULL if unsuccessful. */                                    \
  type *name##List_add_ref(name##List *self, const type *elem);                \
  /* Appends the given elements to the list. Returns a pointer to the appended \
   * data. Returns NULL if unsuccessful. */                                    \
  type *name##List_add_range(name##List *self, const type *elems,              \
                             size_t count);                                    \
  /* Removes an element from the list. */                                      \
  void name##List_remove(name##List *self, size_t start_index);                \
  /* Removes all the items from the list. Remember to clean up memory first.   \
   */                                                                          \
  void name##List_clear(name##List *list);                                     \
  /* Removes a range of elements from the list. */                             \
  void name##List_remove_range(name##List *self, size_t start_index,           \
                               size_t count);                                  \
  /* Copies a list. Returns whether successful. */                             \
  bool name##List_copy(name##List *dest_list, const name##List *list);         \
  /* Gets an Iterator for this name##List */                                   \
  void name##List_get_iterator(const name##List *self, name##Iterator *iter);  \
  /* Gets a reverse Iterator for this name##List */                            \
  void name##List_get_reverse_iterator(const name##List *self,                 \
                                       name##Iterator *iter);                  \
  /* Gets a Sink for this list */                                              \
  void name##List_get_sink(const name##List *self, name##Sink *sink);          \
  /* Gets an Indexer for this list */                                          \
  void name##List_get_indexer(const name##List *self, name##Indexer *indexer);

#define DEFINE_LIST(name, type)                                                \
  name##List *name##List_alloc() {                                             \
    return (name##List *)List_alloc(sizeof(type));                             \
  }                                                                            \
  size_t name##List_count(const name##List *self) {                             \
    return List_count((const List *)self);                                      \
  }                                                                            \
  bool name##List_empty(const name##List *self) {                              \
    return List_empty((const List *)self);                                     \
  }                                                                            \
  size_t name##List_apacity(const name##List *self) {                          \
    return List_capacity((const List *)self);                                  \
  }                                                                            \
  bool name##List_reserve(name##List *self, size_t num_elems) {                \
    return List_reserve((List *)self, num_elems);                              \
  }                                                                            \
  bool name##List_trim(name##List *self) { return List_trim((List *)self); }   \
  void name##List_free(name##List *self) { List_free((List *)self); }          \
  size_t name##List_element_size(const name##List *self) {                     \
    return List_element_size((const List *)self);                              \
  }                                                                            \
  type *name##List_get_data(const name##List *self) {                          \
    return (type *)List_get_data((const List *)self);                          \
  }                                                                            \
  type name##List_get(const name##List *self, size_t index) {                  \
    return *(type *)List_get((const List *)self, index);                       \
  }                                                                            \
  type *name##List_get_ref(const name##List *self, size_t index) {             \
    return (type *)List_get((const List *)self, index);                        \
  }                                                                            \
  void name##List_set(name##List *self, size_t index, const type elem) {       \
    List_set((List *)self, index, &elem);                                      \
  }                                                                            \
  void name##List_set_ref(name##List *self, size_t index, const type *elem) {  \
    List_set((List *)self, index, elem);                                       \
  }                                                                            \
  void name##List_set_range(name##List *self, size_t index, const type *elem,  \
                            int count) {                                       \
    List_set_range((List *)self, index, elem, count);                          \
  }                                                                            \
  type *name##List_insert(name##List *self, size_t index, const type elem) {   \
    return (type *)List_insert((List *)self, index, &elem);                    \
  }                                                                            \
  type *name##List_insert_ref(name##List *self, size_t index,                  \
                              const type *elem) {                              \
    return (type *)List_insert((List *)self, index, elem);                     \
  }                                                                            \
  type *name##List_insert_range(name##List *self, size_t index,                \
                                const type *elems, size_t count) {             \
    return (type *)List_insert_range((List *)self, index, elems, count);       \
  }                                                                            \
  type *name##List_add(name##List *self, const type elem) {                    \
    return (type *)List_add((List *)self, &elem);                              \
  }                                                                            \
  type *name##List_add_ref(name##List *self, const type *elem) {               \
    return (type *)List_add((List *)self, elem);                               \
  }                                                                            \
  type *name##List_add_range(name##List *self, const type *elems,              \
                             size_t count) {                                   \
    return (type *)List_add_range((List *)self, elems, count);                 \
  }                                                                            \
  void name##List_remove(name##List *self, size_t start_index) {               \
    List_remove((List *)self, start_index);                                    \
  }                                                                            \
  void name##List_clear(name##List *self) { List_clear((List *)self); }        \
  void name##List_remove_range(name##List *self, size_t start_index,           \
                               size_t count) {                                 \
    List_remove_range((List *)self, start_index, count);                       \
  }                                                                            \
  bool name##List_copy(name##List *dest_list, const name##List *list) {        \
    return List_copy((List *)dest_list, (List *)list);                         \
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
