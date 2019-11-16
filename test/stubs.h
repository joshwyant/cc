#ifndef TEST_STUBS_H__
#define TEST_STUBS_H__

#include <stddef.h>
#include <stdlib.h>

extern void *(*_malloc_impl)(size_t);
extern void (*_free_impl)(void*);

#define malloc(size) ((*_malloc_impl)(size))
#define free(ptr) ((*_free_impl)(ptr))

// Redefine to use our mocks:
// void *malloc(size_t size) { return (*_malloc_impl)(size); }
// void free(void *ptr) { (*_free_impl)(ptr); }

#endif // TEST_STUBS_H__
