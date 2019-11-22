#ifndef COMMON_PUBLIC_COROUTINE_H__
#define COMMON_PUBLIC_COROUTINE_H__

#include "for_each_macros.h"
#include "iterator.h"

// Derived from: https://www.chiark.greenend.org.uk/~sgtatham/coroutines.html

// Design:
// - Provide a type-safe iterator
// - Define an EOF symbol
// - Use a stateful structure, declare variables
// - Creates a factory function as well, with initial parameters?
// - Instantiate parameters through ITERATOR_BEGIN

#define GENERATOR_DEFINE_MEMBER(T, x) T x;
#define GENERATOR_LOAD_STATE(T, x) T x = iter->x;
#define GENERATOR_SAVE_STATE(T, x) iter->x = x;

#define DECLARE_GENERATOR(T, name) \
typedef struct name##Generator name##Generator;\
name##Generator *name##Generator_alloc();\
bool name##Generator_eof(name##Generator *iter);\
T name(name##Generator *iter);

#define DEFINE_GENERATOR(T, name, ...) \
struct name##Generator {\
  int state;\
  bool eof;\
  T current;\
  T eof_val;\
  DFOR_EACH(GENERATOR_DEFINE_MEMBER, __VA_ARGS__)\
};\
typedef struct name##Generator name##Generator;\
name##Generator *name##Generator_alloc() {\
  return calloc(1, sizeof(name##Generator));\
}\
bool name##Generator_eof(name##Generator *iter) {\
  return iter->eof;\
}\
T name(name##Generator *iter) {\
  T eof;\
  DFOR_EACH(GENERATOR_LOAD_STATE, __VA_ARGS__)\
  goto branch_label;\
yield_label:\
  DFOR_EACH(GENERATOR_SAVE_STATE, __VA_ARGS__)\
  iter->eof_val = eof;\
  return iter->current;\
branch_label:\
  switch (iter->state) { case 0: do{} while(0);

#define yield(x)                                                               \
  do { iter->state = __LINE__; iter->current = (x); goto yield_label; case __LINE__:; } while (0);

#define yield_eof }} iter->eof = true; return eof;

#endif // COMMON_PUBLIC_COROUTINE_H__
