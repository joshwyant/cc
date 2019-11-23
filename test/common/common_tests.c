#include "common_tests.h"

#include "../macros.h"

int common_tests(void) { return vector_tests() || map_tests(); }
