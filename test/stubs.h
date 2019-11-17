#ifndef TEST_STUBS_H__
#define TEST_STUBS_H__

#include <stdbool.h>
#include <stdlib.h>

extern void *(*_malloc_impl)(size_t);
extern void (*_free_impl)(void *);

#ifndef TESTING
#define malloc(size) (*_malloc_impl)(size)
#define free(ptr) (*_free_impl)(ptr)
#else
void init_malloc_logging();
int test_find_leaks();
extern bool _malloc_logging;
extern char _malloc_reference[1000];
#define malloc(size)                                                           \
  (sprintf(_malloc_reference, "file %s, line %d", __FILE__, __LINE__),         \
   (*_malloc_impl)(size))
#define free(ptr) (*_free_impl)(ptr)
#endif // TESTING

#endif // TEST_STUBS_H__
