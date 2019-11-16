#include "list.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "../test/stubs.h"

struct List {
    void *data;

    // sizeof(data)
    size_t data_size;

    // sizeof(element)
    size_t elem_size;
    
    // The actual number of elements.
    size_t elem_count;

    // The ideal capacity -- must be a power of 2.
    size_t reserve_count;

    // Detect changes to the list while iterating
    int version;
};

List *list_alloc(size_t elem_size)
{
    ASSERT(elem_size > 0);
    
    List *list = malloc(sizeof(List));
    list_init(list, elem_size);
    return list;
} // list_alloc

void list_init(List *list, size_t elem_size) {
    ASSERT(elem_size > 0);

    list->data = NULL;
    list->data_size = 0;
    list->elem_size = elem_size;
    list->elem_count = 0;
    list->reserve_count = 0;
    list->version = 0;
} // list_init

size_t list_size(const List *list)
{
    ASSERT(list != NULL);

    return list->elem_count;
} // list_size

bool list_empty(const List *list)
{
    ASSERT(list != NULL);
    
    return list->elem_count == 0;
} // list_size

size_t list_capacity(const List *list)
{
    ASSERT(list != NULL);

    return list->data_size / list->elem_size;
} // list_capacity

bool list_reserve_ext_(List *list, size_t num_elems, bool do_copy)
{
    ASSERT(list != NULL);
    ASSERT(num_elems >= 0);

    if (list->reserve_count >= num_elems) {
        return true; // No-op case
    }
    // Increase capacity by factor of 2
    size_t reserve_count =
        list->reserve_count == 0 ? 1 : list->reserve_count;
    while (reserve_count < num_elems) {
        reserve_count <<= 1;
    }
    void *data = malloc(list->elem_size * reserve_count);
    if (!data) {
        return false;
    }
    // Move the old data.
    if (list->elem_count > 0) {
        if (do_copy) {
            memcpy(data, list->data, list->elem_size * list->elem_count);
        } // If list is not copied, data is assumed to be overwritten (e.g. copied from another list)
        free(list->data);
    }
    list->data = data;
    list->reserve_count = reserve_count;
    list->data_size = reserve_count * list->elem_size;
    return true;
} // list_reserve_ext

bool list_reserve(List *list, size_t num_elems)
{
    return list_reserve_ext_(list, num_elems, true);
} // list_reserve

bool list_trim(List *list)
{
    ASSERT(list != NULL);

    if (list->elem_count == 0) {
        if (list->data) {
            free(list->data);
            list->data = NULL;
        }
        list->data_size = 0;
        list->reserve_count = 0;
        return true;
    }
    if (list->elem_count < list->reserve_count) {
        // Allocate and copy the data
        void *data = malloc(list->elem_size * list->elem_count);
        if (!data) {
            return false;
        }
        list->data_size = list->elem_size * list->elem_count;
        memcpy(data, list->data, list->data_size);
        free(list->data);
        list->data = data;
        // Lower the potential capacity.
        while ((list->reserve_count >> 1) >= list->elem_count) {
            list->reserve_count >>= 1;
        }
    }
    return true;
} // list_trim

void list_free(List *list)
{
    ASSERT(list != NULL);
    
    list_cleanup(list);
    free(list);
} // list_free

void list_cleanup(List *list)
{
    ASSERT(list != NULL);

    if (list->data) {
        free(list->data);
    }
    list->data = NULL;
    list->data_size = 0;
    list->elem_count = 0;
    list->elem_size = 0;
    list->reserve_count = 0;
    list->version++;
} // list_cleanup

size_t list_element_size(const List *list)
{
    ASSERT(list != NULL);

    return list->elem_size;
} // list_element_size

// Gets the data for the list.
void *list_get_data(const List *list)
{
    ASSERT(list != NULL);

    return list->data;
} // list_get_data

// Gets the item at `index'
void *list_get(const List *list, size_t index)
{
    ASSERT(list != NULL);
    ASSERT(index >= 0);
    ASSERT(index < list->elem_count && "index is greater than list size.");

    return list->data + list->elem_size * index;
} // list_get

// Sets the item at `index' to the given data.
// `index' must be within the bounds of the list.
void list_set(List *list, size_t index, const void *elem)
{
    list_set_range(list, index, elem, 1);
} // list_set

// Sets the data at `index' to the given data.
// `index' and data must be within the bounds of the list.
void list_set_range(List *list, size_t index, const void *elems, int count)
{
    ASSERT(list != NULL);
    ASSERT(index >= 0);
    ASSERT(index < list->elem_count && "index is >= list size.");
    ASSERT(index + count <= list->elem_count && "index + count is greater than list size.");
    ASSERT(elems != NULL);
    ASSERT(count >= 0);

    memcpy( list->data + list->elem_size * index,
            elems,
            list->elem_size * count);

    list->version++;
} // list_set_range

void *list_insert(List *list, size_t index, const void *elem)
{
    return list_insert_range(list, index, elem, 1);
} // list_insert

void *list_insert_range(List *list, size_t index, const void *elems, size_t count)
{
    ASSERT(list != NULL);
    ASSERT(index >= 0);
    ASSERT(elems != NULL);
    ASSERT(count >= 0);
    ASSERT(index <= list->elem_count);

    list->version++;
    if (count == 0) {
        return NULL; // No-op case
    }
    if (!list_reserve(list, list->elem_count + count)) {
        return NULL;
    }
    size_t shifted_count = list->elem_count - index;
    size_t shifted_size = list->elem_size * shifted_count;
    size_t data_size = list->elem_size * count;
    void *insertion_dest = list->data + list->elem_size * index;
    // Shift the data to make room if not appending
    if (index != list->elem_count) {
        memcpy(insertion_dest + data_size, insertion_dest, shifted_size);
    }
    if (elems != insertion_dest) { // a trick to shift the data without initializing
        // Copy the elements to the list
        memcpy(insertion_dest, elems, data_size);
    }
    list->elem_count += count;
    return insertion_dest;
} // list_insert_range

// Appends the given element to the end of the list.
// Returns a pointer to the appended element. Returns NULL if unsuccessful.
void *list_append(List *list, const void *elem)
{
    ASSERT(list != NULL);

    return list_insert_range(list, list->elem_count, elem, 1);
} // list_append

// Appends the given element to the end of the list.
// Returns a pointer to the appended element. Returns NULL if unsuccessful.
void *list_append_range(List *list, const void *elems, size_t count)
{
    ASSERT(list != NULL);

    return list_insert_range(list, list->elem_count, elems, count);
} // list_append_range

// Removes an element from the list.
void list_remove(List *list, size_t start_index)
{
    ASSERT(list != NULL);

    list_remove_range(list, start_index, 1); // Additional assertions done here
} // list_remove

void list_clear(List *list) {
    ASSERT(list != NULL);

    list_remove_range(list, 0, list->elem_count);
}

// Removes a range of elements from the list.
void list_remove_range(List *list, size_t start_index, size_t count)
{
    ASSERT(list != NULL);
    ASSERT(start_index >= 0);
    ASSERT(start_index <= list->elem_count);
    ASSERT(count >= 0);
    ASSERT(start_index + count <= list->elem_count);

    if (count == list->elem_count) {
        list_cleanup(list); // clean slate; everything short of freeing the list.
        return;
    }
    list->version++;
    void *new_data = list->data;
    // Can we reduce the size of the list by at least half?
    if (list->reserve_count >> 1 >= list->elem_count - count) {
        do {
            list->reserve_count >>= 1;
        } while (list->reserve_count >> 1 > list->elem_count - count);
        new_data = malloc(list->reserve_count * list->elem_size);
        if (start_index > 0) {
            // Copy the first part of the old data.
            memcpy(new_data, list->data, start_index * list->elem_size);
        }
    }
    if (count > 0 && start_index + count < list->elem_count) {
        // Shift the end of the list down
        memcpy(
            list->elem_size * start_index + new_data,
            list->elem_size * (start_index + count) + list->data,
            list->elem_size * count);
    }
    if (new_data != list->data) {
        free(list->data);
    }
    list->elem_count -= count;
    list->data = new_data;
}

// Copies a list. Returns whether successful.
bool list_copy(List *dest_list, const List *list)
{
    ASSERT(dest_list != NULL);
    ASSERT(list != NULL);
    ASSERT(list->elem_size == dest_list->elem_size);
    // Is there enough space?
    if (dest_list->reserve_count != list->reserve_count ||
        dest_list->data_size / dest_list->elem_size < list->elem_count) {
        list_cleanup(dest_list);
        if (!list_reserve(dest_list, list->elem_count)) {
            return false;
        }
    }
    memcpy(dest_list->data, list->data, list->elem_size * list->elem_count);
    dest_list->elem_count = list->elem_count;
    return true;
}

void *list_iter_current_(const Iterator *iter);
bool list_iter_eof_(const Iterator *iter);
bool list_iter_move_next_(Iterator *iter);

void *list_iter_current_(const Iterator *iter) {
    ASSERT(iter != NULL);
    ASSERT(iter->collection_type == COLLECTION_LIST);
    List *list = iter->collection;
    ASSERT(list != NULL);
    ASSERT(iter->version == list->version && "Collection changed while iterating.");
    if (list_iter_eof_(iter)) {
        return NULL;
    }
    return list_get(list, iter->impl_data1);
}

bool list_iter_eof_(const Iterator *iter) {
    ASSERT(iter != NULL);
    ASSERT(iter->collection_type == COLLECTION_LIST);
    List *list = iter->collection;
    ASSERT(list != NULL);
    ASSERT(iter->version == list->version && "Collection changed while iterating.");
    return iter->impl_data1 < 0 || iter->impl_data1 >= list->elem_count;
}

bool list_iter_move_next_(Iterator *iter) {
    ASSERT(iter != NULL);
    ASSERT(iter->collection_type == COLLECTION_LIST);
    List *list = iter->collection;
    ASSERT(list != NULL);
    ASSERT(iter->version == list->version && "Collection changed while iterating.");
    if (list_iter_eof_(iter)) {
        return false;
    }
    iter->impl_data1++;
    return true;
}

bool list_iter_move_next_reverse_(Iterator *iter) {
    ASSERT(iter != NULL);
    ASSERT(iter->collection_type == COLLECTION_LIST);
    List *list = iter->collection;
    ASSERT(list != NULL);
    ASSERT(iter->version == list->version && "Collection changed while iterating.");
    if (list_iter_eof_(iter)) {
        return false;
    }
    iter->impl_data1--;
    return true;
}

// Gets an Iterator for this List
void list_get_iterator(const List *list, Iterator *iter)
{
    ASSERT(list != NULL);
    ASSERT(iter != NULL);
    iter->collection_type = COLLECTION_LIST;
    iter->collection = (void*)list;
    iter->elem_size = list->elem_size;
    iter->current = list_iter_current_;
    iter->eof = list_iter_eof_;
    iter->move_next = list_iter_move_next_;
    iter->impl_data1 = -1; // Current index
    iter->impl_data2 = 0;
    iter->version = list->version;
}

void list_get_reverse_iterator(const List *list, Iterator *iter)
{
    ASSERT(list != NULL);
    ASSERT(iter != NULL);
    iter->collection_type = COLLECTION_LIST;
    iter->collection = (void*)list;
    iter->elem_size = list->elem_size;
    iter->current = list_iter_current_;
    iter->eof = list_iter_eof_;
    iter->move_next = list_iter_move_next_reverse_;
    iter->impl_data1 = list->elem_count; // Current index
    iter->impl_data2 = 0;
    iter->version = list->version;
}

void *list_sink_add_(const Sink *sink, const void *elem);

void list_get_sink(const List *list, Sink *sink)
{
    ASSERT(list != NULL);
    ASSERT(sink != NULL);
    sink->collection_type = COLLECTION_LIST;
    sink->collection = (void*)list;
    sink->elem_size = list->elem_size;
    sink->add = list_sink_add_;
}

void *list_sink_add_(const Sink *sink, const void *elem) {
    ASSERT(sink != NULL);
    List *list = sink->collection;
    ASSERT(list != NULL);
    return list_append(list, elem);
}

size_t list_indexer_size_(const Indexer *indexer);
void *list_indexer_get_(const Indexer *indexer, size_t index);

// Gets an Indexer for this list
void list_get_indexer(const List *list, Indexer *indexer)
{
    ASSERT(list != NULL);
    ASSERT(indexer != NULL);
    indexer->collection_type = COLLECTION_LIST;
    indexer->collection = (void*)list;
    indexer->elem_size = list->elem_size;
    indexer->size = list_indexer_size_;
    indexer->get = list_indexer_get_;
}

size_t list_indexer_size_(const Indexer *indexer) {
    ASSERT(indexer != NULL);
    List *list = indexer->collection;
    ASSERT(list != NULL);
    return list->elem_count;
}

void *list_indexer_get_(const Indexer *indexer, size_t index) {
    ASSERT(indexer != NULL);
    List *list = indexer->collection;
    ASSERT(list != NULL);
    return list->data + list->elem_size * index;
}
