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

#define ITERATOR_BEGIN                                                         \
  switch (state) {                                                             \
  case 0:

#define YIELD(x)                                                               \
  do {                                                                         \
    state = __LINE__; return x; case __LINE__:;                                \
  } while (0)

#define ITERATOR_END } // TODO: return EOF here

#endif // COMMON_PUBLIC_COROUTINE_H__
