#include "audio.h"
#include <SDL.h>
#include <stdio.h>

static SDL_AudioDeviceID device = 0;

int audio_init(int sample_rate, int buffer_size)
{
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        fprintf(stderr, "SDL_InitSubSystem Error: %s\n", SDL_GetError());
        return -1;
    }

    SDL_AudioSpec want;
    SDL_zero(want);
    want.freq = sample_rate;
    want.format = AUDIO_S16SYS;
    want.channels = 2;
    want.samples = buffer_size;

    device = SDL_OpenAudioDevice(NULL, 0, &want, NULL, 0);
    if (device == 0) {
        fprintf(stderr, "SDL_OpenAudioDevice Error: %s\n", SDL_GetError());
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return -1;
    }

    SDL_PauseAudioDevice(device, 0);
    return 0;
}

void audio_play_sample(int16_t left, int16_t right)
{
    if (device == 0) {
        return;
    }
    int16_t sample[2] = { left, right };
    SDL_QueueAudio(device, sample, sizeof(sample));
}

void audio_destroy()
{
    if (device != 0) {
        SDL_CloseAudioDevice(device);
        device = 0;
    }
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

