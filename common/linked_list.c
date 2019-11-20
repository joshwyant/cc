#include "protected/linked_list.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "../test/stubs.h"
#include "public/assert.h"

// TODO: version

bool LinkedList_init(LinkedList *list, size_t elem_size)
{
    ASSERT(list);
    ASSERT(elem_size);
    list->elem_size = elem_size;
    list->count = 0;
    list->head = NULL;
    list->tail = NULL;
    list->version = 1;
    return true;
}

void LinkedList_cleanup(LinkedList *list)
{
    LinkedListNode *current = list->head;
    while (current) {
        LinkedListNode *next = current->next;
        free(current);
        current = next;
    }
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}

LinkedList *LinkedList_alloc(size_t elem_size)
{
    ASSERT(elem_size);
    LinkedList *list = NULL;
    if ((list = malloc(sizeof(list))) == NULL || !LinkedList_init(list, elem_size)) {
        return NULL;
    }
    return list;
}

size_t LinkedList_count(const LinkedList *list)
{
    ASSERT(list);
    return list->count;
}

bool LinkedList_empty(const LinkedList *list)
{
    ASSERT(list);
    return list->count == 0;
}

void LinkedList_free(LinkedList *list)
{
    ASSERT(list);
    LinkedList_cleanup(list);
    free(list);
}

size_t LinkedList_element_size(const LinkedList *list)
{
    ASSERT(list);
    return list->elem_size;
}

size_t LinkedList_node_size(const LinkedList *list)
{
    ASSERT(list);
    return list->elem_size + sizeof(LinkedListNode);
}

void *LinkedList_get_first(const LinkedList *list)
{
    ASSERT(list);
    return list->head->data;
}

void *LinkedList_get_last(const LinkedList *list)
{
    ASSERT(list);
    return list->tail->data;
}

LinkedListNode *LinkedList_get_first_node(const LinkedList *list)
{
    ASSERT(list);
    return list->head;
}

LinkedListNode *LinkedList_get_last_node(const LinkedList *list)
{
    ASSERT(list);
    return list->tail;
}

LinkedListNode *LinkedList_get_next_node(const LinkedListNode *node)
{
    ASSERT(node);
    return node->next;
}

void LinkedList_set(const LinkedList *list, LinkedListNode *node, const void *data)
{
    ASSERT(list);
    ASSERT(node);
    ASSERT(data);
    memcpy(node->data, data, list->elem_size);
}

const void *LinkedList_get(const LinkedListNode *node)
{
    ASSERT(node);
    return node->data;
}

LinkedListNode *LinkedList_insert_before(LinkedList *list,
                                          LinkedListNode *node,
                                          const void *elem)
{
    ASSERT(list);
    ASSERT(node);
    ASSERT(elem);
    size_t node_size = LinkedList_node_size(list);
    LinkedListNode *new_node = malloc(node_size);
    if (!new_node) return NULL;
    memcpy(new_node, node, node_size); // copy previous to new node
    memcpy(node->data, elem, list->elem_size);
    node->next = new_node;
    if (node == list->tail) {
        list->tail = new_node;
    }
    list->count++;
    return node; // The original we're replacing
}

LinkedListNode *LinkedList_insert_after(LinkedList *list, LinkedListNode *node,
                                         const void *elem)
{
    ASSERT(list);
    ASSERT(node);
    ASSERT(elem);
    size_t node_size = LinkedList_node_size(list);
    LinkedListNode *new_node = malloc(node_size);
    if (!new_node) return NULL;
    memcpy(new_node->data, elem, list->elem_size);
    new_node->next = node->next;
    node->next = new_node;
    if (node == list->tail) {
        list->tail = new_node;
    }
    list->count++;
    return new_node;
}

LinkedListNode *LinkedList_append(LinkedList *list, const void *elem)
{
    ASSERT(list);
    ASSERT(elem);
    if (list->tail) {
        return LinkedList_insert_after(list, list->tail, elem);
    }
    // Singleton list case:
    size_t node_size = LinkedList_node_size(list);
    LinkedListNode *node = malloc(node_size);
    if (!node) return NULL;
    memcpy(node->data, elem, list->elem_size);
    list->head = node;
    list->tail = node;
    list->count++;
    return node;
}

LinkedListNode *LinkedList_prepend(LinkedList *list, const void *elem)
{
    ASSERT(list);
    ASSERT(elem);
    if (list->head) {
        return LinkedList_insert_before(list, list->head, elem);
    }
    // Singleton list case:
    size_t node_size = LinkedList_node_size(list);
    LinkedListNode *node = malloc(node_size);
    if (!node) return NULL;
    memcpy(node->data, elem, list->elem_size);
    list->head = node;
    list->tail = node;
    list->count++;
    return node;
}

void LinkedList_remove(LinkedList *list, LinkedListNode *node)
{
    ASSERT(list);
    ASSERT(node);
    if (node == list->head) {
        list->head = node->next;
    }
    if (node->next) {
        if (node->next == list->tail) {
            list->tail = node;
        }
        memcpy(node, node->next, LinkedList_node_size(list));
        free(node->next);
    } else {
        LinkedListNode *new_tail = NULL;
        LinkedListNode *current = list->head;
        // This is unfortunately O(N) because we don't have a prev ptr.
        while (current) {
            if (current != node) {
                new_tail = current;
            }
            current = current->next;
        }
        list->tail = new_tail;
        free(node);
    }
    list->count--;
}

void LinkedList_remove_first(LinkedList *list)
{
    ASSERT(list);
    if (list->head) {
        LinkedList_remove(list, list->head);
    }
}

void LinkedList_remove_last(LinkedList *list)
{
    ASSERT(list);
    if (list->tail) {
        LinkedList_remove(list, list->tail);
    }
}

void LinkedList_clear(LinkedList *list)
{
    LinkedList_cleanup(list);
}

void LinkedList_remove_range(LinkedList *list, LinkedListNode *node,
                              int count)
{
    ASSERT(list);
    ASSERT(node);
    ASSERT(count >= 0);

    while (count--) {
        LinkedListNode *next = node->next;
        LinkedList_remove(list, node); // Replaces current with next
        if (!next) break;
    }
    ASSERT(count == 0 && "Tried to remove too many nodes");
}

bool LinkedList_copy(LinkedList *dest_list, const LinkedList *list)
{
    ASSERT(list);
    ASSERT(dest_list);
    ASSERT(list->elem_size == dest_list->elem_size);

    for (LinkedListNode *current = list->head; current; current = current->next) {
        if (!LinkedList_append(dest_list, current->data)) {
            return false;
        }
    }
    return true;
}

void LinkedList_get_iterator(const LinkedList *list, Iterator *iter)
{
    // TODO
}

void LinkedList_get_node_iterator(const LinkedList *list, Iterator *iter)
{
    // TODO
}

void LinkedList_get_sink(const LinkedList *list, Sink *sink)
{
    // TODO
}

void LinkedList_get_reverse_sink(const LinkedList *list, Sink *sink)
{
    // TODO
}
