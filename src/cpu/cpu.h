#ifndef CPU_H
#define CPU_H

/**
 * Emulation of the gb cpu
 */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "mem.h"

#pragma pack(push, 1)          // portable way to suppress structure padding
typedef union {
    struct {                   // 8-bit view  (low-address byte first)
        uint8_t f, a;          // AF  (A is high byte, F is low in GB docs)
        uint8_t c, b;          // BC
        uint8_t e, d;          // DE
        uint8_t l, h;          // HL
    };
    struct {                   // 16-bit view, reading same memory
        uint16_t af;
        uint16_t bc;
        uint16_t de;
        uint16_t hl;
    };
} cpu_regs_t;
#pragma pack(pop)

/* Flag masks in F (bit positions are identical to real hardware) */
#define F_Z (1u << 7)
#define F_N (1u << 6)
#define F_H (1u << 5)
#define F_C (1u << 4)

static inline void cpu_set_flag(cpu_regs_t *r, uint8_t mask, int on)
{
    if(on){
        r->f |=  mask;
    } else { 
        r->f &= ~mask;
    }
}

static inline int cpu_get_flag(const cpu_regs_t *r, uint8_t mask)
{
    return (r->f & mask) != 0;
}

typedef struct {
    cpu_regs_t   r;      /* all eight CPU registers, via the union we discussed */
    uint16_t    pc;      /* program counter */
    uint16_t    sp;      /* stack pointer */
    uint8_t     ime;     /* master-interrupt enable flip-flop (0/1) */
    uint64_t    cycles;  /* running machine-cycle counter */
    /* …anything else you track (halt flag, speed switch, etc.) … */
} cpu_t;

// Function prototypes
void cpu_reset(cpu_t *cpu);
void cpu_dump(const cpu_t *c);
int8_t cpu_step(cpu_t *c, mem_t *m);

#ifdef __cplusplus
}
#endif

#endif  // CPU_H
