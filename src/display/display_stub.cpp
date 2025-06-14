#include "display.h"
#include <stdint.h>

int display_init(int width, int height) {
    (void)width; (void)height; return -1; /* indicate SDL not available */
}
void display_render(const uint32_t *pixels) { (void)pixels; }
void display_draw_line(int x1, int y1, int x2, int y2, uint32_t color) {
    (void)x1; (void)y1; (void)x2; (void)y2; (void)color; }
void display_draw_circle(int cx, int cy, int r, uint32_t color) {
    (void)cx; (void)cy; (void)r; (void)color; }
void display_destroy() {}
