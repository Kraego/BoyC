#include "ctest.h"

int main(int argc, char **argv) {
    if (argc > 1) {
        return ctest_run_by_name(argv[1]);
    }
    return ctest_run_all();
}
