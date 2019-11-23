#include "protected/list.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "../test/stubs.h"
#include "public/collections.h"
#include "public/assert.h"

// TODO: version

DEFINE_LIST(Int, int)
DEFINE_LIST(Long, long)
DEFINE_LIST(Char, char)
DEFINE_LIST(Float, float)
DEFINE_LIST(Double, double)
DEFINE_LIST(UnsignedInt, unsigned int)
DEFINE_LIST(UnsignedLong, unsigned long)
DEFINE_LIST(UnsignedChar, unsigned char)
DEFINE_LIST(String, char *)

bool List_init(List *list, size_t elem_size)
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

void List_cleanup(List *list)
{
    ListNode *current = list->head;
    while (current) {
        ListNode *next = current->next;
        free(current);
        current = next;
    }
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}

List *List_alloc(size_t elem_size)
{
    ASSERT(elem_size);
    List *list = NULL;
    if ((list = malloc(sizeof(list))) == NULL || !List_init(list, elem_size)) {
        return NULL;
    }
    return list;
}

size_t List_count(const List *list)
{
    ASSERT(list);
    return list->count;
}

bool List_empty(const List *list)
{
    ASSERT(list);
    return list->count == 0;
}

void List_free(List *list)
{
    ASSERT(list);
    List_cleanup(list);
    free(list);
}

size_t List_element_size(const List *list)
{
    ASSERT(list);
    return list->elem_size;
}

size_t List_node_size(const List *list)
{
    ASSERT(list);
    return list->elem_size + sizeof(ListNode);
}

void *List_get_first(const List *list)
{
    ASSERT(list);
    return list->head->data;
}

void *List_get_last(const List *list)
{
    ASSERT(list);
    return list->tail->data;
}

ListNode *List_get_first_node(const List *list)
{
    ASSERT(list);
    return list->head;
}

ListNode *List_get_last_node(const List *list)
{
    ASSERT(list);
    return list->tail;
}

ListNode *List_get_next_node(const ListNode *node)
{
    ASSERT(node);
    return node->next;
}

ListNode *List_get_previous_node(const ListNode *node)
{
    ASSERT(node);
    return node->prev;
}

void List_set(const List *list, ListNode *node, const void *data)
{
    ASSERT(list);
    ASSERT(node);
    ASSERT(data);
    memcpy(node->data, data, list->elem_size);
}

const void *List_get(const ListNode *node)
{
    ASSERT(node);
    return node->data;
}

ListNode *List_insert_before(List *list,
                                          ListNode *node,
                                          const void *elem)
{
    ASSERT(list);
    ASSERT(node);
    ASSERT(elem);
    size_t node_size = List_node_size(list);
    ListNode *new_node = malloc(node_size);
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

ListNode *List_insert_after(List *list, ListNode *node,
                                         const void *elem)
{
    ASSERT(list);
    ASSERT(node);
    ASSERT(elem);
    size_t node_size = List_node_size(list);
    ListNode *new_node = malloc(node_size);
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

ListNode *List_append(List *list, const void *elem)
{
    ASSERT(list);
    ASSERT(elem);
    if (list->tail) {
        return List_insert_after(list, list->tail, elem);
    }
    // Singleton list case:
    size_t node_size = List_node_size(list);
    ListNode *node = malloc(node_size);
    if (!node) return NULL;
    memcpy(node->data, elem, list->elem_size);
    list->head = node;
    list->tail = node;
    list->count++;
    return node;
}

ListNode *List_prepend(List *list, const void *elem)
{
    ASSERT(list);
    ASSERT(elem);
    if (list->head) {
        return List_insert_before(list, list->head, elem);
    }
    // Singleton list case:
    size_t node_size = List_node_size(list);
    ListNode *node = malloc(node_size);
    if (!node) return NULL;
    memcpy(node->data, elem, list->elem_size);
    list->head = node;
    list->tail = node;
    list->count++;
    return node;
}

void List_remove(List *list, ListNode *node)
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

void List_remove_first(List *list)
{
    ASSERT(list);
    if (list->head) {
        List_remove(list, list->head);
    }
}

void List_remove_last(List *list)
{
    ASSERT(list);
    if (list->tail) {
        List_remove(list, list->tail);
    }
}

void List_clear(List *list)
{
    List_cleanup(list);
}

void List_remove_range(List *list, ListNode *node,
                              int count)
{
    ASSERT(list);
    ASSERT(node);
    ASSERT(count >= 0);

    while (count--) {
        ListNode *next = node->next;
        List_remove(list, node); // Replaces current with next
        if (!next) break;
    }
    ASSERT(count == 0 && "Tried to remove too many nodes");
}

bool List_copy(List *dest_list, const List *list)
{
    ASSERT(list);
    ASSERT(dest_list);
    ASSERT(list->elem_size == dest_list->elem_size);

    for (ListNode *current = list->head; current; current = current->next) {
        if (!List_append(dest_list, current->data)) {
            return false;
        }
    }
    return true;
}

void List_get_iterator(const List *list, Iterator *iter)
{
    // TODO
}

void List_get_reverse_iterator(const List *list, Iterator *iter)
{
    // TODO
}

void List_get_node_iterator(const List *list, Iterator *iter)
{
    // TODO
}

void List_get_node_reverse_iterator(const List *list, Iterator *iter)
{
    // TODO
}

void List_get_sink(const List *list, Sink *sink)
{
    // TODO
}

void List_get_reverse_sink(const List *list, Sink *sink)
{
    // TODO
}
