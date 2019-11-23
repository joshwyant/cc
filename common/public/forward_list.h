#ifndef COMMON_PUBLIC_FORWARD_LIST_H__
#define COMMON_PUBLIC_FORWARD_LIST_H__

#include <stdbool.h>
#include <stddef.h>

#include "iterator.h"

typedef struct ForwardList ForwardList;

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
typedef struct ForwardListNode ForwardListNode;

// Creates a new ForwardList object.
ForwardList *ForwardList_alloc(size_t elem_size);

// Gets the number of elements in the ForwardList
size_t ForwardList_count(const ForwardList *list);

// Returns whether the list is empty
bool ForwardList_empty(const ForwardList *list);

// Frees up the ForwardList object.
void ForwardList_free(ForwardList *list);

// Gets the size of an element in the ForwardList
size_t ForwardList_element_size(const ForwardList *list);

// Gets the size of a node in the ForwardList
size_t ForwardList_node_size(const ForwardList *list);

// Gets the first item
void *ForwardList_get_first(const ForwardList *list);

// Gets the last item
void *ForwardList_get_last(const ForwardList *list);

// Gets the first node
ForwardListNode *ForwardList_get_first_node(const ForwardList *list);

// Gets the last node
ForwardListNode *ForwardList_get_last_node(const ForwardList *list);

// Gets the next node
ForwardListNode *ForwardList_get_next_node(const ForwardListNode *node);

// Sets the data in the linked list node.
void ForwardList_set(const ForwardList *list, ForwardListNode *node, const void *data);

// Gets the data item in the linked list node.
const void *ForwardList_get(const ForwardListNode *node);

// Inserts the given element before the given node. Returns NULL if
// unsuccessful.
ForwardListNode *ForwardList_insert_before(ForwardList *list,
                                          ForwardListNode *node,
                                          const void *elem);

// Inserts the given element after the given node. Returns NULL if unsuccessful.
ForwardListNode *ForwardList_insert_after(ForwardList *list, ForwardListNode *node,
                                         const void *elem);

// Appends the given element to the list.
// Returns a pointer to the appended node. Returns NULL if unsuccessful.
ForwardListNode *ForwardList_append(ForwardList *list, const void *elem);

// Prepends the given element to the list.
// Returns a pointer to the appended node. Returns NULL if unsuccessful.
ForwardListNode *ForwardList_prepend(ForwardList *list, const void *elem);

// Removes a node from the list. If it's the last node, it's an O(N) operation.
void ForwardList_remove(ForwardList *list, ForwardListNode *node);

// Removes the first node from the list.
void ForwardList_remove_first(ForwardList *list);

// Removes the last node from the list. This is an O(N) operation.
// For this reason, an emulated stack should use _prepend() and _remove_first().
void ForwardList_remove_last(ForwardList *list);

// Removes all the items from the list.
// Remember to clean up memory first.
void ForwardList_clear(ForwardList *list);

// Removes a range of elements from the list starting with the given node.
void ForwardList_remove_range(ForwardList *list, ForwardListNode *node,
                              int count);

// Copies a list. Returns whether successful.
bool ForwardList_copy(ForwardList *dest_list, const ForwardList *list);

// Gets an Iterator for this ForwardList
void ForwardList_get_iterator(const ForwardList *list, Iterator *iter);

// Gets an Iterator for the nodes in this ForwardList
void ForwardList_get_node_iterator(const ForwardList *list, Iterator *iter);

// Gets a Sink for this list
void ForwardList_get_sink(const ForwardList *list, Sink *sink);

// Gets a reverse Sink for this list
void ForwardList_get_reverse_sink(const ForwardList *list, Sink *sink);

#define DECLARE_FORWARD_LIST(name, T)                                           \
  typedef struct ForwardList name##ForwardList;                                  \
  typedef struct ForwardListNode name##ForwardListNode;                          \
  name##ForwardList *name##ForwardList_alloc(void);                                  \
  T name##ForwardList_get_first(const name##ForwardList *list);                  \
  T name##ForwardList_get_last(const name##ForwardList *list);                   \
  T *name##ForwardList_get_first_ref(const name##ForwardList *list);             \
  T *name##ForwardList_get_last_ref(const name##ForwardList *list);              \
  name##ForwardListNode *name##ForwardList_get_first_node(                       \
      const name##ForwardList *list);                                           \
  name##ForwardListNode *name##ForwardList_get_last_node(                        \
      const name##ForwardList *list);                                           \
  name##ForwardListNode *name##ForwardList_get_next_node(                        \
      const name##ForwardListNode *node);                                       \
  void name##ForwardList_set(const name##ForwardList *list,                      \
                            name##ForwardListNode *node, const T data);         \
  const T name##ForwardList_get(const name##ForwardListNode *node);              \
  void name##ForwardList_set_ref(const name##ForwardList *list,                  \
                                name##ForwardListNode *node, const T *data);    \
  const T *name##ForwardList_get_ref(const name##ForwardListNode *node);         \
  name##ForwardListNode *name##ForwardList_insert_before(                        \
      name##ForwardList *list, name##ForwardListNode *node, const T *elem);      \
  name##ForwardListNode *name##ForwardList_insert_after(                         \
      name##ForwardList *list, name##ForwardListNode *node, const T *elem);      \
  name##ForwardListNode *name##ForwardList_append(name##ForwardList *list,        \
                                                const T *elem);                \
  name##ForwardListNode *name##ForwardList_prepend(name##ForwardList *list,       \
                                                 const T *elem);               \
  void name##ForwardList_remove(name##ForwardList *list,                         \
                               name##ForwardListNode *node);                    \
  void name##ForwardList_get_iterator(const name##ForwardList *list,             \
                                     name##Iterator *iter);                    \
  void name##ForwardList_get_node_iterator(const name##ForwardList *list,        \
                                          name##Iterator *iter);               \
  void name##ForwardList_get_sink(const name##ForwardList *list,                 \
                                 name##Sink *sink);                            \
  void name##ForwardList_get_reverse_sink(const name##ForwardList *list,         \
                                         name##Sink *sink);

#define DEFINE_FORWARD_LIST(name, T)                                            \
  name##ForwardList *name##ForwardList_alloc(void) {                                 \
    return (name##ForwardList *)ForwardList_alloc(sizeof(ForwardList));           \
  }                                                                            \
  T name##ForwardList_get_first(const name##ForwardList *list) {                 \
    return *name##ForwardList_get_first_ref(list);                              \
  }                                                                            \
  T name##ForwardList_get_last(const name##ForwardList *list) {                  \
    return *name##ForwardList_get_last_ref(list);                               \
  }                                                                            \
  T *name##ForwardList_get_first_ref(const name##ForwardList *list) {            \
    return (T *)ForwardList_get_first((const ForwardList *)list);                \
  }                                                                            \
  T *name##ForwardList_get_last_ref(const name##ForwardList *list) {             \
    return (T *)ForwardList_get_last((const ForwardList *)list);                 \
  }                                                                            \
  name##ForwardListNode *name##ForwardList_get_first_node(                       \
      const name##ForwardList *list) {                                          \
    return (name##ForwardListNode *)ForwardList_get_first_node(                  \
        (const ForwardList *)list);                                             \
  }                                                                            \
  name##ForwardListNode *name##ForwardList_get_last_node(                        \
      const name##ForwardList *list) {                                          \
    return (name##ForwardListNode *)ForwardList_get_last_node(                   \
        (const ForwardList *)list);                                             \
  }                                                                            \
  name##ForwardListNode *name##ForwardList_get_next_node(                        \
      const name##ForwardListNode *node) {                                      \
    return (name##ForwardListNode *)ForwardList_get_next_node(                   \
        (const ForwardListNode *)node);                                         \
  }                                                                            \
  void name##ForwardList_set(const name##ForwardList *list,                      \
                            name##ForwardListNode *node, const T data) {        \
    name##ForwardList_set_ref(list, node, &data);                               \
  }                                                                            \
  const T name##ForwardList_get(const name##ForwardListNode *node) {             \
    return *name##ForwardList_get_ref(node);                                    \
  }                                                                            \
  void name##ForwardList_set_ref(const name##ForwardList *list,                  \
                                name##ForwardListNode *node, const T *data) {   \
    ForwardList_set((const ForwardList *)list, (ForwardListNode *)node, data);    \
  }                                                                            \
  const T *name##ForwardList_get_ref(const name##ForwardListNode *node) {        \
    return (const T *)ForwardList_get((const ForwardListNode *)node);            \
  }                                                                            \
  name##ForwardListNode *name##ForwardList_insert_before(                        \
      name##ForwardList *list, name##ForwardListNode *node, const T *elem) {     \
    return (name##ForwardListNode *)ForwardList_insert_before(                   \
        (ForwardList *)list, (ForwardListNode *)node, elem);                     \
  }                                                                            \
  name##ForwardListNode *name##ForwardList_insert_after(                         \
      name##ForwardList *list, name##ForwardListNode *node, const T *elem) {     \
    return (name##ForwardListNode *)ForwardList_insert_after(                    \
        (ForwardList *)list, (ForwardListNode *)node, elem);                     \
  }                                                                            \
  name##ForwardListNode *name##ForwardList_append(name##ForwardList *list,        \
                                                const T *elem) {               \
    return (name##ForwardListNode *)ForwardList_append((ForwardList *)list,       \
                                                     elem);                    \
  }                                                                            \
  name##ForwardListNode *name##ForwardList_prepend(name##ForwardList *list,       \
                                                 const T *elem) {              \
    return (name##ForwardListNode *)ForwardList_prepend((ForwardList *)list,      \
                                                      elem);                   \
  }                                                                            \
  void name##ForwardList_remove(name##ForwardList *list,                         \
                               name##ForwardListNode *node) {                   \
    ForwardList_remove((ForwardList *)list, (ForwardListNode *)node);             \
  }                                                                            \
  void name##ForwardList_get_iterator(const name##ForwardList *list,             \
                                     name##Iterator *iter) {                   \
    ForwardList_get_iterator((const ForwardList *)list, (Iterator *)iter);       \
  }                                                                            \
  void name##ForwardList_get_node_iterator(const name##ForwardList *list,        \
                                          name##Iterator *iter) {              \
    ForwardList_get_node_iterator((const ForwardList *)list, (Iterator *)iter);  \
  }                                                                            \
  void name##ForwardList_get_sink(const name##ForwardList *list,                 \
                                 name##Sink *sink) {                           \
    ForwardList_get_sink((const ForwardList *)list, (Sink *)sink);               \
  }                                                                            \
  void name##ForwardList_get_reverse_sink(const name##ForwardList *list,         \
                                         name##Sink *sink) {                   \
    ForwardList_get_reverse_sink((const ForwardList *)list, (Sink *)sink);       \
  }

#endif // COMMON_PUBLIC_FORWARD_LIST_H__
