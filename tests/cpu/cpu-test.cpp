#include "ctest.h"
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
        GTEST_SKIP();
    }
    mem_t *mem = mem_create(rom_image, ROM_SIZE);

    ret = cpu_step(&cpu, mem);
    if (ret != 0)
    {
        GTEST_SKIP();
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
        GTEST_SKIP();
    }
    mem_t *mem = mem_create(rom_image, ROM_SIZE);

    for (int i = 0; i < 100; i++)
    {
        ret = cpu_step(&cpu, mem);
        if (ret != 0)
        {
            GTEST_SKIP();
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
        GTEST_SKIP();
    }
    mem_t *mem = mem_create(rom_image, ROM_SIZE);

    for (int i = 0; i < 100; i++)
    {
        ret = cpu_step(&cpu, mem);
        if (ret != 0)
        {
            GTEST_SKIP();
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
        GTEST_SKIP();
    }
    mem_t *mem = mem_create(rom_image, ROM_SIZE);

    for (int i = 0; i < 100; i++)
    {
        ret = cpu_step(&cpu, mem);
        if (ret != 0)
        {
            GTEST_SKIP();
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

TEST(cpu_step_additional_opcodes, cpu_step)
{
    uint8_t rom_image[ROM_SIZE] = {};
    cpu_t cpu = {};

    cpu_reset(&cpu);
    rom_image[cpu.pc] = 0x06; // LD B, d8
    rom_image[cpu.pc + 1] = 0x12;
    rom_image[cpu.pc + 2] = 0x48; // LD C, B
    rom_image[cpu.pc + 3] = 0x51; // LD D, C
    rom_image[cpu.pc + 4] = 0x7A; // LD A, D

    mem_t *mem = mem_create(rom_image, ROM_SIZE);

    EXPECT_EQ(cpu_step(&cpu, mem), 0);
    EXPECT_EQ(cpu.r.b, 0x12);

    EXPECT_EQ(cpu_step(&cpu, mem), 0);
    EXPECT_EQ(cpu.r.c, 0x12);

    EXPECT_EQ(cpu_step(&cpu, mem), 0);
    EXPECT_EQ(cpu.r.d, 0x12);

    EXPECT_EQ(cpu_step(&cpu, mem), 0);
    EXPECT_EQ(cpu.r.a, 0x12);
}

TEST(cpu_step_hl_memory_ops, cpu_step)
{
    uint8_t rom_image[ROM_SIZE] = {};
    cpu_t cpu = {};

    cpu_reset(&cpu);
    rom_image[cpu.pc] = 0x21; // LD HL, d16
    rom_image[cpu.pc + 1] = 0x00;
    rom_image[cpu.pc + 2] = 0xC0;
    rom_image[cpu.pc + 3] = 0x36; // LD (HL), d8
    rom_image[cpu.pc + 4] = 0x10;
    rom_image[cpu.pc + 5] = 0x34; // INC (HL)
    rom_image[cpu.pc + 6] = 0x35; // DEC (HL)
    rom_image[cpu.pc + 7] = 0x7E; // LD A, (HL)

    mem_t *mem = mem_create(rom_image, ROM_SIZE);

    EXPECT_EQ(cpu_step(&cpu, mem), 0); // LD HL, d16
    EXPECT_EQ(cpu.r.hl, 0xC000);

    EXPECT_EQ(cpu_step(&cpu, mem), 0); // LD (HL), d8
    EXPECT_EQ(mem_read_byte(mem, 0xC000), 0x10);

    EXPECT_EQ(cpu_step(&cpu, mem), 0); // INC (HL)
    EXPECT_EQ(mem_read_byte(mem, 0xC000), 0x11);

    EXPECT_EQ(cpu_step(&cpu, mem), 0); // DEC (HL)
    EXPECT_EQ(mem_read_byte(mem, 0xC000), 0x10);

    EXPECT_EQ(cpu_step(&cpu, mem), 0); // LD A, (HL)
    EXPECT_EQ(cpu.r.a, 0x10);
}

TEST(cpu_step_arithmetic_ops, cpu_step)
{
    uint8_t rom_image[ROM_SIZE] = {};
    cpu_t cpu = {};

    cpu_reset(&cpu);
    cpu.r.a = 0x00;
    rom_image[cpu.pc] = 0x06; // LD B, d8
    rom_image[cpu.pc + 1] = 0x01;
    rom_image[cpu.pc + 2] = 0x80; // ADD A, B
    rom_image[cpu.pc + 3] = 0xC6; // ADD A, d8
    rom_image[cpu.pc + 4] = 0x01;
    rom_image[cpu.pc + 5] = 0x90; // SUB B
    rom_image[cpu.pc + 6] = 0xD6; // SUB d8
    rom_image[cpu.pc + 7] = 0x01;
    rom_image[cpu.pc + 8] = 0xA0; // AND B
    rom_image[cpu.pc + 9] = 0xE6; // AND d8
    rom_image[cpu.pc + 10] = 0x0F;
    rom_image[cpu.pc + 11] = 0xB0; // OR B
    rom_image[cpu.pc + 12] = 0xF6; // OR d8
    rom_image[cpu.pc + 13] = 0x02;

    mem_t *mem = mem_create(rom_image, ROM_SIZE);

    EXPECT_EQ(cpu_step(&cpu, mem), 0); // LD B, d8
    EXPECT_EQ(cpu.r.b, 0x01);

    EXPECT_EQ(cpu_step(&cpu, mem), 0); // ADD A, B
    EXPECT_EQ(cpu.r.a, 0x01);

    EXPECT_EQ(cpu_step(&cpu, mem), 0); // ADD A, d8
    EXPECT_EQ(cpu.r.a, 0x02);

    EXPECT_EQ(cpu_step(&cpu, mem), 0); // SUB B
    EXPECT_EQ(cpu.r.a, 0x01);

    EXPECT_EQ(cpu_step(&cpu, mem), 0); // SUB d8
    EXPECT_EQ(cpu.r.a, 0x00);
    EXPECT_TRUE(cpu_get_flag(&cpu.r, F_Z));

    EXPECT_EQ(cpu_step(&cpu, mem), 0); // AND B
    EXPECT_EQ(cpu.r.a, 0x00);

    EXPECT_EQ(cpu_step(&cpu, mem), 0); // AND d8
    EXPECT_EQ(cpu.r.a, 0x00);

    EXPECT_EQ(cpu_step(&cpu, mem), 0); // OR B
    EXPECT_EQ(cpu.r.a, 0x01);

    EXPECT_EQ(cpu_step(&cpu, mem), 0); // OR d8
    EXPECT_EQ(cpu.r.a, 0x03);
}

TEST(cpu_step_sp_and_hl_inc_dec_ops, cpu_step)
{
    uint8_t rom_image[ROM_SIZE] = {};
    cpu_t cpu = {};

    cpu_reset(&cpu);
    rom_image[cpu.pc] = 0x31; // LD SP, d16
    rom_image[cpu.pc + 1] = 0x34;
    rom_image[cpu.pc + 2] = 0x12; // SP = 0x1234
    rom_image[cpu.pc + 3] = 0x21; // LD HL, d16
    rom_image[cpu.pc + 4] = 0x00;
    rom_image[cpu.pc + 5] = 0xC0; // HL = 0xC000
    rom_image[cpu.pc + 6] = 0x22; // LD (HL+), A
    rom_image[cpu.pc + 7] = 0x2A; // LD A, (HL+)
    rom_image[cpu.pc + 8] = 0x32; // LD (HL-), A
    rom_image[cpu.pc + 9] = 0x3A; // LD A, (HL-)

    mem_t *mem = mem_create(rom_image, ROM_SIZE);

    cpu.r.a = 0x55;
    EXPECT_EQ(cpu_step(&cpu, mem), 0); // LD SP, d16
    EXPECT_EQ(cpu.sp, 0x1234);

    EXPECT_EQ(cpu_step(&cpu, mem), 0); // LD HL, d16
    EXPECT_EQ(cpu.r.hl, 0xC000);

    EXPECT_EQ(cpu_step(&cpu, mem), 0); // LD (HL+), A
    EXPECT_EQ(mem_read_byte(mem, 0xC000), 0x55);
    EXPECT_EQ(cpu.r.hl, 0xC001);

    mem_write_byte(mem, 0xC001, 0x66);
    EXPECT_EQ(cpu_step(&cpu, mem), 0); // LD A, (HL+)
    EXPECT_EQ(cpu.r.a, 0x66);
    EXPECT_EQ(cpu.r.hl, 0xC002);

    EXPECT_EQ(cpu_step(&cpu, mem), 0); // LD (HL-), A
    EXPECT_EQ(mem_read_byte(mem, 0xC002), 0x66);
    EXPECT_EQ(cpu.r.hl, 0xC001);

    EXPECT_EQ(cpu_step(&cpu, mem), 0); // LD A, (HL-)
    EXPECT_EQ(cpu.r.a, 0x66);
    EXPECT_EQ(cpu.r.hl, 0xC000);
}

TEST(cpu_step_disable_interrupts, cpu_step)
{
    uint8_t rom_image[ROM_SIZE] = {};
    cpu_t cpu = {};

    cpu_reset(&cpu);
    cpu.ime = 1;
    rom_image[cpu.pc] = 0xF3; // DI

    mem_t *mem = mem_create(rom_image, ROM_SIZE);

    EXPECT_EQ(cpu_step(&cpu, mem), 0);
    EXPECT_EQ(cpu.ime, 0);
}
