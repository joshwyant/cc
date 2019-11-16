#ifndef TEST_MACROS_H__
#define TEST_MACROS_H__

#include <assert.h>
#include <stddef.h>
#include <stdio.h>

#define EXPECT(condition, message) \
    if (!(condition)) { \
        fprintf(stderr, message) \
        return 1; \
    }

#define TEST(name) \
    void _test_##name(); \
    int test_##name() { \
        printf("================================================================================\n"); \
        printf("== Test: %-68s ==\n", "test_" #name); \
        printf("================================================================================\n"); \
        _test_##name(); printf("\n%s completed.\n\n", "test_" #name); return 0; \
    } \
    void _test_##name()

struct test {
    int foo;
    int bar;
};

#endif // TEST_MACROS_H__
