// dev_sd_audio.cpp

#include "dev_sd_audio.h"

#ifdef ESP32

#include <stdio.h>

#include <toolbox.h>

#include "audio_source.h"
#include "audio_status.h"

#define SD_AUDIO_PATH_SIZE 96
#define SD_AUDIO_FOLDER "/audio/"
#define SD_AUDIO_SPI_HZ 4000000

SD_Audio::SD_Audio(const char* name, uint8_t cs, uint8_t sck, uint8_t mosi,
        uint8_t miso)
        : Device(name, 1000000), // nothing is polled, the player reads
          _storage(cs, sck, mosi, miso),
          _source(&_storage) {
    // The player takes the first registered source; in setup.cpp this is the
    // only one.
    audio_sources().add(name, &_source);
}

bool SD_Audio_Storage::mount() {
    if (_mounted) return true;
    // The board has no usable default SPI pins, so they are set explicitly
    // before SD.begin() takes over the bus.
    SPI.begin(_sck, _miso, _mosi, _cs);
    if (!SD.begin(_cs, SPI, SD_AUDIO_SPI_HZ)) {
        ulog(F("sd_audio: mounting the card failed "
            "(CS %u, SCK %u, MOSI %u, MISO %u)."), (unsigned)_cs,
            (unsigned)_sck, (unsigned)_mosi, (unsigned)_miso);
        audio_status_error("error: sd mount");
        return false;
    }
    _mounted = true;
    ulog(F("sd_audio: card mounted (CS %u, SCK %u, MOSI %u, MISO %u)."),
        (unsigned)_cs, (unsigned)_sck, (unsigned)_mosi, (unsigned)_miso);
    return true;
}

Audio_File* SD_Audio_Storage::open(const char* name) {
    if (name == NULL || name[0] == 0) {
        ulog(F("sd_audio: empty file name."));
        audio_status_error("error: no file name");
        return NULL;
    }
    if (!mount()) return NULL;

    char path[SD_AUDIO_PATH_SIZE];
    if (name[0] == '/') {
        snprintf(path, sizeof(path), "%s", name);
    } else {
        snprintf(path, sizeof(path), SD_AUDIO_FOLDER "%s", name);
    }
    File file = SD.open(path);
    if (!file) {
        ulog(F("sd_audio: >%s< not found on the card."), path);
        audio_status_error("error: file missing");
        return NULL;
    }
    _file.close();
    _file.attach(file);
    return &_file;
}

#endif // ESP32
