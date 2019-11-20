#ifndef COMMON_PUBLIC_LINKED_LIST_H__
#define COMMON_PUBLIC_LINKED_LIST_H__

#include <stdbool.h>
#include <stddef.h>

#include "iterator.h"

typedef struct LinkedList LinkedList;

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
typedef struct LinkedListNode LinkedListNode;

// Creates a new LinkedList object.
LinkedList *LinkedList_alloc(size_t elem_size);

// Gets the number of elements in the LinkedList
size_t LinkedList_count(const LinkedList *list);

// Returns whether the list is empty
bool LinkedList_empty(const LinkedList *list);

// Frees up the LinkedList object.
void LinkedList_free(LinkedList *list);

// Gets the size of an element in the LinkedList
size_t LinkedList_element_size(const LinkedList *list);

// Gets the size of a node in the LinkedList
size_t LinkedList_node_size(const LinkedList *list);

// Gets the first item
void *LinkedList_get_first(const LinkedList *list);

// Gets the last item
void *LinkedList_get_last(const LinkedList *list);

// Gets the first node
LinkedListNode *LinkedList_get_first_node(const LinkedList *list);

// Gets the last node
LinkedListNode *LinkedList_get_last_node(const LinkedList *list);

// Gets the next node
LinkedListNode *LinkedList_get_next_node(const LinkedListNode *node);

// Sets the data in the linked list node.
void LinkedList_set(const LinkedList *list, LinkedListNode *node, const void *data);

// Gets the data item in the linked list node.
const void *LinkedList_get(const LinkedListNode *node);

// Inserts the given element before the given node. Returns NULL if
// unsuccessful.
LinkedListNode *LinkedList_insert_before(LinkedList *list,
                                          LinkedListNode *node,
                                          const void *elem);

// Inserts the given element after the given node. Returns NULL if unsuccessful.
LinkedListNode *LinkedList_insert_after(LinkedList *list, LinkedListNode *node,
                                         const void *elem);

// Appends the given element to the list.
// Returns a pointer to the appended node. Returns NULL if unsuccessful.
LinkedListNode *LinkedList_append(LinkedList *list, const void *elem);

// Prepends the given element to the list.
// Returns a pointer to the appended node. Returns NULL if unsuccessful.
LinkedListNode *LinkedList_prepend(LinkedList *list, const void *elem);

// Removes a node from the list. If it's the last node, it's an O(N) operation.
void LinkedList_remove(LinkedList *list, LinkedListNode *node);

// Removes the first node from the list.
void LinkedList_remove_first(LinkedList *list);

// Removes the last node from the list. This is an O(N) operation.
// For this reason, an emulated stack should use _prepend() and _remove_first().
void LinkedList_remove_last(LinkedList *list);

// Removes all the items from the list.
// Remember to clean up memory first.
void LinkedList_clear(LinkedList *list);

// Removes a range of elements from the list starting with the given node.
void LinkedList_remove_range(LinkedList *list, LinkedListNode *node,
                              int count);

// Copies a list. Returns whether successful.
bool LinkedList_copy(LinkedList *dest_list, const LinkedList *list);

// Gets an Iterator for this LinkedList
void LinkedList_get_iterator(const LinkedList *list, Iterator *iter);

// Gets an Iterator for the nodes in this LinkedList
void LinkedList_get_node_iterator(const LinkedList *list, Iterator *iter);

// Gets a Sink for this list
void LinkedList_get_sink(const LinkedList *list, Sink *sink);

// Gets a reverse Sink for this list
void LinkedList_get_reverse_sink(const LinkedList *list, Sink *sink);

#define DECLARE_LINKED_LIST(name, T)                                           \
  typedef struct LinkedList name##LinkedList;                                  \
  typedef struct LinkedListNode name##LinkedListNode;                          \
  name##LinkedList *name##LinkedList_alloc();                                  \
  T name##LinkedList_get_first(const name##LinkedList *list);                  \
  T name##LinkedList_get_last(const name##LinkedList *list);                   \
  T *name##LinkedList_get_first_ref(const name##LinkedList *list);             \
  T *name##LinkedList_get_last_ref(const name##LinkedList *list);              \
  name##LinkedListNode *name##LinkedList_get_first_node(                       \
      const name##LinkedList *list);                                           \
  name##LinkedListNode *name##LinkedList_get_last_node(                        \
      const name##LinkedList *list);                                           \
  name##LinkedListNode *name##LinkedList_get_next_node(                        \
      const name##LinkedListNode *node);                                       \
  void name##LinkedList_set(const name##LinkedList *list,                      \
                            name##LinkedListNode *node, const T data);         \
  const T name##LinkedList_get(const name##LinkedListNode *node);              \
  void name##LinkedList_set_ref(const name##LinkedList *list,                  \
                                name##LinkedListNode *node, const T *data);    \
  const T *name##LinkedList_get_ref(const name##LinkedListNode *node);         \
  name##LinkedListNode *name##LinkedList_insert_before(                        \
      name##LinkedList *list, name##LinkedListNode *node, const T *elem);      \
  name##LinkedListNode *name##LinkedList_insert_after(                         \
      name##LinkedList *list, name##LinkedListNode *node, const T *elem);      \
  name##LinkedListNode *name##LinkedList_append(name##LinkedList *list,        \
                                                const T *elem);                \
  name##LinkedListNode *name##LinkedList_prepend(name##LinkedList *list,       \
                                                 const T *elem);               \
  void name##LinkedList_remove(name##LinkedList *list,                         \
                               name##LinkedListNode *node);                    \
  void name##LinkedList_get_iterator(const name##LinkedList *list,             \
                                     name##Iterator *iter);                    \
  void name##LinkedList_get_node_iterator(const name##LinkedList *list,        \
                                          name##Iterator *iter);               \
  void name##LinkedList_get_sink(const name##LinkedList *list,                 \
                                 name##Sink *sink);                            \
  void name##LinkedList_get_reverse_sink(const name##LinkedList *list,         \
                                         name##Sink *sink);

#define DEFINE_LINKED_LIST(name, T)                                            \
  name##LinkedList *name##LinkedList_alloc() {                                 \
    return (name##LinkedList *)LinkedList_alloc(sizeof(LinkedList));           \
  }                                                                            \
  T name##LinkedList_get_first(const name##LinkedList *list) {                 \
    return *name##LinkedList_get_first_ref(list);                              \
  }                                                                            \
  T name##LinkedList_get_last(const name##LinkedList *list) {                  \
    return *name##LinkedList_get_last_ref(list);                               \
  }                                                                            \
  T *name##LinkedList_get_first_ref(const name##LinkedList *list) {            \
    return (T *)LinkedList_get_first((const LinkedList *)list);                \
  }                                                                            \
  T *name##LinkedList_get_last_ref(const name##LinkedList *list) {             \
    return (T *)LinkedList_get_last((const LinkedList *)list);                 \
  }                                                                            \
  name##LinkedListNode *name##LinkedList_get_first_node(                       \
      const name##LinkedList *list) {                                          \
    return (name##LinkedListNode *)LinkedList_get_first_node(                  \
        (const LinkedList *)list);                                             \
  }                                                                            \
  name##LinkedListNode *name##LinkedList_get_last_node(                        \
      const name##LinkedList *list) {                                          \
    return (name##LinkedListNode *)LinkedList_get_last_node(                   \
        (const LinkedList *)list);                                             \
  }                                                                            \
  name##LinkedListNode *name##LinkedList_get_next_node(                        \
      const name##LinkedListNode *node) {                                      \
    return (name##LinkedListNode *)LinkedList_get_next_node(                   \
        (const LinkedListNode *)node);                                         \
  }                                                                            \
  void name##LinkedList_set(const name##LinkedList *list,                      \
                            name##LinkedListNode *node, const T data) {        \
    name##LinkedList_set_ref(list, node, &data);                               \
  }                                                                            \
  const T name##LinkedList_get(const name##LinkedListNode *node) {             \
    return *name##LinkedList_get_ref(node);                                    \
  }                                                                            \
  void name##LinkedList_set_ref(const name##LinkedList *list,                  \
                                name##LinkedListNode *node, const T *data) {   \
    LinkedList_set((const LinkedList *)list, (LinkedListNode *)node, data);    \
  }                                                                            \
  const T *name##LinkedList_get_ref(const name##LinkedListNode *node) {        \
    return (const T *)LinkedList_get((const LinkedListNode *)node);            \
  }                                                                            \
  name##LinkedListNode *name##LinkedList_insert_before(                        \
      name##LinkedList *list, name##LinkedListNode *node, const T *elem) {     \
    return (name##LinkedListNode *)LinkedList_insert_before(                   \
        (LinkedList *)list, (LinkedListNode *)node, elem);                     \
  }                                                                            \
  name##LinkedListNode *name##LinkedList_insert_after(                         \
      name##LinkedList *list, name##LinkedListNode *node, const T *elem) {     \
    return (name##LinkedListNode *)LinkedList_insert_after(                    \
        (LinkedList *)list, (LinkedListNode *)node, elem);                     \
  }                                                                            \
  name##LinkedListNode *name##LinkedList_append(name##LinkedList *list,        \
                                                const T *elem) {               \
    return (name##LinkedListNode *)LinkedList_append((LinkedList *)list,       \
                                                     elem);                    \
  }                                                                            \
  name##LinkedListNode *name##LinkedList_prepend(name##LinkedList *list,       \
                                                 const T *elem) {              \
    return (name##LinkedListNode *)LinkedList_prepend((LinkedList *)list,      \
                                                      elem);                   \
  }                                                                            \
  void name##LinkedList_remove(name##LinkedList *list,                         \
                               name##LinkedListNode *node) {                   \
    LinkedList_remove((LinkedList *)list, (LinkedListNode *)node);             \
  }                                                                            \
  void name##LinkedList_get_iterator(const name##LinkedList *list,             \
                                     name##Iterator *iter) {                   \
    LinkedList_get_iterator((const LinkedList *)list, (Iterator *)iter);       \
  }                                                                            \
  void name##LinkedList_get_node_iterator(const name##LinkedList *list,        \
                                          name##Iterator *iter) {              \
    LinkedList_get_node_iterator((const LinkedList *)list, (Iterator *)iter);  \
  }                                                                            \
  void name##LinkedList_get_sink(const name##LinkedList *list,                 \
                                 name##Sink *sink) {                           \
    LinkedList_get_sink((const LinkedList *)list, (Sink *)sink);               \
  }                                                                            \
  void name##LinkedList_get_reverse_sink(const name##LinkedList *list,         \
                                         name##Sink *sink) {                   \
    LinkedList_get_reverse_sink((const LinkedList *)list, (Sink *)sink);       \
  }

DECLARE_LINKED_LIST(Int, int)
DECLARE_LINKED_LIST(Long, long)
DECLARE_LINKED_LIST(Char, char)
DECLARE_LINKED_LIST(Float, float)
DECLARE_LINKED_LIST(Double, double)
DECLARE_LINKED_LIST(UnsignedInt, unsigned int)
DECLARE_LINKED_LIST(UnsignedLong, unsigned long)
DECLARE_LINKED_LIST(UnsignedChar, unsigned char)
DECLARE_LINKED_LIST(String, char *)

#endif // COMMON_PUBLIC_LINKED_LIST_H__
