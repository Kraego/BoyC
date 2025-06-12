#include <stdlib.h>
#include <string.h>
#include "mem.h"

struct mem {
    /* Fixed areas */
    uint8_t  wram[8 * 1024];
    uint8_t  vram[8 * 1024];
    uint8_t  hram[0x7F];
    uint8_t  oam [160];

    /* Cartridge area */
    const uint8_t *rom;
    size_t         rom_size;

    /* Mapper state (bank numbers, RTC latch, …) */
    uint8_t  rom_bank;
    uint8_t  ram_bank;
    uint8_t  mbc_type;
    /* … */

    /* Pointers to other subsystems if you want tight coupling
       (PPU, APU, timers) for side-effects inside mem_rb/mem_wb */
};

/* --- read byte ---------------------------------------------------------- */
uint8_t mem_rb(mem_t *m, uint16_t a)
{
    switch (a >> 12) {                          /* high nibble = region */
        case 0x0 ... 0x3: /* 0000–3FFF */       /* fixed ROM bank 0 */
            return m->rom[a];
        case 0x4 ... 0x7: /* 4000–7FFF */       /* switchable ROM */
            return m->rom[(m->rom_bank * 0x4000) | (a & 0x3FFF)];
        case 0x8 ... 0x9: /* 8000–9FFF */       /* VRAM */
            return m->vram[a - 0x8000];
        /* …many other cases (WRAM, echo, OAM, I/O, HRAM)… */
        default:
            return 0xFF;                        /* open bus */
    }
}

/* TODOS: mem_wb(), mem_rw(), mem_ww() would mirror the same map,
   plus call-outs for DMA, joypad latches, timer increments, etc.        */

mem_t *mem_create(const uint8_t *rom_image, size_t rom_size){
    mem_t *memory = (mem_t *) malloc(sizeof(mem_t));
    memory->rom = rom_image;
    memory->rom_size = rom_size;

    return memory;
}

void mem_reset (mem_t *m){
    free(m);
}
