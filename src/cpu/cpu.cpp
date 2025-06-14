#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include "cpu.h"
#include "cpu_ops.h"

/*
* For debugging purpose
*/
void cpu_dump(const cpu_t *cpu)
{
    printf("AF:%04X BC:%04X DE:%04X HL:%04X  PC:%04X SP:%04X  F:%c%c%c%c\n",
           cpu->r.af, cpu->r.bc, cpu->r.de, cpu->r.hl,
           cpu->pc, cpu->sp,
           cpu_get_flag(&cpu->r,F_Z)?'Z':'-',
           cpu_get_flag(&cpu->r,F_N)?'N':'-',
           cpu_get_flag(&cpu->r,F_H)?'H':'-',
           cpu_get_flag(&cpu->r,F_C)?'C':'-');
}

void cpu_reset(cpu_t *cpu)
{
    memset(cpu, 0, sizeof(*cpu));

    /* The DMG boots with these exact values: */
    cpu->r.af = 0x01B0;
    cpu->r.bc = 0x0013;
    cpu->r.de = 0x00D8;
    cpu->r.hl = 0x014D;
    cpu->sp   = 0xFFFE;
    cpu->pc   = 0x0100;          /* first cartridge byte after the header */
}

int8_t cpu_step(cpu_t *cpu, mem_t *m)
{
    uint8_t cycles = 1;
    uint8_t opcode = mem_read_byte(m, cpu->pc);

    switch (opcode)
    {
        case 0x00:
            cycles = nop(cpu);
            break;
        case 0x01:
            cycles = op_ld_bc_d16(cpu, m);
            break;
        case 0x02:
            cycles = op_ld_bc_a(cpu, m);
            break;
        case 0x03:
            cycles = op_inc_bc(cpu);
            break;
        case 0x04:
            cycles = op_inc_b(cpu);
            break;
        case 0x05:
            cycles = op_dec_b(cpu);
            break;
        case 0x06:
            cycles = op_ld_b_d8(cpu, m);
            break;
        case 0x08:
            cycles = op_ld_a16_sp(cpu, m);
            break;
        case 0x09:
            cycles = op_add_hl_bc(cpu);
            break;
        case 0x0A:
            cycles = op_ld_a_bc(cpu, m);
            break;
        case 0x0B:
            cycles = op_dec_bc(cpu);
            break;
        case 0x0C:
            cycles = op_inc_c(cpu);
            break;
        case 0x0D:
            cycles = op_dec_c(cpu);
            break;
        case 0x0E:
            cycles = op_ld_c_d8(cpu, m);
            break;
        case 0x0F:
            cycles = op_rrca(cpu);
            break;
        case 0x07:
            cycles = op_rlca(cpu);
            break;
        case 0x11:
            cycles = op_ld_de_d16(cpu, m);
            break;
        case 0x12:
            cycles = op_ld_de_a(cpu, m);
            break;
        case 0x13:
            cycles = op_inc_de(cpu);
            break;
        case 0x19:
            cycles = op_add_hl_de(cpu);
            break;
        case 0x1A:
            cycles = op_ld_a_de(cpu, m);
            break;
        case 0x1B:
            cycles = op_dec_de(cpu);
            break;
        case 0x18:
            cycles = op_jr_s8(cpu, m);
            break;
        case 0x20:
            cycles = op_jr_nz_s8(cpu, m);
            break;
        case 0x21:
            cycles = op_ld_hl_d16(cpu, m);
            break;
        case 0x28:
            cycles = op_jp_z_s8(cpu, m);
            break;
        case 0x2E:
            cycles = op_ld_l_d8(cpu, m);
            break;
        case 0x40:
            cycles = op_ld_b_b(cpu, m);
            break;
        case 0x41:
            cycles = op_ld_b_c(cpu, m);
            break;
        case 0x3E:
            cycles = op_ld_a_d8(cpu, m);
            break;
        case 0x77:
            cycles = op_ld_hl_a(cpu, m);
            break;
        case 0x78:
            cycles = op_ld_a_b(cpu);
            break;
        case 0xC2:
            cycles = op_jp_z_a16(cpu, m);
            break;
        case 0xC3: 
            cycles = op_jp_a16(cpu, m);
            break;
        case 0xF0:
            cycles = op_ld_a_a8(cpu, m);
            break;
        case 0xF8:
            cycles = op_ld_hl_sp_r8(cpu, m);
            break;
        case 0xFA:
            cycles = op_ld_a_a16(cpu, m);
            break;
        case 0xFE:
            cycles = op_cp_d8(cpu, m);
            break;
        case 0xE6:
            cycles = op_and_d8(cpu, m);
            break;
        case 0xEA:
            cycles = op_ld_a16_a(cpu, m);
            break;
        default:   
            fprintf(stderr,"Unknown opcode %x\n", opcode); 
            return -1;
    }

    cpu->cycles += cycles;   // timing table
    return 0;
}

