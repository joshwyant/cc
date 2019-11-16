#include "iterator.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "array.h"
#include "assert.h"
#include "back_list.h"
#include "list.h"
#include "linked_list.h"
#include "map.h"
#include "priority_queue.h"
#include "queue.h"
#include "set.h"
#include "stack.h"

void for_each(Iterator *iter, void (*action)(void *elem)) {
    ASSERT(iter != NULL);
    if (action) {
        while (iter->move_next(iter)) {
            (*action)(iter->current(iter));
        }
    }
}

void for_each_ext(Iterator *iter, void (*action)(void *elem, const Iterator *iter)) {
    ASSERT(iter != NULL);

    if (action) {
        while (iter->move_next(iter)) {
            (*action)(iter->current(iter), iter);
        }
    }
}

void sink_ensure_size_(const Sink *sink, size_t num_elems) {
    ASSERT(sink != NULL);
    ASSERT(num_elems >= 0);

    if (sink->collection_type == COLLECTION_LIST) {
        list_reserve((List*)sink->collection, num_elems);
    }
}

void sink_ensure_iter_size_(const Sink *dest, Iterator *source) {
    ASSERT(dest != NULL);
    ASSERT(source != NULL);

    size_t size = 0;
    switch (source->collection_type) {
        case COLLECTION_ARRAY:
            size = array_size((Array*)source->collection);
            break;
        case COLLECTION_LIST:
            size = list_size((List*)source->collection);
            break;
        case COLLECTION_LINKED_LIST:
            size = linked_list_size((LinkedList*)source->collection);
            break;
        case COLLECTION_BACK_LIST:
            size = back_list_size((BackList*)source->collection);
            break;
        case COLLECTION_STACK:
            size = stack_size((Stack*)source->collection);
            break;
        case COLLECTION_QUEUE:
            size = queue_size((Queue*)source->collection);
            break;
        case COLLECTION_PRIORITY_QUEUE:
            size = priority_queue_size((PriorityQueue*)source->collection);
            break;
        case COLLECTION_SET:
            size = set_size((Set*)source->collection);
            break;
        case COLLECTION_MAP:
            size = map_size((Map*)source->collection);
            break;
        default:
            return;
    }
    if (size) {
        sink_ensure_size_(dest, size);
    }
}

bool iter_copy(const Sink *dest, Iterator *iter) {
    ASSERT(dest != NULL);
    ASSERT(iter != NULL);

    sink_ensure_iter_size_(dest, iter);
    while (iter->move_next(iter)) {
        void *added = dest->add(dest, iter->current(iter));
        if (!added) {
            return false;
        }
    }
    return true;
}

void indexer_sort(const Indexer *indexer, int (*compare_fn)(const void *a, const void *b))
{
    ASSERT(indexer->collection_type & (COLLECTION_ARRAY | COLLECTION_LIST));
    void *data = NULL;
    switch (indexer->collection_type) {
        case COLLECTION_LIST:
            data = list_get_data((List*)indexer->collection);
            break;
        case COLLECTION_ARRAY:
            data = array_get_data((Array*)indexer->collection);
            break;
        default:
            return; // not supported
    }
    if (data) {
        qsort(data, indexer->size(indexer), indexer->elem_size, compare_fn);
    }
}

bool iter_sort(const Sink *dest, Iterator *iter, int (*compare_fn)(const void *a, const void *b))
{
    Indexer indexer;
    Sink interm_sink;
    List *temp_list;
    Iterator list_iter;
    const Sink *interm_dest = dest;
    if (dest->collection_type & (COLLECTION_ARRAY | COLLECTION_LIST)) {
        // Write directly to the collection instead of using a temporary list
        if (dest->collection_type == COLLECTION_LIST) {
            List *dest_list = dest->collection;
            list_clear(dest_list);
            list_get_indexer(dest_list, &indexer);
        } else {
            Array *dest_array = dest->collection;
            array_get_indexer(dest_array, &indexer);
        }
        interm_dest = dest;
    } else {
        // Use a temporary list
        temp_list = list_alloc(iter->elem_size);
        if (!temp_list) {
            return false;
        }
        list_get_indexer(temp_list, &indexer);
        list_get_iterator(temp_list, &list_iter);
        list_get_sink(temp_list, &interm_sink);
        interm_dest = &interm_sink;
    }
    // Copy the source and sort
    iter_copy(interm_dest, iter);
    indexer_sort(&indexer, compare_fn);
    // Copy and free any temporary list.
    if (temp_list) {
        iter_copy(dest, &list_iter);
        list_free(temp_list);
    }
    return true;
}

void iter_map(const Sink *dest, Iterator *iter, void (*map_fn)(void *dest, const void *elem))
{}

void iter_flat_map(const Sink *dest, Iterator *iter, void (*map_fn)(Sink *dest, const void *elem))
{}

void iter_filter(const Sink *dest, Iterator *iter, bool (*filter_fn)(const void *elem))
{}

int int_compare(const void *a, const void *b) {
    return *((int*)a) - *((int*)b);
}

int long_compare(const void *a, const void *b) {
    return *((long*)a) - *((long*)b);
}

int char_compare(const void *a, const void *b) {
    return *((char*)a) - *((char*)b);
}

int float_compare(const void *a, const void *b) {
    return *((float*)a) - *((float*)b);
}

int double_compare(const void *a, const void *b) {
    return *((double*)a) - *((double*)b);
}

int unsigned_int_compare(const void *a, const void *b) {
    return *((unsigned int*)a) - *((unsigned int*)b);
}

int unsigned_long_compare(const void *a, const void *b) {
    return *((unsigned long*)a) - *((unsigned long*)b);
}

int unsigned_char_compare(const void *a, const void *b) {
    return *((unsigned char*)a) - *((unsigned char*)b);
}

int string_compare(const void *a, const void *b) {
    return strcmp((char*)a, (char*)b);
}

int string_case_compare(const void *a, const void *b) {
    return strcasecmp((char*)a, (char*)b);
}

int int_hash(const void *key)
{
    return *((int*)key) * 7;
}

int long_hash(const void *key)
{
    return *((int*)key) * 7 + *((int*)(key+4)) * 13;
}

int char_hash(const void *key)
{
    return *((char*)key) * 7;
}

int float_hash(const void *key)
{
    return *((int*)key) * 7;
}

int double_hash(const void *key)
{
    return *((int*)key) * 7 + *((int*)(key+4)) * 13;
}

int unsigned_int_hash(const void *key)
{
    return *((int*)key) * 7;
}

int unsigned_long_hash(const void *key)
{
    return *((int*)key) * 7 + *((int*)(key+4)) * 13;
}

int unsigned_char_hash(const void *key)
{
    return *((unsigned char*)key) * 7;
}

int string_hash(const void *key)
{
    int hash = 13;
    for (char *c = (char*)key; *c; c++) {
        hash = hash * 7 + 17 * *c;
    }
    return hash;
}

int string_case_hash(const void *key)
{
    int hash = 13;
    for (char *c = (char*)key; *c; c++) {
        hash = hash * 7 + 17 * toupper(*c);
    }
    return hash;
}
