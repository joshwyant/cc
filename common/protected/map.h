#ifndef COMMON_PROTECTED_MAP_H__
#define COMMON_PROTECTED_MAP_H__

#include "../public/map.h"

#include <stdlib.h>

#include "../public/vector.h"

struct Map {
  Vector *buckets;
  KeyInfo key_info;
  size_t elem_size;
  size_t capacity;
  size_t count;
  int version;
};

struct MapBucket {
  Vector *key_value_pairs;
};

#endif // #ifndef COMMON_PROTECTED_MAP_H__
