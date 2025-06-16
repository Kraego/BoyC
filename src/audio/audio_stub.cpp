#include "audio.h"
#include <stdint.h>

int audio_init(int sample_rate, int buffer_size) { (void)sample_rate; (void)buffer_size; return -1; }
void audio_play_sample(int16_t left, int16_t right) { (void)left; (void)right; }
void audio_destroy() {}

