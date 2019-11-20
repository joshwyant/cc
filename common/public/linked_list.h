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

#endif // COMMON_PUBLIC_LINKED_LIST_H__
