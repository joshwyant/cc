#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "../common/public/iterator.h"
#include "../common/public/map.h"

// Save pointers to the standard library versions here:
void *(*_cmalloc)(size_t) = malloc;
void (*_cfree)(void *) = free;

#ifndef TESTING
extern void *(*_malloc_impl)(size_t) = malloc;
extern void (*_free_impl)(void *) = free;
#else
Map *_malloc_log;
extern bool _malloc_logging = false;
extern char _malloc_reference[1000] = "";
void *test_malloc(size_t size);
void test_free(void *ptr);
extern void *(*_malloc_impl)(size_t) = test_malloc;
extern void (*_free_impl)(void *) = test_free;
#endif

#include "macros.h"
#include "stubs.h"
#include <stdio.h>

#ifdef TESTING
struct MemoryLog {
  char *file_info;
  size_t num_bytes;
};

void *test_malloc(size_t size) {
  void *data = (*_cmalloc)(size);
  if (!_malloc_logging) {
    _malloc_logging = true;
    struct MemoryLog log;
    log.file_info = (*_cmalloc)(strlen(_malloc_reference) + 1);
    strcpy(log.file_info, _malloc_reference);
    log.num_bytes = size;
    Map_add(_malloc_log, &data, &log);
    // printf("malloc called: %4d 0x%016llx\n", (unsigned int)size,
    //       (unsigned long long)data);
    // printf("%s\n", log.file_info);
    _malloc_logging = false;
  }
  return data;
}

void test_free(void *ptr) {
  (*_cfree)(ptr);
  if (!_malloc_logging) {
    _malloc_logging = true;
    Map_delete(_malloc_log, &ptr);
    // printf("free called:        0x%016llx\n", (unsigned long long)ptr);
    _malloc_logging = false;
  }
}

void init_malloc_logging() {
  _malloc_logging = true;
  _malloc_log = Map_alloc(&UnsignedLongKeyInfo, sizeof(struct MemoryLog));
  _malloc_logging = false;
}

void find_leaks_for_each(void *elem) {
  struct MemoryLog log = *(struct MemoryLog *)elem;
  printf("***** Memory leak detected, %zu bytes:\n      In malloc called from "
         "%s\n",
         log.num_bytes, log.file_info);
}

TEST(find_leaks) {
  Iterator iter;
  Map_get_value_iterator(_malloc_log, &iter);
  for_each(&iter, find_leaks_for_each);

  assert(Map_count(_malloc_log) == 0);
}
#endif