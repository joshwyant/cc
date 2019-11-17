#include "list_tests.h"

TEST(list_basics) {
  struct test t = {1, 2};

  List *list = list_alloc(sizeof(struct test));

  assert(list_get_data(list) == NULL);
  assert(list_empty(list));
  assert(list_size(list) == 0);
  assert(list_capacity(list) == 0);
  assert(list_element_size(list) == sizeof(struct test));

  list_append(list, &t);
  assert(!list_empty(list));
  assert(list_size(list) == 1);
  assert(list_capacity(list) == 1);
  struct test *item = list_get(list, 0);
  assert(item->foo == 1);

  t.foo = 3;
  list_append(list, &t);
  assert(list_size(list) == 2);
  assert(list_capacity(list) == 2);
  item = list_get(list, 1);
  t.foo = 4;
  t.bar = 5;
  assert(item->foo == 3);
  assert(item->bar == 2);

  list_append(list, &t);
  list_append(list, &t);
  list_append(list, &t);
  assert(list_size(list) == 5);
  assert(list_capacity(list) == 8);

  list_remove_range(list, 3, 2);
  assert(list_size(list) == 3);

  list_free(list);
}

int list_tests() { return test_list_basics(); }
