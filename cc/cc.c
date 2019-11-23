#include "cc.h"

#include <stdio.h>

#include "../test/stubs.h"

int main(int argc, char **argv) { 
    init_malloc_logging();
    printf("Josh's C Compiler\n");
    find_leaks();
}
