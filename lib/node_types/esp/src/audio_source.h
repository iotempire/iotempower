// audio_source.h
// Common interface between the i2s_player (consumer) and the devices that
// deliver audio data (sources: tone generator now, SD card and internal flash
// later, an MQTT stream much later).
//
// A source hands out interleaved 16 bit PCM frames. It knows nothing about
// I2S, volume, tasks or MQTT.
//
// read() return value:
//   > 0  number of frames written to the buffer
//     0  end of stream (file finished)
//   < 0  no data available right now (stream source, not an error)
//
// The source is opened with the raw payload of the play command; what that
// payload means (file name, tone frequency, ...) is up to the source.

#ifndef _AUDIO_SOURCE_H_
#define _AUDIO_SOURCE_H_

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#ifdef ESP32

class Audio_Source {
    public:
        virtual ~Audio_Source() {}

        virtual bool open(const char* payload) = 0;
        virtual uint32_t sample_rate() const = 0;
        virtual uint8_t channels() const = 0; // 1 (mono) or 2 (stereo)
        virtual int read(int16_t* frames, size_t want) = 0;
        // Rewind for looping. Sources that cannot rewind return false.
        virtual bool rewind() { return false; }
        virtual bool loopable() const { return false; }
        virtual void close() = 0;
};

// Sources register themselves so the player can find them by device name.
// Fixed size, no dynamic memory: a handful of sources is plenty.
class Audio_Source_Registry {
    public:
        static const uint8_t MAX = 4;

        void add(const char* name, Audio_Source* source) {
            if (name == NULL || source == NULL || _count >= MAX) return;
            _entries[_count].name = name;
            _entries[_count].source = source;
            _count++;
        }

        Audio_Source* find(const char* name) {
            if (name == NULL) return NULL;
            for (uint8_t i = 0; i < _count; i++) {
                if (strcmp(_entries[i].name, name) == 0) {
                    return _entries[i].source;
                }
            }
            return NULL;
        }

        // The first registered source. Used while the player has no source
        // selection command: with a single source in setup.cpp this is it.
        Audio_Source* first() {
            return _count > 0 ? _entries[0].source : NULL;
        }

        uint8_t count() const { return _count; }

    private:
        struct Entry {
            const char* name;
            Audio_Source* source;
        };
        Entry _entries[MAX];
        uint8_t _count = 0;
};

// Function local static: one registry per program, safe to use during static
// initialisation (sources register in their constructors).
inline Audio_Source_Registry& audio_sources() {
    static Audio_Source_Registry registry;
    return registry;
}

#endif // ESP32

#endif // _AUDIO_SOURCE_H_
