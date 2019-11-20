#ifndef COMMON_PROTECTED_ARRAY_H__
#define COMMON_PROTECTED_ARRAY_H__

#include "../public/array.h"

#include <stdbool.h>
#include <stddef.h>

struct Array
{
    size_t count;
    size_t elem_size;
    unsigned char data[];
};

#endif // COMMON_PROTECTED_ARRAY_H__
