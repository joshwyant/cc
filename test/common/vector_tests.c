#include "vector_tests.h"

#include <string.h>

void test_string_foreach(char **item);

void test_int_foreach(int *item);

TEST(vector_basics) {
  struct test t = {1, 2};

  Vector *vector = Vector_alloc(sizeof(struct test));

  assert(Vector_get_data(vector) == NULL);
  assert(Vector_empty(vector));
  assert(Vector_count(vector) == 0);
  assert(Vector_capacity(vector) == 0);
  assert(Vector_element_size(vector) == sizeof(struct test));

  Vector_add(vector, &t);
  assert(!Vector_empty(vector));
  assert(Vector_count(vector) == 1);
  assert(Vector_capacity(vector) == 1);
  struct test *item = Vector_get(vector, 0);
  assert(item->foo == 1);

  t.foo = 3;
  Vector_add(vector, &t);
  assert(Vector_count(vector) == 2);
  assert(Vector_capacity(vector) == 2);
  item = Vector_get(vector, 1);
  t.foo = 4;
  t.bar = 5;
  assert(item->foo == 3);
  assert(item->bar == 2);

  Vector_add(vector, &t);
  Vector_add(vector, &t);
  Vector_add(vector, &t);
  assert(Vector_count(vector) == 5);
  assert(Vector_capacity(vector) == 8);

  Vector_reserve(vector, 9);
  assert(Vector_capacity(vector) == 9);
  Vector_add(vector, &t);
  Vector_add(vector, &t);
  Vector_add(vector, &t);
  Vector_add(vector, &t);
  assert(Vector_capacity(vector) == 9);
  Vector_add(vector, &t);
  assert(Vector_capacity(vector) != 9);

  Vector_remove_range(vector, 3, 2);
  assert(Vector_count(vector) == 8);

  Vector_free(vector);

  CStringVector *svector = CStringVector_alloc();
  CStringIterator siter;
  CStringVector_add(svector, "ABC");
  CStringVector_add(svector, "DEF");
  CStringVector_add(svector, "GHI");

  // TODO: wrongly accepts any kind of vector
  // Also check to make sure container element sizes are the same.
  CStringVector_get_iterator(svector, &siter);
  CString_for_each(&siter, test_string_foreach);
  Vector_free((Vector*)svector);

  IntVector *ivector = IntVector_alloc();
  IntIterator iiter;
  IntVector_add(ivector, 1);
  IntVector_add(ivector, 2);
  IntVector_add(ivector, 3);

  IntVector_get_iterator(ivector, &iiter);
  Int_for_each(&iiter, test_int_foreach);
  Vector_free((Vector*)ivector);
}

TEST(vector_delete) {
  IntVector *vector = IntVector_alloc();
  for (int i = 0; i <= 11; i++) {
    IntVector_add(vector, i);
  }

  // Test 1: Remove
  Vector_remove((Vector*)vector, 10);

  int test1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11};
  assert(Vector_count((Vector*)vector) == 11);
  assert(memcmp(test1, Vector_get_data((Vector*)vector), sizeof test1) == 0);

  // Test 2: Truncate
  Vector_truncate((Vector*)vector, 9);

  int test2[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
  assert(Vector_count((Vector*)vector) == 9);
  assert(memcmp(test2, Vector_get_data((Vector*)vector), sizeof test2) == 0);

  // Test 3: Remove range
  Vector_remove_range((Vector*)vector, 2, 2);

  int test3and4[] = {0, 1, 4, 5, 6, 7, 8};
  assert(Vector_count((Vector*)vector) == 7);
  assert(memcmp(test3and4, Vector_get_data((Vector*)vector), sizeof test3and4) == 0);

  // Test 4: Trim
  Vector_trim((Vector*)vector);
  
  assert(Vector_count((Vector*)vector) == 7);
  assert(Vector_capacity((Vector*)vector) == 7);
  assert(memcmp(test3and4, Vector_get_data((Vector*)vector), sizeof test3and4) == 0);

  Vector_free((Vector*)vector);
}

TEST(vector_insert) {
  IntVector *vector = IntVector_alloc();
  for (int i = 0; i <= 11; i++) {
    IntVector_add(vector, i);
  }

  // Test 1: Insert
  IntVector_insert(vector, 10, 99);

  int test1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 99, 10, 11};
  assert(Vector_count((Vector*)vector) == 13);
  assert(memcmp(test1, Vector_get_data((Vector*)vector), sizeof test1) == 0);

  // Test 2: Insert Range
  int test2_insert[] = {97, 98};
  IntVector_insert_range(vector, 6, test2_insert, 2);

  int test2[] = {0, 1, 2, 3, 4, 5, 97, 98, 6, 7, 8, 9, 99, 10, 11};
  assert(Vector_count((Vector*)vector) == 15);
  assert(memcmp(test2, Vector_get_data((Vector*)vector), sizeof test2) == 0);

  // Test 3: Add Range
  int test3_add[] = {100, 101, 199};
  IntVector_add_range(vector, test3_add, 3);

  int test3[] = {0, 1, 2, 3, 4, 5, 97, 98, 6, 7, 8, 9, 99, 10, 11, 100, 101, 199};
  assert(Vector_count((Vector*)vector) == 18);
  assert(memcmp(test3, Vector_get_data((Vector*)vector), sizeof test3) == 0);

  // Test 4: Expand
  Vector_expand((Vector*)vector, 27);

  int test4[] = {0, 1, 2, 3, 4, 5, 97, 98, 6, 7, 8, 9, 99, 10, 11, 100, 101, 199, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  assert(Vector_count((Vector*)vector) == 27);
  assert(memcmp(test3, Vector_get_data((Vector*)vector), sizeof test3) == 0);

  // Test 5: Clear
  Vector_clear((Vector*)vector);

  assert(Vector_count((Vector*)vector) == 0);
  assert(Vector_capacity((Vector*)vector) == 0);

  IntVector_add(vector, 3);
  assert(Vector_count((Vector*)vector) == 1);
  assert(Vector_capacity((Vector*)vector) == 1);
  assert(IntVector_get(vector, 0) == 3);

  Vector_free((Vector*)vector);
}

TEST(vector_update) {
  struct test t = {1, 2};
  IntVector *vector = IntVector_alloc();
  for (int i = 0; i <= 11; i++) {
    IntVector_add(vector, i);
  }

  // Test 1: Update
  IntVector_set(vector, 5, 99);

  int test1[] = {0, 1, 2, 3, 4, 99, 6, 7, 8, 9, 10, 11};
  assert(Vector_count((Vector*)vector) == 12);
  assert(IntVector_get(vector, 5) == 99);
  assert(memcmp(test1, Vector_get_data((Vector*)vector), sizeof test1) == 0);

  // Test 1: Update range
  int test2_update[] = {97, 98, 91};
  IntVector_set_range(vector, 5, test2_update, 3);

  int test2[] = {0, 1, 2, 3, 4, 97, 98, 91, 8, 9, 10, 11};
  assert(Vector_count((Vector*)vector) == 12);
  assert(IntVector_get(vector, 5) == 97);
  assert(memcmp(test2, Vector_get_data((Vector*)vector), sizeof test2) == 0);

  Vector_free((Vector*)vector);
}

TEST(vector_copy) {
  struct test t = {1, 2};
  IntVector *vector = IntVector_alloc();
  for (int i = 0; i <= 11; i++) {
    IntVector_add(vector, i);
  }

  Vector *vector2 = Vector_alloc(sizeof(int));

  Vector_copy(vector2, (const Vector*)vector);

  int test1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
  assert(Vector_count(vector2) == 12);
  assert(memcmp(test1, Vector_get_data(vector2), sizeof test1) == 0);

  Vector_free(vector2);
  Vector_free((Vector*)vector);
}

int vector_tests(void) {
  return test_vector_basics() || test_vector_delete() || test_vector_insert() ||
         test_vector_update() || test_vector_copy();
}

void test_string_foreach(char **item) { /*...*/ }

void test_int_foreach(int *item) { /*...*/ }
