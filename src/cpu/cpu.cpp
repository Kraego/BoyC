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
        case 0x10:
            cycles = op_stop(cpu);
            break;
        case 0x14:
            cycles = op_inc_d(cpu);
            break;
        case 0x15:
            cycles = op_dec_d(cpu);
            break;
        case 0x16:
            cycles = op_ld_d_d8(cpu, m);
            break;
        case 0x17:
            cycles = op_rla(cpu);
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
        case 0x29:
            cycles = op_add_hl_hl(cpu);
            break;
        case 0x39:
            cycles = op_add_hl_sp(cpu);
            break;
        case 0x1A:
            cycles = op_ld_a_de(cpu, m);
            break;
        case 0x1B:
            cycles = op_dec_de(cpu);
            break;
        case 0x1C:
            cycles = op_inc_e(cpu);
            break;
        case 0x1D:
            cycles = op_dec_e(cpu);
            break;
        case 0x1E:
            cycles = op_ld_e_d8(cpu, m);
            break;
        case 0x1F:
            cycles = op_rra(cpu);
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
        case 0x22:
            cycles = op_ld_hl_inc_a(cpu, m);
            break;
        case 0x23:
            cycles = op_inc_hl(cpu);
            break;
        case 0x24:
            cycles = op_inc_h(cpu);
            break;
        case 0x25:
            cycles = op_dec_h(cpu);
            break;
        case 0x26:
            cycles = op_ld_h_d8(cpu, m);
            break;
        case 0x27:
            cycles = op_daa(cpu);
            break;
        case 0x28:
            cycles = op_jr_z_s8(cpu, m);
            break;
        case 0x30:
            cycles = op_jr_nc_s8(cpu, m);
            break;
        case 0x2A:
            cycles = op_ld_a_hl_inc(cpu, m);
            break;
        case 0x2B:
            cycles = op_dec_hl(cpu);
            break;
        case 0x2E:
            cycles = op_ld_l_d8(cpu, m);
            break;
        case 0x2F:
            cycles = op_cpl(cpu);
            break;
        case 0x2C:
            cycles = op_inc_l(cpu);
            break;
        case 0x2D:
            cycles = op_dec_l(cpu);
            break;
        case 0x31:
            cycles = op_ld_sp_d16(cpu, m);
            break;
        case 0x32:
            cycles = op_ld_hl_dec_a(cpu, m);
            break;
        case 0x38:
            cycles = op_jr_c_s8(cpu, m);
            break;
        case 0x3A:
            cycles = op_ld_a_hl_dec(cpu, m);
            break;
        case 0x33:
            cycles = op_inc_sp(cpu);
            break;
        case 0x3B:
            cycles = op_dec_sp(cpu);
            break;
        case 0x3C:
            cycles = op_inc_a(cpu);
            break;
        case 0x3D:
            cycles = op_dec_a(cpu);
            break;
        case 0x37:
            cycles = op_scf(cpu);
            break;
        case 0x3F:
            cycles = op_ccf(cpu);
            break;
        case 0x40:
            cycles = op_ld_b_b(cpu, m);
            break;
        case 0x41:
            cycles = op_ld_b_c(cpu, m);
            break;
        case 0x42:
            cycles = op_ld_b_d(cpu);
            break;
        case 0x43:
            cycles = op_ld_b_e(cpu);
            break;
        case 0x44:
            cycles = op_ld_b_h(cpu);
            break;
        case 0x45:
            cycles = op_ld_b_l(cpu);
            break;
        case 0x46:
            cycles = op_ld_b_hl(cpu, m);
            break;
        case 0x47:
            cycles = op_ld_b_a(cpu);
            break;
        case 0x48:
            cycles = op_ld_c_b(cpu);
            break;
        case 0x49:
            cycles = op_ld_c_c(cpu);
            break;
        case 0x4A:
            cycles = op_ld_c_d(cpu);
            break;
        case 0x4B:
            cycles = op_ld_c_e(cpu);
            break;
        case 0x4C:
            cycles = op_ld_c_h(cpu);
            break;
        case 0x4D:
            cycles = op_ld_c_l(cpu);
            break;
        case 0x4E:
            cycles = op_ld_c_hl(cpu, m);
            break;
        case 0x4F:
            cycles = op_ld_c_a(cpu);
            break;
        case 0x50:
            cycles = op_ld_d_b(cpu);
            break;
        case 0x51:
            cycles = op_ld_d_c(cpu);
            break;
        case 0x52:
            cycles = op_ld_d_d(cpu);
            break;
        case 0x53:
            cycles = op_ld_d_e(cpu);
            break;
        case 0x54:
            cycles = op_ld_d_h(cpu);
            break;
        case 0x55:
            cycles = op_ld_d_l(cpu);
            break;
        case 0x56:
            cycles = op_ld_d_hl(cpu, m);
            break;
        case 0x57:
            cycles = op_ld_d_a(cpu);
            break;
        case 0x58:
            cycles = op_ld_e_b(cpu);
            break;
        case 0x59:
            cycles = op_ld_e_c(cpu);
            break;
        case 0x5A:
            cycles = op_ld_e_d(cpu);
            break;
        case 0x5B:
            cycles = op_ld_e_e(cpu);
            break;
        case 0x5C:
            cycles = op_ld_e_h(cpu);
            break;
        case 0x5D:
            cycles = op_ld_e_l(cpu);
            break;
        case 0x5E:
            cycles = op_ld_e_hl(cpu, m);
            break;
        case 0x5F:
            cycles = op_ld_e_a(cpu);
            break;
        case 0x60:
            cycles = op_ld_h_b(cpu);
            break;
        case 0x61:
            cycles = op_ld_h_c(cpu);
            break;
        case 0x62:
            cycles = op_ld_h_d(cpu);
            break;
        case 0x63:
            cycles = op_ld_h_e(cpu);
            break;
        case 0x64:
            cycles = op_ld_h_h(cpu);
            break;
        case 0x65:
            cycles = op_ld_h_l(cpu);
            break;
        case 0x66:
            cycles = op_ld_h_hl(cpu, m);
            break;
        case 0x67:
            cycles = op_ld_h_a(cpu);
            break;
        case 0x68:
            cycles = op_ld_l_b(cpu);
            break;
        case 0x69:
            cycles = op_ld_l_c(cpu);
            break;
        case 0x6A:
            cycles = op_ld_l_d(cpu);
            break;
        case 0x6B:
            cycles = op_ld_l_e(cpu);
            break;
        case 0x6C:
            cycles = op_ld_l_h(cpu);
            break;
        case 0x6D:
            cycles = op_ld_l_l(cpu);
            break;
        case 0x6E:
            cycles = op_ld_l_hl(cpu, m);
            break;
        case 0x6F:
            cycles = op_ld_l_a(cpu);
            break;
        case 0x70:
            cycles = op_ld_hl_b(cpu, m);
            break;
        case 0x71:
            cycles = op_ld_hl_c(cpu, m);
            break;
        case 0x72:
            cycles = op_ld_hl_d(cpu, m);
            break;
        case 0x73:
            cycles = op_ld_hl_e(cpu, m);
            break;
        case 0x74:
            cycles = op_ld_hl_h(cpu, m);
            break;
        case 0x75:
            cycles = op_ld_hl_l(cpu, m);
            break;
        case 0x76:
            cycles = op_halt(cpu);
            break;
        case 0x34:
            cycles = op_inc_mem_hl(cpu, m);
            break;
        case 0x35:
            cycles = op_dec_mem_hl(cpu, m);
            break;
        case 0x36:
            cycles = op_ld_hl_d8(cpu, m);
            break;
        case 0x3E:
            cycles = op_ld_a_d8(cpu, m);
            break;
        case 0x77:
            cycles = op_ld_hl_a(cpu, m);
            break;
        case 0x7E:
            cycles = op_ld_a_hl(cpu, m);
            break;
        case 0x78:
            cycles = op_ld_a_b(cpu);
            break;
        case 0x79:
            cycles = op_ld_a_c(cpu);
            break;
        case 0x7A:
            cycles = op_ld_a_d(cpu);
            break;
        case 0x7B:
            cycles = op_ld_a_e(cpu);
            break;
        case 0x7C:
            cycles = op_ld_a_h(cpu);
            break;
        case 0x7D:
            cycles = op_ld_a_l(cpu);
            break;
        case 0x7F:
            cycles = op_ld_a_a(cpu);
            break;
        case 0x80:
            cycles = op_add_a_b(cpu);
            break;
        case 0x81:
            cycles = op_add_a_c(cpu);
            break;
        case 0x82:
            cycles = op_add_a_d(cpu);
            break;
        case 0x83:
            cycles = op_add_a_e(cpu);
            break;
        case 0x84:
            cycles = op_add_a_h(cpu);
            break;
        case 0x85:
            cycles = op_add_a_l(cpu);
            break;
        case 0x86:
            cycles = op_add_a_hl(cpu, m);
            break;
        case 0x87:
            cycles = op_add_a_a(cpu);
            break;
        case 0x88:
            cycles = op_adc_a_b(cpu);
            break;
        case 0x89:
            cycles = op_adc_a_c(cpu);
            break;
        case 0x8A:
            cycles = op_adc_a_d(cpu);
            break;
        case 0x8B:
            cycles = op_adc_a_e(cpu);
            break;
        case 0x8C:
            cycles = op_adc_a_h(cpu);
            break;
        case 0x8D:
            cycles = op_adc_a_l(cpu);
            break;
        case 0x8E:
            cycles = op_adc_a_hl(cpu, m);
            break;
        case 0x8F:
            cycles = op_adc_a_a(cpu);
            break;
        case 0xC6:
            cycles = op_add_a_d8(cpu, m);
            break;
        case 0xCE:
            cycles = op_adc_a_d8(cpu, m);
            break;
        case 0x90:
            cycles = op_sub_b(cpu);
            break;
        case 0x91:
            cycles = op_sub_c(cpu);
            break;
        case 0x92:
            cycles = op_sub_d(cpu);
            break;
        case 0x93:
            cycles = op_sub_e(cpu);
            break;
        case 0x94:
            cycles = op_sub_h(cpu);
            break;
        case 0x95:
            cycles = op_sub_l(cpu);
            break;
        case 0x96:
            cycles = op_sub_hl(cpu, m);
            break;
        case 0x97:
            cycles = op_sub_a_a(cpu);
            break;
        case 0x98:
            cycles = op_sbc_a_b(cpu);
            break;
        case 0x99:
            cycles = op_sbc_a_c(cpu);
            break;
        case 0x9A:
            cycles = op_sbc_a_d(cpu);
            break;
        case 0x9B:
            cycles = op_sbc_a_e(cpu);
            break;
        case 0x9C:
            cycles = op_sbc_a_h(cpu);
            break;
        case 0x9D:
            cycles = op_sbc_a_l(cpu);
            break;
        case 0x9E:
            cycles = op_sbc_a_hl(cpu, m);
            break;
        case 0x9F:
            cycles = op_sbc_a_a(cpu);
            break;
        case 0xD6:
            cycles = op_sub_d8(cpu, m);
            break;
        case 0xDE:
            cycles = op_sbc_a_d8(cpu, m);
            break;
        case 0xA0:
            cycles = op_and_b(cpu);
            break;
        case 0xA1:
            cycles = op_and_c(cpu);
            break;
        case 0xA2:
            cycles = op_and_d(cpu);
            break;
        case 0xA3:
            cycles = op_and_e(cpu);
            break;
        case 0xA4:
            cycles = op_and_h(cpu);
            break;
        case 0xA5:
            cycles = op_and_l(cpu);
            break;
        case 0xA6:
            cycles = op_and_hl(cpu, m);
            break;
        case 0xA7:
            cycles = op_and_a_a(cpu);
            break;
        case 0xA8:
            cycles = op_xor_b(cpu);
            break;
        case 0xA9:
            cycles = op_xor_c(cpu);
            break;
        case 0xAA:
            cycles = op_xor_d(cpu);
            break;
        case 0xAB:
            cycles = op_xor_e(cpu);
            break;
        case 0xAC:
            cycles = op_xor_h(cpu);
            break;
        case 0xAD:
            cycles = op_xor_l(cpu);
            break;
        case 0xAE:
            cycles = op_xor_hl(cpu, m);
            break;
        case 0xAF:
            cycles = op_xor_a_a(cpu);
            break;
        case 0xB0:
            cycles = op_or_b(cpu);
            break;
        case 0xB1:
            cycles = op_or_c(cpu);
            break;
        case 0xB2:
            cycles = op_or_d(cpu);
            break;
        case 0xB3:
            cycles = op_or_e(cpu);
            break;
        case 0xB4:
            cycles = op_or_h(cpu);
            break;
        case 0xB5:
            cycles = op_or_l(cpu);
            break;
        case 0xB6:
            cycles = op_or_hl(cpu, m);
            break;
        case 0xB7:
            cycles = op_or_a_a(cpu);
            break;
        case 0xB8:
            cycles = op_cp_b(cpu);
            break;
        case 0xB9:
            cycles = op_cp_c(cpu);
            break;
        case 0xBA:
            cycles = op_cp_d(cpu);
            break;
        case 0xBB:
            cycles = op_cp_e(cpu);
            break;
        case 0xBC:
            cycles = op_cp_h(cpu);
            break;
        case 0xBD:
            cycles = op_cp_l(cpu);
            break;
        case 0xBE:
            cycles = op_cp_hl(cpu, m);
            break;
        case 0xBF:
            cycles = op_cp_a_a(cpu);
            break;
        case 0xF6:
            cycles = op_or_d8(cpu, m);
            break;
        case 0xF3:
            cycles = op_di(cpu);
            break;
        case 0xFB:
            cycles = op_ei(cpu);
            break;
        case 0xC0:
            cycles = op_ret_nz(cpu, m);
            break;
        case 0xC1:
            cycles = op_pop_bc(cpu, m);
            break;
        case 0xC4:
            cycles = op_call_nz_a16(cpu, m);
            break;
        case 0xC5:
            cycles = op_push_bc(cpu, m);
            break;
        case 0xC7:
            cycles = op_rst_00(cpu, m);
            break;
        case 0xC8:
            cycles = op_ret_z(cpu, m);
            break;
        case 0xC9:
            cycles = op_ret(cpu, m);
            break;
        case 0xCA:
            cycles = op_jp_z_a16(cpu, m);
            break;
        case 0xCC:
            cycles = op_call_z_a16(cpu, m);
            break;
        case 0xCD:
            cycles = op_call_a16(cpu, m);
            break;
        case 0xCF:
            cycles = op_rst_08(cpu, m);
            break;
        case 0xD0:
            cycles = op_ret_nc(cpu, m);
            break;
        case 0xD1:
            cycles = op_pop_de(cpu, m);
            break;
        case 0xD2:
            cycles = op_jp_nc_a16(cpu, m);
            break;
        case 0xD4:
            cycles = op_call_nc_a16(cpu, m);
            break;
        case 0xD5:
            cycles = op_push_de(cpu, m);
            break;
        case 0xD7:
            cycles = op_rst_10(cpu, m);
            break;
        case 0xD8:
            cycles = op_ret_c(cpu, m);
            break;
        case 0xD9:
            cycles = op_reti(cpu, m);
            break;
        case 0xDA:
            cycles = op_jp_c_a16(cpu, m);
            break;
        case 0xDC:
            cycles = op_call_c_a16(cpu, m);
            break;
        case 0xDF:
            cycles = op_rst_18(cpu, m);
            break;
        case 0xE0:
            cycles = op_ldh_a8_a(cpu, m);
            break;
        case 0xE1:
            cycles = op_pop_hl(cpu, m);
            break;
        case 0xE2:
            cycles = op_ld_c_a_ind(cpu, m);
            break;
        case 0xE5:
            cycles = op_push_hl(cpu, m);
            break;
        case 0xE7:
            cycles = op_rst_20(cpu, m);
            break;
        case 0xE8:
            cycles = op_add_sp_r8(cpu, m);
            break;
        case 0xE9:
            cycles = op_jp_hl(cpu);
            break;
        case 0xEF:
            cycles = op_rst_28(cpu, m);
            break;
        case 0xF1:
            cycles = op_pop_af(cpu, m);
            break;
        case 0xF2:
            cycles = op_ld_a_c_ind(cpu, m);
            break;
        case 0xF5:
            cycles = op_push_af(cpu, m);
            break;
        case 0xF7:
            cycles = op_rst_30(cpu, m);
            break;
        case 0xC2:
            cycles = op_jp_nz_a16(cpu, m);
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
        case 0xF9:
            cycles = op_ld_sp_hl(cpu);
            break;
        case 0xFA:
            cycles = op_ld_a_a16(cpu, m);
            break;
        case 0xEE:
            cycles = op_xor_d8(cpu, m);
            break;
        case 0xFF:
            cycles = op_rst_38(cpu, m);
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

