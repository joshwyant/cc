#ifndef COMMON_PUBLIC_LIST_H__
#define COMMON_PUBLIC_LIST_H__

#include <stdbool.h>
#include <stddef.h>

#include "iterator.h"

typedef struct List List;

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
typedef struct ListNode ListNode;

// Creates a new List object.
List *List_alloc(size_t elem_size);

// Gets the number of elements in the List
size_t List_count(const List *list);

// Returns whether the list is empty
bool List_empty(const List *list);

// Frees up the List object.
void List_free(List *list);

// Gets the size of an element in the List
size_t List_element_size(const List *list);

// Gets the size of a node in the List
size_t List_node_size(const List *list);

// Gets the first item
void *List_get_first(const List *list);

// Gets the last item
void *List_get_last(const List *list);

// Gets the first node
ListNode *List_get_first_node(const List *list);

// Gets the last node
ListNode *List_get_last_node(const List *list);

// Gets the next node
ListNode *List_get_next_node(const ListNode *node);

// Gets the previous node
ListNode *List_get_previous_node(const ListNode *node);

// Sets the data in the linked list node.
void List_set(const List *list, ListNode *node, const void *data);

// Gets the data item in the linked list node.
const void *List_get(const ListNode *node);

// Inserts the given element before the given node. Returns NULL if
// unsuccessful.
ListNode *List_insert_before(List *list, ListNode *node,
                                      const void *elem);

// Inserts the given element after the given node. Returns NULL if unsuccessful.
ListNode *List_insert_after(List *list, ListNode *node,
                                     const void *elem);

// Appends the given element to the list.
// Returns a pointer to the appended node. Returns NULL if unsuccessful.
ListNode *List_append(List *list, const void *elem);

// Prepends the given element to the list.
// Returns a pointer to the appended node. Returns NULL if unsuccessful.
ListNode *List_prepend(List *list, const void *elem);

// Removes a node from the list.
void List_remove(List *list, ListNode *node);

// Removes the first node from the list.
void List_remove_first(List *list);

// Removes the last node from the list.
void List_remove_last(List *list);

// Removes all the items from the list.
// Remember to clean up memory first.
void List_clear(List *list);

// Removes a range of elements from the list starting with the given node.
void List_remove_range(List *list, ListNode *node, int count);

// Copies a list. Returns whether successful.
bool List_copy(List *dest_list, const List *list);

// Gets an Iterator for this List
void List_get_iterator(const List *list, Iterator *iter);

// Gets a reverse Iterator for this List
void List_get_reverse_iterator(const List *list, Iterator *iter);

// Gets an Iterator for the nodes in this List
void List_get_node_iterator(const List *list, Iterator *iter);

// Gets a reverse Iterator for the nodes in this List
void List_get_node_reverse_iterator(const List *list, Iterator *iter);

// Gets a Sink for this list
void List_get_sink(const List *list, Sink *sink);

// Gets a reverse Sink for this list
void List_get_reverse_sink(const List *list, Sink *sink);


#define DECLARE_LIST(name, T)                                             \
  typedef struct List name##List;                                      \
  typedef struct ListNode name##ListNode;                                      \
  name##List *name##List_alloc(void);                                      \
  T name##List_get_first(const name##List *list);                     \
  T name##List_get_last(const name##List *list);                      \
  T *name##List_get_first_ref(const name##List *list);                     \
  T *name##List_get_last_ref(const name##List *list);                      \
  name##ListNode *name##List_get_first_node(                           \
      const name##List *list);                                             \
  name##ListNode *name##List_get_last_node(                            \
      const name##List *list);                                             \
  name##ListNode *name##List_get_next_node(                            \
      const name##ListNode *node);                                         \
  name##ListNode *name##List_get_previous_node(                        \
      const name##ListNode *node);                                         \
  void name##List_set(const name##List *list,                          \
                          name##ListNode *node, const T data);             \
  const T name##List_get(const name##ListNode *node);                  \
  void name##List_set_ref(const name##List *list,                      \
                              name##ListNode *node, const T *data);        \
  const T *name##List_get_ref(const name##ListNode *node);             \
  name##ListNode *name##List_insert_before(                            \
      name##List *list, name##ListNode *node, const T *elem);          \
  name##ListNode *name##List_insert_after(                             \
      name##List *list, name##ListNode *node, const T *elem);          \
  name##ListNode *name##List_append(name##List *list,              \
                                            const T *elem);                    \
  name##ListNode *name##List_prepend(name##List *list,             \
                                             const T *elem);                   \
  void name##List_remove(name##List *list, name##ListNode *node);  \
  void name##List_get_iterator(const name##List *list,                 \
                                   name##Iterator *iter);                      \
  void name##List_get_reverse_iterator(const name##List *list,         \
                                           name##Iterator *iter);              \
  void name##List_get_node_iterator(const name##List *list,            \
                                        name##Iterator *iter);                 \
  void name##List_get_node_reverse_iterator(const name##List *list,    \
                                                name##Iterator *iter);         \
  void name##List_get_sink(const name##List *list, name##Sink *sink);  \
  void name##List_get_reverse_sink(const name##List *list,             \
                                       name##Sink *sink);

#define DEFINE_LIST(name, T)                                              \
  name##List *name##List_alloc(void) {                                     \
    return (name##List *)List_alloc(sizeof(List));                 \
  }                                                                            \
  T name##List_get_first(const name##List *list) {                     \
    return *name##List_get_first_ref(list);                                \
  }                                                                            \
  T name##List_get_last(const name##List *list) {                      \
    return *name##List_get_last_ref(list);                                 \
  }                                                                            \
  T *name##List_get_first_ref(const name##List *list) {                \
    return (T *)List_get_first((const List *)list);                    \
  }                                                                            \
  T *name##List_get_last_ref(const name##List *list) {                 \
    return (T *)List_get_last((const List *)list);                     \
  }                                                                            \
  name##ListNode *name##List_get_first_node(                           \
      const name##List *list) {                                            \
    return (name##ListNode *)List_get_first_node(                      \
        (const List *)list);                                               \
  }                                                                            \
  name##ListNode *name##List_get_last_node(                            \
      const name##List *list) {                                            \
    return (name##ListNode *)List_get_last_node(                       \
        (const List *)list);                                               \
  }                                                                            \
  name##ListNode *name##List_get_next_node(                            \
      const name##ListNode *node) {                                        \
    return (name##ListNode *)List_get_next_node(                       \
        (const ListNode *)node);                                           \
  }                                                                            \
  name##ListNode *name##List_get_previous_node(                        \
      const name##ListNode *node) {                                        \
    return (name##ListNode *)List_get_previous_node(                   \
        (const ListNode *)node);                                           \
  }                                                                            \
  void name##List_set(const name##List *list,                          \
                          name##ListNode *node, const T data) {            \
    name##List_set_ref(list, node, &data);                                 \
  }                                                                            \
  const T name##List_get(const name##ListNode *node) {                 \
    return *name##List_get_ref(node);                                      \
  }                                                                            \
  void name##List_set_ref(const name##List *list,                      \
                              name##ListNode *node, const T *data) {       \
    List_set((const List *)list, (ListNode *)node, data);          \
  }                                                                            \
  const T *name##List_get_ref(const name##ListNode *node) {            \
    return (const T *)List_get((const ListNode *)node);                \
  }                                                                            \
  name##ListNode *name##List_insert_before(                            \
      name##List *list, name##ListNode *node, const T *elem) {         \
    return (name##ListNode *)List_insert_before(                       \
        (List *)list, (ListNode *)node, elem);                         \
  }                                                                            \
  name##ListNode *name##List_insert_after(                             \
      name##List *list, name##ListNode *node, const T *elem) {         \
    return (name##ListNode *)List_insert_after(                        \
        (List *)list, (ListNode *)node, elem);                         \
  }                                                                            \
  name##ListNode *name##List_append(name##List *list,              \
                                            const T *elem) {                   \
    return (name##ListNode *)List_append((List *)list, elem);      \
  }                                                                            \
  name##ListNode *name##List_prepend(name##List *list,             \
                                             const T *elem) {                  \
    return (name##ListNode *)List_prepend((List *)list, elem);     \
  }                                                                            \
  void name##List_remove(name##List *list, name##ListNode *node) { \
    List_remove((List *)list, (ListNode *)node);                   \
  }                                                                            \
  void name##List_get_iterator(const name##List *list,                 \
                                   name##Iterator *iter) {                     \
    List_get_iterator((const List *)list, (Iterator *)iter);           \
  }                                                                            \
  void name##List_get_reverse_iterator(const name##List *list,         \
                                           name##Iterator *iter) {             \
    List_get_reverse_iterator((const List *)list, (Iterator *)iter);   \
  }                                                                            \
  void name##List_get_node_iterator(const name##List *list,            \
                                        name##Iterator *iter) {                \
    List_get_node_iterator((const List *)list, (Iterator *)iter);      \
  }                                                                            \
  void name##List_get_node_reverse_iterator(const name##List *list,    \
                                                name##Iterator *iter) {        \
    List_get_node_reverse_iterator((const List *)list,                 \
                                       (Iterator *)iter);                      \
  }                                                                            \
  void name##List_get_sink(const name##List *list, name##Sink *sink) { \
    List_get_sink((const List *)list, (Sink *)sink);                   \
  }                                                                            \
  void name##List_get_reverse_sink(const name##List *list,             \
                                       name##Sink *sink) {                     \
    List_get_reverse_sink((const List *)list, (Sink *)sink);           \
  }

#endif // COMMON_PUBLIC_LIST_H__
