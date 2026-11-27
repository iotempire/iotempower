// dev_i2s_tone.h
// Test source for the I2S output. Declare it in setup.cpp before the player:
//
//   i2s_tone(quelle, 440);
//   i2s_player(player, 40, 36, 38);
//
// The device registers itself as an audio source, so the player uses it
// without knowing anything about tones. It has no MQTT topics of its own.

#ifndef _I2S_TONE_H_
#define _I2S_TONE_H_

#include <device.h>

#ifdef ESP32

#include "audio_source_tone.h"

class I2S_Tone : public Device {
    public:
        I2S_Tone(const char* name, float default_frequency = 440.0f);

    private:
        Audio_Source_Tone _source;
};

#else

// Tones exist only on ESP32. The type stays compilable elsewhere and simply
// does nothing.
class I2S_Tone : public Device {
    public:
        I2S_Tone(const char* name, float = 440.0f) : Device(name, 1000000) {}
};

#endif // ESP32

#endif // _I2S_TONE_H_
