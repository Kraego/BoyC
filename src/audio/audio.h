#ifndef AUDIO_H
#define AUDIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

int audio_init(int sample_rate, int buffer_size);
void audio_play_sample(int16_t left, int16_t right);
void audio_destroy();

#ifdef __cplusplus
}
#endif

#endif /* AUDIO_H */
