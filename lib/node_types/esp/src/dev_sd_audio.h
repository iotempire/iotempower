// dev_sd_audio.h
// SD card as the audio source of the i2s_player. Declare it before the
// player, so the player finds it as the first registered source:
//
//   sd_audio(quelle, 2, 4, 6, 8);   // CS, SCK, MOSI, MISO
//   i2s_player(player, 40, 36, 38);
//
// The device has no MQTT topics of its own. The "play" payload of the player
// is the file name; files live in /audio/ on the card, so "play ding.wav"
// reads /audio/ding.wav. Names are case sensitive.
//
// The card is mounted on the first play, not in start(): a node without a
// card still boots and reports the problem in the log when playing.

#ifndef _SD_AUDIO_H_
#define _SD_AUDIO_H_

#include <device.h>

#ifdef ESP32

#include <FS.h>
#include <SD.h>
#include <SPI.h>

#include "audio_source_wav.h"
#include "audio_storage.h"

// One file at a time: the player reads a single stream.
class SD_Audio_File : public Audio_File {
    public:
        void attach(File file) { _file = file; }
        int read(void* buffer, size_t bytes) {
            return _file ? (int)_file.read((uint8_t*)buffer, bytes) : -1;
        }
        bool seek(uint32_t position) { return _file && _file.seek(position); }
        uint32_t position() { return _file ? (uint32_t)_file.position() : 0; }
        uint32_t size() { return _file ? (uint32_t)_file.size() : 0; }
        void close() {
            if (_file) _file.close();
        }

    private:
        File _file;
};

class SD_Audio_Storage : public Audio_Storage {
    public:
        SD_Audio_Storage(uint8_t cs, uint8_t sck, uint8_t mosi, uint8_t miso)
                : _cs(cs), _sck(sck), _mosi(mosi), _miso(miso) {}

        Audio_File* open(const char* name);

    private:
        bool mount();

        uint8_t _cs;
        uint8_t _sck;
        uint8_t _mosi;
        uint8_t _miso;
        bool _mounted = false;
        SD_Audio_File _file;
};

class SD_Audio : public Device {
    public:
        SD_Audio(const char* name, uint8_t cs, uint8_t sck, uint8_t mosi,
            uint8_t miso);

    private:
        SD_Audio_Storage _storage;
        Audio_Source_Wav _source;
};

#else

// Storage exists only on ESP32. The type stays compilable elsewhere and
// simply does nothing.
class SD_Audio : public Device {
    public:
        SD_Audio(const char* name, uint8_t, uint8_t, uint8_t, uint8_t)
            : Device(name, 1000000) {}
};

#endif // ESP32

#endif // _SD_AUDIO_H_
