#ifndef COMMON_PROTECTED_QUEUE_H__
#define COMMON_PROTECTED_QUEUE_H__

#include <stdbool.h>
#include <stddef.h>

#include "../public/queue.h"
#include "../public/list.h"

struct Queue
{
    List *list;
    size_t begin;
    size_t current;
    size_t count;
    size_t elem_size;
    size_t capacity; // != list->capacity; == list->count
    int version;
};

#endif // COMMON_PROTECTED_QUEUE_H__
