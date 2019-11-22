#include "protected/forward_list.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "../test/stubs.h"
#include "public/assert.h"

// TODO: version

DEFINE_FORWARD_LIST(Int, int)
DEFINE_FORWARD_LIST(Long, long)
DEFINE_FORWARD_LIST(Char, char)
DEFINE_FORWARD_LIST(Float, float)
DEFINE_FORWARD_LIST(Double, double)
DEFINE_FORWARD_LIST(UnsignedInt, unsigned int)
DEFINE_FORWARD_LIST(UnsignedLong, unsigned long)
DEFINE_FORWARD_LIST(UnsignedChar, unsigned char)
DEFINE_FORWARD_LIST(String, char *)

bool ForwardList_init(ForwardList *list, size_t elem_size)
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

void ForwardList_cleanup(ForwardList *list)
{
    ForwardListNode *current = list->head;
    while (current) {
        ForwardListNode *next = current->next;
        free(current);
        current = next;
    }
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}

ForwardList *ForwardList_alloc(size_t elem_size)
{
    ASSERT(elem_size);
    ForwardList *list = NULL;
    if ((list = malloc(sizeof(list))) == NULL || !ForwardList_init(list, elem_size)) {
        return NULL;
    }
    return list;
}

size_t ForwardList_count(const ForwardList *list)
{
    ASSERT(list);
    return list->count;
}

bool ForwardList_empty(const ForwardList *list)
{
    ASSERT(list);
    return list->count == 0;
}

void ForwardList_free(ForwardList *list)
{
    ASSERT(list);
    ForwardList_cleanup(list);
    free(list);
}

size_t ForwardList_element_size(const ForwardList *list)
{
    ASSERT(list);
    return list->elem_size;
}

size_t ForwardList_node_size(const ForwardList *list)
{
    ASSERT(list);
    return list->elem_size + sizeof(ForwardListNode);
}

void *ForwardList_get_first(const ForwardList *list)
{
    ASSERT(list);
    return list->head->data;
}

void *ForwardList_get_last(const ForwardList *list)
{
    ASSERT(list);
    return list->tail->data;
}

ForwardListNode *ForwardList_get_first_node(const ForwardList *list)
{
    ASSERT(list);
    return list->head;
}

ForwardListNode *ForwardList_get_last_node(const ForwardList *list)
{
    ASSERT(list);
    return list->tail;
}

ForwardListNode *ForwardList_get_next_node(const ForwardListNode *node)
{
    ASSERT(node);
    return node->next;
}

void ForwardList_set(const ForwardList *list, ForwardListNode *node, const void *data)
{
    ASSERT(list);
    ASSERT(node);
    ASSERT(data);
    memcpy(node->data, data, list->elem_size);
}

const void *ForwardList_get(const ForwardListNode *node)
{
    ASSERT(node);
    return node->data;
}

ForwardListNode *ForwardList_insert_before(ForwardList *list,
                                          ForwardListNode *node,
                                          const void *elem)
{
    ASSERT(list);
    ASSERT(node);
    ASSERT(elem);
    size_t node_size = ForwardList_node_size(list);
    ForwardListNode *new_node = malloc(node_size);
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

ForwardListNode *ForwardList_insert_after(ForwardList *list, ForwardListNode *node,
                                         const void *elem)
{
    ASSERT(list);
    ASSERT(node);
    ASSERT(elem);
    size_t node_size = ForwardList_node_size(list);
    ForwardListNode *new_node = malloc(node_size);
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

ForwardListNode *ForwardList_append(ForwardList *list, const void *elem)
{
    ASSERT(list);
    ASSERT(elem);
    if (list->tail) {
        return ForwardList_insert_after(list, list->tail, elem);
    }
    // Singleton list case:
    size_t node_size = ForwardList_node_size(list);
    ForwardListNode *node = malloc(node_size);
    if (!node) return NULL;
    memcpy(node->data, elem, list->elem_size);
    list->head = node;
    list->tail = node;
    list->count++;
    return node;
}

ForwardListNode *ForwardList_prepend(ForwardList *list, const void *elem)
{
    ASSERT(list);
    ASSERT(elem);
    if (list->head) {
        return ForwardList_insert_before(list, list->head, elem);
    }
    // Singleton list case:
    size_t node_size = ForwardList_node_size(list);
    ForwardListNode *node = malloc(node_size);
    if (!node) return NULL;
    memcpy(node->data, elem, list->elem_size);
    list->head = node;
    list->tail = node;
    list->count++;
    return node;
}

void ForwardList_remove(ForwardList *list, ForwardListNode *node)
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
        memcpy(node, node->next, ForwardList_node_size(list));
        free(node->next);
    } else {
        ForwardListNode *new_tail = NULL;
        ForwardListNode *current = list->head;
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

void ForwardList_remove_first(ForwardList *list)
{
    ASSERT(list);
    if (list->head) {
        ForwardList_remove(list, list->head);
    }
}

void ForwardList_remove_last(ForwardList *list)
{
    ASSERT(list);
    if (list->tail) {
        ForwardList_remove(list, list->tail);
    }
}

void ForwardList_clear(ForwardList *list)
{
    ForwardList_cleanup(list);
}

void ForwardList_remove_range(ForwardList *list, ForwardListNode *node,
                              int count)
{
    ASSERT(list);
    ASSERT(node);
    ASSERT(count >= 0);

    while (count--) {
        ForwardListNode *next = node->next;
        ForwardList_remove(list, node); // Replaces current with next
        if (!next) break;
    }
    ASSERT(count == 0 && "Tried to remove too many nodes");
}

bool ForwardList_copy(ForwardList *dest_list, const ForwardList *list)
{
    ASSERT(list);
    ASSERT(dest_list);
    ASSERT(list->elem_size == dest_list->elem_size);

    for (ForwardListNode *current = list->head; current; current = current->next) {
        if (!ForwardList_append(dest_list, current->data)) {
            return false;
        }
    }
    return true;
}

void ForwardList_get_iterator(const ForwardList *list, Iterator *iter)
{
    // TODO
}

void ForwardList_get_node_iterator(const ForwardList *list, Iterator *iter)
{
    // TODO
}

void ForwardList_get_sink(const ForwardList *list, Sink *sink)
{
    // TODO
}

void ForwardList_get_reverse_sink(const ForwardList *list, Sink *sink)
{
    // TODO
}
