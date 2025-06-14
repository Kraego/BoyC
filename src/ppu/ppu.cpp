#include "ppu.h"
#include <string.h>

static const uint32_t palette[4] = {
    0xFFFFFFFF, 0xAAAAAAFF, 0x555555FF, 0x000000FF
};

void ppu_init(ppu_t *p, uint32_t *frame)
{
    memset(p, 0, sizeof(*p));
    p->frame = frame;
}

void ppu_reset(ppu_t *p)
{
    uint32_t *fb = p->frame;
    memset(p, 0, sizeof(*p));
    p->frame = fb;
}

static void draw_tile_row(ppu_t *p, mem_t *m,
                          uint16_t tile_base, int tile_index,
                          int row, int x, int y)
{
    uint16_t adr = tile_base + tile_index * 16 + row * 2;
    uint8_t lo = mem_read_byte(m, adr);
    uint8_t hi = mem_read_byte(m, adr + 1);
    for (int i = 0; i < 8; ++i) {
        int bit = 7 - i;
        int color_id = ((hi >> bit) & 1) << 1 | ((lo >> bit) & 1);
        uint32_t color = palette[color_id];
        int px = x + i;
        int py = y;
        if (px >= 0 && px < 160 && py >= 0 && py < 144) {
            p->frame[py * 160 + px] = color;
        }
    }
}

static void render_frame(ppu_t *p, mem_t *m)
{
    uint16_t bg_map = 0x9800;
    uint16_t tile_base = 0x8000;
    for (int ty = 0; ty < 18; ++ty) {
        for (int tx = 0; tx < 20; ++tx) {
            int map_index = ty * 32 + tx;
            int tile_index = mem_read_byte(m, bg_map + map_index);
            for (int row = 0; row < 8; ++row) {
                draw_tile_row(p, m, tile_base, tile_index, row,
                              tx * 8, ty * 8 + row);
            }
        }
    }
}

void ppu_step(ppu_t *p, uint64_t delta, mem_t *m)
{
    p->cycle += delta;
    const uint64_t CYCLES_PER_FRAME = 70224;
    if (p->cycle >= CYCLES_PER_FRAME) {
        p->cycle -= CYCLES_PER_FRAME;
        render_frame(p, m);
    }
}
