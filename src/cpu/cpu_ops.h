#ifndef CPU_OPS_H
#define CPU_OPS_H

/**
 * Emulation of the gb cpu
 */
#ifdef __cplusplus
extern "C" {
#endif

#include "cpu.h"

#define OPS_COUNT   (1)

uint8_t instr_cycles[OPS_COUNT] = {};

static inline void op_ld_a_b(cpu_t *c);

#ifdef __cplusplus
}
#endif

#endif  // CPU_OPS_H