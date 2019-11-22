#include "protected/priority_queue.h"

#include <string.h>

#include "../test/stubs.h"
#include "public/assert.h"

bool PriorityQueue_init(PriorityQueue *queue, RelationalKeyInfo *key_info,
                                    size_t elem_size, PriorityQueueMode mode)
{
    ASSERT(queue != NULL);
    ASSERT(key_info != NULL);
    ASSERT(key_info->key_info != NULL);
    ASSERT(key_info->key_info->key_size > 0);
    ASSERT(elem_size > 0);
    ASSERT(mode == PQ_MODE_SMALLEST_FIRST || mode == PQ_MODE_LARGEST_FIRST);
    if (NULL == (Vector_alloc(elem_size + key_info->key_info->key_size))) {
        return false;
    }
    queue->key_info = key_info;
    queue->elem_size = elem_size;
    queue->count = 0;
    queue->mode = mode;
    return true;
}

void PriorityQueue_cleanup(PriorityQueue *queue)
{
    ASSERT(queue != NULL);
    Vector_free(queue->list);
    queue->list = NULL;
    queue->count = 0;
    queue->key_info = NULL;
    queue->elem_size = 0;
}

PriorityQueue *PriorityQueue_alloc(RelationalKeyInfo *key_info,
                                    size_t elem_size, PriorityQueueMode mode)
{
    ASSERT(key_info != NULL);
    ASSERT(key_info->key_info != NULL);
    ASSERT(key_info->key_info->key_size > 0);
    ASSERT(elem_size > 0);
    ASSERT(mode == PQ_MODE_SMALLEST_FIRST || mode == PQ_MODE_LARGEST_FIRST);
    PriorityQueue *queue = NULL;
    if (!(queue = malloc(sizeof(PriorityQueue)))) {
        return NULL;
    }
    PriorityQueue_init(queue, key_info, elem_size, mode);
    return queue;
}

size_t PriorityQueue_count(const PriorityQueue *queue)
{
    ASSERT(queue);
    return queue->count;
}

bool PriorityQueue_empty(const PriorityQueue *queue)
{
    ASSERT(queue);
    return queue->count == 0;
}

size_t PriorityQueue_capacity(const PriorityQueue *queue)
{
    ASSERT(queue);
    return queue->list ? Vector_capacity(queue->list) : 0;
}

bool PriorityQueue_reserve(PriorityQueue *queue, size_t num_elems) {
    ASSERT(queue);
    if (!queue->list &&
        NULL == (Vector_alloc(queue->elem_size +
                            queue->key_info->key_info->key_size))) {
        return false;
    }
    return Vector_reserve(queue->list, num_elems);
}

bool PriorityQueue_trim(PriorityQueue *queue)
{
    ASSERT(queue);
    if (!queue->list &&
        NULL == (Vector_alloc(queue->elem_size +
                            queue->key_info->key_info->key_size))) {
        return false;
    }
    Vector_truncate(queue->list, queue->count);
    return Vector_trim(queue->list);
}

void PriorityQueue_free(PriorityQueue *queue)
{
    ASSERT(queue);
    PriorityQueue_cleanup(queue);
    free(queue);
}

size_t PriorityQueue_element_size(const PriorityQueue *queue)
{
    ASSERT(queue);
    return queue->elem_size;
}

PriorityQueueMode PriorityQueue_mode(const PriorityQueue *queue)
{
    ASSERT(queue);
    return queue->mode;
}

RelationalKeyInfo *PriorityQueue_key_info(const PriorityQueue *queue)
{
    ASSERT(queue);
    return queue->key_info;
}

void trickle_down(PriorityQueue *queue, size_t index)
{
    ASSERT(queue);
    ASSERT(index >= 0);
    ASSERT(index < queue->count);
    int larger_child;
    int multiplier = queue->mode == PQ_MODE_LARGEST_FIRST ? 1 : -1;
    size_t data_size = Vector_element_size(queue->list);
    char top_data[data_size];
    memcpy(top_data, Vector_get(queue->list, 0), data_size);
    while (index < queue->count / 2)
    {
        size_t left_child = 2 * index + 1;
        size_t right_child = left_child + 1;

        if (right_child < queue->count) {
          if (queue->key_info->compare_fn(Vector_get(queue->list, left_child),
                                          Vector_get(queue->list, right_child)) *
                  multiplier < 0)
            larger_child = right_child;
        }
        else {
            larger_child = left_child;
        }
        void *larger_child_data = Vector_get(queue->list, larger_child);
        if (queue->key_info->compare_fn(top_data, larger_child_data) *
                multiplier >= 0)
          break;

        Vector_set(queue->list, index, larger_child_data);
        index = larger_child;
    }
    Vector_set(queue->list, index, top_data);
}

void trickle_up(PriorityQueue *queue, size_t index)
{
    ASSERT(queue);
    ASSERT(index >= 0);
    ASSERT(index < queue->count);
    size_t parent = (index - 1) / 2;
    int multiplier = queue->mode == PQ_MODE_LARGEST_FIRST ? 1 : -1;
    size_t data_size = Vector_element_size(queue->list);
    char bottom_data[data_size];
    memcpy(bottom_data, Vector_get(queue->list, index), data_size);

    while (index > 0 &&
           multiplier * queue->key_info->compare_fn(
                            Vector_get(queue->list, parent), bottom_data) <
               0) {
        Vector_set(queue->list, index, Vector_get(queue->list, parent));
        index = parent;
        parent = (parent - 1) / 2;
    }
    Vector_set(queue->list, index, bottom_data);
}

void *PriorityQueue_enqueue(PriorityQueue *queue, const void *key,
                             const void *data)
{
    ASSERT(queue);
    ASSERT(key);
    ASSERT(data);
    char buffer[Vector_element_size(queue->list)];
    memcpy(buffer, key, queue->key_info->key_info->key_size);
    memcpy(buffer + queue->key_info->key_info->key_size, data, queue->elem_size);
    if (queue->count >= Vector_count(queue->list)) {
        Vector_add(queue->list, buffer);
    } else {
        Vector_set(queue->list, queue->count, buffer);
    }

    trickle_up(queue, queue->count++);

    return NULL; // No way to get the data
}

bool PriorityQueue_dequeue(PriorityQueue *queue, void *key_out,
                           void *data_out)
{
    ASSERT(queue);
    ASSERT(key_out);
    ASSERT(data_out);
    if (!queue->count) return false;
    void *data = Vector_get(queue->list, 0);
    memcpy(key_out, data, queue->key_info->key_info->key_size);
    memcpy(data_out, data + queue->key_info->key_info->key_size, queue->elem_size);
    Vector_set(queue->list, 0, Vector_get(queue->list, --queue->count));
    trickle_down(queue, 0);
    return true;
}

bool PriorityQueue_peek(PriorityQueue *queue, void *key_out,
                         void *data_out)
{
    ASSERT(queue);
    ASSERT(key_out);
    ASSERT(data_out);
    if (!queue->count) return false;
    void *data = Vector_get_data(queue->list);
    memcpy(key_out, data, queue->key_info->key_info->key_size);
    memcpy(data_out, data + queue->key_info->key_info->key_size, queue->elem_size);
    return true;
}

void PriorityQueue_clear(PriorityQueue *queue)
{
    ASSERT(queue);
    if (queue->list) Vector_clear(queue->list);
    queue->count = 0;
}

bool PriorityQueue_copy(PriorityQueue *dest_queue, const PriorityQueue *queue)
{
    ASSERT(dest_queue);
    ASSERT(queue);
    ASSERT(dest_queue->elem_size == queue->elem_size);
    ASSERT(dest_queue->key_info->key_info->key_size == 
                queue->key_info->key_info->key_size);
    PriorityQueue new_queue = *queue;
    if (!Vector_copy(new_queue.list, queue->list)) return false;
    *dest_queue = new_queue;
    return true;
}

void *PriorityQueue_iter_current_(const Iterator *iter);
bool PriorityQueue_iter_eof_(const Iterator *iter);
bool PriorityQueue_iter_move_next_(Iterator *iter);

void *PriorityQueue_iter_current_(const Iterator *iter) 
{
  ASSERT(iter != NULL);
  ASSERT(iter->collection_type == COLLECTION_PRIORITY_QUEUE);
  PriorityQueue *queue = iter->collection;
  ASSERT(queue != NULL);
  ASSERT(iter->version == queue->version &&
         "Collection changed while iterating.");
  if (PriorityQueue_iter_eof_(iter)) {
    return NULL;
  }
  // Get the data after the key for the first item in the list.
  return Vector_get_data(queue->list) + queue->key_info->key_info->key_size;
}

bool PriorityQueue_iter_eof_(const Iterator *iter) 
{
  ASSERT(iter != NULL);
  ASSERT(iter->collection_type == COLLECTION_PRIORITY_QUEUE);
  PriorityQueue *queue = iter->collection;
  ASSERT(queue != NULL);
  ASSERT(iter->version == queue->version &&
         "Collection changed while iterating.");
  return PriorityQueue_empty(queue) || iter->impl_data1 >= (int)queue->count;
}

bool PriorityQueue_iter_move_next_(Iterator *iter) 
{
  ASSERT(iter != NULL);
  ASSERT(iter->collection_type == COLLECTION_PRIORITY_QUEUE);
  PriorityQueue *queue = iter->collection;
  ASSERT(queue != NULL);
  ASSERT(iter->version == queue->version &&
         "Collection changed while iterating.");
  if (PriorityQueue_iter_eof_(iter)) {
    return false;
  }
  char dummy_key[queue->key_info->key_info->key_size];
  char dummy_value[queue->elem_size];
  PriorityQueue_dequeue(queue, dummy_key, dummy_value);
  return PriorityQueue_iter_eof_(iter);
  // TODO: Test this logic.
}

// Gets an Iterator for this PriorityQueue
void PriorityQueue_get_iterator(const PriorityQueue *queue, Iterator *iter) 
{
  ASSERT(queue != NULL);
  ASSERT(iter != NULL);
  iter->collection_type = COLLECTION_PRIORITY_QUEUE;
  iter->collection = (void *)queue;
  iter->elem_size = queue->elem_size;
  iter->current = PriorityQueue_iter_current_;
  iter->eof = PriorityQueue_iter_eof_;
  iter->move_next = PriorityQueue_iter_move_next_;
  iter->impl_data1 = -1; // Current index
  iter->impl_data2 = 0;
  iter->version = queue->version;
}

void *PriorityQueue_sink_add_(Sink *sink, const void *elem);

void PriorityQueue_get_sink(const PriorityQueue *queue, Sink *sink) 
{
  ASSERT(queue != NULL);
  ASSERT(sink != NULL);
  sink->collection_type = COLLECTION_PRIORITY_QUEUE;
  sink->collection = (void *)queue;
  sink->elem_size = queue->elem_size;
  sink->add = PriorityQueue_sink_add_;
}

void *PriorityQueue_sink_add_(Sink *sink, const void *elem) 
{
  ASSERT(sink != NULL);
  PriorityQueue *queue = sink->collection;
  ASSERT(queue != NULL);
  KeyValuePair *kvp = (KeyValuePair *)elem;
  return PriorityQueue_enqueue(queue, kvp->key, kvp->value);
}
