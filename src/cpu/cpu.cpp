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
        case 0x18:
            cycles = op_jr_s8(cpu, m);
            break;
        case 0x21:
            cycles = op_ld_hl_d16(cpu, m);
            break;
        case 0x3E:
            cycles = op_ld_a_d8(cpu, m);
            break;
        case 0x77:
            cycles = op_ld_hl_a(cpu, m);
            break;
        case 0xC3: 
            cycles = op_jp_a16(cpu, m);
            break;
        default:   
            fprintf(stderr,"Unknown opcode %x\n", opcode); 
            return -1;
    }

    cpu->cycles += cycles;   // timing table
    return 0;
}

