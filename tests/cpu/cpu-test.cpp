#include <gtest/gtest.h>
#include "cpu.h"
#include "mem.h"

TEST(cpu_dump_test_sueccess, cpu_dump) {
    cpu_t cpu = {};

    cpu_dump(&cpu);
    EXPECT_EQ(1,1); // just check for no mem fault
}

TEST(cpu_reset_test_success, cpu_reset) {
    cpu_t cpu = {};

    cpu_reset(&cpu);
    EXPECT_EQ(cpu.r.af, 0x01B0);
    EXPECT_EQ(cpu.r.bc, 0x0013);
    EXPECT_EQ(cpu.r.de, 0x00D8);
    EXPECT_EQ(cpu.r.hl, 0x014D);
    EXPECT_EQ(cpu.sp,   0xFFFE);
    EXPECT_EQ(cpu.pc,   0x0100);
}

TEST(cpu_step_corrupt_opcode_fails, cpu_step) {
    cpu_t cpu = {};
    cpu_reset(&cpu);

    uint8_t rom_image[1000]; // TODO: load broken ROM
    size_t rom_size = 1000; // TODO: estimate

    mem_t *mem = mem_create(rom_image, rom_size);

    int ret = cpu_step(&cpu, mem);

    EXPECT_EQ(ret, -1);
}
