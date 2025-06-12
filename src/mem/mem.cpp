#include <stdlib.h>
#include <string.h>
#include "mem.h"

struct mem {
    /* Fixed areas */
    uint8_t  wram[8 * 1024];
    uint8_t  vram[8 * 1024];
    uint8_t  hram[0x7F];
    uint8_t  oam [160];
    uint8_t  eram[(7 * 1024) + 512];
    uint8_t  io[128];
    uint8_t  ie;

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

uint8_t mem_read_byte(mem_t *m, uint16_t adr)
{
    switch (adr >> 12) { // high nibble of address
        case 0x0 ... 0x3: // 0000–3FFF: Fixed ROM bank 0
            return m->rom[adr];
        case 0x4 ... 0x7: // 4000–7FFF: Switchable ROM bank
            return m->rom[(m->rom_bank * 0x4000) + (adr & 0x3FFF)];
        case 0x8 ... 0x9: // 8000–9FFF: VRAM
            return m->vram[adr - 0x8000];
        case 0xA ... 0xB: // A000–BFFF: External (cartridge) RAM
            return m->eram[adr - 0xA000];
        case 0xC:         // C000–CFFF: Work RAM bank 0
            return m->wram[adr - 0xC000];
        case 0xD:         // D000–DFFF: Work RAM bank 1
            return m->wram[0x1000 + (adr - 0xD000)];
        case 0xE:         // E000–EFFF: Echo RAM (mirror of C000–DDFF)
            return m->wram[adr - 0xE000];
        case 0xF:
            if (adr < 0xFE00) { // F000–FDFF: Echo RAM continued
                return m->wram[adr - 0xE000];
            } else if (adr < 0xFEA0) { // FE00–FE9F: Sprite attribute table (OAM)
                return m->oam[adr - 0xFE00];
            } else if (adr < 0xFF00) { // FEA0–FEFF: Unusable memory
                return 0xFF;
            } else if (adr < 0xFF80) { // FF00–FF7F: I/O Registers
                return m->io[adr - 0xFF00];
            } else if (adr < 0xFFFF) { // FF80–FFFE: High RAM (HRAM)
                return m->hram[adr - 0xFF80];
            } else { // FFFF: Interrupt Enable Register
                return m->ie;
            }
        default:
            return 0xFF; // Open bus
    }
}

uint16_t mem_read_word(mem_t *m, uint16_t adr)
{
    uint8_t low = mem_read_byte(m, adr);
    uint8_t high = mem_read_byte(m, adr + 1);

    return (high << 8) | low;
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
