#ifndef COMMON_ASSERT_H__
#define COMMON_ASSERT_H__

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#ifndef NDEBUG
    #define ASSERT(condition) assert(condition)
#else
    #define ASSERT(condition) do { } while (false)
#endif // NDEBUG

#endif // COMMON_ASSERT_H__
