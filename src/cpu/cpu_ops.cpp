
#include "cpu.h"

/* LD A,B  (opcode 0x78) */
static inline void op_ld_a_b(cpu_t *c) { c->r.a = c->r.b; }