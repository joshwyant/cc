#include "list_tests.h"

void test_string_foreach(char **item);

void test_int_foreach(int *item);

TEST(list_basics) {
  struct test t = {1, 2};

  List *list = List_alloc(sizeof(struct test));

  assert(List_get_data(list) == NULL);
  assert(List_empty(list));
  assert(List_count(list) == 0);
  assert(List_capacity(list) == 0);
  assert(List_element_size(list) == sizeof(struct test));

  List_add(list, &t);
  assert(!List_empty(list));
  assert(List_count(list) == 1);
  assert(List_capacity(list) == 1);
  struct test *item = List_get(list, 0);
  assert(item->foo == 1);

  t.foo = 3;
  List_add(list, &t);
  assert(List_count(list) == 2);
  assert(List_capacity(list) == 2);
  item = List_get(list, 1);
  t.foo = 4;
  t.bar = 5;
  assert(item->foo == 3);
  assert(item->bar == 2);

  List_add(list, &t);
  List_add(list, &t);
  List_add(list, &t);
  assert(List_count(list) == 5);
  assert(List_capacity(list) == 8);

  List_remove_range(list, 3, 2);
  assert(List_count(list) == 3);

  List_free(list);

  StringList *slist = StringList_alloc();
  StringIterator siter;
  StringList_add(slist, "ABC");
  StringList_add(slist, "DEF");
  StringList_add(slist, "GHI");

  // TODO: wrongly accepts any kind of list
  // Also check to make sure container element sizes are the same.
  StringList_get_iterator(slist, &siter);
  String_for_each(&siter, test_string_foreach);

  IntList *ilist = IntList_alloc();
  IntIterator iiter;
  IntList_add(ilist, 1);
  IntList_add(ilist, 2);
  IntList_add(ilist, 3);

  IntList_get_iterator(ilist, &iiter);
  Int_for_each(&iiter, test_int_foreach);
}

int list_tests() { return test_list_basics(); }

void test_string_foreach(char **item) { printf("%s\n", *item); }

void test_int_foreach(int *item) { printf("%d\n", *item); }