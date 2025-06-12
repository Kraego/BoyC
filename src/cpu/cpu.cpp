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

int cpu_step(cpu_t *cpu, mem_t *m)
{
    uint8_t opcode = mem_rb(m, cpu->pc++);   /* read byte, advance PC */

    switch (opcode)
    {
        case 0x00: /* NOP */                          break;
        // case 0x78: op_ld_a_b(c);                      break;
        // case 0x09: op_add_hl_bc(c);                   break;
        /* …hundreds more … */
        default:   fprintf(stderr,"Unknown opcode\n"); return -1;
    }

    cpu->cycles += instr_cycles[opcode];   /* timing table */
    return 0;
}

