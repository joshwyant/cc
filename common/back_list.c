#include "protected/back_list.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "../test/stubs.h"
#include "public/assert.h"

// TODO: version

bool BackList_init(BackList *list, size_t elem_size)
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

void BackList_cleanup(BackList *list)
{
    BackListNode *current = list->head;
    while (current) {
        BackListNode *next = current->next;
        free(current);
        current = next;
    }
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}

BackList *BackList_alloc(size_t elem_size)
{
    ASSERT(elem_size);
    BackList *list = NULL;
    if ((list = malloc(sizeof(list))) == NULL || !BackList_init(list, elem_size)) {
        return NULL;
    }
    return list;
}

size_t BackList_count(const BackList *list)
{
    ASSERT(list);
    return list->count;
}

bool BackList_empty(const BackList *list)
{
    ASSERT(list);
    return list->count == 0;
}

void BackList_free(BackList *list)
{
    ASSERT(list);
    BackList_cleanup(list);
    free(list);
}

size_t BackList_element_size(const BackList *list)
{
    ASSERT(list);
    return list->elem_size;
}

size_t BackList_node_size(const BackList *list)
{
    ASSERT(list);
    return list->elem_size + sizeof(BackListNode);
}

void *BackList_get_first(const BackList *list)
{
    ASSERT(list);
    return list->head->data;
}

void *BackList_get_last(const BackList *list)
{
    ASSERT(list);
    return list->tail->data;
}

BackListNode *BackList_get_first_node(const BackList *list)
{
    ASSERT(list);
    return list->head;
}

BackListNode *BackList_get_last_node(const BackList *list)
{
    ASSERT(list);
    return list->tail;
}

BackListNode *BackList_get_next_node(const BackListNode *node)
{
    ASSERT(node);
    return node->next;
}

BackListNode *BackList_get_previous_node(const BackListNode *node)
{
    ASSERT(node);
    return node->prev;
}

void BackList_set(const BackList *list, BackListNode *node, const void *data)
{
    ASSERT(list);
    ASSERT(node);
    ASSERT(data);
    memcpy(node->data, data, list->elem_size);
}

const void *BackList_get(const BackListNode *node)
{
    ASSERT(node);
    return node->data;
}

BackListNode *BackList_insert_before(BackList *list,
                                          BackListNode *node,
                                          const void *elem)
{
    ASSERT(list);
    ASSERT(node);
    ASSERT(elem);
    size_t node_size = BackList_node_size(list);
    BackListNode *new_node = malloc(node_size);
    if (!new_node) return NULL;
    memcpy(new_node->data, elem, list->elem_size);
    new_node->next = node;
    node->prev = new_node;
    if (node == list->head) {
        list->head = new_node;
    }
    list->count++;
    return node;
}

BackListNode *BackList_insert_after(BackList *list, BackListNode *node,
                                         const void *elem)
{
    ASSERT(list);
    ASSERT(node);
    ASSERT(elem);
    size_t node_size = BackList_node_size(list);
    BackListNode *new_node = malloc(node_size);
    if (!new_node) return NULL;
    memcpy(new_node->data, elem, list->elem_size);
    new_node->prev = node;
    node->next = new_node;
    if (node == list->tail) {
        list->tail = new_node;
    }
    list->count++;
    return new_node;
}

BackListNode *BackList_append(BackList *list, const void *elem)
{
    ASSERT(list);
    ASSERT(elem);
    if (list->tail) {
        return BackList_insert_after(list, list->tail, elem);
    }
    // Singleton list case:
    size_t node_size = BackList_node_size(list);
    BackListNode *node = malloc(node_size);
    if (!node) return NULL;
    memcpy(node->data, elem, list->elem_size);
    list->head = node;
    list->tail = node;
    list->count++;
    return node;
}

BackListNode *BackList_prepend(BackList *list, const void *elem)
{
    ASSERT(list);
    ASSERT(elem);
    if (list->head) {
        return BackList_insert_before(list, list->head, elem);
    }
    // Singleton list case:
    size_t node_size = BackList_node_size(list);
    BackListNode *node = malloc(node_size);
    if (!node) return NULL;
    memcpy(node->data, elem, list->elem_size);
    list->head = node;
    list->tail = node;
    list->count++;
    return node;
}

void BackList_remove(BackList *list, BackListNode *node)
{
    ASSERT(list);
    ASSERT(node);
    if (node == list->head) {
        list->head = node->next;
    }
    if (node == list->tail) {
        list->tail = node->prev;
    }
    if (node->prev) {
        node->prev->next = node->next;
    }
    if (node->next) {
        node->next->prev = node->prev;
    }
    free(node);
    list->count--;
}

void BackList_remove_first(BackList *list)
{
    ASSERT(list);
    if (list->head) {
        BackList_remove(list, list->head);
    }
}

void BackList_remove_last(BackList *list)
{
    ASSERT(list);
    if (list->tail) {
        BackList_remove(list, list->tail);
    }
}

void BackList_clear(BackList *list)
{
    BackList_cleanup(list);
}

void BackList_remove_range(BackList *list, BackListNode *node,
                              int count)
{
    ASSERT(list);
    ASSERT(node);
    ASSERT(count >= 0);

    while (count--) {
        BackListNode *next = node->next;
        BackList_remove(list, node); // Replaces current with next
        if (!next) break;
    }
    ASSERT(count == 0 && "Tried to remove too many nodes");
}

bool BackList_copy(BackList *dest_list, const BackList *list)
{
    ASSERT(list);
    ASSERT(dest_list);
    ASSERT(list->elem_size == dest_list->elem_size);

    for (BackListNode *current = list->head; current; current = current->next) {
        if (!BackList_append(dest_list, current->data)) {
            return false;
        }
    }
    return true;
}

void BackList_get_iterator(const BackList *list, Iterator *iter)
{
    // TODO
}

void BackList_get_reverse_iterator(const BackList *list, Iterator *iter)
{
    // TODO
}

void BackList_get_node_iterator(const BackList *list, Iterator *iter)
{
    // TODO
}

void BackList_get_node_reverse_iterator(const BackList *list, Iterator *iter)
{
    // TODO
}

void BackList_get_sink(const BackList *list, Sink *sink)
{
    // TODO
}

void BackList_get_reverse_sink(const BackList *list, Sink *sink)
{
    // TODO
}
