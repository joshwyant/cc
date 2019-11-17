#include "test.h"
#include "stubs.h"

#ifdef TESTING
int main(int argc, char **argv) {
  init_malloc_logging();

  int result = common_tests() || test_find_leaks();

  printf("All tests completed.\n");

  return result;
}
#endif