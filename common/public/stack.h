#ifndef COMMON_PUBLIC_STACK_H__
#define COMMON_PUBLIC_STACK_H__

#include <stdbool.h>
#include <stddef.h>

#include "iterator.h"

// A LIFO stack data structure.
typedef struct Stack Stack;

// Creates a new Stack object.
Stack *Stack_alloc(size_t elem_size);

// Gets the number of elements in the Stack
size_t Stack_count(const Stack *stack);

// Returns whether the stack is empty
bool Stack_empty(const Stack *stack);

// Gets the capacity of the Stack
size_t Stack_capacity(const Stack *stack);

// Reserves memory for a total of `num_elems' objects.
// Capacity always increases by a factor of 2.
// Returns whether the allocation was successful.
bool Stack_reserve(Stack *stack, size_t num_elems);

// Removes excess memory for the Stack object.
// Returns whether the allocation was successful.
bool Stack_trim(Stack *stack);

// Frees up the Stack object.
void Stack_free(Stack *stack);

// Gets the size of an element in the Stack
size_t Stack_element_size(const Stack *stack);

// Gets the data for the stack.
void *Stack_get_data(const Stack *stack);

// Gets the item at `index'
void *Stack_get(const Stack *stack, size_t index);

// Adds the item to the end of the stack and returns pointer to new data.
// Returns NULL if unsuccessful.
void *Stack_push(Stack *stack, const void *data);

// Removes an item from the end of the stack and stores it in the given
// location.
bool Stack_pop(Stack *stack, void *data_out);

// Peeks the item on the end of the stack and stores it in the given location.
bool Stack_peek(const Stack *stack, void *data_out);

// Removes all the items from the stack.
// Remember to clean up memory first.
void Stack_clear(Stack *stack);

// Copies a stack. Returns whether successful.
bool Stack_copy(Stack *dest_stack, const Stack *stack);

// Gets an Iterator for this Stack. Modifies the stack!
void Stack_get_iterator(const Stack *stack, Iterator *iter);

// Gets a Sink for this stack. Maintains LIFO order.
void Stack_get_sink(const Stack *stack, Sink *sink);

// Gets an Indexer for this stack. Index 0 is the oldest item in the stack.
void Stack_get_indexer(const Stack *stack, Indexer *indexer);

#endif // COMMON_PUBLIC_STACK_H__
