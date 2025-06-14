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


// Functions behind opcodes

/* NOP */
static inline uint8_t nop(cpu_t *cpu){
    cpu->pc++;
    return 1;
}

/* LD BC, d16 (opcode 0x01) */
static inline uint8_t op_ld_bc_d16(cpu_t *cpu, mem_t *m){
    cpu->r.bc = mem_read_word(m, cpu->pc + 1);
    cpu->pc += 3;
    return 3;
}

/* LD A,B  (opcode 0x78) */
static inline uint8_t op_ld_a_b(cpu_t *cpu){ 
    cpu->r.a = cpu->r.b;
    cpu->pc++;
    return 1;
}

/* JP NZ, a16  (opcode 0xC2)*/
static inline uint8_t op_jp_z_a16(cpu_t *cpu, mem_t *m){     
    if (cpu_get_flag(&cpu->r,F_Z) == 0) {
        uint16_t a16 = mem_read_word(m, cpu->pc + 1);
        cpu->pc = a16;
        return 4;
    }
    cpu->pc += 3;
    return 3;
}

/* JR Z, s8  (opcode 0x28)*/
static inline uint8_t op_jp_z_s8(cpu_t *cpu, mem_t *m){  
    int8_t s8 = (int8_t) mem_read_byte(m, cpu->pc + 1);

    if (cpu_get_flag(&cpu->r,F_Z) == 1) {
        cpu->pc = (uint16_t)((int32_t)(cpu->pc + 2) + s8);
        return 3;
    }
    cpu->pc += 2;
    return 2;
}

/* JP a16  (opcode 0xC3)*/
static inline uint8_t op_jp_a16(cpu_t *cpu, mem_t *m){  
    uint16_t a16 = mem_read_word(m, cpu->pc + 1);
    cpu->pc = a16;
    return 4;  
}

/* RLCA (opcode 0x07)*/
static inline uint8_t op_rlca(cpu_t *cpu){
    uint8_t a = cpu->r.a;
    uint8_t carry = (a >> 7) & 0x01;

    cpu->r.a = (a << 1) | carry;

    cpu_set_flag(&cpu->r, F_Z, 0); // RLCA always resets Z
    cpu_set_flag(&cpu->r, F_N, 0);
    cpu_set_flag(&cpu->r, F_H, 0);
    cpu_set_flag(&cpu->r, F_C, carry);
    cpu->pc++;
    return 1;  
}

/* RRCA (opcode 0x0F) */
static inline uint8_t op_rrca(cpu_t *cpu){
    uint8_t a = cpu->r.a;
    uint8_t carry = a & 0x01;

    cpu->r.a = (a >> 1) | (carry << 7);

    cpu_set_flag(&cpu->r, F_Z, 0); // RRCA always resets Z
    cpu_set_flag(&cpu->r, F_N, 0);
    cpu_set_flag(&cpu->r, F_H, 0);
    cpu_set_flag(&cpu->r, F_C, carry);
    cpu->pc++;
    return 1;
}

/* INC BC (opcode 0x03)*/
static inline uint8_t op_inc_bc(cpu_t *cpu){  
    cpu->r.bc += 1;
    cpu->pc++;
    return 2;  
}

/* LD HL, d16  (opcode 0x21)*/
static inline uint8_t op_ld_hl_d16(cpu_t *cpu, mem_t *m){  
    uint16_t d16 = mem_read_word(m, cpu->pc + 1);
    cpu->r.hl = d16;
    cpu->pc += 3;
    return 3;  
}

/* LD A, d8  (opcode 0x3E)*/
static inline uint8_t op_ld_a_d8(cpu_t *cpu, mem_t *m){  
    uint8_t d8 = mem_read_byte(m, cpu->pc + 1);
    cpu->r.a = d8;
    cpu->pc += 2;
    return 2;  
}

/* LD (HL), A  (opcode 0x77)*/
static inline uint8_t op_ld_hl_a(cpu_t *cpu, mem_t *m){
    mem_write_byte(m, cpu->r.hl, cpu->r.a);
    cpu->pc++;
    return 1;
}

/* JR s8  (opcode 0x18)*/
static inline uint8_t op_jr_s8(cpu_t *cpu, mem_t *m) {
    int8_t offset = (int8_t)mem_read_byte(m, cpu->pc + 1);
    cpu->pc = (uint16_t)((int32_t)(cpu->pc + 2) + offset);
    return 3;
}

/* JR NZ, s8 (opcode 0x20)*/
static inline uint8_t op_jr_nz_s8(cpu_t *cpu, mem_t *m) {
    if (cpu_get_flag(&cpu->r,F_Z) == 0) {
        int8_t s8 = (int8_t)mem_read_byte(m, cpu->pc + 1);
        cpu->pc = (uint16_t)((int32_t)(cpu->pc + 2) + s8);        
        return 3;
    }

    cpu->pc += 2;
    return 2;
}

/* LD (a16), SP (opcode 0x08) */
static inline uint8_t op_ld_a16_sp(cpu_t *cpu, mem_t *m){
    uint16_t addr = mem_read_word(m, cpu->pc + 1);
    mem_write_word(m, addr, cpu->sp);
    cpu->pc += 3;
    return 5;
}

/* LD (a16), A  (opcode 0xEA)*/
static inline uint8_t op_ld_a16_a(cpu_t *cpu, mem_t *m){
    uint16_t target = mem_read_word(m, cpu->pc + 1);
    mem_write_byte(m, target, cpu->r.a);
    cpu->pc += 3;
    return 4;
}

/* LD A, (a16) (opcode 0xFA)*/
static inline uint8_t op_ld_a_a16(cpu_t *cpu, mem_t *m){  
    uint16_t a16 = mem_read_word(m, cpu->pc + 1);
    cpu->r.a = mem_read_byte(m, a16);
    cpu->pc += 3;
    return 4;  
}

/* LD A, (a8) (opcode 0xF0)*/
static inline uint8_t op_ld_a_a8(cpu_t *cpu, mem_t *m){  
    uint8_t lower = mem_read_byte(m, cpu->pc + 1);
    uint16_t target = 0xFF00 | lower;
    cpu->r.a = mem_read_byte(m, target);
    cpu->pc += 2;
    return 3;  
}

/* CP d8 (opcode 0xFE)*/
static inline uint8_t op_cp_d8(cpu_t *cpu, mem_t *m){  
    uint8_t d8 = mem_read_byte(m, cpu->pc + 1);
    uint8_t diff = cpu->r.a - d8;

    cpu_set_flag(&cpu->r,F_Z, diff == 0);
    cpu_set_flag(&cpu->r, F_N, 1);
    cpu_set_flag(&cpu->r, F_H, (cpu->r.a & 0x0F) < (d8 & 0x0F));
    cpu_set_flag(&cpu->r, F_C, cpu->r.a < d8);
    cpu->pc += 2;
    return 2;  
}

/* AND d8 (opcode 0xE6)*/
static inline uint8_t op_and_d8(cpu_t *cpu, mem_t *m){  
    uint8_t d8 = mem_read_byte(m, cpu->pc + 1);

    cpu->r.a &= d8;
    cpu_set_flag(&cpu->r,F_Z, cpu->r.a == 0);
    cpu_set_flag(&cpu->r,F_N, 0);
    cpu_set_flag(&cpu->r,F_H, 1);
    cpu_set_flag(&cpu->r,F_C, 0);

    cpu->pc += 2;
    return 3;  
}

/* LD HL, SP+s8 (opcode 0xF8)*/
static inline uint8_t op_ld_hl_sp_r8(cpu_t *cpu, mem_t *m) {
    int8_t offset = (int8_t)mem_read_byte(m, cpu->pc + 1);
    uint16_t sp = cpu->sp;
    uint16_t result = (uint16_t)((int32_t)sp + offset);

    cpu->r.hl = result;

    cpu_set_flag(&cpu->r, F_Z, 0);
    cpu_set_flag(&cpu->r, F_N, 0);
    cpu_set_flag(&cpu->r, F_H, ((sp & 0x0F) + (offset & 0x0F)) > 0x0F);
    cpu_set_flag(&cpu->r, F_C, ((sp & 0xFF) + (offset & 0xFF)) > 0xFF);

    cpu->pc += 2;
    return 3;
}

/* LD B, C (opcode 0x41)*/
static inline uint8_t op_ld_b_c(cpu_t *cpu, mem_t *m) {
    cpu->r.b = cpu->r.c;
    cpu->pc++;
    return 1;
}

/* LD B, D (opcode 0x42) */
static inline uint8_t op_ld_b_d(cpu_t *cpu) {
    cpu->r.b = cpu->r.d;
    cpu->pc++;
    return 1;
}

/* LD B, E (opcode 0x43) */
static inline uint8_t op_ld_b_e(cpu_t *cpu) {
    cpu->r.b = cpu->r.e;
    cpu->pc++;
    return 1;
}

/* LD B, H (opcode 0x44) */
static inline uint8_t op_ld_b_h(cpu_t *cpu) {
    cpu->r.b = cpu->r.h;
    cpu->pc++;
    return 1;
}

/* LD B, L (opcode 0x45) */
static inline uint8_t op_ld_b_l(cpu_t *cpu) {
    cpu->r.b = cpu->r.l;
    cpu->pc++;
    return 1;
}

/* LD B, (HL) (opcode 0x46) */
static inline uint8_t op_ld_b_hl(cpu_t *cpu, mem_t *m) {
    cpu->r.b = mem_read_byte(m, cpu->r.hl);
    cpu->pc++;
    return 2;
}

/* LD B, A (opcode 0x47) */
static inline uint8_t op_ld_b_a(cpu_t *cpu) {
    cpu->r.b = cpu->r.a;
    cpu->pc++;
    return 1;
}

/* LD C, A (opcode 0x4F) */
static inline uint8_t op_ld_c_a(cpu_t *cpu) {
    cpu->r.c = cpu->r.a;
    cpu->pc++;
    return 1;
}

/* LD B, B (opcode 0x40)*/
static inline uint8_t op_ld_b_b(cpu_t *cpu, mem_t *m) {
    cpu->r.b = cpu->r.b;
    cpu->pc++;
    return 1;
}

/* LD L, d8 (opcode 0x2E)*/
static inline uint8_t op_ld_l_d8(cpu_t *cpu, mem_t *m) {
    uint8_t d8 = mem_read_byte(m, cpu->pc + 1);

    cpu->r.l = d8;
    cpu->pc += 2;
    return 2;
}

/* LD D, d8 (opcode 0x16) */
static inline uint8_t op_ld_d_d8(cpu_t *cpu, mem_t *m) {
    cpu->r.d = mem_read_byte(m, cpu->pc + 1);
    cpu->pc += 2;
    return 2;
}

/* LD E, d8 (opcode 0x1E) */
static inline uint8_t op_ld_e_d8(cpu_t *cpu, mem_t *m) {
    cpu->r.e = mem_read_byte(m, cpu->pc + 1);
    cpu->pc += 2;
    return 2;
}

/* LD H, d8 (opcode 0x26) */
static inline uint8_t op_ld_h_d8(cpu_t *cpu, mem_t *m) {
    cpu->r.h = mem_read_byte(m, cpu->pc + 1);
    cpu->pc += 2;
    return 2;
}

/* LD (BC), A (opcode 0x02) */
static inline uint8_t op_ld_bc_a(cpu_t *cpu, mem_t *m) {
    mem_write_byte(m, cpu->r.bc, cpu->r.a);
    cpu->pc++;
    return 1;
}

/* LD B, d8 (opcode 0x06) */
static inline uint8_t op_ld_b_d8(cpu_t *cpu, mem_t *m) {
    cpu->r.b = mem_read_byte(m, cpu->pc + 1);
    cpu->pc += 2;
    return 2;
}

/* INC D (opcode 0x14) */
static inline uint8_t op_inc_d(cpu_t *cpu) {
    uint8_t val = cpu->r.d + 1;
    cpu_set_flag(&cpu->r, F_Z, val == 0);
    cpu_set_flag(&cpu->r, F_N, 0);
    cpu_set_flag(&cpu->r, F_H, (cpu->r.d & 0x0F) + 1 > 0x0F);
    cpu->r.d = val;
    cpu->pc++;
    return 1;
}

/* DEC D (opcode 0x15) */
static inline uint8_t op_dec_d(cpu_t *cpu) {
    uint8_t val = cpu->r.d - 1;
    cpu_set_flag(&cpu->r, F_Z, val == 0);
    cpu_set_flag(&cpu->r, F_N, 1);
    cpu_set_flag(&cpu->r, F_H, (cpu->r.d & 0x0F) == 0);
    cpu->r.d = val;
    cpu->pc++;
    return 1;
}

/* INC E (opcode 0x1C) */
static inline uint8_t op_inc_e(cpu_t *cpu) {
    uint8_t val = cpu->r.e + 1;
    cpu_set_flag(&cpu->r, F_Z, val == 0);
    cpu_set_flag(&cpu->r, F_N, 0);
    cpu_set_flag(&cpu->r, F_H, (cpu->r.e & 0x0F) + 1 > 0x0F);
    cpu->r.e = val;
    cpu->pc++;
    return 1;
}

/* DEC E (opcode 0x1D) */
static inline uint8_t op_dec_e(cpu_t *cpu) {
    uint8_t val = cpu->r.e - 1;
    cpu_set_flag(&cpu->r, F_Z, val == 0);
    cpu_set_flag(&cpu->r, F_N, 1);
    cpu_set_flag(&cpu->r, F_H, (cpu->r.e & 0x0F) == 0);
    cpu->r.e = val;
    cpu->pc++;
    return 1;
}

/* INC H (opcode 0x24) */
static inline uint8_t op_inc_h(cpu_t *cpu) {
    uint8_t val = cpu->r.h + 1;
    cpu_set_flag(&cpu->r, F_Z, val == 0);
    cpu_set_flag(&cpu->r, F_N, 0);
    cpu_set_flag(&cpu->r, F_H, (cpu->r.h & 0x0F) + 1 > 0x0F);
    cpu->r.h = val;
    cpu->pc++;
    return 1;
}

/* DEC H (opcode 0x25) */
static inline uint8_t op_dec_h(cpu_t *cpu) {
    uint8_t val = cpu->r.h - 1;
    cpu_set_flag(&cpu->r, F_Z, val == 0);
    cpu_set_flag(&cpu->r, F_N, 1);
    cpu_set_flag(&cpu->r, F_H, (cpu->r.h & 0x0F) == 0);
    cpu->r.h = val;
    cpu->pc++;
    return 1;
}

/* INC L (opcode 0x2C) */
static inline uint8_t op_inc_l(cpu_t *cpu) {
    uint8_t val = cpu->r.l + 1;
    cpu_set_flag(&cpu->r, F_Z, val == 0);
    cpu_set_flag(&cpu->r, F_N, 0);
    cpu_set_flag(&cpu->r, F_H, (cpu->r.l & 0x0F) + 1 > 0x0F);
    cpu->r.l = val;
    cpu->pc++;
    return 1;
}

/* DEC L (opcode 0x2D) */
static inline uint8_t op_dec_l(cpu_t *cpu) {
    uint8_t val = cpu->r.l - 1;
    cpu_set_flag(&cpu->r, F_Z, val == 0);
    cpu_set_flag(&cpu->r, F_N, 1);
    cpu_set_flag(&cpu->r, F_H, (cpu->r.l & 0x0F) == 0);
    cpu->r.l = val;
    cpu->pc++;
    return 1;
}

/* INC B (opcode 0x04) */
static inline uint8_t op_inc_b(cpu_t *cpu) {
    uint8_t val = cpu->r.b + 1;
    cpu_set_flag(&cpu->r, F_Z, val == 0);
    cpu_set_flag(&cpu->r, F_N, 0);
    cpu_set_flag(&cpu->r, F_H, (cpu->r.b & 0x0F) + 1 > 0x0F);
    cpu->r.b = val;
    cpu->pc++;
    return 1;
}

/* DEC B (opcode 0x05) */
static inline uint8_t op_dec_b(cpu_t *cpu) {
    uint8_t val = cpu->r.b - 1;
    cpu_set_flag(&cpu->r, F_Z, val == 0);
    cpu_set_flag(&cpu->r, F_N, 1);
    cpu_set_flag(&cpu->r, F_H, (cpu->r.b & 0x0F) == 0);
    cpu->r.b = val;
    cpu->pc++;
    return 1;
}

/* LD C, d8 (opcode 0x0E) */
static inline uint8_t op_ld_c_d8(cpu_t *cpu, mem_t *m) {
    cpu->r.c = mem_read_byte(m, cpu->pc + 1);
    cpu->pc += 2;
    return 2;
}

/* INC C (opcode 0x0C) */
static inline uint8_t op_inc_c(cpu_t *cpu) {
    uint8_t val = cpu->r.c + 1;
    cpu_set_flag(&cpu->r, F_Z, val == 0);
    cpu_set_flag(&cpu->r, F_N, 0);
    cpu_set_flag(&cpu->r, F_H, (cpu->r.c & 0x0F) + 1 > 0x0F);
    cpu->r.c = val;
    cpu->pc++;
    return 1;
}

/* DEC C (opcode 0x0D) */
static inline uint8_t op_dec_c(cpu_t *cpu) {
    uint8_t val = cpu->r.c - 1;
    cpu_set_flag(&cpu->r, F_Z, val == 0);
    cpu_set_flag(&cpu->r, F_N, 1);
    cpu_set_flag(&cpu->r, F_H, (cpu->r.c & 0x0F) == 0);
    cpu->r.c = val;
    cpu->pc++;
    return 1;
}

/* LD DE, d16 (opcode 0x11) */
static inline uint8_t op_ld_de_d16(cpu_t *cpu, mem_t *m) {
    cpu->r.de = mem_read_word(m, cpu->pc + 1);
    cpu->pc += 3;
    return 3;
}

/* LD (DE), A (opcode 0x12) */
static inline uint8_t op_ld_de_a(cpu_t *cpu, mem_t *m) {
    mem_write_byte(m, cpu->r.de, cpu->r.a);
    cpu->pc++;
    return 1;
}

/* INC DE (opcode 0x13) */
static inline uint8_t op_inc_de(cpu_t *cpu) {
    cpu->r.de++;
    cpu->pc++;
    return 2;
}

/* ADD HL, BC (opcode 0x09) */
static inline uint8_t op_add_hl_bc(cpu_t *cpu) {
    uint32_t res = cpu->r.hl + cpu->r.bc;
    cpu_set_flag(&cpu->r, F_N, 0);
    cpu_set_flag(&cpu->r, F_H, ((cpu->r.hl & 0x0FFF) + (cpu->r.bc & 0x0FFF)) > 0x0FFF);
    cpu_set_flag(&cpu->r, F_C, res > 0xFFFF);
    cpu->r.hl = (uint16_t)res;
    cpu->pc++;
    return 2;
}

/* ADD HL, DE (opcode 0x19) */
static inline uint8_t op_add_hl_de(cpu_t *cpu) {
    uint32_t res = cpu->r.hl + cpu->r.de;
    cpu_set_flag(&cpu->r, F_N, 0);
    cpu_set_flag(&cpu->r, F_H, ((cpu->r.hl & 0x0FFF) + (cpu->r.de & 0x0FFF)) > 0x0FFF);
    cpu_set_flag(&cpu->r, F_C, res > 0xFFFF);
    cpu->r.hl = (uint16_t)res;
    cpu->pc++;
    return 2;
}

/* LD A, (BC) (opcode 0x0A) */
static inline uint8_t op_ld_a_bc(cpu_t *cpu, mem_t *m) {
    cpu->r.a = mem_read_byte(m, cpu->r.bc);
    cpu->pc++;
    return 2;
}

/* LD A, (DE) (opcode 0x1A) */
static inline uint8_t op_ld_a_de(cpu_t *cpu, mem_t *m) {
    cpu->r.a = mem_read_byte(m, cpu->r.de);
    cpu->pc++;
    return 2;
}

/* LD A, (HL) (opcode 0x7E) */
static inline uint8_t op_ld_a_hl(cpu_t *cpu, mem_t *m) {
    cpu->r.a = mem_read_byte(m, cpu->r.hl);
    cpu->pc++;
    return 2;
}

/* DEC BC (opcode 0x0B) */
static inline uint8_t op_dec_bc(cpu_t *cpu) {
    cpu->r.bc--;
    cpu->pc++;
    return 2;
}

/* DEC DE (opcode 0x1B) */
static inline uint8_t op_dec_de(cpu_t *cpu) {
    cpu->r.de--;
    cpu->pc++;
    return 2;
}




#ifdef __cplusplus
}
#endif

#endif  // CPU_OPS_H
