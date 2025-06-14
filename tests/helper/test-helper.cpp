#include <stdio.h>
#include "test-helper.h"

#define GB_MICROTEST_TEST_RESULT          (0xFF82)
#define GB_MICROTEST_TEST_TEST_EXPECTED   (0xFF81)
#define GB_MICROTEST_TEST_TEST_ACTUAL     (0xFF80)

uint8_t gb_microtest_check_result(mem_t *mem) {
    uint8_t result = mem_read_byte(mem, GB_MICROTEST_TEST_RESULT);

    if (result != 0x01){
        uint8_t expected = mem_read_byte(mem, GB_MICROTEST_TEST_TEST_EXPECTED);
        uint8_t actual = mem_read_byte(mem, GB_MICROTEST_TEST_TEST_ACTUAL);
        printf("Epected: %x", expected);
        printf("Actual: %x", actual);
    }
    return result;
}