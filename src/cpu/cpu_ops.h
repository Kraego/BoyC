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

/* Stack helpers */
static inline void push_word(cpu_t *cpu, mem_t *m, uint16_t value)
{
    cpu->sp--;
    mem_write_byte(m, cpu->sp, value >> 8);
    cpu->sp--;
    mem_write_byte(m, cpu->sp, value & 0xFF);
}

static inline uint16_t pop_word(cpu_t *cpu, mem_t *m)
{
    uint8_t low = mem_read_byte(m, cpu->sp);
    cpu->sp++;
    uint8_t high = mem_read_byte(m, cpu->sp);
    cpu->sp++;
    return (high << 8) | low;
}

/* Generic 8-bit register helpers used by CB-prefixed opcodes */
static inline uint8_t read_reg8(cpu_t *cpu, mem_t *m, uint8_t idx)
{
    switch (idx)
    {
        case 0: return cpu->r.b;
        case 1: return cpu->r.c;
        case 2: return cpu->r.d;
        case 3: return cpu->r.e;
        case 4: return cpu->r.h;
        case 5: return cpu->r.l;
        case 6: return mem_read_byte(m, cpu->r.hl);
        default: return cpu->r.a;
    }
}

static inline void write_reg8(cpu_t *cpu, mem_t *m, uint8_t idx, uint8_t val)
{
    switch (idx)
    {
        case 0: cpu->r.b = val; break;
        case 1: cpu->r.c = val; break;
        case 2: cpu->r.d = val; break;
        case 3: cpu->r.e = val; break;
        case 4: cpu->r.h = val; break;
        case 5: cpu->r.l = val; break;
        case 6: mem_write_byte(m, cpu->r.hl, val); break;
        default: cpu->r.a = val; break;
    }
}


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
static inline uint8_t op_jp_nz_a16(cpu_t *cpu, mem_t *m){
    if (cpu_get_flag(&cpu->r,F_Z) == 0) {
        uint16_t a16 = mem_read_word(m, cpu->pc + 1);
        cpu->pc = a16;
        return 4;
    }
    cpu->pc += 3;
    return 3;
}

/* JR Z, s8  (opcode 0x28)*/
static inline uint8_t op_jr_z_s8(cpu_t *cpu, mem_t *m){
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

/* LD SP, d16 (opcode 0x31) */
static inline uint8_t op_ld_sp_d16(cpu_t *cpu, mem_t *m){
    uint16_t d16 = mem_read_word(m, cpu->pc + 1);
    cpu->sp = d16;
    cpu->pc += 3;
    return 3;
}

/* LD (HL+), A (opcode 0x22) */
static inline uint8_t op_ld_hl_inc_a(cpu_t *cpu, mem_t *m){
    mem_write_byte(m, cpu->r.hl, cpu->r.a);
    cpu->r.hl++;
    cpu->pc++;
    return 2;
}

/* LD (HL-), A (opcode 0x32) */
static inline uint8_t op_ld_hl_dec_a(cpu_t *cpu, mem_t *m){
    mem_write_byte(m, cpu->r.hl, cpu->r.a);
    cpu->r.hl--;
    cpu->pc++;
    return 2;
}

/* LD A, (HL+) (opcode 0x2A) */
static inline uint8_t op_ld_a_hl_inc(cpu_t *cpu, mem_t *m){
    cpu->r.a = mem_read_byte(m, cpu->r.hl);
    cpu->r.hl++;
    cpu->pc++;
    return 2;
}

/* LD A, (HL-) (opcode 0x3A) */
static inline uint8_t op_ld_a_hl_dec(cpu_t *cpu, mem_t *m){
    cpu->r.a = mem_read_byte(m, cpu->r.hl);
    cpu->r.hl--;
    cpu->pc++;
    return 2;
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

/* JR NC, s8 (opcode 0x30)*/
static inline uint8_t op_jr_nc_s8(cpu_t *cpu, mem_t *m) {
    if (cpu_get_flag(&cpu->r, F_C) == 0) {
        int8_t s8 = (int8_t)mem_read_byte(m, cpu->pc + 1);
        cpu->pc = (uint16_t)((int32_t)(cpu->pc + 2) + s8);
        return 3;
    }

    cpu->pc += 2;
    return 2;
}

/* JR C, s8 (opcode 0x38)*/
static inline uint8_t op_jr_c_s8(cpu_t *cpu, mem_t *m) {
    if (cpu_get_flag(&cpu->r, F_C) == 1) {
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

/* Helper for CP operations */
static inline void op_cp(cpu_t *cpu, uint8_t value)
{
    uint8_t diff = cpu->r.a - value;
    cpu_set_flag(&cpu->r, F_Z, diff == 0);
    cpu_set_flag(&cpu->r, F_N, 1);
    cpu_set_flag(&cpu->r, F_H, (cpu->r.a & 0x0F) < (value & 0x0F));
    cpu_set_flag(&cpu->r, F_C, cpu->r.a < value);
}

/* CP d8 (opcode 0xFE)*/
static inline uint8_t op_cp_d8(cpu_t *cpu, mem_t *m){
    uint8_t d8 = mem_read_byte(m, cpu->pc + 1);
    op_cp(cpu, d8);
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

/* LD SP, HL (opcode 0xF9)*/
static inline uint8_t op_ld_sp_hl(cpu_t *cpu) {
    cpu->sp = cpu->r.hl;
    cpu->pc++;
    return 2;
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

/* INC A (opcode 0x3C) */
static inline uint8_t op_inc_a(cpu_t *cpu) {
    uint8_t val = cpu->r.a + 1;
    cpu_set_flag(&cpu->r, F_Z, val == 0);
    cpu_set_flag(&cpu->r, F_N, 0);
    cpu_set_flag(&cpu->r, F_H, (cpu->r.a & 0x0F) + 1 > 0x0F);
    cpu->r.a = val;
    cpu->pc++;
    return 1;
}

/* DEC A (opcode 0x3D) */
static inline uint8_t op_dec_a(cpu_t *cpu) {
    uint8_t val = cpu->r.a - 1;
    cpu_set_flag(&cpu->r, F_Z, val == 0);
    cpu_set_flag(&cpu->r, F_N, 1);
    cpu_set_flag(&cpu->r, F_H, (cpu->r.a & 0x0F) == 0);
    cpu->r.a = val;
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

/* ADD HL, HL (opcode 0x29) */
static inline uint8_t op_add_hl_hl(cpu_t *cpu) {
    uint32_t res = cpu->r.hl + cpu->r.hl;
    cpu_set_flag(&cpu->r, F_N, 0);
    cpu_set_flag(&cpu->r, F_H, ((cpu->r.hl & 0x0FFF) + (cpu->r.hl & 0x0FFF)) > 0x0FFF);
    cpu_set_flag(&cpu->r, F_C, res > 0xFFFF);
    cpu->r.hl = (uint16_t)res;
    cpu->pc++;
    return 2;
}

/* ADD HL, SP (opcode 0x39) */
static inline uint8_t op_add_hl_sp(cpu_t *cpu) {
    uint32_t res = cpu->r.hl + cpu->sp;
    cpu_set_flag(&cpu->r, F_N, 0);
    cpu_set_flag(&cpu->r, F_H, ((cpu->r.hl & 0x0FFF) + (cpu->sp & 0x0FFF)) > 0x0FFF);
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

/* INC HL (opcode 0x23) */
static inline uint8_t op_inc_hl(cpu_t *cpu) {
    cpu->r.hl++;
    cpu->pc++;
    return 2;
}

/* DEC HL (opcode 0x2B) */
static inline uint8_t op_dec_hl(cpu_t *cpu) {
    cpu->r.hl--;
    cpu->pc++;
    return 2;
}

/* INC SP (opcode 0x33) */
static inline uint8_t op_inc_sp(cpu_t *cpu) {
    cpu->sp++;
    cpu->pc++;
    return 2;
}

/* DEC SP (opcode 0x3B) */
static inline uint8_t op_dec_sp(cpu_t *cpu) {
    cpu->sp--;
    cpu->pc++;
    return 2;
}

/* LD C, B (opcode 0x48) */
static inline uint8_t op_ld_c_b(cpu_t *cpu) {
    cpu->r.c = cpu->r.b;
    cpu->pc++;
    return 1;
}

/* LD C, C (opcode 0x49) */
static inline uint8_t op_ld_c_c(cpu_t *cpu) {
    cpu->r.c = cpu->r.c;
    cpu->pc++;
    return 1;
}

/* LD C, D (opcode 0x4A) */
static inline uint8_t op_ld_c_d(cpu_t *cpu) {
    cpu->r.c = cpu->r.d;
    cpu->pc++;
    return 1;
}

/* LD C, E (opcode 0x4B) */
static inline uint8_t op_ld_c_e(cpu_t *cpu) {
    cpu->r.c = cpu->r.e;
    cpu->pc++;
    return 1;
}

/* LD C, H (opcode 0x4C) */
static inline uint8_t op_ld_c_h(cpu_t *cpu) {
    cpu->r.c = cpu->r.h;
    cpu->pc++;
    return 1;
}

/* LD C, L (opcode 0x4D) */
static inline uint8_t op_ld_c_l(cpu_t *cpu) {
    cpu->r.c = cpu->r.l;
    cpu->pc++;
    return 1;
}

/* LD C, (HL) (opcode 0x4E) */
static inline uint8_t op_ld_c_hl(cpu_t *cpu, mem_t *m) {
    cpu->r.c = mem_read_byte(m, cpu->r.hl);
    cpu->pc++;
    return 2;
}

/* LD D, A (opcode 0x57) */
static inline uint8_t op_ld_d_a(cpu_t *cpu) {
    cpu->r.d = cpu->r.a;
    cpu->pc++;
    return 1;
}

/* LD D, B (opcode 0x50) */
static inline uint8_t op_ld_d_b(cpu_t *cpu) {
    cpu->r.d = cpu->r.b;
    cpu->pc++;
    return 1;
}

/* LD D, C (opcode 0x51) */
static inline uint8_t op_ld_d_c(cpu_t *cpu) {
    cpu->r.d = cpu->r.c;
    cpu->pc++;
    return 1;
}

/* LD D, D (opcode 0x52) */
static inline uint8_t op_ld_d_d(cpu_t *cpu) {
    cpu->r.d = cpu->r.d;
    cpu->pc++;
    return 1;
}

/* LD D, E (opcode 0x53) */
static inline uint8_t op_ld_d_e(cpu_t *cpu) {
    cpu->r.d = cpu->r.e;
    cpu->pc++;
    return 1;
}

/* LD D, H (opcode 0x54) */
static inline uint8_t op_ld_d_h(cpu_t *cpu) {
    cpu->r.d = cpu->r.h;
    cpu->pc++;
    return 1;
}

/* LD D, L (opcode 0x55) */
static inline uint8_t op_ld_d_l(cpu_t *cpu) {
    cpu->r.d = cpu->r.l;
    cpu->pc++;
    return 1;
}

/* LD D, (HL) (opcode 0x56) */
static inline uint8_t op_ld_d_hl(cpu_t *cpu, mem_t *m) {
    cpu->r.d = mem_read_byte(m, cpu->r.hl);
    cpu->pc++;
    return 2;
}

/* LD E, A (opcode 0x5F) */
static inline uint8_t op_ld_e_a(cpu_t *cpu) {
    cpu->r.e = cpu->r.a;
    cpu->pc++;
    return 1;
}

/* LD E, B (opcode 0x58) */
static inline uint8_t op_ld_e_b(cpu_t *cpu) {
    cpu->r.e = cpu->r.b;
    cpu->pc++;
    return 1;
}

/* LD E, C (opcode 0x59) */
static inline uint8_t op_ld_e_c(cpu_t *cpu) {
    cpu->r.e = cpu->r.c;
    cpu->pc++;
    return 1;
}

/* LD E, D (opcode 0x5A) */
static inline uint8_t op_ld_e_d(cpu_t *cpu) {
    cpu->r.e = cpu->r.d;
    cpu->pc++;
    return 1;
}

/* LD E, E (opcode 0x5B) */
static inline uint8_t op_ld_e_e(cpu_t *cpu) {
    cpu->r.e = cpu->r.e;
    cpu->pc++;
    return 1;
}

/* LD E, H (opcode 0x5C) */
static inline uint8_t op_ld_e_h(cpu_t *cpu) {
    cpu->r.e = cpu->r.h;
    cpu->pc++;
    return 1;
}

/* LD E, L (opcode 0x5D) */
static inline uint8_t op_ld_e_l(cpu_t *cpu) {
    cpu->r.e = cpu->r.l;
    cpu->pc++;
    return 1;
}

/* LD E, (HL) (opcode 0x5E) */
static inline uint8_t op_ld_e_hl(cpu_t *cpu, mem_t *m) {
    cpu->r.e = mem_read_byte(m, cpu->r.hl);
    cpu->pc++;
    return 2;
}

/* LD H, A (opcode 0x67) */
static inline uint8_t op_ld_h_a(cpu_t *cpu) {
    cpu->r.h = cpu->r.a;
    cpu->pc++;
    return 1;
}

/* LD H, B (opcode 0x60) */
static inline uint8_t op_ld_h_b(cpu_t *cpu) {
    cpu->r.h = cpu->r.b;
    cpu->pc++;
    return 1;
}

/* LD H, C (opcode 0x61) */
static inline uint8_t op_ld_h_c(cpu_t *cpu) {
    cpu->r.h = cpu->r.c;
    cpu->pc++;
    return 1;
}

/* LD H, D (opcode 0x62) */
static inline uint8_t op_ld_h_d(cpu_t *cpu) {
    cpu->r.h = cpu->r.d;
    cpu->pc++;
    return 1;
}

/* LD H, E (opcode 0x63) */
static inline uint8_t op_ld_h_e(cpu_t *cpu) {
    cpu->r.h = cpu->r.e;
    cpu->pc++;
    return 1;
}

/* LD H, H (opcode 0x64) */
static inline uint8_t op_ld_h_h(cpu_t *cpu) {
    cpu->r.h = cpu->r.h;
    cpu->pc++;
    return 1;
}

/* LD H, L (opcode 0x65) */
static inline uint8_t op_ld_h_l(cpu_t *cpu) {
    cpu->r.h = cpu->r.l;
    cpu->pc++;
    return 1;
}

/* LD H, (HL) (opcode 0x66) */
static inline uint8_t op_ld_h_hl(cpu_t *cpu, mem_t *m) {
    cpu->r.h = mem_read_byte(m, cpu->r.hl);
    cpu->pc++;
    return 2;
}

/* LD L, A (opcode 0x6F) */
static inline uint8_t op_ld_l_a(cpu_t *cpu) {
    cpu->r.l = cpu->r.a;
    cpu->pc++;
    return 1;
}

/* LD L, B (opcode 0x68) */
static inline uint8_t op_ld_l_b(cpu_t *cpu) {
    cpu->r.l = cpu->r.b;
    cpu->pc++;
    return 1;
}

/* LD L, C (opcode 0x69) */
static inline uint8_t op_ld_l_c(cpu_t *cpu) {
    cpu->r.l = cpu->r.c;
    cpu->pc++;
    return 1;
}

/* LD L, D (opcode 0x6A) */
static inline uint8_t op_ld_l_d(cpu_t *cpu) {
    cpu->r.l = cpu->r.d;
    cpu->pc++;
    return 1;
}

/* LD L, E (opcode 0x6B) */
static inline uint8_t op_ld_l_e(cpu_t *cpu) {
    cpu->r.l = cpu->r.e;
    cpu->pc++;
    return 1;
}

/* LD L, H (opcode 0x6C) */
static inline uint8_t op_ld_l_h(cpu_t *cpu) {
    cpu->r.l = cpu->r.h;
    cpu->pc++;
    return 1;
}

/* LD L, L (opcode 0x6D) */
static inline uint8_t op_ld_l_l(cpu_t *cpu) {
    cpu->r.l = cpu->r.l;
    cpu->pc++;
    return 1;
}

/* LD L, (HL) (opcode 0x6E) */
static inline uint8_t op_ld_l_hl(cpu_t *cpu, mem_t *m) {
    cpu->r.l = mem_read_byte(m, cpu->r.hl);
    cpu->pc++;
    return 2;
}

/* LD (HL), B (opcode 0x70) */
static inline uint8_t op_ld_hl_b(cpu_t *cpu, mem_t *m) {
    mem_write_byte(m, cpu->r.hl, cpu->r.b);
    cpu->pc++;
    return 1;
}

/* LD (HL), C (opcode 0x71) */
static inline uint8_t op_ld_hl_c(cpu_t *cpu, mem_t *m) {
    mem_write_byte(m, cpu->r.hl, cpu->r.c);
    cpu->pc++;
    return 1;
}

/* LD (HL), D (opcode 0x72) */
static inline uint8_t op_ld_hl_d(cpu_t *cpu, mem_t *m) {
    mem_write_byte(m, cpu->r.hl, cpu->r.d);
    cpu->pc++;
    return 1;
}

/* LD (HL), E (opcode 0x73) */
static inline uint8_t op_ld_hl_e(cpu_t *cpu, mem_t *m) {
    mem_write_byte(m, cpu->r.hl, cpu->r.e);
    cpu->pc++;
    return 1;
}

/* LD (HL), H (opcode 0x74) */
static inline uint8_t op_ld_hl_h(cpu_t *cpu, mem_t *m) {
    mem_write_byte(m, cpu->r.hl, cpu->r.h);
    cpu->pc++;
    return 1;
}

/* LD (HL), L (opcode 0x75) */
static inline uint8_t op_ld_hl_l(cpu_t *cpu, mem_t *m) {
    mem_write_byte(m, cpu->r.hl, cpu->r.l);
    cpu->pc++;
    return 1;
}

/* INC (HL) (opcode 0x34) */
static inline uint8_t op_inc_mem_hl(cpu_t *cpu, mem_t *m) {
    uint8_t val = mem_read_byte(m, cpu->r.hl) + 1;
    mem_write_byte(m, cpu->r.hl, val);
    cpu_set_flag(&cpu->r, F_Z, val == 0);
    cpu_set_flag(&cpu->r, F_N, 0);
    cpu_set_flag(&cpu->r, F_H, (val & 0x0F) == 0x00);
    cpu->pc++;
    return 3;
}

/* DEC (HL) (opcode 0x35) */
static inline uint8_t op_dec_mem_hl(cpu_t *cpu, mem_t *m) {
    uint8_t val = mem_read_byte(m, cpu->r.hl) - 1;
    mem_write_byte(m, cpu->r.hl, val);
    cpu_set_flag(&cpu->r, F_Z, val == 0);
    cpu_set_flag(&cpu->r, F_N, 1);
    cpu_set_flag(&cpu->r, F_H, (val & 0x0F) == 0x0F);
    cpu->pc++;
    return 3;
}

/* LD (HL), d8 (opcode 0x36) */
static inline uint8_t op_ld_hl_d8(cpu_t *cpu, mem_t *m) {
    uint8_t d8 = mem_read_byte(m, cpu->pc + 1);
    mem_write_byte(m, cpu->r.hl, d8);
    cpu->pc += 2;
    return 3;
}

/* LD A, C (opcode 0x79) */
static inline uint8_t op_ld_a_c(cpu_t *cpu) {
    cpu->r.a = cpu->r.c;
    cpu->pc++;
    return 1;
}

/* LD A, D (opcode 0x7A) */
static inline uint8_t op_ld_a_d(cpu_t *cpu) {
    cpu->r.a = cpu->r.d;
    cpu->pc++;
    return 1;
}

/* LD A, E (opcode 0x7B) */
static inline uint8_t op_ld_a_e(cpu_t *cpu) {
    cpu->r.a = cpu->r.e;
    cpu->pc++;
    return 1;
}

/* LD A, H (opcode 0x7C) */
static inline uint8_t op_ld_a_h(cpu_t *cpu) {
    cpu->r.a = cpu->r.h;
    cpu->pc++;
    return 1;
}

/* LD A, L (opcode 0x7D) */
static inline uint8_t op_ld_a_l(cpu_t *cpu) {
    cpu->r.a = cpu->r.l;
    cpu->pc++;
    return 1;
}

/* LD A, A (opcode 0x7F) */
static inline uint8_t op_ld_a_a(cpu_t *cpu) {
    cpu->r.a = cpu->r.a;
    cpu->pc++;
    return 1;
}

/* Helper for 8-bit ADD operations */
static inline void op_add_a(cpu_t *cpu, uint8_t value)
{
    uint16_t res = cpu->r.a + value;
    cpu_set_flag(&cpu->r, F_Z, (res & 0xFF) == 0);
    cpu_set_flag(&cpu->r, F_N, 0);
    cpu_set_flag(&cpu->r, F_H, ((cpu->r.a & 0x0F) + (value & 0x0F)) > 0x0F);
    cpu_set_flag(&cpu->r, F_C, res > 0xFF);
    cpu->r.a = (uint8_t)res;
}

/* Helper for 8-bit SUB operations */
static inline void op_sub_a(cpu_t *cpu, uint8_t value)
{
    uint16_t res = cpu->r.a - value;
    cpu_set_flag(&cpu->r, F_Z, (res & 0xFF) == 0);
    cpu_set_flag(&cpu->r, F_N, 1);
    cpu_set_flag(&cpu->r, F_H, (cpu->r.a & 0x0F) < (value & 0x0F));
    cpu_set_flag(&cpu->r, F_C, cpu->r.a < value);
    cpu->r.a = (uint8_t)res;
}

/* Helper for 8-bit AND operations */
static inline void op_and_a(cpu_t *cpu, uint8_t value)
{
    cpu->r.a &= value;
    cpu_set_flag(&cpu->r, F_Z, cpu->r.a == 0);
    cpu_set_flag(&cpu->r, F_N, 0);
    cpu_set_flag(&cpu->r, F_H, 1);
    cpu_set_flag(&cpu->r, F_C, 0);
}

/* Helper for 8-bit OR operations */
static inline void op_or_a(cpu_t *cpu, uint8_t value)
{
    cpu->r.a |= value;
    cpu_set_flag(&cpu->r, F_Z, cpu->r.a == 0);
    cpu_set_flag(&cpu->r, F_N, 0);
    cpu_set_flag(&cpu->r, F_H, 0);
    cpu_set_flag(&cpu->r, F_C, 0);
}

/* Helper for 8-bit XOR operations */
static inline void op_xor_a(cpu_t *cpu, uint8_t value)
{
    cpu->r.a ^= value;
    cpu_set_flag(&cpu->r, F_Z, cpu->r.a == 0);
    cpu_set_flag(&cpu->r, F_N, 0);
    cpu_set_flag(&cpu->r, F_H, 0);
    cpu_set_flag(&cpu->r, F_C, 0);
}

/* ADD A, B (opcode 0x80) */
static inline uint8_t op_add_a_b(cpu_t *cpu) {
    op_add_a(cpu, cpu->r.b);
    cpu->pc++;
    return 1;
}

/* ADD A, C (opcode 0x81) */
static inline uint8_t op_add_a_c(cpu_t *cpu) {
    op_add_a(cpu, cpu->r.c);
    cpu->pc++;
    return 1;
}

/* ADD A, D (opcode 0x82) */
static inline uint8_t op_add_a_d(cpu_t *cpu) {
    op_add_a(cpu, cpu->r.d);
    cpu->pc++;
    return 1;
}

/* ADD A, E (opcode 0x83) */
static inline uint8_t op_add_a_e(cpu_t *cpu) {
    op_add_a(cpu, cpu->r.e);
    cpu->pc++;
    return 1;
}

/* ADD A, H (opcode 0x84) */
static inline uint8_t op_add_a_h(cpu_t *cpu) {
    op_add_a(cpu, cpu->r.h);
    cpu->pc++;
    return 1;
}

/* ADD A, L (opcode 0x85) */
static inline uint8_t op_add_a_l(cpu_t *cpu) {
    op_add_a(cpu, cpu->r.l);
    cpu->pc++;
    return 1;
}

/* ADD A, (HL) (opcode 0x86) */
static inline uint8_t op_add_a_hl(cpu_t *cpu, mem_t *m) {
    op_add_a(cpu, mem_read_byte(m, cpu->r.hl));
    cpu->pc++;
    return 2;
}

/* ADD A, A (opcode 0x87) */
static inline uint8_t op_add_a_a(cpu_t *cpu) {
    op_add_a(cpu, cpu->r.a);
    cpu->pc++;
    return 1;
}

/* ADD A, d8 (opcode 0xC6) */
static inline uint8_t op_add_a_d8(cpu_t *cpu, mem_t *m) {
    uint8_t d8 = mem_read_byte(m, cpu->pc + 1);
    op_add_a(cpu, d8);
    cpu->pc += 2;
    return 2;
}

/* Helper for 8-bit ADC operations */
static inline void op_adc_a(cpu_t *cpu, uint8_t value)
{
    uint8_t carry = cpu_get_flag(&cpu->r, F_C);
    uint16_t res = cpu->r.a + value + carry;

    cpu_set_flag(&cpu->r, F_Z, (res & 0xFF) == 0);
    cpu_set_flag(&cpu->r, F_N, 0);
    cpu_set_flag(&cpu->r, F_H,
                 ((cpu->r.a & 0x0F) + (value & 0x0F) + carry) > 0x0F);
    cpu_set_flag(&cpu->r, F_C, res > 0xFF);

    cpu->r.a = (uint8_t)res;
}

/* ADC A, B (opcode 0x88) */
static inline uint8_t op_adc_a_b(cpu_t *cpu) {
    op_adc_a(cpu, cpu->r.b);
    cpu->pc++;
    return 1;
}

/* ADC A, C (opcode 0x89) */
static inline uint8_t op_adc_a_c(cpu_t *cpu) {
    op_adc_a(cpu, cpu->r.c);
    cpu->pc++;
    return 1;
}

/* ADC A, D (opcode 0x8A) */
static inline uint8_t op_adc_a_d(cpu_t *cpu) {
    op_adc_a(cpu, cpu->r.d);
    cpu->pc++;
    return 1;
}

/* ADC A, E (opcode 0x8B) */
static inline uint8_t op_adc_a_e(cpu_t *cpu) {
    op_adc_a(cpu, cpu->r.e);
    cpu->pc++;
    return 1;
}

/* ADC A, H (opcode 0x8C) */
static inline uint8_t op_adc_a_h(cpu_t *cpu) {
    op_adc_a(cpu, cpu->r.h);
    cpu->pc++;
    return 1;
}

/* ADC A, L (opcode 0x8D) */
static inline uint8_t op_adc_a_l(cpu_t *cpu) {
    op_adc_a(cpu, cpu->r.l);
    cpu->pc++;
    return 1;
}

/* ADC A, (HL) (opcode 0x8E) */
static inline uint8_t op_adc_a_hl(cpu_t *cpu, mem_t *m) {
    op_adc_a(cpu, mem_read_byte(m, cpu->r.hl));
    cpu->pc++;
    return 2;
}

/* ADC A, A (opcode 0x8F) */
static inline uint8_t op_adc_a_a(cpu_t *cpu) {
    op_adc_a(cpu, cpu->r.a);
    cpu->pc++;
    return 1;
}

/* ADC A, d8 (opcode 0xCE) */
static inline uint8_t op_adc_a_d8(cpu_t *cpu, mem_t *m) {
    uint8_t d8 = mem_read_byte(m, cpu->pc + 1);
    op_adc_a(cpu, d8);
    cpu->pc += 2;
    return 2;
}

/* SUB B (opcode 0x90) */
static inline uint8_t op_sub_b(cpu_t *cpu) {
    op_sub_a(cpu, cpu->r.b);
    cpu->pc++;
    return 1;
}

/* SUB C (opcode 0x91) */
static inline uint8_t op_sub_c(cpu_t *cpu) {
    op_sub_a(cpu, cpu->r.c);
    cpu->pc++;
    return 1;
}

/* SUB D (opcode 0x92) */
static inline uint8_t op_sub_d(cpu_t *cpu) {
    op_sub_a(cpu, cpu->r.d);
    cpu->pc++;
    return 1;
}

/* SUB E (opcode 0x93) */
static inline uint8_t op_sub_e(cpu_t *cpu) {
    op_sub_a(cpu, cpu->r.e);
    cpu->pc++;
    return 1;
}

/* SUB H (opcode 0x94) */
static inline uint8_t op_sub_h(cpu_t *cpu) {
    op_sub_a(cpu, cpu->r.h);
    cpu->pc++;
    return 1;
}

/* SUB L (opcode 0x95) */
static inline uint8_t op_sub_l(cpu_t *cpu) {
    op_sub_a(cpu, cpu->r.l);
    cpu->pc++;
    return 1;
}

/* SUB (HL) (opcode 0x96) */
static inline uint8_t op_sub_hl(cpu_t *cpu, mem_t *m) {
    op_sub_a(cpu, mem_read_byte(m, cpu->r.hl));
    cpu->pc++;
    return 2;
}

/* SUB A (opcode 0x97) */
static inline uint8_t op_sub_a_a(cpu_t *cpu) {
    op_sub_a(cpu, cpu->r.a);
    cpu->pc++;
    return 1;
}

/* SUB d8 (opcode 0xD6) */
static inline uint8_t op_sub_d8(cpu_t *cpu, mem_t *m) {
    uint8_t d8 = mem_read_byte(m, cpu->pc + 1);
    op_sub_a(cpu, d8);
    cpu->pc += 2;
    return 2;
}

/* Helper for 8-bit SBC operations */
static inline void op_sbc_a(cpu_t *cpu, uint8_t value)
{
    uint8_t carry = cpu_get_flag(&cpu->r, F_C);
    uint16_t res = cpu->r.a - value - carry;

    cpu_set_flag(&cpu->r, F_Z, (res & 0xFF) == 0);
    cpu_set_flag(&cpu->r, F_N, 1);
    cpu_set_flag(&cpu->r, F_H,
                 (cpu->r.a & 0x0F) < ((value & 0x0F) + carry));
    cpu_set_flag(&cpu->r, F_C, cpu->r.a < value + carry);

    cpu->r.a = (uint8_t)res;
}

/* SBC A, B (opcode 0x98) */
static inline uint8_t op_sbc_a_b(cpu_t *cpu) {
    op_sbc_a(cpu, cpu->r.b);
    cpu->pc++;
    return 1;
}

/* SBC A, C (opcode 0x99) */
static inline uint8_t op_sbc_a_c(cpu_t *cpu) {
    op_sbc_a(cpu, cpu->r.c);
    cpu->pc++;
    return 1;
}

/* SBC A, D (opcode 0x9A) */
static inline uint8_t op_sbc_a_d(cpu_t *cpu) {
    op_sbc_a(cpu, cpu->r.d);
    cpu->pc++;
    return 1;
}

/* SBC A, E (opcode 0x9B) */
static inline uint8_t op_sbc_a_e(cpu_t *cpu) {
    op_sbc_a(cpu, cpu->r.e);
    cpu->pc++;
    return 1;
}

/* SBC A, H (opcode 0x9C) */
static inline uint8_t op_sbc_a_h(cpu_t *cpu) {
    op_sbc_a(cpu, cpu->r.h);
    cpu->pc++;
    return 1;
}

/* SBC A, L (opcode 0x9D) */
static inline uint8_t op_sbc_a_l(cpu_t *cpu) {
    op_sbc_a(cpu, cpu->r.l);
    cpu->pc++;
    return 1;
}

/* SBC A, (HL) (opcode 0x9E) */
static inline uint8_t op_sbc_a_hl(cpu_t *cpu, mem_t *m) {
    op_sbc_a(cpu, mem_read_byte(m, cpu->r.hl));
    cpu->pc++;
    return 2;
}

/* SBC A, A (opcode 0x9F) */
static inline uint8_t op_sbc_a_a(cpu_t *cpu) {
    op_sbc_a(cpu, cpu->r.a);
    cpu->pc++;
    return 1;
}

/* SBC A, d8 (opcode 0xDE) */
static inline uint8_t op_sbc_a_d8(cpu_t *cpu, mem_t *m) {
    uint8_t d8 = mem_read_byte(m, cpu->pc + 1);
    op_sbc_a(cpu, d8);
    cpu->pc += 2;
    return 2;
}

/* AND B (opcode 0xA0) */
static inline uint8_t op_and_b(cpu_t *cpu) {
    op_and_a(cpu, cpu->r.b);
    cpu->pc++;
    return 1;
}

/* AND C (opcode 0xA1) */
static inline uint8_t op_and_c(cpu_t *cpu) {
    op_and_a(cpu, cpu->r.c);
    cpu->pc++;
    return 1;
}

/* AND D (opcode 0xA2) */
static inline uint8_t op_and_d(cpu_t *cpu) {
    op_and_a(cpu, cpu->r.d);
    cpu->pc++;
    return 1;
}

/* AND E (opcode 0xA3) */
static inline uint8_t op_and_e(cpu_t *cpu) {
    op_and_a(cpu, cpu->r.e);
    cpu->pc++;
    return 1;
}

/* AND H (opcode 0xA4) */
static inline uint8_t op_and_h(cpu_t *cpu) {
    op_and_a(cpu, cpu->r.h);
    cpu->pc++;
    return 1;
}

/* AND L (opcode 0xA5) */
static inline uint8_t op_and_l(cpu_t *cpu) {
    op_and_a(cpu, cpu->r.l);
    cpu->pc++;
    return 1;
}

/* AND (HL) (opcode 0xA6) */
static inline uint8_t op_and_hl(cpu_t *cpu, mem_t *m) {
    op_and_a(cpu, mem_read_byte(m, cpu->r.hl));
    cpu->pc++;
    return 2;
}

/* AND A (opcode 0xA7) */
static inline uint8_t op_and_a_a(cpu_t *cpu) {
    op_and_a(cpu, cpu->r.a);
    cpu->pc++;
    return 1;
}

/* XOR B (opcode 0xA8) */
static inline uint8_t op_xor_b(cpu_t *cpu) {
    op_xor_a(cpu, cpu->r.b);
    cpu->pc++;
    return 1;
}

/* XOR C (opcode 0xA9) */
static inline uint8_t op_xor_c(cpu_t *cpu) {
    op_xor_a(cpu, cpu->r.c);
    cpu->pc++;
    return 1;
}

/* XOR D (opcode 0xAA) */
static inline uint8_t op_xor_d(cpu_t *cpu) {
    op_xor_a(cpu, cpu->r.d);
    cpu->pc++;
    return 1;
}

/* XOR E (opcode 0xAB) */
static inline uint8_t op_xor_e(cpu_t *cpu) {
    op_xor_a(cpu, cpu->r.e);
    cpu->pc++;
    return 1;
}

/* XOR H (opcode 0xAC) */
static inline uint8_t op_xor_h(cpu_t *cpu) {
    op_xor_a(cpu, cpu->r.h);
    cpu->pc++;
    return 1;
}

/* XOR L (opcode 0xAD) */
static inline uint8_t op_xor_l(cpu_t *cpu) {
    op_xor_a(cpu, cpu->r.l);
    cpu->pc++;
    return 1;
}

/* XOR (HL) (opcode 0xAE) */
static inline uint8_t op_xor_hl(cpu_t *cpu, mem_t *m) {
    op_xor_a(cpu, mem_read_byte(m, cpu->r.hl));
    cpu->pc++;
    return 2;
}

/* XOR A (opcode 0xAF) */
static inline uint8_t op_xor_a_a(cpu_t *cpu) {
    op_xor_a(cpu, cpu->r.a);
    cpu->pc++;
    return 1;
}

/* XOR d8 (opcode 0xEE) */
static inline uint8_t op_xor_d8(cpu_t *cpu, mem_t *m) {
    uint8_t d8 = mem_read_byte(m, cpu->pc + 1);
    op_xor_a(cpu, d8);
    cpu->pc += 2;
    return 2;
}

/* OR B (opcode 0xB0) */
static inline uint8_t op_or_b(cpu_t *cpu) {
    op_or_a(cpu, cpu->r.b);
    cpu->pc++;
    return 1;
}

/* OR C (opcode 0xB1) */
static inline uint8_t op_or_c(cpu_t *cpu) {
    op_or_a(cpu, cpu->r.c);
    cpu->pc++;
    return 1;
}

/* OR D (opcode 0xB2) */
static inline uint8_t op_or_d(cpu_t *cpu) {
    op_or_a(cpu, cpu->r.d);
    cpu->pc++;
    return 1;
}

/* OR E (opcode 0xB3) */
static inline uint8_t op_or_e(cpu_t *cpu) {
    op_or_a(cpu, cpu->r.e);
    cpu->pc++;
    return 1;
}

/* OR H (opcode 0xB4) */
static inline uint8_t op_or_h(cpu_t *cpu) {
    op_or_a(cpu, cpu->r.h);
    cpu->pc++;
    return 1;
}

/* OR L (opcode 0xB5) */
static inline uint8_t op_or_l(cpu_t *cpu) {
    op_or_a(cpu, cpu->r.l);
    cpu->pc++;
    return 1;
}

/* OR (HL) (opcode 0xB6) */
static inline uint8_t op_or_hl(cpu_t *cpu, mem_t *m) {
    op_or_a(cpu, mem_read_byte(m, cpu->r.hl));
    cpu->pc++;
    return 2;
}

/* OR A (opcode 0xB7) */
static inline uint8_t op_or_a_a(cpu_t *cpu) {
    op_or_a(cpu, cpu->r.a);
    cpu->pc++;
    return 1;
}

/* CP B (opcode 0xB8) */
static inline uint8_t op_cp_b(cpu_t *cpu) {
    op_cp(cpu, cpu->r.b);
    cpu->pc++;
    return 1;
}

/* CP C (opcode 0xB9) */
static inline uint8_t op_cp_c(cpu_t *cpu) {
    op_cp(cpu, cpu->r.c);
    cpu->pc++;
    return 1;
}

/* CP D (opcode 0xBA) */
static inline uint8_t op_cp_d(cpu_t *cpu) {
    op_cp(cpu, cpu->r.d);
    cpu->pc++;
    return 1;
}

/* CP E (opcode 0xBB) */
static inline uint8_t op_cp_e(cpu_t *cpu) {
    op_cp(cpu, cpu->r.e);
    cpu->pc++;
    return 1;
}

/* CP H (opcode 0xBC) */
static inline uint8_t op_cp_h(cpu_t *cpu) {
    op_cp(cpu, cpu->r.h);
    cpu->pc++;
    return 1;
}

/* CP L (opcode 0xBD) */
static inline uint8_t op_cp_l(cpu_t *cpu) {
    op_cp(cpu, cpu->r.l);
    cpu->pc++;
    return 1;
}

/* CP (HL) (opcode 0xBE) */
static inline uint8_t op_cp_hl(cpu_t *cpu, mem_t *m) {
    op_cp(cpu, mem_read_byte(m, cpu->r.hl));
    cpu->pc++;
    return 2;
}

/* CP A (opcode 0xBF) */
static inline uint8_t op_cp_a_a(cpu_t *cpu) {
    op_cp(cpu, cpu->r.a);
    cpu->pc++;
    return 1;
}

/* OR d8 (opcode 0xF6) */
static inline uint8_t op_or_d8(cpu_t *cpu, mem_t *m) {
    uint8_t d8 = mem_read_byte(m, cpu->pc + 1);
    op_or_a(cpu, d8);
    cpu->pc += 2;
    return 2;
}

/* DI (opcode 0xF3) */
static inline uint8_t op_di(cpu_t *cpu) {
    cpu->ime = 0;
    cpu->pc++;
    return 1;
}

/* SCF (opcode 0x37) */
static inline uint8_t op_scf(cpu_t *cpu) {
    cpu_set_flag(&cpu->r, F_C, 1);
    cpu_set_flag(&cpu->r, F_N, 0);
    cpu_set_flag(&cpu->r, F_H, 0);
    cpu->pc++;
    return 1;
}

/* CCF (opcode 0x3F) */
static inline uint8_t op_ccf(cpu_t *cpu) {
    cpu_set_flag(&cpu->r, F_C, !cpu_get_flag(&cpu->r, F_C));
    cpu_set_flag(&cpu->r, F_N, 0);
    cpu_set_flag(&cpu->r, F_H, 0);
    cpu->pc++;
    return 1;
}

/* STOP (opcode 0x10) */
static inline uint8_t op_stop(cpu_t *cpu) {
    cpu->pc += 2; /* skip stop and following byte */
    return 1;
}

/* HALT (opcode 0x76) */
static inline uint8_t op_halt(cpu_t *cpu) {
    cpu->pc++; /* simplified, ignore halt state */
    return 1;
}

/* RLA (opcode 0x17) */
static inline uint8_t op_rla(cpu_t *cpu) {
    uint8_t carry = cpu_get_flag(&cpu->r, F_C);
    uint8_t new_carry = (cpu->r.a >> 7) & 1;
    cpu->r.a = (cpu->r.a << 1) | carry;
    cpu_set_flag(&cpu->r, F_C, new_carry);
    cpu_set_flag(&cpu->r, F_Z, 0);
    cpu_set_flag(&cpu->r, F_N, 0);
    cpu_set_flag(&cpu->r, F_H, 0);
    cpu->pc++;
    return 1;
}

/* RRA (opcode 0x1F) */
static inline uint8_t op_rra(cpu_t *cpu) {
    uint8_t carry = cpu_get_flag(&cpu->r, F_C);
    uint8_t new_carry = cpu->r.a & 1;
    cpu->r.a = (cpu->r.a >> 1) | (carry << 7);
    cpu_set_flag(&cpu->r, F_C, new_carry);
    cpu_set_flag(&cpu->r, F_Z, 0);
    cpu_set_flag(&cpu->r, F_N, 0);
    cpu_set_flag(&cpu->r, F_H, 0);
    cpu->pc++;
    return 1;
}

/* DAA (opcode 0x27) */
static inline uint8_t op_daa(cpu_t *cpu) {
    uint8_t a = cpu->r.a;
    uint8_t adjust = 0;
    uint8_t carry = cpu_get_flag(&cpu->r, F_C);
    if (!cpu_get_flag(&cpu->r, F_N)) {
        if (cpu_get_flag(&cpu->r, F_H) || (a & 0x0F) > 0x09)
            adjust |= 0x06;
        if (carry || a > 0x99) {
            adjust |= 0x60;
            carry = 1;
        }
        a += adjust;
    } else {
        if (cpu_get_flag(&cpu->r, F_H)) adjust |= 0x06;
        if (carry) adjust |= 0x60;
        a -= adjust;
    }
    cpu->r.a = a;
    cpu_set_flag(&cpu->r, F_Z, cpu->r.a == 0);
    cpu_set_flag(&cpu->r, F_H, 0);
    cpu_set_flag(&cpu->r, F_C, carry);
    cpu->pc++;
    return 1;
}

/* CPL (opcode 0x2F) */
static inline uint8_t op_cpl(cpu_t *cpu) {
    cpu->r.a ^= 0xFF;
    cpu_set_flag(&cpu->r, F_N, 1);
    cpu_set_flag(&cpu->r, F_H, 1);
    cpu->pc++;
    return 1;
}

/* JP Z, a16 (opcode 0xCA) */
static inline uint8_t op_jp_z_a16(cpu_t *cpu, mem_t *m) {
    if (cpu_get_flag(&cpu->r, F_Z)) {
        uint16_t a16 = mem_read_word(m, cpu->pc + 1);
        cpu->pc = a16;
        return 4;
    }
    cpu->pc += 3;
    return 3;
}

/* JP NC, a16 (opcode 0xD2) */
static inline uint8_t op_jp_nc_a16(cpu_t *cpu, mem_t *m) {
    if (!cpu_get_flag(&cpu->r, F_C)) {
        uint16_t a16 = mem_read_word(m, cpu->pc + 1);
        cpu->pc = a16;
        return 4;
    }
    cpu->pc += 3;
    return 3;
}

/* JP C, a16 (opcode 0xDA) */
static inline uint8_t op_jp_c_a16(cpu_t *cpu, mem_t *m) {
    if (cpu_get_flag(&cpu->r, F_C)) {
        uint16_t a16 = mem_read_word(m, cpu->pc + 1);
        cpu->pc = a16;
        return 4;
    }
    cpu->pc += 3;
    return 3;
}

/* JP (HL) (opcode 0xE9) */
static inline uint8_t op_jp_hl(cpu_t *cpu) {
    cpu->pc = cpu->r.hl;
    return 1;
}

/* CALL a16 (opcode 0xCD) */
static inline uint8_t op_call_a16(cpu_t *cpu, mem_t *m) {
    uint16_t a16 = mem_read_word(m, cpu->pc + 1);
    push_word(cpu, m, cpu->pc + 3);
    cpu->pc = a16;
    return 6;
}

static inline uint8_t op_call_cond_a16(cpu_t *cpu, mem_t *m, int cond) {
    if (cond) {
        uint16_t a16 = mem_read_word(m, cpu->pc + 1);
        push_word(cpu, m, cpu->pc + 3);
        cpu->pc = a16;
        return 6;
    }
    cpu->pc += 3;
    return 3;
}

/* CALL NZ, a16 (opcode 0xC4) */
static inline uint8_t op_call_nz_a16(cpu_t *cpu, mem_t *m) {
    return op_call_cond_a16(cpu, m, cpu_get_flag(&cpu->r, F_Z) == 0);
}

/* CALL Z, a16 (opcode 0xCC) */
static inline uint8_t op_call_z_a16(cpu_t *cpu, mem_t *m) {
    return op_call_cond_a16(cpu, m, cpu_get_flag(&cpu->r, F_Z) != 0);
}

/* CALL NC, a16 (opcode 0xD4) */
static inline uint8_t op_call_nc_a16(cpu_t *cpu, mem_t *m) {
    return op_call_cond_a16(cpu, m, cpu_get_flag(&cpu->r, F_C) == 0);
}

/* CALL C, a16 (opcode 0xDC) */
static inline uint8_t op_call_c_a16(cpu_t *cpu, mem_t *m) {
    return op_call_cond_a16(cpu, m, cpu_get_flag(&cpu->r, F_C) != 0);
}

/* RET (opcode 0xC9) */
static inline uint8_t op_ret(cpu_t *cpu, mem_t *m) {
    cpu->pc = pop_word(cpu, m);
    return 4;
}

static inline uint8_t op_ret_cond(cpu_t *cpu, mem_t *m, int cond) {
    if (cond) {
        cpu->pc = pop_word(cpu, m);
        return 5;
    }
    cpu->pc++;
    return 2;
}

/* RET NZ (opcode 0xC0) */
static inline uint8_t op_ret_nz(cpu_t *cpu, mem_t *m) {
    return op_ret_cond(cpu, m, cpu_get_flag(&cpu->r, F_Z) == 0);
}

/* RET Z (opcode 0xC8) */
static inline uint8_t op_ret_z(cpu_t *cpu, mem_t *m) {
    return op_ret_cond(cpu, m, cpu_get_flag(&cpu->r, F_Z) != 0);
}

/* RET NC (opcode 0xD0) */
static inline uint8_t op_ret_nc(cpu_t *cpu, mem_t *m) {
    return op_ret_cond(cpu, m, cpu_get_flag(&cpu->r, F_C) == 0);
}

/* RET C (opcode 0xD8) */
static inline uint8_t op_ret_c(cpu_t *cpu, mem_t *m) {
    return op_ret_cond(cpu, m, cpu_get_flag(&cpu->r, F_C) != 0);
}

/* RETI (opcode 0xD9) */
static inline uint8_t op_reti(cpu_t *cpu, mem_t *m) {
    cpu->pc = pop_word(cpu, m);
    cpu->ime = 1;
    return 4;
}

/* PUSH BC (opcode 0xC5) */
static inline uint8_t op_push_bc(cpu_t *cpu, mem_t *m) {
    push_word(cpu, m, cpu->r.bc);
    cpu->pc++;
    return 4;
}

/* PUSH DE (opcode 0xD5) */
static inline uint8_t op_push_de(cpu_t *cpu, mem_t *m) {
    push_word(cpu, m, cpu->r.de);
    cpu->pc++;
    return 4;
}

/* PUSH HL (opcode 0xE5) */
static inline uint8_t op_push_hl(cpu_t *cpu, mem_t *m) {
    push_word(cpu, m, cpu->r.hl);
    cpu->pc++;
    return 4;
}

/* PUSH AF (opcode 0xF5) */
static inline uint8_t op_push_af(cpu_t *cpu, mem_t *m) {
    push_word(cpu, m, cpu->r.af);
    cpu->pc++;
    return 4;
}

/* POP BC (opcode 0xC1) */
static inline uint8_t op_pop_bc(cpu_t *cpu, mem_t *m) {
    cpu->r.bc = pop_word(cpu, m);
    cpu->pc++;
    return 3;
}

/* POP DE (opcode 0xD1) */
static inline uint8_t op_pop_de(cpu_t *cpu, mem_t *m) {
    cpu->r.de = pop_word(cpu, m);
    cpu->pc++;
    return 3;
}

/* POP HL (opcode 0xE1) */
static inline uint8_t op_pop_hl(cpu_t *cpu, mem_t *m) {
    cpu->r.hl = pop_word(cpu, m);
    cpu->pc++;
    return 3;
}

/* POP AF (opcode 0xF1) */
static inline uint8_t op_pop_af(cpu_t *cpu, mem_t *m) {
    cpu->r.af = pop_word(cpu, m) & 0xFFF0;
    cpu->pc++;
    return 3;
}

/* RST helpers */
static inline uint8_t op_rst(cpu_t *cpu, mem_t *m, uint16_t addr) {
    push_word(cpu, m, cpu->pc);
    cpu->pc = addr;
    return 4;
}

/* RST 00 (opcode 0xC7) */
static inline uint8_t op_rst_00(cpu_t *cpu, mem_t *m) { return op_rst(cpu,m,0x00); }
/* RST 08 (opcode 0xCF) */
static inline uint8_t op_rst_08(cpu_t *cpu, mem_t *m) { return op_rst(cpu,m,0x08); }
/* RST 10 (opcode 0xD7) */
static inline uint8_t op_rst_10(cpu_t *cpu, mem_t *m) { return op_rst(cpu,m,0x10); }
/* RST 18 (opcode 0xDF) */
static inline uint8_t op_rst_18(cpu_t *cpu, mem_t *m) { return op_rst(cpu,m,0x18); }
/* RST 20 (opcode 0xE7) */
static inline uint8_t op_rst_20(cpu_t *cpu, mem_t *m) { return op_rst(cpu,m,0x20); }
/* RST 28 (opcode 0xEF) */
static inline uint8_t op_rst_28(cpu_t *cpu, mem_t *m) { return op_rst(cpu,m,0x28); }
/* RST 30 (opcode 0xF7) */
static inline uint8_t op_rst_30(cpu_t *cpu, mem_t *m) { return op_rst(cpu,m,0x30); }
/* RST 38 (opcode 0xFF) */
static inline uint8_t op_rst_38(cpu_t *cpu, mem_t *m) { return op_rst(cpu,m,0x38); }

/* LDH (a8), A (opcode 0xE0) */
static inline uint8_t op_ldh_a8_a(cpu_t *cpu, mem_t *m) {
    uint8_t offset = mem_read_byte(m, cpu->pc + 1);
    mem_write_byte(m, 0xFF00 | offset, cpu->r.a);
    cpu->pc += 2;
    return 3;
}

/* LD (C), A (opcode 0xE2) */
static inline uint8_t op_ld_c_a_ind(cpu_t *cpu, mem_t *m) {
    mem_write_byte(m, 0xFF00 | cpu->r.c, cpu->r.a);
    cpu->pc++;
    return 2;
}

/* LD A, (C) (opcode 0xF2) */
static inline uint8_t op_ld_a_c_ind(cpu_t *cpu, mem_t *m) {
    cpu->r.a = mem_read_byte(m, 0xFF00 | cpu->r.c);
    cpu->pc++;
    return 2;
}

/* ADD SP, r8 (opcode 0xE8) */
static inline uint8_t op_add_sp_r8(cpu_t *cpu, mem_t *m) {
    int8_t r8 = (int8_t)mem_read_byte(m, cpu->pc + 1);
    uint16_t sp = cpu->sp;
    uint16_t res = (uint16_t)((int32_t)sp + r8);
    cpu_set_flag(&cpu->r, F_Z, 0);
    cpu_set_flag(&cpu->r, F_N, 0);
    cpu_set_flag(&cpu->r, F_H, ((sp & 0x0F) + (r8 & 0x0F)) > 0x0F);
    cpu_set_flag(&cpu->r, F_C, ((sp & 0xFF) + (r8 & 0xFF)) > 0xFF);
    cpu->sp = res;
    cpu->pc += 2;
    return 4;
}

/* EI (opcode 0xFB) */
static inline uint8_t op_ei(cpu_t *cpu) {
    cpu->ime = 1;
    cpu->pc++;
    return 1;
}

/* Handle CB-prefixed opcodes */
static inline uint8_t op_prefix_cb(cpu_t *cpu, mem_t *m)
{
    uint8_t opcode = mem_read_byte(m, cpu->pc + 1);
    uint8_t x = opcode >> 6;
    uint8_t y = (opcode >> 3) & 0x07;
    uint8_t z = opcode & 0x07;

    uint8_t value = read_reg8(cpu, m, z);
    uint8_t result = value;
    uint8_t cycles = (z == 6) ? 4 : 2;

    switch (x)
    {
        case 0: /* rotate/shift */
            switch (y)
            {
                case 0: /* RLC */
                    result = (value << 1) | (value >> 7);
                    cpu_set_flag(&cpu->r, F_C, (value >> 7) & 1);
                    cpu_set_flag(&cpu->r, F_Z, result == 0);
                    cpu_set_flag(&cpu->r, F_N, 0);
                    cpu_set_flag(&cpu->r, F_H, 0);
                    break;
                case 1: /* RRC */
                    result = (value >> 1) | (value << 7);
                    cpu_set_flag(&cpu->r, F_C, value & 1);
                    cpu_set_flag(&cpu->r, F_Z, result == 0);
                    cpu_set_flag(&cpu->r, F_N, 0);
                    cpu_set_flag(&cpu->r, F_H, 0);
                    break;
                case 2: /* RL */
                {
                    uint8_t carry = cpu_get_flag(&cpu->r, F_C);
                    cpu_set_flag(&cpu->r, F_C, (value >> 7) & 1);
                    result = (value << 1) | carry;
                    cpu_set_flag(&cpu->r, F_Z, result == 0);
                    cpu_set_flag(&cpu->r, F_N, 0);
                    cpu_set_flag(&cpu->r, F_H, 0);
                    break;
                }
                case 3: /* RR */
                {
                    uint8_t carry = cpu_get_flag(&cpu->r, F_C);
                    cpu_set_flag(&cpu->r, F_C, value & 1);
                    result = (value >> 1) | (carry << 7);
                    cpu_set_flag(&cpu->r, F_Z, result == 0);
                    cpu_set_flag(&cpu->r, F_N, 0);
                    cpu_set_flag(&cpu->r, F_H, 0);
                    break;
                }
                case 4: /* SLA */
                    cpu_set_flag(&cpu->r, F_C, (value >> 7) & 1);
                    result = value << 1;
                    cpu_set_flag(&cpu->r, F_Z, result == 0);
                    cpu_set_flag(&cpu->r, F_N, 0);
                    cpu_set_flag(&cpu->r, F_H, 0);
                    break;
                case 5: /* SRA */
                    cpu_set_flag(&cpu->r, F_C, value & 1);
                    result = (value >> 1) | (value & 0x80);
                    cpu_set_flag(&cpu->r, F_Z, result == 0);
                    cpu_set_flag(&cpu->r, F_N, 0);
                    cpu_set_flag(&cpu->r, F_H, 0);
                    break;
                case 6: /* SWAP */
                    result = (value << 4) | (value >> 4);
                    cpu_set_flag(&cpu->r, F_Z, result == 0);
                    cpu_set_flag(&cpu->r, F_N, 0);
                    cpu_set_flag(&cpu->r, F_H, 0);
                    cpu_set_flag(&cpu->r, F_C, 0);
                    break;
                case 7: /* SRL */
                    cpu_set_flag(&cpu->r, F_C, value & 1);
                    result = value >> 1;
                    cpu_set_flag(&cpu->r, F_Z, result == 0);
                    cpu_set_flag(&cpu->r, F_N, 0);
                    cpu_set_flag(&cpu->r, F_H, 0);
                    break;
            }
            write_reg8(cpu, m, z, result);
            break;

        case 1: /* BIT y, r */
            cpu_set_flag(&cpu->r, F_Z, (value & (1 << y)) == 0);
            cpu_set_flag(&cpu->r, F_N, 0);
            cpu_set_flag(&cpu->r, F_H, 1);
            break;

        case 2: /* RES y, r */
            result = value & ~(1 << y);
            write_reg8(cpu, m, z, result);
            break;

        case 3: /* SET y, r */
            result = value | (1 << y);
            write_reg8(cpu, m, z, result);
            break;
    }

    cpu->pc += 2;
    return cycles;
}




#ifdef __cplusplus
}
#endif

#endif  // CPU_OPS_H
