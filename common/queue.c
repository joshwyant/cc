#include "protected/queue.h"

#include <string.h>

#include "../test/stubs.h"
#include "public/assert.h"

bool Queue_init(Queue *queue, size_t elem_size);

void Queue_cleanup(Queue *queue);

// TODO: Version

Queue *Queue_alloc(size_t elem_size)
{
    ASSERT(elem_size > 0);

    Queue *queue = NULL;
    if ((queue = malloc(sizeof(Queue))) == NULL || !Queue_init(queue, elem_size)) {
        return NULL;
    }
    return queue;
}

// Do not allocate; used by cleanup
bool Queue_init(Queue *queue, size_t elem_size)
{
    ASSERT(queue != NULL);
    ASSERT(elem_size > 0);

    queue->list = NULL;
    queue->begin = 0;
    queue->current = -1;
    queue->elem_size = elem_size;
    queue->count = 0;
    queue->capacity = 0;
    return true;
}

size_t Queue_count(const Queue *queue)
{
    ASSERT(queue != NULL);

    return queue->count;
}

bool Queue_empty(const Queue *queue)
{
    ASSERT(queue != NULL);

    return queue->count == 0;
}

size_t Queue_capacity(const Queue *queue)
{
    ASSERT(queue != NULL);

    return queue->list ? 0 : List_capacity(queue->list); // actual capacity
}

bool Queue_reserve(Queue *queue, size_t num_elems)
{
    ASSERT(queue != NULL);
    ASSERT(num_elems >= 0);
    if (num_elems <= queue->capacity) return true; // Nothing to do

    if (!queue->list && !(queue->list == List_alloc(queue->elem_size))) return false;  

    if (!List_expand(queue->list, num_elems)) return false;
    void *data = List_get_data(queue->list);
    if (queue->begin >= queue->current) {
        // Queue looks like this: [>>>>>---->>>]
        // Must copy in reverse in case (to_shift > added capacity).
        size_t to_shift = num_elems - queue->capacity;
        for (size_t i = (to_shift - 1); i >= 0; i--) {
            memcpy(queue->elem_size * (num_elems - queue->capacity + i) + data,
                   queue->elem_size * (queue->begin + i)                + data,
                   queue->elem_size);
        }
    }
    queue->capacity = num_elems;
    return true;
}

bool Queue_trim(Queue *queue)
{
    ASSERT(queue != NULL);

    if (!queue->list) return true;

    // Compress the queue
    void *data = List_get_data(queue->list);
    if (queue->begin > queue->current) {
        memcpy(data,
               queue->elem_size * queue->begin + data,
               queue->elem_size * (queue->capacity - queue->begin));
    } else {
        memcpy(data,
               queue->elem_size * queue->begin + data,
               queue->elem_size * queue->count);
    }
    List_truncate(queue->list, queue->capacity);
    return List_trim(queue->list);
}

void Queue_free(Queue *queue)
{
    ASSERT(queue != NULL);
    Queue_cleanup(queue);
    free(queue);
}

void Queue_cleanup(Queue *queue)
{
    ASSERT(queue != NULL);
    List_free(queue->list);
    Queue_init(queue, queue->elem_size);
}

size_t Queue_element_size(const Queue *queue)
{
    ASSERT(queue != NULL);
    return queue->elem_size;
}

void *Queue_get(const Queue *queue, size_t index)
{
    ASSERT(queue != NULL);
    ASSERT(index >= 0);
    ASSERT(index < queue->count);

    if (queue->count == 0) return NULL;

    return List_get_data(queue->list) + queue->elem_size * queue->begin;
}

void *Queue_enqueue(Queue *queue, const void *data)
{
    ASSERT(queue != NULL);
    ASSERT(data != NULL);

    if (!queue->list && !(queue->list == List_alloc(queue->elem_size))) return NULL;  

    if (queue->count == queue->capacity) {
        size_t new_capacity = queue->count == 0 ? 1 : 2 * queue->capacity;
        if (!Queue_reserve(queue, new_capacity)) {
            return NULL;
        }
    }
    queue->current += 1;
    queue->current %= queue->capacity;
    void *new_data = List_get(queue->list, queue->current);
    memcpy(new_data, data, queue->elem_size);
    return new_data;
}

bool Queue_dequeue(Queue *queue, void *data_out)
{
    ASSERT(queue != NULL);
    ASSERT(data_out != NULL);

    if (Queue_empty(queue)) return false;
    memcpy(data_out, List_get(queue->list, queue->begin), queue->elem_size);
    queue->begin += 1;
    queue->begin %= queue->capacity;
    return true;
}

bool Queue_peek(Queue *queue, void *data_out)
{
    ASSERT(queue != NULL);
    ASSERT(data_out != NULL);

    if (Queue_empty(queue)) return false;
    memcpy(data_out, List_get(queue->list, queue->begin), queue->elem_size);
    return true;
}

void Queue_clear(Queue *queue)
{
    Queue_cleanup(queue);
}

bool Queue_copy(Queue *dest_queue, const Queue *queue) {
  Queue copy = *queue;
  Queue_init(&copy, queue->elem_size);
  if (!((copy.list = List_alloc(queue->capacity)) &&
        (List_copy(copy.list, queue->list)))) {
    return false;
  }
  *dest_queue = *queue;
  return true;
}

void *Queue_iter_current_(const Iterator *iter);
bool Queue_iter_eof_(const Iterator *iter);
bool Queue_iter_move_next_(Iterator *iter);

void *Queue_iter_current_(const Iterator *iter) 
{
  ASSERT(iter != NULL);
  ASSERT(iter->collection_type == COLLECTION_QUEUE);
  Queue *queue = iter->collection;
  ASSERT(queue != NULL);
  ASSERT(iter->version == queue->version &&
         "Collection changed while iterating.");
  if (Queue_iter_eof_(iter)) {
    return NULL;
  }
  return Queue_get(queue, iter->impl_data1);
}

bool Queue_iter_eof_(const Iterator *iter) 
{
  ASSERT(iter != NULL);
  ASSERT(iter->collection_type == COLLECTION_QUEUE);
  Queue *queue = iter->collection;
  ASSERT(queue != NULL);
  ASSERT(iter->version == queue->version &&
         "Collection changed while iterating.");
  return Queue_empty(queue) || iter->impl_data1 >= (int)queue->count;
}

bool Queue_iter_move_next_(Iterator *iter) 
{
  ASSERT(iter != NULL);
  ASSERT(iter->collection_type == COLLECTION_QUEUE);
  Queue *queue = iter->collection;
  ASSERT(queue != NULL);
  ASSERT(iter->version == queue->version &&
         "Collection changed while iterating.");
  if (Queue_iter_eof_(iter)) {
    return false;
  }
  iter->impl_data1++;
  return !Queue_iter_eof_(iter);
}

// Gets an Iterator for this Queue
void Queue_get_iterator(const Queue *queue, Iterator *iter) 
{
  ASSERT(queue != NULL);
  ASSERT(iter != NULL);
  iter->collection_type = COLLECTION_QUEUE;
  iter->collection = (void *)queue;
  iter->elem_size = queue->elem_size;
  iter->current = Queue_iter_current_;
  iter->eof = Queue_iter_eof_;
  iter->move_next = Queue_iter_move_next_;
  iter->impl_data1 = -1; // Current index
  iter->impl_data2 = 0;
  iter->version = queue->version;
}

void *Queue_sink_add_(const Sink *sink, const void *elem);

void Queue_get_sink(const Queue *queue, Sink *sink) 
{
  ASSERT(queue != NULL);
  ASSERT(sink != NULL);
  sink->collection_type = COLLECTION_QUEUE;
  sink->collection = (void *)queue;
  sink->elem_size = queue->elem_size;
  sink->add = Queue_sink_add_;
}

void *Queue_sink_add_(const Sink *sink, const void *elem) 
{
  ASSERT(sink != NULL);
  Queue *queue = sink->collection;
  ASSERT(queue != NULL);
  return Queue_enqueue(queue, elem);
}

size_t Queue_indexer_size_(const Indexer *indexer);
void *Queue_indexer_get_(const Indexer *indexer, size_t index);
void Queue_indexer_set_(Indexer *indexer, size_t index, const void *data);

// Gets an Indexer for this queue
void Queue_get_indexer(const Queue *queue, Indexer *indexer) 
{
  ASSERT(queue != NULL);
  ASSERT(indexer != NULL);
  indexer->collection_type = COLLECTION_QUEUE;
  indexer->collection = (void *)queue;
  indexer->elem_size = queue->elem_size;
  indexer->size = Queue_indexer_size_;
  indexer->get = Queue_indexer_get_;
  indexer->set = Queue_indexer_set_;
}

size_t Queue_indexer_size_(const Indexer *indexer) 
{
  ASSERT(indexer != NULL);
  Queue *queue = indexer->collection;
  ASSERT(queue != NULL);
  return queue->count;
}

void *Queue_indexer_get_(const Indexer *indexer, size_t index) 
{
  ASSERT(indexer != NULL);
  Queue *queue = indexer->collection;
  return Queue_get(queue, index);
}

void Queue_indexer_set_(Indexer *indexer, size_t index, const void *data) 
{
    ASSERT(false && "Queue random access is not supported.");
}
