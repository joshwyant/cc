#include "protected/stack.h"

#include <string.h>

#include "../test/stubs.h"
#include "public/assert.h"

// TODO: Version

bool Stack_init(Stack *stack, size_t elem_size)
{
    ASSERT(stack);
    ASSERT(elem_size);
    stack->list = NULL;
    stack->current = -1;
    stack->elem_size = elem_size;
    return true;
}

void Stack_cleanup(Stack *stack)
{
    ASSERT(stack);
    if (stack->list) List_free(stack->list);
    Stack_init(stack, stack->elem_size);
}

Stack *Stack_alloc(size_t elem_size)
{
    ASSERT(elem_size);

    Stack *stack = NULL;
    if ((stack = malloc(sizeof(Stack))) == NULL || !Stack_init(stack, elem_size)) {
        return NULL;
    }
    return stack;
}

size_t Stack_count(const Stack *stack)
{
    ASSERT(stack);
    return stack->current + 1;
}

bool Stack_empty(const Stack *stack)
{
    ASSERT(stack);
    return stack->current == -1;
}

size_t Stack_capacity(const Stack *stack)
{
    ASSERT(stack);
    return stack->list ? 0 : List_capacity(stack->list); // actual capacity
}

bool Stack_reserve(Stack *stack, size_t num_elems)
{
    ASSERT(stack);
    ASSERT(num_elems);
    return List_reserve(stack->list, num_elems);
}

bool Stack_trim(Stack *stack)
{
    ASSERT(stack);
    if (!stack->list) return true;
    List_truncate(stack->list, stack->current + 1);
    return List_trim(stack->list);
}

void Stack_free(Stack *stack)
{
    ASSERT(stack);
    Stack_cleanup(stack);
    free(stack);
}

size_t Stack_element_size(const Stack *stack)
{
    ASSERT(stack);
    return stack->elem_size;
}

void *Stack_get_data(const Stack *stack)
{
    ASSERT(stack);
    return stack->list ? List_get_data(stack->list) : NULL;
}

void *Stack_get(const Stack *stack, size_t index)
{
    ASSERT(stack);
    ASSERT(index < Stack_count(stack));
    return stack->list ? List_get(stack->list, index) : NULL;
}

void *Stack_push(Stack *stack, const void *data)
{
    ASSERT(stack);
    ASSERT(data);
    if (!stack->list && !(stack->list = List_alloc(stack->elem_size))) return NULL;
    stack->current++;
    if (stack->current >= List_count(stack->list)) {
        return List_add(stack->list, data);
    } else {
        List_set(stack->list, stack->current, data);
        return List_get(stack->list, stack->current);
    }
}

bool Stack_pop(Stack *stack, void *data_out)
{
    ASSERT(stack);
    ASSERT(data_out);
    if (!stack->list || stack->current < 0) return false;
    memcpy(data_out, (void*)List_get(stack->list, stack->current--), stack->elem_size);
    return true;
}

bool Stack_peek(const Stack *stack, void *data_out)
{
    ASSERT(stack);
    ASSERT(data_out);
    if (!stack->list || stack->current < 0) return false;
    memcpy(data_out, (void*)List_get(stack->list, stack->current), stack->elem_size);
    return true;
}

void Stack_clear(Stack *stack)
{
    ASSERT(stack);
    Stack_init(stack, stack->elem_size);
}

bool Stack_copy(Stack *dest_stack, const Stack *stack)
{
    ASSERT(dest_stack);
    ASSERT(stack);
    List *old_list = dest_stack->list;
    if (!(List_copy(old_list, stack->list))) return false;
    *dest_stack = *stack;
    dest_stack->list = old_list;
    return true;
}

void Stack_get_iterator(const Stack *stack, Iterator *iter)
{
    // TODO
}

void Stack_get_sink(const Stack *stack, Sink *sink)
{
    // TODO
}

void Stack_get_indexer(const Stack *stack, Indexer *indexer)
{
    // TODO
}
