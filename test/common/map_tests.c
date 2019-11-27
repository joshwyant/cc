#include "map_tests.h"

TEST(map_basics) {
  struct test t = {1, 2};
  Map *m;
  int one = 1, two = 2, three = 3;
  char *a = "a", *b = "b", *c = "c";
  m = Map_alloc(&CStringKeyInfo, sizeof(int));

  Map_add(m, &a, &one);
  Map_add(m, &b, &two);
  Map_add(m, &c, &three);

  assert(Map_count(m) == 3);

  int val1, val2, val3;
  assert(Map_get(m, &a, &val1));
  assert(Map_get(m, &b, &val2));
  assert(Map_get(m, &c, &val3));

  assert(val1 == 1);
  assert(val2 == 2);
  assert(val3 == 3);

  Map_free(m);
}

int map_tests(void) { return test_map_basics(); }
