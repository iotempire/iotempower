// audio_source_tone.cpp

#include "audio_source_tone.h"

#ifdef ESP32

#include <math.h>
#include <stdlib.h>

#include <toolbox.h>

#define AUDIO_TONE_TWO_PI 6.28318530718f
#define AUDIO_TONE_AMPLITUDE 8000
#define AUDIO_TONE_MAX_FREQUENCY 8000.0f

Audio_Source_Tone::Audio_Source_Tone(float default_frequency)
        : _default_frequency(default_frequency) {
}

bool Audio_Source_Tone::open(const char* payload) {
    float frequency = _default_frequency;
    if (payload != NULL && payload[0] != 0) {
        char* end = NULL;
        float parsed = strtof(payload, &end);
        if (end != payload && parsed > 0.0f
                && parsed <= AUDIO_TONE_MAX_FREQUENCY) {
            frequency = parsed;
        } else {
            ulog(F("i2s_tone: invalid frequency >%s<, using %d Hz."),
                payload, (int)frequency);
        }
    }
    _frequency = frequency;
    _phase = 0.0f;
    _step = AUDIO_TONE_TWO_PI * _frequency / (float)sample_rate();
    ulog(F("i2s_tone: %d Hz, %u Hz sample rate."), (int)_frequency,
        (unsigned)sample_rate());
    return true;
}

int Audio_Source_Tone::read(int16_t* frames, size_t want) {
    for (size_t i = 0; i < want; i++) {
        frames[i] = (int16_t)(sinf(_phase) * AUDIO_TONE_AMPLITUDE);
        _phase += _step;
        if (_phase >= AUDIO_TONE_TWO_PI) _phase -= AUDIO_TONE_TWO_PI;
    }
    return (int)want;
}

#endif // ESP32
