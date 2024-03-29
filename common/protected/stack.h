#ifndef COMMON_PROTECTED_STACK_H__
#define COMMON_PROTECTED_STACK_H__

#include "../public/stack.h"
#include "../public/vector.h"

#include <stdlib.h>

struct Stack
{
    Vector *list;
    size_t current;
    size_t elem_size;
    int version;
};

#endif // COMMON_PROTECTED_STACK_H__
