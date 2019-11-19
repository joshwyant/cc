#ifndef TEST_COMMON_MAP_TESTS_H__
#define TEST_COMMON_MAP_TESTS_H__

#include "../../common/public/list.h"
#include "../../common/public/map.h"
#include "../macros.h"

// COPIED from map.c
struct Map {
  List *buckets;
  KeyInfo key_info;
  size_t elem_size;
  size_t capacity;
  size_t count;
  int version;
};

int map_tests();

#endif // TEST_COMMON_MAP_TESTS_H__
