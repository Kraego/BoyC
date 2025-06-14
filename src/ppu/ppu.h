#ifndef PPU_H
#define PPU_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "mem.h"

typedef struct {
    uint64_t cycle;
    uint32_t *frame;
} ppu_t;

void ppu_init(ppu_t *p, uint32_t *frame);
void ppu_reset(ppu_t *p);
void ppu_step(ppu_t *p, uint64_t delta, mem_t *m);

#ifdef __cplusplus
}
#endif

#endif /* PPU_H */
