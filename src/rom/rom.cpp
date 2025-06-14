#include "rom.h"
#include <stdio.h>

int load_rom(const char *filename, uint8_t *rom, size_t max_size) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Failed to open ROM file\n");
        return -1;
    }

    size_t read_bytes = fread(rom, 1, max_size, file);
    fclose(file);

    if (read_bytes == 0) {
        printf("ROM file is empty or read error occurred\n");
        return -2;
    }

    printf("Loaded %zu bytes from %s\n", read_bytes, filename);
    return 0;
}