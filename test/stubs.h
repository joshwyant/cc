#ifndef TEST_STUBS_H__
#define TEST_STUBS_H__

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

enum {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
};

extern char _log_indent[];

#define LOGARGS(level, format)                                                 \
"%-5s %40s:%-5d: %s" format "\n", #level, __FILE__, __LINE__, _log_indent
#define LOG_FORMAT(level, format, ...)                                         \
  do {                                                                         \
    if ((level) >= LOGLEVEL) {                                                 \
      if ((level) < WARN) {                                                    \
        printf(LOGARGS(level, format), __VA_ARGS__);                           \
      } else {                                                                 \
        fprintf(stderr, LOGARGS(level, format), __VA_ARGS__);                  \
      }                                                                        \
    }                                                                          \
  } while (0)
#define LOG_FORMAT_INDENT(level, format, ...)                                  \
  do {                                                                         \
    if ((level) >= LOGLEVEL) {                                                 \
      LOG_INDENT();                                                            \
      LOG_FORMAT(level, format, __VA_ARGS__);                                  \
      LOG_DEINDENT();                                                          \
    }                                                                          \
  } while (0)
#define LOG(level, message) LOG_FORMAT(level, "%s", message)
#define LOG_INDENT()                                                           \
  do {                                                                         \
    strcat(_log_indent, "  ");                                                 \
  } while (0)
#define LOG_DEINDENT()                                                         \
  do {                                                                         \
    _log_indent[strlen(_log_indent) - 2] = '\0';                               \
  } while (0)

#ifndef TESTING
#ifndef LOGLEVEL
#define LOGLEVEL INFO
#endif
#else
#ifndef LOGLEVEL
#define LOGLEVEL DEBUG
#endif
void *malloc_ext(size_t size);
void *calloc_ext(size_t num, size_t size);
void *realloc_ext(void * ptr, size_t size);
void free_ext(void *ptr);
void init_malloc_logging(void);
int test_find_leaks(void);
extern char _malloc_reference[1000];
#define ALLOC_REF sprintf(_malloc_reference, "%s:%d", __FILE__, __LINE__)
// These macros hide the stdlib.h versions:
#define malloc(size) (ALLOC_REF, malloc_ext(size))
#define calloc(num, size) (ALLOC_REF, calloc_ext(num, size))
#define realloc(ptr, size) (ALLOC_REF, realloc_ext(ptr, size))
#define free(ptr) (ALLOC_REF, free_ext(ptr))
#endif // TESTING

#endif // TEST_STUBS_H__
