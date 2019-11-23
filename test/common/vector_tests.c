#include "vector_tests.h"

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

  Vector_remove_range(vector, 3, 2);
  assert(Vector_count(vector) == 3);

  Vector_free(vector);

  StringVector *svector = StringVector_alloc();
  StringIterator siter;
  StringVector_add(svector, "ABC");
  StringVector_add(svector, "DEF");
  StringVector_add(svector, "GHI");

  // TODO: wrongly accepts any kind of vector
  // Also check to make sure container element sizes are the same.
  StringVector_get_iterator(svector, &siter);
  String_for_each(&siter, test_string_foreach);

  IntVector *ivector = IntVector_alloc();
  IntIterator iiter;
  IntVector_add(ivector, 1);
  IntVector_add(ivector, 2);
  IntVector_add(ivector, 3);

  IntVector_get_iterator(ivector, &iiter);
  Int_for_each(&iiter, test_int_foreach);
}

int vector_tests(void) { return test_vector_basics(); }

void test_string_foreach(char **item) { printf("%s\n", *item); }

void test_int_foreach(int *item) { printf("%d\n", *item); }
