#include <stdio.h>
#include "cpu.h"
#include "mem.h"
#include "rom.h"
#include <stdlib.h>
#include "display.h"
#include "ppu.h"
#include <SDL.h>

int quit = 0;

int main(int argc, char const *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <rom file>\n", argv[0]);
        return 1;
    }

    const size_t cart_size = 0x200000; /* 2MB upper limit */
    uint8_t *cart_image = (uint8_t *)malloc(cart_size);
    if (!cart_image) {
        fprintf(stderr, "Failed to allocate ROM buffer\n");
        return 1;
    }

    if (load_rom(argv[1], cart_image, cart_size) != 0) {
        free(cart_image);
        return 1;
    }

    mem_t *mem = mem_create(cart_image, cart_size);
    cpu_t cpu;
    cpu_reset(&cpu);
    ppu_t ppu;
    uint32_t frame[160 * 144] = {0};
    ppu_init(&ppu, frame);

    if (display_init(160, 144) != 0) {
        mem_reset(mem);
        free(cart_image);
        return 1;
    }

    while (!quit) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }

        uint64_t start_cycles = cpu.cycles;

        cpu_step(&cpu, mem);             /* advance one instruction */

        uint64_t delta = cpu.cycles - start_cycles;
        ppu_step(&ppu, delta, mem);         /* keep PPU in lock-step   */
        (void)delta; /* silence unused variable warning if not used */

        display_render(frame);
        SDL_Delay(16);
    }

    display_destroy();

    ppu_reset(&ppu);

    mem_reset(mem);
    free(cart_image);
    return 0;
}

