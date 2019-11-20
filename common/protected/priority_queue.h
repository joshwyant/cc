#ifndef COMMON_PROTECTED_PRIORITY_QUEUE_H__
#define COMMON_PROTECTED_PRIORITY_QUEUE_H__

#include <stdbool.h>
#include <stddef.h>

#include "../public/priority_queue.h"
#include "../public/list.h"

struct PriorityQueue
{
    List *list;
    PriorityQueueMode mode;
    RelationalKeyInfo *key_info;
    size_t count;
    size_t elem_size;
    int version;
};

#endif // COMMON_PROTECTED_PRIORITY_QUEUE_H__
