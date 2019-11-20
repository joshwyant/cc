#ifndef COMMON_PROTECTED_MAP_H__
#define COMMON_PROTECTED_MAP_H__

#include "../public/map.h"

#include <stdlib.h>

#include "../public/list.h"

struct Map {
  List *buckets;
  KeyInfo key_info;
  size_t elem_size;
  size_t capacity;
  size_t count;
  int version;
};

struct MapBucket {
  List *key_value_pairs;
};

#endif // #ifndef COMMON_PROTECTED_MAP_H__
