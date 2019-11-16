#ifndef COMMON_BACK_LIST_H__
#define COMMON_BACK_LIST_H__

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
BackList *back_list_alloc(size_t elem_size);

// Initializes a pre-allocated BackList object
void back_list_init(BackList *list, size_t elem_size);

// Gets the number of elements in the BackList
size_t back_list_size(const BackList *list);

// Returns whether the list is empty
bool back_list_empty(const BackList *list);

// Frees up the BackList object.
void back_list_free(BackList *list);

// Cleans up the BackList object, but does not free it.
void back_list_cleanup(BackList *list);

// Gets the size of an element in the BackList
size_t back_list_element_size(const BackList *list);

// Gets the size of a node in the BackList
size_t back_list_node_size(const BackList *list);

// Gets the first item
void *back_list_get_first(const BackList *list);

// Gets the last item
void *back_list_get_last(const BackList *list);

// Gets the first node
BackListNode *back_list_get_first_node(const BackList *list);

// Gets the last node
BackListNode *back_list_get_last_node(const BackList *list);

// Gets the next node
BackListNode *back_list_get_next_node(const BackListNode *node);

// Gets the previous node
BackListNode *back_list_get_previous_node(const BackListNode *node);

// Sets the data in the linked list node.
void back_list_set(const BackListNode *node, const void *data);

// Gets the data item in the linked list node.
void *back_list_get(const BackListNode *node);

// Inserts the given element before the given node. Returns NULL if unsuccessful.
BackListNode *back_list_insert_before(BackList *list, BackListNode *node, const void *elem);

// Inserts the given element after the given node. Returns NULL if unsuccessful.
BackListNode *back_list_insert_after(BackList *list, BackListNode *node, const void *elem);

// Appends the given element to the list.
// Returns a pointer to the appended node. Returns NULL if unsuccessful.
BackListNode *back_list_append(BackList *list, const void *elem);

// Prepends the given element to the list.
// Returns a pointer to the appended node. Returns NULL if unsuccessful.
BackListNode *back_list_prepend(BackList *list, const void *elem);

// Removes a node from the list.
void back_list_remove(BackList *list, BackListNode *node);

// Removes the first node from the list.
void back_list_remove_first(BackList *list);

// Removes the last node from the list.
void back_list_remove_last(BackList *list);

// Removes all the items from the list.
// Remember to clean up memory first.
void back_list_clear(BackList *list);

// Removes a range of elements from the list starting with the given node.
void back_list_remove_range(BackList *list, BackListNode *node, int count);

// Copies a list. Returns whether successful.
bool back_list_copy(BackList *dest_list, const BackList *list);

// Gets an Iterator for this BackList
void back_list_get_iterator(const BackList *list, Iterator *iter);

// Gets a reverse Iterator for this BackList
void back_list_get_reverse_iterator(const BackList *list, Iterator *iter);

// Gets an Iterator for the nodes in this BackList
void back_list_get_node_iterator(const BackList *list, Iterator *iter);

// Gets a reverse Iterator for the nodes in this BackList
void back_list_get_node_reverse_iterator(const BackList *list, Iterator *iter);

// Gets a Sink for this list
void back_list_get_sink(const BackList *list, Sink *sink);

// Gets a reverse Sink for this list
void back_list_get_reverse_sink(const BackList *list, Sink *sink);

#endif // COMMON_BACK_LIST_H__
