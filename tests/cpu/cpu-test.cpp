#include <gtest/gtest.h>
#include "../src/cpu/cpu.h"

TEST(cpu_reset_test, cpu_reset) {
    cpu_t cpu_state = {};

    cpu_reset(&cpu_state);
    // Add assertions here
    EXPECT_EQ(1, 1);
}
