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

char _log_indent[1000] = "";

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
  LOG_FORMAT(TRACE, "malloc(%zu) called (re-entry = %s)", size,
             _malloc_logging ? "true" : "false");
  if (_malloc_logging) return malloc(size);
  _malloc_logging = true;
  struct MemoryLog log;
  void *data;
  size += (4 - (size & 3)) & 3; // align to 4 bytes
  LOG_FORMAT(TRACE, "Aligned to %zu bytes", size);
  if ((data = malloc(size)) == NULL) goto error;
  LOG_FORMAT(TRACE, "NEW allocated address: %p", data);
  LOG(TRACE, "Overwriting memory...");
  for (int i = 0; i < size >> 2; i++) {
    ((unsigned int*)data)[i] = 0xDEADBEEF; // to detect uninitialized memory
  }
  LOG(TRACE, "Allocating new file_info string...");
  if ((log.file_info = calloc(strlen(_malloc_reference) + 1, 1)) == NULL) {
    goto error_data;
  }
  strcpy(log.file_info, _malloc_reference);
  log.num_bytes = size;
  LOG_FORMAT(TRACE, "Adding new memory log (num_bytes=%zu, file_info=%s)...",
             log.num_bytes, log.file_info);
  if (!Map_add(_malloc_log, &data, &log).key) {
    goto error_file_info;
  }
  _malloc_logging = false;
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
  LOG_FORMAT(TRACE, "calloc(%zu, %zu) called (e-entry = %s)", num, size,
             _malloc_logging ? "true" : "false");
  if (_malloc_logging) return calloc(num, size);
  LOG_INDENT();
  _malloc_logging = true;
  struct MemoryLog log;
  void *data;
  size += (4 - (size & 3)) & 3; // align to 4 bytes
  LOG_FORMAT(TRACE, "Aligned to %zu bytes", size);
  if (NULL == (data = calloc(num, size))) { // automatically initializes memory to 0
    goto error;
  }
  LOG_FORMAT(TRACE, "NEW allocated address: %p", data);
  LOG(TRACE, "Allocating new file_info string...");
  if (NULL == (log.file_info = calloc(strlen(_malloc_reference) + 1, 1))) {
    goto error_data;
  }
  strcpy(log.file_info, _malloc_reference);
  log.num_bytes = num * size;
  LOG_FORMAT(TRACE, "Adding new memory log (num_bytes=%zu, file_info=%s)...",
             log.num_bytes, log.file_info);
  if (!Map_add(_malloc_log, &data, &log).key) {
    goto error_file_info;
  }
  _malloc_logging = false;
  LOG_DEINDENT();
  return data;
// error_map:
//   Map_delete(_malloc_log, &data);
error_file_info:
  free(log.file_info);
error_data:
  free(data);
error:
  _malloc_logging = false;
  LOG_DEINDENT();
  assert(0 && "calloc failed.");
  return NULL;
}

void *realloc_ext(void *ptr, size_t size) {
  LOG_FORMAT(TRACE, "realloc(%p, %zu) called (e-entry = %s)", ptr, size,
             _malloc_logging ? "true" : "false");
  if (_malloc_logging) return realloc(ptr, size);
  LOG_INDENT();
  _malloc_logging = true;
  struct MemoryLog log;
  log.file_info = NULL;
  void *data;
  size += (4 - (size & 3)) & 3; // align to 4 bytes
  LOG_FORMAT(TRACE, "Aligned to %zu bytes", size);
  if (ptr != NULL) {
    assert(Map_contains_key(_malloc_log, &ptr) && "Tried to realloc already-freed memory.");
    Map_get(_malloc_log, &ptr, &log);
    LOG_FORMAT(TRACE, "Existing element being reallocated (%p): (num_bytes=%zu, file_info=%s)",
              ptr, log.num_bytes, log.file_info);
    if (log.num_bytes > size) {
      LOG(TRACE, "Overwriting shrunken memory tail...");
      // Overwrite trailing memory before shrinking to detect already-freed memory:
      for (int i = log.num_bytes >> 2; i < size >> 2; i++) {
        ((unsigned int*)data)[i] = 0xDEADBEEF; // to detect uninitialized memory
      }
    }
  }
  if (NULL == (data = malloc(size))) goto error; // don't use realloc, to simulate semantics on error
  if (ptr != NULL) {
    memcpy(data, ptr, log.num_bytes);
  }
  LOG_FORMAT(TRACE, "NEW reallocated address: %p", data);
  LOG(TRACE, "Reallocating file info...");
  if (NULL == (log.file_info = realloc(log.file_info, strlen(_malloc_reference) + 1))) {
    goto error_data;
  }
  strcpy(log.file_info, _malloc_reference);
  log.num_bytes = size;
  LOG_FORMAT(TRACE, "Adding new log element (num_bytes=%zu, file_info=%s)...", log.num_bytes, log.file_info);
  if (!Map_add(_malloc_log, &data, &log).key) {
    goto error_file_info;
  }
  if (ptr != NULL) {
    LOG_FORMAT(TRACE, "Removing old map element and deleting old block (%p)", ptr);
    Map_delete(_malloc_log, &ptr); // Remove the old entry.
    free(ptr); // freeing old memory instead.
  }
  _malloc_logging = false;
  LOG_DEINDENT();
  return data;
// error_map:
//   Map_delete(_malloc_log, &data);
error_file_info:
  free(log.file_info);
error_data:
  free(data);
error:
  _malloc_logging = false;
  LOG_DEINDENT();
  assert(0 && "realloc failed.");
  return NULL;
}

void free_ext(void *ptr) {
  LOG_FORMAT(TRACE, "free(%p) called (e-entry = %s)", ptr,
             _malloc_logging ? "true" : "false");
  if (_malloc_logging) {
    free(ptr);
    return;
  }
  LOG_INDENT();
  if (!ptr) return;
  _malloc_logging = true;
  struct MemoryLog log;
  // TODO: Un-comment
  assert(Map_contains_key(_malloc_log, &ptr) && "Tried to free already-freed memory.");
  Map_get(_malloc_log, &ptr, &log);
  LOG_FORMAT(TRACE, "Element being freed (%p): (num_bytes=%zu, file_info=%s)",
             ptr, log.num_bytes, log.file_info);
  // Overwrite memory before releasing to detect already-freed memory:
  for (int i = 0; i < log.num_bytes >> 2; i++) {
    ((unsigned int*)ptr)[i] = 0xDEADBEEF; // to detect uninitialized memory
  }
  Map_delete(_malloc_log, &ptr);
  free(log.file_info);
  free(ptr);
  _malloc_logging = false;
  LOG_DEINDENT();
}

void _Map_malloc_log_key_print(const Map *self, char *str, const void **elem) {
  struct MemoryLog log;
  unsigned char byte[4];
  unsigned char *buffer = (unsigned char*)*elem;
  str[0] = '\0';
  sprintf(str, "Entry for allocated memory location (void*)%p -> ", *elem);
  Map_get(self, elem, &log);
  size_t i;
  for (i = 0; i < log.num_bytes; i++) {
    sprintf((char *)byte, " %02x", ((unsigned char*)buffer)[i]);
    if (i % 32 == 0) strcat(str, "\n");
    strcat(str, (char*)byte);
    if (i % 8 == 7) strcat(str, "  ");
    if (i % 4 == 3) strcat(str, " ");
  }
  strcat(str, " \n(");
  str += strlen(str);
  for (i = 0; i < log.num_bytes; i++) {
    str[i] = buffer[i] >= ' ' && buffer[i] <= '~' ? buffer[i] : '.';
  }
  str[i] = 0;
  strcat(str, ")\n");
}

void _Map_malloc_log_value_print(const Map *self, char *str, const struct MemoryLog *elem) {
  sprintf(str, "MemoryLog { num_bytes: %zu file_info: \"%s\" }", elem->num_bytes, elem->file_info);
}

void init_malloc_logging(void) {
  LOG(TRACE, "init_malloc_logging()");
  LOG_INDENT();
  _malloc_logging = true;
  _malloc_log = Map_alloc(&UnsignedLongKeyInfo, sizeof(struct MemoryLog));
  Map_print_key_fn(_malloc_log, (void(*)(const Map*,char*,const void*))_Map_malloc_log_key_print);
  Map_print_value_fn(_malloc_log, (void(*)(const Map*,char*,const void*))_Map_malloc_log_value_print);
  assert(_malloc_log && "Unable to initialize malloc logging.");
  _malloc_logging = false;
  LOG_DEINDENT();
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
