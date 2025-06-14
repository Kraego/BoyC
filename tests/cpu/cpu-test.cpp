#include <gtest/gtest.h>
#include "cpu.h"
#include "mem.h"
#include "rom.h"
#include "test-helper.h"


#define ROM_SIZE        (0x8000) // 32KB

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

TEST(cpu_step_one_simple_step_success, cpu_step) {
    uint8_t rom_image[ROM_SIZE]; 
    char rom_path[256];
    cpu_t cpu = {};
    int ret = 0;
    
    cpu_reset(&cpu);
    snprintf(rom_path, sizeof(rom_path), "%s/gbmicrotest/cpu_bus_1.gb", TEST_ROM_DIR);
    ret = load_rom(rom_path, rom_image, ROM_SIZE);
    EXPECT_EQ(ret, 0);
    mem_t *mem = mem_create(rom_image, ROM_SIZE);

    ret = cpu_step(&cpu, mem);
    EXPECT_EQ(ret, 0);
}

TEST(cpu_step_some_simple_steps_success, cpu_step) {
    uint8_t rom_image[ROM_SIZE]; 
    char rom_path[256];
    cpu_t cpu = {};
    int ret = 0;
    
    cpu_reset(&cpu);
    snprintf(rom_path, sizeof(rom_path), "%s/gbmicrotest/cpu_bus_1.gb", TEST_ROM_DIR);
    ret = load_rom(rom_path, rom_image, ROM_SIZE);
    EXPECT_EQ(ret, 0);
    mem_t *mem = mem_create(rom_image, ROM_SIZE);

    for (int i=0; i<100; i++){
        ret = cpu_step(&cpu, mem);
        EXPECT_EQ(ret, 0);
    }
    EXPECT_EQ(0x01, gb_microtest_check_result(mem));
}
