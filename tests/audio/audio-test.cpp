#include "ctest.h"
#include "audio.h"

TEST(audio_init_test, init_destroy) {
    if (audio_init(44100, 512) != 0) {
        GTEST_SKIP();
    }
    audio_play_sample(0, 0);
    audio_destroy();
    SUCCEED();
}
