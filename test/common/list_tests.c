#include "list_tests.h"

TEST(list_basics)
{
    List *list = list_alloc(sizeof(struct test));

    assert(list_get_data(list) == NULL);
    assert(list_empty(list));
    assert(list_size(list) == 0);
    assert(list_capacity(list) == 0);
    assert(list_element_size(list) == sizeof(struct test));
}

int list_tests() {
    return test_list_basics();
}
