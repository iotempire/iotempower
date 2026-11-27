// dev_sd_audio.cpp

#include "dev_sd_audio.h"

#ifdef ESP32

#include <stdio.h>

#include <toolbox.h>

#include "audio_source.h"
#include "audio_status.h"

#define SD_AUDIO_PATH_SIZE 96
#define SD_AUDIO_FOLDER "/audio/"

SD_Audio::SD_Audio(const char* name, SD_Card& card)
        : Device(name, 1000000), // nothing is polled, the player reads
          _storage(card),
          _source(&_storage) {
    // The player takes the first registered source; in setup.cpp this is the
    // only one.
    audio_sources().add(name, &_source);
}

Audio_File* SD_Audio_Storage::open(const char* name) {
    if (name == NULL || name[0] == 0) {
        ulog(F("sd_audio: empty file name."));
        audio_status_error("error: no file name");
        return NULL;
    }

    char path[SD_AUDIO_PATH_SIZE];
    if (name[0] == '/') {
        snprintf(path, sizeof(path), "%s", name);
    } else {
        snprintf(path, sizeof(path), SD_AUDIO_FOLDER "%s", name);
    }
    File file = _card.open(path);
    if (!file) {
        if (!_card.mounted()) {
            ulog(F("sd_audio: the card is not mounted."));
            audio_status_error("error: sd mount");
        } else {
            ulog(F("sd_audio: >%s< not found on the card."), path);
            audio_status_error("error: file missing");
        }
        return NULL;
    }
    _file.close();
    _file.attach(file);
    return &_file;
}

#endif // ESP32
