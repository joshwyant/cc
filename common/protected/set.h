#ifndef COMMON_PROTECTED_SET_H__
#define COMMON_PROTECTED_SET_H__

#include "../public/set.h"

#include <stdlib.h>

#include "../public/vector.h"

struct Set {
  Vector *buckets;
  KeyInfo key_info;
  size_t capacity;
  size_t count;
  int version;
};

struct SetBucket {
  Vector *items; // Holds void* items to allocated copies
};

#endif // COMMON_PROTECTED_SET_H__
