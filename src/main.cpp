#include <stdio.h>
#include "cpu.h"
#include "mem.h"

int quit = 0;

int main(int argc, char const *argv[])
{
    cpu_t  cpu;
    uint8_t *cart_image;
    size_t cart_size;

    mem_t *mem = mem_create(cart_image, cart_size);
    cpu_reset(&cpu);

    while (!quit) {
        uint64_t start_cycles = cpu.cycles;

        cpu_step(&cpu, mem);             /* advance one instruction */

        // uint64_t delta = cpu.cycles - start_cycles;
        // ppu_step(&ppu, delta, mem);         /* keep PPU in lock-step   */
        // apu_step(&apu, delta);
        // timer_step(&timer, delta);
        // /* …v-blank sync / input / audio flush… */
    }
}
