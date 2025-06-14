#include "ctest.h"
#include <stdlib.h>

#define MAX_TESTS 1024

static struct {
    const char *name;
    ctest_func func;
} ctest_cases[MAX_TESTS];
static int ctest_count = 0;
int ctest_current_failed = 0;

void ctest_register(const char *name, ctest_func func) {
    if (ctest_count < MAX_TESTS) {
        ctest_cases[ctest_count].name = name;
        ctest_cases[ctest_count].func = func;
        ++ctest_count;
    }
}

int ctest_run_all(void) {
    int fails = 0;
    for (int i = 0; i < ctest_count; ++i) {
        printf("[ RUN      ] %s\n", ctest_cases[i].name);
        ctest_current_failed = 0;
        ctest_cases[i].func();
        if (ctest_current_failed) {
            printf("[  FAILED  ] %s\n", ctest_cases[i].name);
            ++fails;
        } else {
            printf("[       OK ] %s\n", ctest_cases[i].name);
        }
    }
    printf("[==========] %d tests ran. %d failed.\n", ctest_count, fails);
    return fails ? 1 : 0;
}
