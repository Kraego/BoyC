#include <gtest/gtest.h>
#include "cpu.h"

TEST(cpu_reset_test, cpu_reset) {
    cpu_t cpu_state = {};

    cpu_reset(&cpu_state);
    EXPECT_EQ(cpu_state.r.af, 0x01B0);
    EXPECT_EQ(cpu_state.r.bc, 0x0013);
    EXPECT_EQ(cpu_state.r.de, 0x00D8);
    EXPECT_EQ(cpu_state.r.hl, 0x014D);
    EXPECT_EQ(cpu_state.sp,   0xFFFE);
    EXPECT_EQ(cpu_state.pc,   0x0100);
}
