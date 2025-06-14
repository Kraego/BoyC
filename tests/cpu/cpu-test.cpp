#include <gtest/gtest.h>
#include "cpu.h"
#include "mem.h"
#include "rom.h"
#include "test-helper.h"

#define ROM_SIZE (0x8000) // 32KB

TEST(cpu_dump_test_sueccess, cpu_dump)
{
    cpu_t cpu = {};

    cpu_dump(&cpu);
    EXPECT_EQ(1, 1); // just check for no mem fault
}

TEST(cpu_reset_test_success, cpu_reset)
{
    cpu_t cpu = {};

    cpu_reset(&cpu);
    EXPECT_EQ(cpu.r.af, 0x01B0);
    EXPECT_EQ(cpu.r.bc, 0x0013);
    EXPECT_EQ(cpu.r.de, 0x00D8);
    EXPECT_EQ(cpu.r.hl, 0x014D);
    EXPECT_EQ(cpu.sp, 0xFFFE);
    EXPECT_EQ(cpu.pc, 0x0100);
}

TEST(cpu_step_one_simple_step_success, cpu_step)
{
    uint8_t rom_image[ROM_SIZE];
    char rom_path[256];
    cpu_t cpu = {};
    int ret = 0;

    cpu_reset(&cpu);
    snprintf(rom_path, sizeof(rom_path), "%s/gbmicrotest/cpu_bus_1.gb", TEST_ROM_DIR);
    ret = load_rom(rom_path, rom_image, ROM_SIZE);
    if (ret != 0)
    {
        EXPECT_EQ(ret, 0);
        return;
    }
    mem_t *mem = mem_create(rom_image, ROM_SIZE);

    ret = cpu_step(&cpu, mem);
    if (ret != 0)
    {
        EXPECT_EQ(ret, 0);
        return;
    }
}

TEST(cpu_step_write_x8000_success, cpu_step)
{
    uint8_t rom_image[ROM_SIZE];
    char rom_path[256];
    cpu_t cpu = {};
    int ret = 0;

    cpu_reset(&cpu);
    snprintf(rom_path, sizeof(rom_path), "%s/gbmicrotest/000-write_to_x8000.gb", TEST_ROM_DIR);
    ret = load_rom(rom_path, rom_image, ROM_SIZE);
    if (ret != 0)
    {
        EXPECT_EQ(ret, 0);
        return;
    }
    mem_t *mem = mem_create(rom_image, ROM_SIZE);

    for (int i = 0; i < 100; i++)
    {
        ret = cpu_step(&cpu, mem);
        if (ret != 0)
        {
            EXPECT_EQ(ret, 0);
            return;
        }
    }
}

TEST(cpu_step_oam_lock_success, cpu_step)
{
    uint8_t rom_image[ROM_SIZE];
    char rom_path[256];
    cpu_t cpu = {};
    int ret = 0;

    cpu_reset(&cpu);
    snprintf(rom_path, sizeof(rom_path), "%s/gbmicrotest/000-oam_lock.gb", TEST_ROM_DIR);
    ret = load_rom(rom_path, rom_image, ROM_SIZE);
    if (ret != 0)
    {
        EXPECT_EQ(ret, 0);
        return;
    }
    mem_t *mem = mem_create(rom_image, ROM_SIZE);

    for (int i = 0; i < 100; i++)
    {
        ret = cpu_step(&cpu, mem);
        if (ret != 0)
        {
            EXPECT_EQ(ret, 0);
            return;
        }
    }
}

TEST(cpu_step_vram_unlocked_success, cpu_step)
{
    uint8_t rom_image[ROM_SIZE];
    char rom_path[256];
    cpu_t cpu = {};
    int ret = 0;

    cpu_reset(&cpu);
    snprintf(rom_path, sizeof(rom_path), "%s/gbmicrotest/001-vram_unlocked.gb", TEST_ROM_DIR);
    ret = load_rom(rom_path, rom_image, ROM_SIZE);
    if (ret != 0)
    {
        EXPECT_EQ(ret, 0);
        return;
    }
    mem_t *mem = mem_create(rom_image, ROM_SIZE);

    for (int i = 0; i < 100; i++)
    {
        ret = cpu_step(&cpu, mem);
        if (ret != 0)
        {
            EXPECT_EQ(ret, 0);
            return;
        }
    }
}

TEST(cpu_step_ld_d_opcodes, cpu_step)
{
    uint8_t rom_image[ROM_SIZE] = {};
    cpu_t cpu = {};

    cpu_reset(&cpu);
    rom_image[cpu.pc] = 0x16;   // LD D, d8
    rom_image[cpu.pc + 1] = 0x10;
    rom_image[cpu.pc + 2] = 0x14; // INC D
    rom_image[cpu.pc + 3] = 0x15; // DEC D

    mem_t *mem = mem_create(rom_image, ROM_SIZE);

    EXPECT_EQ(cpu_step(&cpu, mem), 0);
    EXPECT_EQ(cpu.r.d, 0x10);

    EXPECT_EQ(cpu_step(&cpu, mem), 0);
    EXPECT_EQ(cpu.r.d, 0x11);

    EXPECT_EQ(cpu_step(&cpu, mem), 0);
    EXPECT_EQ(cpu.r.d, 0x10);
}

TEST(cpu_step_ld_a_hl, cpu_step)
{
    uint8_t rom_image[ROM_SIZE] = {};
    cpu_t cpu = {};

    cpu_reset(&cpu);
    rom_image[cpu.pc] = 0x7E; // LD A, (HL)

    mem_t *mem = mem_create(rom_image, ROM_SIZE);
    cpu.r.hl = 0xC000;
    mem_write_byte(mem, 0xC000, 0x55);

    EXPECT_EQ(cpu_step(&cpu, mem), 0);
    EXPECT_EQ(cpu.r.a, 0x55);
}
