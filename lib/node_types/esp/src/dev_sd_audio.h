// dev_sd_audio.h
// SD card as the audio source of the i2s_player. The card itself is the
// generic sd_card device; this device adds the audio decoding and the /audio/
// path convention:
//
//   sd_card(sd, 2, 4, 6, 8);      // CS, SCK, MOSI, MISO
//   sd_audio(quelle, IN(sd));
//   i2s_player(player, 40, 36, 38);
//
// The "play" payload of the player is the file name; files live in /audio/ on
// the card, so "play ding.wav" reads /audio/ding.wav. Names are case
// sensitive. The device has no MQTT topics of its own.

#ifndef _SD_AUDIO_H_
#define _SD_AUDIO_H_

#include <device.h>

#ifdef ESP32

#include <FS.h>

#include "audio_source_wav.h"
#include "audio_storage.h"
#include "dev_sd_card.h"

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
        explicit SD_Audio_Storage(SD_Card& card) : _card(card) {}

        Audio_File* open(const char* name);

    private:
        SD_Card& _card;
        SD_Audio_File _file;
};

class SD_Audio : public Device {
    public:
        SD_Audio(const char* name, SD_Card& card);

    private:
        SD_Audio_Storage _storage;
        Audio_Source_Wav _source;
};

#else

// Storage exists only on ESP32. The type stays compilable elsewhere and
// simply does nothing.
class SD_Audio : public Device {
    public:
        SD_Audio(const char* name, SD_Card&) : Device(name, 1000000) {}
};

#endif // ESP32

#endif // _SD_AUDIO_H_
