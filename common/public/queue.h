#ifndef COMMON_PUBLIC_QUEUE_H__
#define COMMON_PUBLIC_QUEUE_H__

#include "iterator.h"

// A FIFO queue data structure.
typedef struct Queue Queue;

// Creates a new Queue object.
Queue *Queue_alloc(size_t elem_size);

// Gets the number of elements in the Queue
size_t Queue_count(const Queue *queue);

// Returns whether the queue is empty
bool Queue_empty(const Queue *queue);

// Gets the capacity of the Queue
size_t Queue_capacity(const Queue *queue);

// Reserves memory for a total of `num_elems' objects.
// Capacity always increases by a factor of 2.
// Returns whether the allocation was successful.
bool Queue_reserve(Queue *queue, size_t num_elems);

// Removes excess memory for the Queue object.
// Returns whether the allocation was successful.
bool Queue_trim(Queue *queue);

// Frees up the Queue object.
void Queue_free(Queue *queue);

// Gets the size of an element in the Queue
size_t Queue_element_size(const Queue *queue);

// Gets the item at `index'
void *Queue_get(const Queue *queue, size_t index);

// Adds the item to the queue and returns pointer to new data.
// Returns NULL if unsuccessful.
void *Queue_enqueue(Queue *queue, const void *data);

// Removes an item from the queue and stores it in the given location.
bool Queue_dequeue(Queue *queue, void *data_out);

// Peeks the oldest item in the queue and stores it in the given location.
bool Queue_peek(Queue *queue, void *data_out);

// Removes all the items from the queue.
// Remember to clean up memory first.
void Queue_clear(Queue *queue);

// Copies a queue. Returns whether successful.
bool Queue_copy(Queue *dest_queue, const Queue *queue);

// Gets an Iterator for this Queue. Maintains FIFO order.
void Queue_get_iterator(const Queue *queue, Iterator *iter);

// Gets a Sink for this queue. Maintains FIFO order.
void Queue_get_sink(const Queue *queue, Sink *sink);

// Gets an Indexer for this queue. Index 0 is the oldest item in the queue.
void Queue_get_indexer(const Queue *queue, Indexer *indexer);

#endif // COMMON_PUBLIC_QUEUE_H__
