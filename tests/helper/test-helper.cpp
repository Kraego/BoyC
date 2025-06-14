#include <cstdio>
#include <cstdint>
#include <gtest/gtest.h>
#include <iostream>
#include "test-helper.h"

// These are assumed to be defined in C code, so we declare them extern "C"
extern "C" {
    #include "memory.h"
}

#define GB_MICROTEST_TEST_RESULT          (0xFF82)
#define GB_MICROTEST_TEST_TEST_EXPECTED   (0xFF81)
#define GB_MICROTEST_TEST_TEST_ACTUAL     (0xFF80)

#define COUT_MSG_LEN    (32)

uint8_t gb_microtest_check_result(mem_t *mem) {
    uint8_t result = mem_read_byte(mem, GB_MICROTEST_TEST_RESULT);

    if (result != 0x01) {
        char expected_out[COUT_MSG_LEN] = {};
        char actual_out[COUT_MSG_LEN] = {};

        uint8_t expected = mem_read_byte(mem, GB_MICROTEST_TEST_TEST_EXPECTED);
        uint8_t actual = mem_read_byte(mem, GB_MICROTEST_TEST_TEST_ACTUAL);

        snprintf(expected_out, COUT_MSG_LEN, "Expected: %x", expected);
        std::cerr << "[          ] [ INFO ] " << expected_out << std::endl;

        snprintf(actual_out, COUT_MSG_LEN, "Actual: %x", actual);
        std::cerr << "[          ] [ INFO ] " << actual_out << std::endl;
    }

    return result;
}
