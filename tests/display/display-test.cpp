#include "ctest.h"
#include "display.h"

TEST(display_line_test, draw_line) {
    if (display_init(64, 64) != 0) {
        GTEST_SKIP();
    }
    display_draw_line(0, 0, 63, 63, 0xFFFFFFFF);
    display_destroy();
    SUCCEED();
}

TEST(display_circle_test, draw_circle) {
    if (display_init(64, 64) != 0) {
        GTEST_SKIP();
    }
    display_draw_circle(32, 32, 10, 0xFFFFFFFF);
    display_destroy();
    SUCCEED();
}
