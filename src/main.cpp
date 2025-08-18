#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include "cpu.h"
#include "mem.h"
#include "rom.h"
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
            } else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
                int pressed = (e.type == SDL_KEYDOWN);
                switch (e.key.keysym.scancode) {
                    case SDL_SCANCODE_RIGHT:
                        mem_set_joypad(mem, JOY_RIGHT, pressed);
                        break;
                    case SDL_SCANCODE_LEFT:
                        mem_set_joypad(mem, JOY_LEFT, pressed);
                        break;
                    case SDL_SCANCODE_UP:
                        mem_set_joypad(mem, JOY_UP, pressed);
                        break;
                    case SDL_SCANCODE_DOWN:
                        mem_set_joypad(mem, JOY_DOWN, pressed);
                        break;
                    case SDL_SCANCODE_Z:
                        mem_set_joypad(mem, JOY_A, pressed);
                        break;
                    case SDL_SCANCODE_X:
                        mem_set_joypad(mem, JOY_B, pressed);
                        break;
                    case SDL_SCANCODE_RETURN:
                        mem_set_joypad(mem, JOY_START, pressed);
                        break;
                    case SDL_SCANCODE_RSHIFT:
                        mem_set_joypad(mem, JOY_SELECT, pressed);
                        break;
                    default:
                        break;
                }
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

