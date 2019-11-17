#include <stdlib.h>

// Save pointers to the standard library versions here:
void *(*_cmalloc)(size_t) = malloc;
void (*_cfree)(void*) = free;

#ifndef TESTING
extern void *(*_malloc_impl)(size_t) = malloc;
extern void (*_free_impl)(void*) = free;
#else
extern char _malloc_reference[1000] = "";
void *test_malloc(size_t size);
void test_free(void *ptr);
extern void *(*_malloc_impl)(size_t) = test_malloc;
extern void (*_free_impl)(void*) = test_free;
#endif

#include <stdio.h>

#include "stubs.h"

#ifdef TESTING
void *test_malloc(size_t size) {
    void *data = (*_cmalloc)(size);
    printf("malloc called: %4d 0x%016llx\n", (unsigned int)size, (unsigned long long)data);
    printf("%s\n", _malloc_reference);
    return data;
}

void test_free(void *ptr) {
    printf("free called:        0x%016llx\n", (unsigned long long)ptr);
    (*_cfree)(ptr);
}
#endif