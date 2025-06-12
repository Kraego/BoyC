#include <gtest/gtest.h>
#include "cpu.h"
#include "mem.h"
#include "rom.h"

#define ROM_SIZE 0x8000 // 32KB

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

TEST(cpu_step_one_step_success, cpu_step) {
    cpu_t cpu = {};
    int ret = 0;
    
    cpu_reset(&cpu);

    uint8_t rom_image[ROM_SIZE]; 

    ret = load_rom("./tests/test_roms/bin/cpu_bus_1.gb", rom_image, ROM_SIZE);
    EXPECT_EQ(ret, 0);

    mem_t *mem = mem_create(rom_image, ROM_SIZE);
    ret = cpu_step(&cpu, mem);

    EXPECT_EQ(ret, -1);
}
