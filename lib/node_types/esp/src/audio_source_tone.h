// audio_source_tone.h
// Endless sine wave source. It is the test vehicle for the I2S output: it
// proves that playback works without any storage medium.

#ifndef _AUDIO_SOURCE_TONE_H_
#define _AUDIO_SOURCE_TONE_H_

#include "audio_source.h"

#ifdef ESP32

class Audio_Source_Tone : public Audio_Source {
    public:
        explicit Audio_Source_Tone(float default_frequency = 440.0f);

        // Payload is a frequency in Hz. An empty payload uses the default
        // frequency given in setup.cpp.
        bool open(const char* payload);
        uint32_t sample_rate() const { return 16000; }
        uint8_t channels() const { return 1; }
        int read(int16_t* frames, size_t want);
        void close() {}

    private:
        float _default_frequency;
        float _frequency = 440.0f;
        float _phase = 0.0f;
        float _step = 0.0f;
};

#endif // ESP32

#endif // _AUDIO_SOURCE_TONE_H_
