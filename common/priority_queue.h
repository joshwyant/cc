#ifndef COMMON_PRIORITY_QUEUE_H__
#define COMMON_PRIORITY_QUEUE_H__

#include <stdbool.h>
#include <stddef.h>

#include "iterator.h"

// A priority queue data structure.
typedef struct PriorityQueue PriorityQueue;

typedef enum {
    PQ_MODE_INVALID,
    PQ_MODE_SMALLEST_FIRST,
    PQ_MODE_LARGEST_FIRST,
} PriorityQueueMode;

// Creates a new PriorityQueue object.
PriorityQueue *priority_queue_alloc(RelationalKeyInfo *key_info, size_t elem_size, PriorityQueueMode mode);

// Initializes a pre-allocated PriorityQueue object
bool priority_queue_init(PriorityQueue *queue, RelationalKeyInfo *key_info, size_t elem_size, PriorityQueueMode mode);

// Gets the number of elements in the PriorityQueue
size_t priority_queue_size(const PriorityQueue *queue);

// Returns whether the priority queue is empty
bool priority_queue_empty(const PriorityQueue *queue);

// Gets the capacity of the PriorityQueue
size_t priority_queue_capacity(const PriorityQueue *queue);

// Reserves memory for a total of `num_elems' objects.
// Capacity always increases by a factor of 2.
// Returns whether the allocation was successful.
bool priority_queue_reserve(PriorityQueue *queue, size_t num_elems);

// Removes excess memory for the PriorityQueue object.
// Returns whether the allocation was successful.
bool priority_queue_trim(PriorityQueue *queue);

// Frees up the PriorityQueue object.
void priority_queue_free(PriorityQueue *queue);

// Cleans up the PriorityQueue object, but does not free it.
void priority_queue_cleanup(PriorityQueue *queue);

// Gets the size of an element in the PriorityQueue
size_t priority_queue_element_size(const PriorityQueue *queue);

// Gets the priority queue mode
PriorityQueueMode priority_queue_mode(const PriorityQueue *queue);

// Gets the key info for the PriorityQueue
RelationalKeyInfo *priority_queue_key_info(const PriorityQueue *queue);

// Adds the item to the queue and returns pointer to new data.
// Returns NULL if unsuccessful.
void *priority_queue_enqueue(PriorityQueue *queue, const void *key, const void *data);

// Removes an item from the queue and stores it in the given location.
void priority_queue_dequeue(PriorityQueue *queue, const void *key_out, const void *data_out);

// Peeks the highest priority item in the queue and stores it in the given location.
void priority_queue_peek(PriorityQueue *queue, const void *key_out, const void *data_out);

// Removes all the items from the queue.
// Remember to clean up memory first.
void priority_queue_clear(PriorityQueue *queue);

// Copies a queue. Returns whether successful.
bool priority_queue_copy(PriorityQueue *dest_queue, const PriorityQueue *queue);

// Gets an Iterator for this PriorityQueue. Maintains order.
// Modifies the priority queue!
void priority_queue_get_iterator(const PriorityQueue *queue, Iterator *iter);

// Gets a Sink for this queue.
void priority_queue_get_sink(const PriorityQueue *queue, Sink *sink);

#endif // COMMON_PRIORITY_QUEUE_H__
