#ifndef TEST_STUBS_H__
#define TEST_STUBS_H__

#include <stdbool.h>
#include <stdlib.h>

#ifndef TESTING
#else
void *malloc_ext(size_t size);
void *calloc_ext(size_t num, size_t size);
void *realloc_ext(void * ptr, size_t size);
void free_ext(void *ptr);
void init_malloc_logging(void);
int test_find_leaks(void);
extern char _malloc_reference[1000];
#define ALLOC_REF sprintf(_malloc_reference, "file %s, line %d", __FILE__, __LINE__)
// These macros hide the stdlib.h versions:
#define malloc(size) (ALLOC_REF, malloc_ext(size))
#define calloc(num, size) (ALLOC_REF, calloc_ext(num, size))
#define realloc(ptr, size) (ALLOC_REF, realloc_ext(ptr, size))
#define free(ptr) (ALLOC_REF, free_ext(ptr))
#endif // TESTING

#endif // TEST_STUBS_H__
