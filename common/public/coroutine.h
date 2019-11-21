#ifndef COMMON_PUBLIC_COROUTINE_H__
#define COMMON_PUBLIC_COROUTINE_H__

#include "iterator.h"

// Derived from: https://www.chiark.greenend.org.uk/~sgtatham/coroutines.html

// Design:
// - Provide a type-safe iterator
// - Define an EOF symbol
// - Use a stateful structure, declare variables
// - Creates a factory function as well, with initial parameters?
// - Instantiate parameters through ITERATOR_BEGIN

#define CONCATENATE(arg1, arg2)   CONCATENATE1(arg1, arg2)
#define CONCATENATE1(arg1, arg2)  CONCATENATE2(arg1, arg2)
#define CONCATENATE2(arg1, arg2)  arg1##arg2

#define DFOR_EACH_1(what, x, y) what(x, y)
#define DFOR_EACH_2(what, x, y, ...)\
  what(x, y)\
  DFOR_EACH_1(what,  __VA_ARGS__)
#define DFOR_EACH_3(what, x, y, ...)\
  what(x, y)\
  DFOR_EACH_2(what, __VA_ARGS__)
#define DFOR_EACH_4(what, x, y, ...)\
  what(x, y)\
  DFOR_EACH_3(what,  __VA_ARGS__)
#define DFOR_EACH_5(what, x, y, ...)\
  what(x, y)\
 DFOR_EACH_4(what,  __VA_ARGS__)
#define DFOR_EACH_6(what, x, y, ...)\
  what(x, y)\
  DFOR_EACH_5(what,  __VA_ARGS__)
#define DFOR_EACH_7(what, x, y, ...)\
  what(x, y)\
  DFOR_EACH_6(what,  __VA_ARGS__)
#define DFOR_EACH_8(what, x, y, ...)\
  what(x, y)\
  DFOR_EACH_7(what,  __VA_ARGS__)

#define DFOR_EACH_NARG(...) DFOR_EACH_NARG_(__VA_ARGS__, DFOR_EACH_RSEQ_N())
#define DFOR_EACH_NARG_(...) DFOR_EACH_ARG_N(__VA_ARGS__) 
#define DFOR_EACH_ARG_N(_1, _1_, _2, _2_, _3, _3_, _4, _4_, _5, _5_, _6, _6_, _7, _7_, _8, _8_, N, ...) N 
#define DFOR_EACH_RSEQ_N() 8, 8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0

#define DFOR_EACH_(N, what, ...) CONCATENATE(DFOR_EACH_, N)(what, __VA_ARGS__)
#define DFOR_EACH(what, ...) DFOR_EACH_(DFOR_EACH_NARG(__VA_ARGS__), what, __VA_ARGS__)

#define GENERATOR_DEFINE_MEMBER(T, x) T x;
#define GENERATOR_LOAD_STATE(T, x) T x = iter->x;
#define GENERATOR_SAVE_STATE(T, x) iter->x = x;

#define DECLARE_GENERATOR(T, name) \
typedef struct name##_generator name##_generator;\
name##_generator *create_##name##_generator();\
bool name##_generator_eof(name##_generator *iter);\
T name(name##_generator *iter);

#define DEFINE_GENERATOR(T, name, ...) \
struct name##_generator {\
  int state;\
  bool eof;\
  T current;\
  T eof_val;\
  DFOR_EACH(GENERATOR_DEFINE_MEMBER, __VA_ARGS__)\
};\
typedef struct name##_generator name##_generator;\
name##_generator *create_##name##_generator() {\
  return calloc(1, sizeof(name##_generator));\
}\
bool name##_generator_eof(name##_generator *iter) {\
  return iter->eof;\
}\
T name(name##_generator *iter) {\
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
