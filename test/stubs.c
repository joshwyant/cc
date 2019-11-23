#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "../common/public/iterator.h"
#include "../common/public/map.h"

#ifndef TESTING
#else
Map *_malloc_log;
bool _malloc_logging = false;
char _malloc_reference[1000] = "";
#endif

#include "macros.h"
#include "stubs.h"
#include <stdio.h>

// Undefine the test macros so we can use the real versions here
#undef malloc
#undef calloc
#undef realloc
#undef free

#ifdef TESTING
struct MemoryLog {
  char *file_info;
  size_t num_bytes;
};

void *malloc_ext(size_t size) {
  struct MemoryLog log;
  void *data;
  size += (4 - (size & 3)) & 3; // align to 4 bytes
  if ((data = malloc(size)) == NULL) goto error;
  for (int i = 0; i < size >> 2; i++) {
    ((unsigned int*)data)[i] = 0xDEADBEEF; // to detect uninitialized memory
  }
  if (!_malloc_logging) {
    _malloc_logging = true;
    if ((log.file_info = calloc(strlen(_malloc_reference) + 1, 1)) == NULL) {
      goto error_data;
    }
    strcpy(log.file_info, _malloc_reference);
    log.num_bytes = size;
    if (!Map_add(_malloc_log, &data, &log).key) {
      goto error_file_info;
    }
    _malloc_logging = false;
  }
  return data;
// error_map:
//   Map_delete(_malloc_log, &data);
error_file_info:
  free(log.file_info);
error_data:
  free(data);
error:
  _malloc_logging = false;
  assert(0 && "malloc failed.");
  return NULL;
}

void *calloc_ext(size_t num, size_t size) {
  struct MemoryLog log;
  void *data;
  size += (4 - (size & 3)) & 3; // align to 4 bytes
  if (NULL == (data = calloc(num, size))) { // automatically initializes memory to 0
    goto error;
  }
  if (!_malloc_logging) {
    _malloc_logging = true;
    if (NULL == (log.file_info = calloc(strlen(_malloc_reference) + 1, 1))) {
      goto error_data;
    }
    strcpy(log.file_info, _malloc_reference);
    log.num_bytes = num * size;
    if (!Map_add(_malloc_log, &data, &log).key) {
      goto error_file_info;
    }
    _malloc_logging = false;
  }
  return data;
// error_map:
//   Map_delete(_malloc_log, &data);
error_file_info:
  free(log.file_info);
error_data:
  free(data);
error:
  _malloc_logging = false;
  assert(0 && "calloc failed.");
  return NULL;
}

void *realloc_ext(void *ptr, size_t size) {
  struct MemoryLog log;
  void *data;
  size += (4 - (size & 3)) & 3; // align to 4 bytes
  assert(Map_contains_key(_malloc_log, &ptr) && "Tried to realloc already-freed memory.");
  Map_get(_malloc_log, &ptr, &log);
  if (log.num_bytes > size) {
    // Overwrite trailing memory before shrinking to detect already-freed memory:
    for (int i = size >> 2; i < log.num_bytes >> 2; i++) {
      ((unsigned int*)data)[i] = 0xDEADBEEF; // to detect uninitialized memory
    }
  }
  if (NULL == (data = malloc(size))) goto error; // don't use realloc, to simulate semantics on error
  if (!_malloc_logging) {
    _malloc_logging = true;
    if (NULL == (log.file_info = realloc(log.file_info, strlen(_malloc_reference) + 1))) {
      goto error_data;
    }
    strcpy(log.file_info, _malloc_reference);
    log.num_bytes = size;
    if (!Map_add(_malloc_log, &data, &log).key) {
      goto error_file_info;
    }
    Map_delete(_malloc_log, &ptr); // Remove the old entry.
    free(ptr); // freeing old memory instead.
    _malloc_logging = false;
  }
  return data;
// error_map:
//   Map_delete(_malloc_log, &data);
error_file_info:
  free(log.file_info);
error_data:
  free(data);
error:
  _malloc_logging = false;
  assert(0 && "realloc failed.");
  return NULL;
}

void free_ext(void *ptr) {
  if (!_malloc_logging) {
    if (!ptr) return;
    _malloc_logging = true;
    struct MemoryLog log;
    assert(Map_contains_key(_malloc_log, &ptr) && "Tried to free already-freed memory.");
    Map_get(_malloc_log, &ptr, &log);
    Map_delete(_malloc_log, &ptr);
    free(log.file_info);
    // Overwrite memory before releasing to detect already-freed memory:
    for (int i = 0; i < log.num_bytes >> 2; i++) {
      ((unsigned int*)ptr)[i] = 0xDEADBEEF; // to detect uninitialized memory
    }
    _malloc_logging = false;
  }
  free(ptr);
}

void init_malloc_logging(void) {
  _malloc_logging = true;
  _malloc_log = Map_alloc(&UnsignedLongKeyInfo, sizeof(struct MemoryLog));
  assert(_malloc_log && "Unable to initialize malloc logging.");
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
