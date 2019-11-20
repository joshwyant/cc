#ifndef COMMON_PROTECTED_STACK_H__
#define COMMON_PROTECTED_STACK_H__

#include "../public/stack.h"
#include "../public/list.h"

#include <stdlib.h>

struct Stack
{
    List *list;
    size_t current;
    size_t elem_size;
    int version;
};

#endif // COMMON_PROTECTED_STACK_H__
