#ifndef TEST_COMMON_LIST_TESTS_H__
#define TEST_COMMON_LIST_TESTS_H__

#include "../../common/public/list.h"
#include "../../common/public/list_generic.h"
#include "../macros.h"

// COPIED from list.c
struct List {
  void *data;
  size_t data_size;
  size_t elem_size;
  size_t elem_count;
  size_t reserve_count;
  int version;
};

int list_tests();

#endif // TEST_COMMON_LIST_TESTS_H__
