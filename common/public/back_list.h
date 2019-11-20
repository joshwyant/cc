#ifndef COMMON_PUBLIC_BACK_LIST_H__
#define COMMON_PUBLIC_BACK_LIST_H__

#include <stdbool.h>
#include <stddef.h>

#include "iterator.h"

typedef struct BackList BackList;

// TODO:
//  - -Return node after operation-
//  - ??
//  - -Get iterator for nodes-
//  - Get [previous,] -next node-
//  - Insert -[before,] after node-
//  - ~Insert range [before,] after node~
//  - -Remove particular node-
//  - -Remove range of nodes-
//  - -Reverse iterator-
typedef struct BackListNode BackListNode;

// Creates a new BackList object.
BackList *BackList_alloc(size_t elem_size);

// Gets the number of elements in the BackList
size_t BackList_count(const BackList *list);

// Returns whether the list is empty
bool BackList_empty(const BackList *list);

// Frees up the BackList object.
void BackList_free(BackList *list);

// Gets the size of an element in the BackList
size_t BackList_element_size(const BackList *list);

// Gets the size of a node in the BackList
size_t BackList_node_size(const BackList *list);

// Gets the first item
void *BackList_get_first(const BackList *list);

// Gets the last item
void *BackList_get_last(const BackList *list);

// Gets the first node
BackListNode *BackList_get_first_node(const BackList *list);

// Gets the last node
BackListNode *BackList_get_last_node(const BackList *list);

// Gets the next node
BackListNode *BackList_get_next_node(const BackListNode *node);

// Gets the previous node
BackListNode *BackList_get_previous_node(const BackListNode *node);

// Sets the data in the linked list node.
void BackList_set(const BackList *list, BackListNode *node, const void *data);

// Gets the data item in the linked list node.
const void *BackList_get(const BackListNode *node);

// Inserts the given element before the given node. Returns NULL if
// unsuccessful.
BackListNode *BackList_insert_before(BackList *list, BackListNode *node,
                                      const void *elem);

// Inserts the given element after the given node. Returns NULL if unsuccessful.
BackListNode *BackList_insert_after(BackList *list, BackListNode *node,
                                     const void *elem);

// Appends the given element to the list.
// Returns a pointer to the appended node. Returns NULL if unsuccessful.
BackListNode *BackList_append(BackList *list, const void *elem);

// Prepends the given element to the list.
// Returns a pointer to the appended node. Returns NULL if unsuccessful.
BackListNode *BackList_prepend(BackList *list, const void *elem);

// Removes a node from the list.
void BackList_remove(BackList *list, BackListNode *node);

// Removes the first node from the list.
void BackList_remove_first(BackList *list);

// Removes the last node from the list.
void BackList_remove_last(BackList *list);

// Removes all the items from the list.
// Remember to clean up memory first.
void BackList_clear(BackList *list);

// Removes a range of elements from the list starting with the given node.
void BackList_remove_range(BackList *list, BackListNode *node, int count);

// Copies a list. Returns whether successful.
bool BackList_copy(BackList *dest_list, const BackList *list);

// Gets an Iterator for this BackList
void BackList_get_iterator(const BackList *list, Iterator *iter);

// Gets a reverse Iterator for this BackList
void BackList_get_reverse_iterator(const BackList *list, Iterator *iter);

// Gets an Iterator for the nodes in this BackList
void BackList_get_node_iterator(const BackList *list, Iterator *iter);

// Gets a reverse Iterator for the nodes in this BackList
void BackList_get_node_reverse_iterator(const BackList *list, Iterator *iter);

// Gets a Sink for this list
void BackList_get_sink(const BackList *list, Sink *sink);

// Gets a reverse Sink for this list
void BackList_get_reverse_sink(const BackList *list, Sink *sink);


#define DECLARE_BACK_LIST(name, T)                                             \
  typedef struct BackList name##BackList;                                      \
  typedef struct BackListNode name##BackListNode;                                      \
  name##BackList *name##BackList_alloc();                                      \
  T name##BackList_get_first(const name##BackList *list);                     \
  T name##BackList_get_last(const name##BackList *list);                      \
  T *name##BackList_get_first_ref(const name##BackList *list);                     \
  T *name##BackList_get_last_ref(const name##BackList *list);                      \
  name##BackListNode *name##BackList_get_first_node(                           \
      const name##BackList *list);                                             \
  name##BackListNode *name##BackList_get_last_node(                            \
      const name##BackList *list);                                             \
  name##BackListNode *name##BackList_get_next_node(                            \
      const name##BackListNode *node);                                         \
  name##BackListNode *name##BackList_get_previous_node(                        \
      const name##BackListNode *node);                                         \
  void name##BackList_set(const name##BackList *list,                          \
                          name##BackListNode *node, const T data);             \
  const T name##BackList_get(const name##BackListNode *node);                  \
  void name##BackList_set_ref(const name##BackList *list,                      \
                              name##BackListNode *node, const T *data);        \
  const T *name##BackList_get_ref(const name##BackListNode *node);             \
  name##BackListNode *name##BackList_insert_before(                            \
      name##BackList *list, name##BackListNode *node, const T *elem);          \
  name##BackListNode *name##BackList_insert_after(                             \
      name##BackList *list, name##BackListNode *node, const T *elem);          \
  name##BackListNode *name##BackList_append(name##BackList *list,              \
                                            const T *elem);                    \
  name##BackListNode *name##BackList_prepend(name##BackList *list,             \
                                             const T *elem);                   \
  void name##BackList_remove(name##BackList *list, name##BackListNode *node);  \
  void name##BackList_get_iterator(const name##BackList *list,                 \
                                   name##Iterator *iter);                      \
  void name##BackList_get_reverse_iterator(const name##BackList *list,         \
                                           name##Iterator *iter);              \
  void name##BackList_get_node_iterator(const name##BackList *list,            \
                                        name##Iterator *iter);                 \
  void name##BackList_get_node_reverse_iterator(const name##BackList *list,    \
                                                name##Iterator *iter);         \
  void name##BackList_get_sink(const name##BackList *list, name##Sink *sink);  \
  void name##BackList_get_reverse_sink(const name##BackList *list,             \
                                       name##Sink *sink);

#define DEFINE_BACK_LIST(name, T)                                              \
  name##BackList *name##BackList_alloc() {                                     \
    return (name##BackList *)BackList_alloc(sizeof(BackList));                 \
  }                                                                            \
  T name##BackList_get_first(const name##BackList *list) {                     \
    return *name##BackList_get_first_ref(list);                                \
  }                                                                            \
  T name##BackList_get_last(const name##BackList *list) {                      \
    return *name##BackList_get_last_ref(list);                                 \
  }                                                                            \
  T *name##BackList_get_first_ref(const name##BackList *list) {                \
    return (T *)BackList_get_first((const BackList *)list);                    \
  }                                                                            \
  T *name##BackList_get_last_ref(const name##BackList *list) {                 \
    return (T *)BackList_get_last((const BackList *)list);                     \
  }                                                                            \
  name##BackListNode *name##BackList_get_first_node(                           \
      const name##BackList *list) {                                            \
    return (name##BackListNode *)BackList_get_first_node(                      \
        (const BackList *)list);                                               \
  }                                                                            \
  name##BackListNode *name##BackList_get_last_node(                            \
      const name##BackList *list) {                                            \
    return (name##BackListNode *)BackList_get_last_node(                       \
        (const BackList *)list);                                               \
  }                                                                            \
  name##BackListNode *name##BackList_get_next_node(                            \
      const name##BackListNode *node) {                                        \
    return (name##BackListNode *)BackList_get_next_node(                       \
        (const BackListNode *)node);                                           \
  }                                                                            \
  name##BackListNode *name##BackList_get_previous_node(                        \
      const name##BackListNode *node) {                                        \
    return (name##BackListNode *)BackList_get_previous_node(                   \
        (const BackListNode *)node);                                           \
  }                                                                            \
  void name##BackList_set(const name##BackList *list,                          \
                          name##BackListNode *node, const T data) {            \
    name##BackList_set_ref(list, node, &data);                                 \
  }                                                                            \
  const T name##BackList_get(const name##BackListNode *node) {                 \
    return *name##BackList_get_ref(node);                                      \
  }                                                                            \
  void name##BackList_set_ref(const name##BackList *list,                      \
                              name##BackListNode *node, const T *data) {       \
    BackList_set((const BackList *)list, (BackListNode *)node, data);          \
  }                                                                            \
  const T *name##BackList_get_ref(const name##BackListNode *node) {            \
    return (const T *)BackList_get((const BackListNode *)node);                \
  }                                                                            \
  name##BackListNode *name##BackList_insert_before(                            \
      name##BackList *list, name##BackListNode *node, const T *elem) {         \
    return (name##BackListNode *)BackList_insert_before(                       \
        (BackList *)list, (BackListNode *)node, elem);                         \
  }                                                                            \
  name##BackListNode *name##BackList_insert_after(                             \
      name##BackList *list, name##BackListNode *node, const T *elem) {         \
    return (name##BackListNode *)BackList_insert_after(                        \
        (BackList *)list, (BackListNode *)node, elem);                         \
  }                                                                            \
  name##BackListNode *name##BackList_append(name##BackList *list,              \
                                            const T *elem) {                   \
    return (name##BackListNode *)BackList_append((BackList *)list, elem);      \
  }                                                                            \
  name##BackListNode *name##BackList_prepend(name##BackList *list,             \
                                             const T *elem) {                  \
    return (name##BackListNode *)BackList_prepend((BackList *)list, elem);     \
  }                                                                            \
  void name##BackList_remove(name##BackList *list, name##BackListNode *node) { \
    BackList_remove((BackList *)list, (BackListNode *)node);                   \
  }                                                                            \
  void name##BackList_get_iterator(const name##BackList *list,                 \
                                   name##Iterator *iter) {                     \
    BackList_get_iterator((const BackList *)list, (Iterator *)iter);           \
  }                                                                            \
  void name##BackList_get_reverse_iterator(const name##BackList *list,         \
                                           name##Iterator *iter) {             \
    BackList_get_reverse_iterator((const BackList *)list, (Iterator *)iter);   \
  }                                                                            \
  void name##BackList_get_node_iterator(const name##BackList *list,            \
                                        name##Iterator *iter) {                \
    BackList_get_node_iterator((const BackList *)list, (Iterator *)iter);      \
  }                                                                            \
  void name##BackList_get_node_reverse_iterator(const name##BackList *list,    \
                                                name##Iterator *iter) {        \
    BackList_get_node_reverse_iterator((const BackList *)list,                 \
                                       (Iterator *)iter);                      \
  }                                                                            \
  void name##BackList_get_sink(const name##BackList *list, name##Sink *sink) { \
    BackList_get_sink((const BackList *)list, (Sink *)sink);                   \
  }                                                                            \
  void name##BackList_get_reverse_sink(const name##BackList *list,             \
                                       name##Sink *sink) {                     \
    BackList_get_reverse_sink((const BackList *)list, (Sink *)sink);           \
  }

DECLARE_BACK_LIST(Int, int)
DECLARE_BACK_LIST(Long, long)
DECLARE_BACK_LIST(Char, char)
DECLARE_BACK_LIST(Float, float)
DECLARE_BACK_LIST(Double, double)
DECLARE_BACK_LIST(UnsignedInt, unsigned int)
DECLARE_BACK_LIST(UnsignedLong, unsigned long)
DECLARE_BACK_LIST(UnsignedChar, unsigned char)
DECLARE_BACK_LIST(String, char *)

#endif // COMMON_PUBLIC_BACK_LIST_H__
