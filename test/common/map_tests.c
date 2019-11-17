#include "map_tests.h"

TEST(map_basics)
{
    struct test t = { 1, 2 };
    Map m;
    int one = 1, two = 2, three = 3;
    char *a = "a", *b = "b", *c = "c";
    map_init(&m, &StringKeyInfo, sizeof(int));

    map_add(&m, &a, &one);
    map_add(&m, &b, &two);
    map_add(&m, &c, &three);

    assert(map_size(&m) == 3);

    int val1, val2, val3;
    assert(map_get(&m, &a, &val1));
    assert(map_get(&m, &b, &val2));
    assert(map_get(&m, &c, &val3));

    assert(val1 == 1);
    assert(val2 == 2);
    assert(val3 == 3);
}

int map_tests() {
    return test_map_basics();
}
