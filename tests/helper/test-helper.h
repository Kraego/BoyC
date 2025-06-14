#ifndef TESTHELPER_H
#define TESTHELPER_H

/**
 * Emulation of the gb memory
 */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "mem.h"

uint8_t gb_microtest_check_result(mem_t *mem);


#ifdef __cplusplus
}
#endif

#endif  // TESTHELPER_H