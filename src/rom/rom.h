#include <cstdint>
#include <cstddef>
#ifndef ROM_H
#define ROM_H

/**
 * Emulation of the gb memory
 */
#ifdef __cplusplus
extern "C" {
#endif

int load_rom(const char *filename, uint8_t *rom, size_t max_size);

#ifdef __cplusplus
}
#endif

#endif  // ROM_H