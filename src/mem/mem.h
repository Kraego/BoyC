#ifndef MEM_H
#define MEM_H

/**
 * Emulation of the gb memory
 */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <cstddef>

typedef struct mem mem_t;      /* forward-declare opaque struct */

/* Public bus helpers */
uint8_t mem_rb (mem_t *m, uint16_t addr);               /* read  byte  */
void mem_wb (mem_t *m, uint16_t addr, uint8_t value);   /* write byte */

uint16_t mem_rw (mem_t *m, uint16_t addr);              /* read  word  */
void mem_ww (mem_t *m, uint16_t addr, uint16_t value);  /* write word */

/* Constructor / reset */
mem_t *mem_create(const uint8_t *rom_image, size_t rom_size);
void mem_reset (mem_t *m);

#ifdef __cplusplus
}
#endif

#endif  // MEM_H