// dev_i2s_tone.cpp

#include "dev_i2s_tone.h"

#ifdef ESP32

#include "audio_source.h"

I2S_Tone::I2S_Tone(const char* name, float default_frequency)
        : Device(name, 1000000), _source(default_frequency) {
    audio_sources().add(name, &_source);
}

#endif // ESP32
