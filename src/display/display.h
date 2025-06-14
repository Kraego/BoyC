#ifndef DISPLAY_H
#define DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

int display_init(int width, int height);
void display_render(const uint32_t *pixels);
void display_draw_line(int x1, int y1, int x2, int y2, uint32_t color);
void display_draw_circle(int cx, int cy, int r, uint32_t color);
void display_destroy();

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_H */
