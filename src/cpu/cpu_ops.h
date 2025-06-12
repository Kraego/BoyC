#ifndef CPU_OPS_H
#define CPU_OPS_H

/**
 * Emulation of the gb cpu
 */
#ifdef __cplusplus
extern "C" {
#endif

#include "cpu.h"

#define OPS_COUNT   (256)

uint8_t instr_cycles[OPS_COUNT] = {
    1, // 0x00 NOP
    3, // 0x01 LD BC, d16
    2, // 0x02 LD (BC), A
    2, // 0x03 INC BC
    1, // 0x04 INC B
    1, // 0x05 DEC B
    2, // 0x06 LD B, d8
    1, // 0x07 RLCA
    5, // 0x08 LD (a16), SP
    2, // 0x09 ADD HL, BC
    2, // 0x0A LD A, (BC)
    2, // 0x0B DEC BC
    1, // 0x0C INC C
    1, // 0x0D DEC C
    2, // 0x0E LD C, d8
    1, // 0x0F RRCA
    1, // 0x10 STOP
    3, // 0x11 LD DE, d16
    2, // 0x12 LD (DE), A
    2, // 0x13 INC DE
    1, // 0x14 INC D
    1, // 0x15 DEC D
    2, // 0x16 LD D, d8
    1, // 0x17 RLA
    3, // 0x18 JR s8
    2, // 0x19 ADD HL, DE
    2, // 0x1A LD A, (DE)
    2, // 0x1B DEC DE
    1, // 0x1C INC E
    1, // 0x1D DEC E
    2, // 0x1E LD E, d8
    1, // 0x1F RRA
    3, // 0x20 JR NZ, s8
    3, // 0x21 LD HL, d16
    2, // 0x22 LD (HL+), A
    2, // 0x23 INC HL
    1, // 0x24 INC H
    1, // 0x25 DEC H
    2, // 0x26 LD H, d8
    1, // 0x27 DAA
    3, // 0x28 JR Z, s8
    2, // 0x29 ADD HL, HL
    2, // 0x2A LD A, (HL+)
    2, // 0x2B DEC HL
    1, // 0x2C INC L
    1, // 0x2D DEC L
    2, // 0x2E LD L, d8
    1, // 0x2F CPL
    3, // 0x30 JR NC, s8
    3, // 0x31 LD SP, d16
    2, // 0x32 LD (HL-), A
    2, // 0x33 INC SP
    3, // 0x34 INC (HL)
    3, // 0x35 DEC (HL)
    3, // 0x36 LD (HL), d8
    1, // 0x37 SCF
    3, // 0x38 JR C, s8
    2, // 0x39 ADD HL, SP
    2, // 0x3A LD A, (HL-)
    2, // 0x3B DEC SP
    1, // 0x3C INC A
    1, // 0x3D DEC A
    2, // 0x3E LD A, d8
    1, // 0x3F CCF
    1, // 0x40 LD B, B
    1, // 0x41 LD B, C
    1, // 0x42 LD B, D
    1, // 0x43 LD B, E
    1, // 0x44 LD B, H
    1, // 0x45 LD B, L
    2, // 0x46 LD B, (HL)
    1, // 0x47 LD B, A
    1, // 0x48 LD C, B
    1, // 0x49 LD C, C
    1, // 0x4A LD C, D
    1, // 0x4B LD C, E
    1, // 0x4C LD C, H
    1, // 0x4D LD C, L
    2, // 0x4E LD C, (HL)
    1, // 0x4F LD C, A
    1, // 0x50 LD D, B
    1, // 0x51 LD D, C
    1, // 0x52 LD D, D
    1, // 0x53 LD D, E
    1, // 0x54 LD D, H
    1, // 0x55 LD D, L
    2, // 0x56 LD D, (HL)
    1, // 0x57 LD D, A
    1, // 0x58 LD E, B
    1, // 0x59 LD E, C
    1, // 0x5A LD E, D
    1, // 0x5B LD E, E
    1, // 0x5C LD E, H
    1, // 0x5D LD E, L
    2, // 0x5E LD E, (HL)
    1, // 0x5F LD E, A
    1, // 0x60 LD H, B
    1, // 0x61 LD H, C
    1, // 0x62 LD H, D
    1, // 0x63 LD H, E
    1, // 0x64 LD H, H
    1, // 0x65 LD H, L
    2, // 0x66 LD H, (HL)
    1, // 0x67 LD H, A
    1, // 0x68 LD L, B
    1, // 0x69 LD L, C
    1, // 0x6A LD L, D
    1, // 0x6B LD L, E
    1, // 0x6C LD L, H
    1, // 0x6D LD L, L
    2, // 0x6E LD L, (HL)
    1, // 0x6F LD L, A
    2, // 0x70 LD (HL), B
    2, // 0x71 LD (HL), C
    2, // 0x72 LD (HL), D
    2, // 0x73 LD (HL), E
    2, // 0x74 LD (HL), H
    2, // 0x75 LD (HL), L
    1, // 0x76 HALT
    2, // 0x77 LD (HL), A
    1, // 0x78 LD A, B
    1, // 0x79 LD A, C
    1, // 0x7A LD A, D
    1, // 0x7B LD A, E
    1, // 0x7C LD A, H
    1, // 0x7D LD A, L
    2, // 0x7E LD A, (HL)
    1, // 0x7F LD A, A
    1, // 0x80 ADD A, B
    1, // 0x81 ADD A, C
    1, // 0x82 ADD A, D
    1, // 0x83 ADD A, E
    1, // 0x84 ADD A, H
    1, // 0x85 ADD A, L
    2, // 0x86 ADD A, (HL)
    1, // 0x87 ADD A, A
    1, // 0x88 ADC A, B
    1, // 0x89 ADC A, C
    1, // 0x8A ADC A, D
    1, // 0x8B ADC A, E
    1, // 0x8C ADC A, H
    1, // 0x8D ADC A, L
    2, // 0x8E ADC A, (HL)
    1, // 0x8F ADC A, A
    1, // 0x90 SUB B
    1, // 0x91 SUB C
    1, // 0x92 SUB D
    1, // 0x93 SUB E
    1, // 0x94 SUB H
    1, // 0x95 SUB L
    2, // 0x96 SUB (HL)
    1, // 0x97 SUB A
    1, // 0x98 SBC A, B
    1, // 0x99 SBC A, C
    1, // 0x9A SBC A, D
    1, // 0x9B SBC A, E
    1, // 0x9C SBC A, H
    1, // 0x9D SBC A, L
    2, // 0x9E SBC A, (HL)
    1, // 0x9F SBC A, A
    1, // 0xA0 AND B
    1, // 0xA1 AND C
    1, // 0xA2 AND D
    1, // 0xA3 AND E
    1, // 0xA4 AND H
    1, // 0xA5 AND L
    2, // 0xA6 AND (HL)
    1, // 0xA7 AND A
    1, // 0xA8 XOR B
    1, // 0xA9 XOR C
    1, // 0xAA XOR D
    1, // 0xAB XOR E
    1, // 0xAC XOR H
    1, // 0xAD XOR L
    2, // 0xAE XOR (HL)
    1, // 0xAF XOR A
    1, // 0xB0 OR B
    1, // 0xB1 OR C
    1, // 0xB2 OR D
    1, // 0xB3 OR E
    1, // 0xB4 OR H
    1, // 0xB5 OR L
    2, // 0xB6 OR (HL)
    1, // 0xB7 OR A
    1, // 0xB8 CP B
    1, // 0xB9 CP C
    1, // 0xBA CP D
    1, // 0xBB CP E
    1, // 0xBC CP H
    1, // 0xBD CP L
    2, // 0xBE CP (HL)
    1, // 0xBF CP A
    5, // 0xC0 RET NZ
    3, // 0xC1 POP BC
    4, // 0xC2 JP NZ, a16
    4, // 0xC3 JP a16
    6, // 0xC4 CALL NZ, a16
    4, // 0xC5 PUSH BC
    2, // 0xC6 ADD A, d8
    4, // 0xC7 RST 0
    5, // 0xC8 RET Z
    4, // 0xC9 RET
    4, // 0xCA JP Z, a16
    0, // 0xCB 
    6, // 0xCC CALL Z, a16
    6, // 0xCD CALL a16
    2, // 0xCE ADC A, d8
    4, // 0xCF RST 1
    5, // 0xD0 RET NC
    3, // 0xD1 POP DE
    4, // 0xD2 JP NC, a16
    0, // 0xD3 
    6, // 0xD4 CALL NC, a16
    4, // 0xD5 PUSH DE
    2, // 0xD6 SUB d8
    4, // 0xD7 RST 2
    5, // 0xD8 RET C
    4, // 0xD9 RETI
    4, // 0xDA JP C, a16
    0, // 0xDB 
    6, // 0xDC CALL C, a16
    0, // 0xDD 
    2, // 0xDE SBC A, d8
    4, // 0xDF RST 3
    3, // 0xE0 LD (a8), A
    3, // 0xE1 POP HL
    2, // 0xE2 LD (C), A
    0, // 0xE3 
    0, // 0xE4 
    4, // 0xE5 PUSH HL
    2, // 0xE6 AND d8
    4, // 0xE7 RST 4
    4, // 0xE8 ADD SP, s8
    1, // 0xE9 JP HL
    4, // 0xEA LD (a16), A
    0, // 0xEB 
    0, // 0xEC 
    0, // 0xED 
    2, // 0xEE XOR d8
    4, // 0xEF RST 5
    3, // 0xF0 LD A, (a8)
    3, // 0xF1 POP AF
    2, // 0xF2 LD A, (C)
    1, // 0xF3 DI
    0, // 0xF4 
    4, // 0xF5 PUSH AF
    2, // 0xF6 OR d8
    4, // 0xF7 RST 6
    3, // 0xF8 LD HL, SP+s8
    2, // 0xF9 LD SP, HL
    4, // 0xFA LD A, (a16)
    1, // 0xFB EI
    0, // 0xFC 
    0, // 0xFD 
    2, // 0xFE CP d8
    4, // 0xFF RST 7
};

static inline void op_ld_a_b(cpu_t *c);

#ifdef __cplusplus
}
#endif

#endif  // CPU_OPS_H
