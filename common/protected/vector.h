#ifndef COMMON_PROTECTED_VECTOR_H__
#define COMMON_PROTECTED_VECTOR_H__

#include "../public/vector.h"

#include <stdlib.h>

struct Vector {
  void *data;

  // sizeof(data)
  size_t data_size;

  // sizeof(element)
  size_t elem_size;

  // The actual number of elements.
  size_t elem_count;

  // The ideal capacity -- must be a power of 2.
  size_t reserve_count;

  // Detect changes to the list while iterating
  int version;

  void (*print)(const Vector *, char*, const void*);
};

#endif // #ifndef COMMON_PROTECTED_VECTOR_H__
